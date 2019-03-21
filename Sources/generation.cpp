/*
#
# COPYRIGHT INFORMATION
#
# Copyright (C) 2012 Indian Institute of Science Bangalore 560012, INDIA
#
# This program is part of the Anguli synthetic fingerprint generator
# software distribution deveoped at the Database Systems Lab, Indian
# Institute of Science. The use of the software is free and its use is
# governed by the licensing agreement set up between the copyright owner,
# Indian Institute of Science, and the licensee.
#
# This program is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# The URL of the Anguli project is
#     http://dsl.serc.iisc.ernet.in/projects/Anguli/
# and the email contact address is
#     anguli@dsl.serc.iisc.ernet.in
#
#
# For any issues, contact
# Prof. Jayant R. Haritsa
# SERC
# Indian Institute of Science
# Bangalore 560012, India.
# Telephone : (+91) 80 2293-2793
# Fax       : (+91) 80 2360-2648
# Email     : haritsa@dsl.serc.iisc.ernet.in
# WWW: http://dsl.serc.iisc.ernet.in/~haritsa/
#
*/
        
#include "generation.h"
#include "cv_type_converter.h"
#include "iostream"
#include "fstream"

using namespace std;
using namespace anguli;

/*
  Constructor.
  */
Generator::Generator()
{
    int i;
    temp_fp_2Dmat = new float *[H + margin];
    for (i = 0; i < H + margin; i++)
            temp_fp_2Dmat[i] = new float[W + margin];

    noise_obj = new noise::Noise();
}

/*
  Destructor
  */
Generator::~Generator()
{
    int i;

    for (i = 0; i < H + margin; i++)
        delete[] temp_fp_2Dmat[i];

    delete[] temp_fp_2Dmat;

    delete noise_obj;
}

/*
  Generates the master fingerprint and impressions as per
  setting.
  */
void Generator::generation(void) {
    int i, j, g_k;

    {
        int rand_num;
        
        switch(class_distribution)
        {
            case 0:

                rand_num = rand_r(&generation_seed);
                rand_num = int((double(rand_num) / INT_MAX) * 1000);

                if (rand_num <= 37) {
                    singularity_type = 1;
                    //archN++;
                } else if (rand_num > 37 && rand_num <= 66) {
                    singularity_type = 2;
                    //tarchN++;
                } else if (rand_num > 66 && rand_num <= 383) {
                    singularity_type = 3;
                    //rloopN++;
                } else if (rand_num > 383 && rand_num <= 721) {
                    singularity_type = 4;
                    //lloopN++;
                } else if (rand_num > 721 && rand_num <= 1000) {
                    if (rand_num <= 860) {
                        singularity_type = 5;
                       // dloopN++;
                    } else {
                        singularity_type = 6;
                        //whorlN++;
                    }
                }
                break;
            default:
                singularity_type = class_distribution;
        }
        
        init_para();
        set_param();
        seed_pos();
    }

    orientmap();
    sel_n_merg_densitymap();
    pre_filtering();
    set_filter_area();
    filter_image_firstpass();
    set_filter_area();

    for (g_k = 0; g_k < 2; g_k++) {
        filter_image_withmap();
        set_filter_area();
    }

    for (g_k = 0; g_k < 3; g_k++) {
        filter_image();
    }


    for (i = 0; i < H + margin; i++)
        for (j = 0; j < W + margin; j++)
            temp_fp_2Dmat[i][j] = 1 - f_print1_2Dmat[i][j] / 100.0;

    picturize(temp_fp_2Dmat);
    mat2DtoIpl(temp_fp_2Dmat, f_print1);

    noise_obj->rotmax = ((double)rotation * 22) / (7 * 180);
    noise_obj->rotmin = -noise_obj->rotmax;

    noise_obj->transmax = ((double)translation);
    noise_obj->transmin = -noise_obj->transmax;

    noise_obj->max_noiseLevel = max_noise;
    noise_obj->min_noiseLevel = min_noise;

    std::stringstream filename;
    filename << path_o_f << "/fp_" << n_fo << "/" << n_fp << "." << image_type;

    cvSaveImage(filename.str().c_str(), f_print1);

    int num_scratches = min_scratches + (max_scratches - min_scratches) * ahaq_rand();

    for(int num = 0; num < num_scratches; num++)
    {
        int width = f_print1->width;
        int height = f_print1->height;
        int part = 10;
        int x1 = (width / part) + noise::ahaq_rand() * (((part - 2) * width / part));

        int y1 = (height / part) + noise::ahaq_rand() * (((part - 2) * height / part));

        int x2 = (width / part) + noise::ahaq_rand() * (((part - 2) * width / part));

        int y2 = (height / part) + noise::ahaq_rand() * (((part - 2) * height / part));

        int wide = 1 + noise::ahaq_rand() * 6;

        CvScalar white = CV_RGB(255,255,255);
        cvLine(f_print1, cvPoint(x1, y1), cvPoint(x2, y2), white, wide, 4, 0);
    }

    cvCopy(f_print1, noise_obj->f_print1);
    Ipltomat2D(noise_obj->f_print1, noise_obj->f_print1_2Dmat);
    
    for (i = 0; i < noise::H + noise::margin; i++) {
        for (j = 0; j < noise::W + noise::margin; j++) {
            if (noise_obj->f_print1_2Dmat[i][j] > 127)
                noise_obj->f_print1_2Dmat[i][j] = 255;
            else
                noise_obj->f_print1_2Dmat[i][j] = 0;
            }
        }

        noise_obj->fixed_noise(noise_obj->f_print1);


        int k;
        cvCopy(noise_obj->f_print1, noise_obj->f_print2);
            
        for (i = 0; i < noise::H + noise::margin; i++) {
            for (j = 0; j < noise::W + noise::margin; j++) {
                noise_obj->f_print2_2Dmat[i][j] = noise_obj->f_print1_2Dmat[i][j];
            }
        }
            
        for(k = 0; k < n_impr; k++)
        {
            filename.str("");
            filename << path_impr << "_" << k + 1 << "/fp_" << n_fo << "/" << n_fp << "." << image_type;
            //cout << filename.str() << endl;

            cvCopy(noise_obj->f_print2, noise_obj->f_print1);
            Ipltomat2D(noise_obj->f_print1, noise_obj->f_print1_2Dmat);
            for (i = 0; i < noise::H + noise::margin; i++) {
                for (j = 0; j < noise::W + noise::margin; j++) {
                if (noise_obj->f_print1_2Dmat[i][j] > 127)
                    noise_obj->f_print1_2Dmat[i][j] = 255;
                else
                    noise_obj->f_print1_2Dmat[i][j] = 0;
                }
            }
            
            noise_obj->random_noise(noise_obj->f_print1);
            noise_obj->distortion(filename.str().c_str());

        }

    if(save_meta) {
        filename.str("");
        filename << path_o_ref << "/fp_" << n_fo << "/" << n_fp << ".txt";

        ofstream outfile;
        outfile.open(filename.str().c_str());
        switch(singularity_type)
        {
        case 1:
            outfile << "Type : Arch" << endl;
            break;
        case 2:
            outfile << "Type : Tented Arch" << endl;
            break;
        case 3:
            outfile << "Type : Right Loop" << endl;
            break;
        case 4:
            outfile << "Type : Left Loop" << endl;
            break;
        case 5:
            outfile << "Type : Double Loop" << endl;
            break;
        case 6:
            outfile << "Type : Whorl" << endl;
            break;
        }

        if (singularity_type == 1) {
            outfile << k_arch << endl;
        }

        if (singularity_type == 2 || singularity_type == 3 || singularity_type
                == 4) {
            outfile << "Loop  : " << l[1].x << "\t" << l[1].y << endl;
            outfile << "Delta : " << d[1].x << "\t" << d[1].y << endl;
        }

        if (singularity_type == 5 || singularity_type == 6) {
            outfile << "Loop  : " << l[1].x << "\t" << l[1].y << endl;
            outfile << "Loop  : " << l[2].x << "\t" << l[2].y << endl;
            outfile << "Delta : " << d[1].x << "\t" << d[1].y << endl;
            outfile << "Delta : " << d[2].x << "\t" << d[2].y << endl;
        }

        outfile.close();
    }
}

/*
  Sets the parameters for Generation.
  */
void Generator::set_low_den_para() {
    int den_1, den_2, w1, w2, cy1, cy2, dy1, dy2, cx1, cx2, dx1, dx2;
    float k1, k2;

    den_1 = l[1].y;
    den_2 = d[1].y;

    w1 = den_1;
    w2 = H - den_2;

    k1 = w1 / 4.0;
    k2 = w2 / 5.0;

    cy1 = floor(ahaq_rand() * k1);
    cy2 = floor(w1 - ahaq_rand() * k1);
    dy1 = floor(den_2 + ahaq_rand() * k2);
    dy2 = floor(H - ahaq_rand() * k2);

    cx1 = floor(ahaq_rand() * (W / 2.0));
    cx2 = floor(cx1 + W / 3.0 + 1 * (2 * W / 3.0 - cx1) - 1 * k1);

    dx1 = floor(ahaq_rand() * W / 6.0);
    dx2 = floor(dx1 + 2 * W / 3.0 + 1 * (W / 3.0 - dx1) - 1 * k2);
}

/*
  Initializes the number of seeds and their positions.
  */
void Generator::seed_pos() {
    int i, j, i_blob, j_blob, n_seeds;

    n_seeds = 750 + floor(ahaq_rand() * 100);


    for (k = 0; k < n_seeds; k++) {
        i_blob = floor(5 + ahaq_rand() * (H + margin - 10));
        j_blob = floor(5 + ahaq_rand() * (W + margin - 10));


        filt_map[i_blob][j_blob] = 1;

        for (i = i_blob; i < i_blob + 4; i++) {
            for (j = j_blob; j < j_blob + 4; j++) {
                f_print1_2Dmat[i][j] = 100;
                filt_map[i][j] = 1;
            }
        }
    }
}

/*
  Sets the parameters for fingerprints.
  */
void Generator::set_param() {
    float u, v, u1, v1, f1, f2;

    if (singularity_type == 2) {
        f1 = 2.0 / 3.0;
        f2 = 2.0 / 3.0;
        u = -90 * M_PI / 180.0 + ahaq_rand() * (45 * M_PI / 180.0); 
        v = 45 * M_PI / 180.0 + ahaq_rand() * (45 * M_PI / 180.0); 

        g_cap[1][1][1] = -M_PI + u;
        g_cap[1][1][2] = -3 * M_PI / 4 + f1 * u;
        g_cap[1][1][3] = -M_PI / 2;
        g_cap[1][1][4] = -M_PI / 4 + f1 * v;
        g_cap[1][1][5] = v;
        g_cap[1][1][6] = M_PI / 4 + f1 * v;
        g_cap[1][1][7] = M_PI / 2;
        g_cap[1][1][8] = 3 * M_PI / 4 + f1 * u;
        g_cap[1][1][9] = M_PI + u;

        u1 = 0;
        v1 = 0;

        g_cap[1][2][1] = -M_PI + 2 * u1 / 3;
        g_cap[1][2][2] = -3 * M_PI / 4;
        g_cap[1][2][3] = -M_PI / 2;
        g_cap[1][2][4] = -M_PI / 4;
        g_cap[1][2][5] = 2 * v1 / 3;
        g_cap[1][2][6] = M_PI / 4 + v1;
        g_cap[1][2][7] = M_PI / 2 + 2 * (u1 + v1) / 3;
        g_cap[1][2][8] = 3 * M_PI / 4 + u1;
        g_cap[1][2][9] = M_PI + 2 * u1 / 3;
    } else if (singularity_type == 3) {
        f1 = 2.0 / 3.0;
        //f1=3.0/3.0;
        u = -90 * M_PI / 180.0 + ahaq_rand() * (45 * M_PI / 180.0); 
        v = 60 * M_PI / 180.0 + ahaq_rand() * (45 * M_PI / 180.0); 
        //u=0;v=0;

        g_cap[1][1][1] = -M_PI + u;
        g_cap[1][1][2] = -3 * M_PI / 4 + f1 * u;
        g_cap[1][1][3] = -M_PI / 2;
        g_cap[1][1][4] = -M_PI / 4 + f1 * v;
        g_cap[1][1][5] = v;
        g_cap[1][1][6] = M_PI / 4 + f1 * v;
        g_cap[1][1][7] = M_PI / 2;
        g_cap[1][1][8] = 3 * M_PI / 4 + f1 * u;
        g_cap[1][1][9] = M_PI + u;

        u1 = 0;
        v1 = 0;

        g_cap[1][2][1] = -M_PI + 2 * u1 / 3;
        g_cap[1][2][2] = -3 * M_PI / 4;
        g_cap[1][2][3] = -M_PI / 2;
        g_cap[1][2][4] = -M_PI / 4;
        g_cap[1][2][5] = 2 * v1 / 3;
        g_cap[1][2][6] = M_PI / 4 + v1;
        g_cap[1][2][7] = M_PI / 2 + 2 * (u1 + v1) / 3;
        g_cap[1][2][8] = 3 * M_PI / 4 + u1;
        g_cap[1][2][9] = M_PI + 2 * u1 / 3;
    }

    if (singularity_type == 4) {
        f1 = 2.0 / 3.0;
        f2 = 3.0 / 3.0;
        u = -120 * M_PI / 180.0 + ahaq_rand() * (45 * M_PI / 180.0); 
        v = 50 * M_PI / 180.0 + ahaq_rand() * (45 * M_PI / 180.0); 
        //u=0;v=0;

        g_cap[1][1][1] = -M_PI + u;
        g_cap[1][1][2] = -3 * M_PI / 4 + f1 * u;
        g_cap[1][1][3] = -M_PI / 2;
        g_cap[1][1][4] = -M_PI / 4 + f1 * v;
        g_cap[1][1][5] = v;
        g_cap[1][1][6] = M_PI / 4 + f1 * v;
        g_cap[1][1][7] = M_PI / 2;
        g_cap[1][1][8] = 3 * M_PI / 4 + f1 * u;
        g_cap[1][1][9] = M_PI + u;

        u1 = 0;
        v1 = 0;

        g_cap[1][2][1] = -M_PI + 2 * u1 / 3;
        g_cap[1][2][2] = -3 * M_PI / 4;
        g_cap[1][2][3] = -M_PI / 2;
        g_cap[1][2][4] = -M_PI / 4;
        g_cap[1][2][5] = 2 * v1 / 3;
        g_cap[1][2][6] = M_PI / 4 + v1;
        g_cap[1][2][7] = M_PI / 2 + 2 * (u1 + v1) / 3;
        g_cap[1][2][8] = 3 * M_PI / 4 + u1;
        g_cap[1][2][9] = M_PI + 2 * u1 / 3;
    }

    if (singularity_type == 5 || singularity_type == 6) {
        f1 = 2.0 / 3.0;
        f2 = 2.0 / 3.0;

        u = -60 * M_PI / 180.0 + ahaq_rand() * (15 * M_PI / 180.0); 
        v = 40 * M_PI / 180.0 + ahaq_rand() * (15 * M_PI / 180.0);

        g_cap[1][1][1] = -M_PI + u;
        g_cap[1][1][2] = -3 * M_PI / 4 + f1 * u;
        g_cap[1][1][3] = -M_PI / 2;
        g_cap[1][1][4] = -M_PI / 4 + f1 * v;
        g_cap[1][1][5] = v;
        g_cap[1][1][6] = M_PI / 4 + f2 * v;
        g_cap[1][1][7] = M_PI / 2;
        g_cap[1][1][8] = 3 * M_PI / 4 + f2 * u;
        g_cap[1][1][9] = M_PI + u;

        u1 = 0;
        v1 = 0;

        g_cap[1][2][1] = -M_PI + 2 * u1 / 3;
        g_cap[1][2][2] = -3 * M_PI / 4;
        g_cap[1][2][3] = -M_PI / 2;
        g_cap[1][2][4] = -M_PI / 4;
        g_cap[1][2][5] = 2 * v1 / 3;
        g_cap[1][2][6] = M_PI / 4 + v1;
        g_cap[1][2][7] = M_PI / 2 + 2 * (u1 + v1) / 3;
        g_cap[1][2][8] = 3 * M_PI / 4 + u1;
        g_cap[1][2][9] = M_PI + 2 * u1 / 3;

        u = 10 * M_PI / 180.0 + ahaq_rand() * (15 * M_PI / 180.0); 
        v = 30 * M_PI / 180.0 + ahaq_rand() * (15 * M_PI / 180.0); 
        g_cap[2][1][1] = -M_PI + u;
        g_cap[2][1][2] = -3 * M_PI / 4 + f1 * u;
        g_cap[2][1][3] = -M_PI / 2;
        g_cap[2][1][4] = -M_PI / 4 + f1 * v;
        g_cap[2][1][5] = v;
        g_cap[2][1][6] = M_PI / 4 + f2 * v;
        g_cap[2][1][7] = M_PI / 2;
        g_cap[2][1][8] = 3 * M_PI / 4 + f2 * u;
        g_cap[2][1][9] = M_PI + u;

        u1 = 0;
        v1 = 0;

        g_cap[2][2][1] = -M_PI + 2 * u1 / 3;
        g_cap[2][2][2] = -3 * M_PI / 4;
        g_cap[2][2][3] = -M_PI / 2;
        g_cap[2][2][4] = -M_PI / 4;
        g_cap[2][2][5] = 2 * v1 / 3;
        g_cap[2][2][6] = M_PI / 4 + v1;
        g_cap[2][2][7] = M_PI / 2 + 2 * (u1 + v1) / 3;
        g_cap[2][2][8] = 3 * M_PI / 4 + u1;
        g_cap[2][2][9] = M_PI + 2 * u1 / 3;
    }
}

/*
  Initializes the core and delta points for fingerprint
  according to its type.
  */
void Generator::init_para(void) {
    int crx1, crx2, cry1, cry2;
    int cwx1, cwx2, cwy1, cwy2;
    int cw1x1, cw1x2, cw1y1, cw1y2;
    int cw2x1, cw2x2, cw2y1, cw2y2;
    int d0x1, d0x2, d0y1, d0y2;
    int d1x1, d1x2, d1y1, d1y2;
    int d2x1, d2x2, d2y1, d2y2;
    int dw1x1, dw1x2, dw1y1, dw1y2;
    int dw2x1, dw2x2, dw2y1, dw2y2;
    int ucrx1, ucrx2, ucry1, ucry2;
    int i, j;

    //fixing core and delta points

    //fixing core1-region

    //Blue thick line
    //centered 25%
    ucrx1 = floor(W * 0.25);
    ucrx2 = floor(W - W * 0.25);
    ucry1 = floor(H * 0.25);
    ucry2 = floor(H - H * 0.25);

    //Red thick line
    //core region for rloop, lloop, tarch
    crx1 = floor(W * 0.4);
    crx2 = floor(W - W * 0.4);
    cry1 = floor(H * 0.35);
    cry2 = floor(H - H * 0.45);

    cwx1 = floor(W * 0.4);
    cwx2 = floor(W - W * 0.4);
    cwy1 = floor(H * 0.40);
    cwy2 = floor(H * 0.575);

    //fixing double loop-region
    cw1x1 = floor(W * 0.4);
    cw1x2 = floor(W * 0.5);
    cw1y1 = floor(H * 0.325);
    cw1y2 = floor(H * 0.45);

    cw2x1 = floor(W * (0.5));
    cw2x2 = floor(W * (0.6));
    cw2y1 = floor(H * 0.475);
    cw2y2 = floor(H * 0.6);

    dw1x1 = floor(W * 0.05);
    dw1x2 = floor(W - W * 0.75);
    dw1y1 = floor(H * 0.625);
    dw1y2 = floor(H * 0.875);

    dw2x1 = floor(W * 0.75);
    dw2x2 = floor(W - W * 0.05);
    dw2y1 = floor(H * 0.65);
    dw2y2 = floor(H * 0.85);

    //fixing delta1-region
    d0x1 = crx1;
    d0x2 = crx2;
    d0y1 = floor(H * 0.6);
    d0y2 = floor(H - H * 0.3);

    //fixing delta1-region
    d1x1 = floor(W * 0.12);
    d1x2 = floor(W - W * 0.675);
    d1y1 = floor(H * 0.625);
    d1y2 = floor(H - H * 0.185);

    //fixing delta2-region
    d2x1 = floor(W * 0.675);
    d2x2 = floor(W - W * 0.12);
    d2y1 = floor(H * 0.625);
    d2y2 = floor(H - H * 0.185);


    if (singularity_type == 1) {
        arch_fact1 = (.8 + .4 * ahaq_rand());
        arch_fact2 = (.6 + .8 * ahaq_rand());
        k_arch = (1.2+ahaq_rand()*1.5);
    } else if (singularity_type == 2) {
        l[1].x = floor(crx1+ahaq_rand()*(crx2-crx1));
        l[1].y = floor(cry1+ahaq_rand()*(cry2-cry1));

        d[1].x = l[1].x;
        d[1].y = floor(d0y1+ahaq_rand()*(d0y2-d0y1));

    } else if (singularity_type == 3) {
        l[1].x = floor(crx1 + ahaq_rand() * (crx2 - crx1));
        l[1].y = floor(cry1 + ahaq_rand() * (cry2 - cry1));

        d[1].x = floor(d1x1 + ahaq_rand() * (d1x2 - d1x1));
        d[1].y = floor(d1y1 + ahaq_rand() * (d1y2 - d1y1));

    } else if (singularity_type == 4) {
        l[1].x = floor(crx1 + ahaq_rand() * (crx2 - crx1));
        l[1].y = floor(cry1 + ahaq_rand() * (cry2 - cry1));

        d[1].x = floor(d2x1 + ahaq_rand() * (d2x2 - d2x1));
        d[1].y = floor(d2y1 + ahaq_rand() * (d2y2 - d2y1));
    } else if (singularity_type == 5) {
        l[1].x = floor(cw1x1 + ahaq_rand() * (cw1x2 - cw1x1));
        l[1].y = floor(cw1y1 + ahaq_rand() * (cw1y2 - cw1y1));

        l[2].x = floor(cw2x1 + ahaq_rand() * (cw2x2 - cw2x1));
        l[2].y = floor(cw2y1 + ahaq_rand() * (cw2y2 - cw2y1));

        d[1].x = floor(dw1x1 + ahaq_rand() * (dw1x2 - dw1x1));
        d[1].y = floor(dw1y1 + ahaq_rand() * (dw1y2 - dw1y1));

        d[2].x = floor(dw2x1 + ahaq_rand() * (dw2x2 - dw2x1));
        d[2].y = floor(dw2y1 + ahaq_rand() * (dw2y2 - dw2y1));
    } else if (singularity_type == 6) {
        float tmp1, tmp2;
        l[1].x = floor(cwx1 + ahaq_rand() * (cwx2 - cwx1));
        l[2].x = l[1].x;

        tmp1 = floor(cwy1 + ahaq_rand() * (cwy2 - cwy1));
        tmp2 = floor(cwy1 + ahaq_rand() * (cwy2 - cwy1));
        if (tmp1 < tmp2) {
            l[1].y = tmp1;
            l[2].y = tmp2;
        } else {
            l[1].y = tmp2;
            l[2].y = tmp1;
        }
        
        d[1].x = floor(d1x1 + ahaq_rand() * (d1x2 - d1x1));
        
        d[2].x = floor(d2x1 + ahaq_rand() * (d2x2 - d2x1));
        
        tmp1 = floor(d1y1 + ahaq_rand() * (d1y2 - d1y1));
        tmp2 = floor(d2y1 + ahaq_rand() * (d2y2 - d2y1));

        if (tmp1 < tmp2) {
            d[1].y = tmp1;
            d[2].y = tmp2;
        } else {
            d[1].y = tmp2;
            d[2].y = tmp1;
        }
    }

    for (i = 0; i < H + margin; i++) {
        for (j = 0; j < W + margin; j++) {
            filt_map[i][j] = 0;
            f_print2_2Dmat[i][j] = 100;
            f_print1_2Dmat[i][j] = 0;//it was 100 for white image and black dot noise
        }
    }
}
