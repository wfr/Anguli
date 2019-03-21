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

#include "densitymap.h"
#include "global.h"
#include "cv_type_converter.h"

using namespace anguli;
/* Allocate memories for required Objects. */
DensityMaps::DensityMaps()
{
    int i;
    
    f_den_2Dmat = new float *[H + margin];
    for (i = 0; i < H + margin; i++)
            f_den_2Dmat[i] = new float [W + margin];
            
    f_den_int_2Dmat = new int *[H + margin];
    for (i = 0; i < H + margin; i++)
            f_den_int_2Dmat[i] = new int [W + margin];
    
}

/* Delete allocated memory reverse of constructor. */
DensityMaps::~DensityMaps()
{
    int i;
    
    for (i = 0; i < H + margin; i++)
            delete[] f_den_2Dmat[i];
    delete[] f_den_2Dmat;
    
    for (i = 0; i < H + margin; i++)
            delete[] f_den_int_2Dmat[i];
    delete[] f_den_int_2Dmat;
}

/*
  Selects the 2/3 density maps and merges them to create a new density Maps.
  */
void DensityMaps::sel_n_merg_densitymap(void) {
    int r, c, flag_3 = 0;
    float tmp;
    std::stringstream str;
    str << path_density_maps << "/" << int(1 + ahaq_rand() * (density_map_count - 1)) << ".jpeg";

    IplImage *freq1 = cvLoadImage(str.str().c_str(), 0);

    str.str("");
    str << path_density_maps << "/" << int(1 + ahaq_rand() * (density_map_count - 1)) << ".jpeg";
    IplImage *freq2 = cvLoadImage(str.str().c_str(), 0);

    IplImage *freq3;

    if (ahaq_rand() > .5) {
        flag_3 = 1;
        str.str("");
        str << path_density_maps << "/" << int(1 + ahaq_rand() * (density_map_count - 1)) << ".jpeg";

        freq3 = cvLoadImage(str.str().c_str(), 0);
    }

    IplImage *freq = cvCreateImage(cvSize(freq1->width, freq1->height), IPL_DEPTH_32F, 1);

    if (flag_3 == 0) {
        for (r = 0; r < freq1->height; r++) {
            for (c = 0; c < freq1->width; c++) {
                tmp = (cvGetReal2D(freq1, r, c) + cvGetReal2D(freq2, r, c)) / 2;
                cvSetReal2D(freq, r, c, tmp / 255);
            }
        }
    }

    else {
        for (r = 0; r < freq1->height; r++) {
            for (c = 0; c < freq1->width; c++) {
                tmp = (cvGetReal2D(freq1, r, c) + cvGetReal2D(freq2, r, c)
                                                               + cvGetReal2D(freq3, r, c)) / 3;
                cvSetReal2D(freq, r, c, tmp / 255);
            }
        }
    }

    IplImage* f_den = cvCreateImage(cvSize(W + margin, H + margin),
                                                                            IPL_DEPTH_32F, 1);
    cvResize(freq, f_den);

    cvReleaseImage(&freq);
    cvReleaseImage(&freq1);
    cvReleaseImage(&freq2);
    if (flag_3 == 1)
        cvReleaseImage(&freq3);

    Ipltomat2D_float(f_den, f_den_2Dmat);
    int i, j, max, min;
    max = 99999;
    min= 0;
    for (i = 0; i < H + margin; i++) {
        for (j = 0; j < W + margin; j++) {
            f_den_int_2Dmat[i][j] = int((f_den_2Dmat[i][j]) * 100);

            if (max < f_den_int_2Dmat[i][j])
                max = f_den_int_2Dmat[i][j];

            if (min > f_den_int_2Dmat[i][j])
                min = f_den_int_2Dmat[i][j];
        }
    }

    for (i = 0; i < H + margin; i++) {
        for (j = 0; j < W + margin; j++) {
            f_den_int_2Dmat[i][j] = f_den_int_2Dmat[i][j] - min;
        }
    }

    for (i = 0; i < H + margin; i++) {
        for (j = 0; j < W + margin; j++) {
            f_den_int_2Dmat[i][j] = (f_den_int_2Dmat[i][j] / max) * 99;
        }
    }

    for (r = 0; r < f_den->height; r++) {
        for (c = 0; c < f_den->width; c++) {
            tmp = cvGetReal2D(f_den, r, c);
            cvSetReal2D(f_den, r, c, tmp * 255);
        }
    }

    cvReleaseImage(&f_den);
}

/*
  Intializes the parameters to create density maps.
  */
void DensityMaps::densitymap(void) {
    int i, j;
    int den_1, den_2, w1, w2, cy1, cy2, dy1, dy2, cx1, cx2, dx1, dx2;
    float k1, k2;
    CvScalar pix_val;

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

    int k_blur = 32;
    IplImage* f_den = cvCreateImage(cvSize(W + margin, H + margin), IPL_DEPTH_32F, 1);

    int iW, iH;
    
    //initial background
    for (iH = 0; iH < H + margin; iH = iH + k_blur) {
        for (iW = 0; iW < W + margin; iW = iW + k_blur) {
            pix_val.val[0] = 0.2 + ahaq_rand() * .05;
            for (i = iH; i < iH + k_blur; i++) {
                for (j = iW; j < iW + k_blur; j++) {
                    if (i > 0 && j > 0 && i < H + margin && j < W + margin)
                        cvSet2D(f_den, i, j, pix_val);
                }
            }
        }
    }

    IplImage* dst = cvCreateImage(cvSize((W + margin) / 5, (H + margin) / 5),
                                                                                        IPL_DEPTH_32F, 1);
    cvResize(f_den, dst);
    cvSmooth(dst, dst, CV_GAUSSIAN, 31, 31);

    cvResize(dst, f_den);

    cvReleaseImage(&dst);
}

/* Generates the random density Maps. */
void DensityMaps::random_densitymap(void) {
    int i, j;
    int den_1, den_2, w1, w2, cy1, cy2, dy1, dy2, cx1, cx2, dx1, dx2;
    int cry1, cry2, dly1, dly2;
    float k1, k2;
    CvScalar pix_val;

    cry1 = floor(H * 0.2);
    cry2 = floor(H * 0.25);

    dly1 = floor(H * 0.75);
    dly2 = floor(H * 0.80);

    den_1 = floor(cry1 + ahaq_rand() * (cry2 - cry1));
    den_2 = floor(dly1 + ahaq_rand() * (dly2 - dly1));

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
    
    int k_blur = 32;
    IplImage* f_den = cvCreateImage(cvSize(W + margin, H + margin),
                                                                            IPL_DEPTH_32F, 1);

    int iW, iH;
    
    // Initial background
    for (iH = 0; iH < H + margin; iH = iH + k_blur) {
        for (iW = 0; iW < W + margin; iW = iW + k_blur) {
            pix_val.val[0] = 0.2 + ahaq_rand() * .05;
            for (i = iH; i < iH + k_blur; i++) {
                for (j = iW; j < iW + k_blur; j++) {
                    if (i > 0 && j > 0 && i < H + margin && j < W + margin)
                        cvSet2D(f_den, i, j, pix_val);
                }
            }
        }            
    }

    int el_count, fil_count, centx, centy, e, p;
    float A, B, col, theta;

    // Initial dark patches
    // Final few less dark.
    el_count = 2 + ahaq_rand() * 2;
    
    for (e = 1; e < el_count; e++) {
        A = W / 16 + ahaq_rand() * (W / 8);
        B = H / 16 + ahaq_rand() * (H / 8);
        centx = ahaq_rand() * W;
        centy = ahaq_rand() * H;
        col = .1 + ahaq_rand() * .05;

        fil_count = 100 + ahaq_rand() * 100;
        for (p = 1; p < fil_count; p++) {
            theta = ahaq_rand() * 2 * M_PI;
            iW = floor(ahaq_rand() * A * cos(theta) + centx + padding);
            iH = floor(ahaq_rand() * B * sin(theta) + centy + padding);

            if (iW < W + margin && iW > 0 && iH < H + margin && iH > 0) {
                pix_val.val[0] = col + ahaq_rand() * .02;
                for (i = iH; i < iH + k_blur; i++) {
                    for (j = iW; j < iW + k_blur; j++) {
                        if (i > 0 && j > 0 && i < H + margin && j < W + margin)
                            cvSet2D(f_den, i, j, pix_val);
                    }
                }
            }
        }
    }

    // Singularity
    A = (cx2 - cx1) / 2;
    B = (cy2 - cy1) / 2;
    centx = (cx2 + cx1) / 2;
    centy = (cy2 + cy1) / 2;
    col = .1 + ahaq_rand() * .05;

    fil_count = 200 + ahaq_rand() * 100;
    for (p = 1; p < fil_count; p++) {
        theta = ahaq_rand() * 2 * M_PI;
        iW = floor(ahaq_rand() * A * cos(theta) + centx + padding);
        iH = floor(ahaq_rand() * B * sin(theta) + centy + padding);
        //printf("iH=%d,iW=%d",iH,iW);getchar();
        if (iW < W + margin && iW > 0 && iH < H + margin && iH > 0) {
            pix_val.val[0] = .75 + ahaq_rand() * .05;
            for (i = iH; i < iH + k_blur; i++) {
                for (j = iW; j < iW + k_blur; j++) {
                    if (i > 0 && j > 0 && i < H + margin && j < W + margin)
                        cvSet2D(f_den, i, j, pix_val);
                }
            }
        }
    }

    A = (dx2 - dx1) / 2;
    B = (dy2 - dy1) / 2;
    centx = (dx2 + dx1) / 2;
    centy = (dy2 + dy1) / 2;
    col = .1 + ahaq_rand() * .05;

    fil_count = 200 + ahaq_rand() * 100;
    for (p = 1; p < fil_count; p++) {
        theta = ahaq_rand() * 2 * M_PI;
        iW = floor(ahaq_rand() * A * cos(theta) + centx + padding);
        iH = floor(ahaq_rand() * B * sin(theta) + centy + padding);

        if (iW < W + margin && iW > 0 && iH < H + margin && iH > 0) {
            pix_val.val[0] = .75 + ahaq_rand() * .05;
            for (i = iH; i < iH + k_blur; i++) {
                for (j = iW; j < iW + k_blur; j++) {
                    if (i > 0 && j > 0 && i < H + margin && j < W + margin)
                        cvSet2D(f_den, i, j, pix_val);
                }
            }
        }
    }

    IplImage* dst = cvCreateImage(cvSize(floor((W + margin) / 15.0), floor((H
                                                            + margin) / 15.0)), IPL_DEPTH_32F, 1);
    cvResize(f_den, dst);
    cvSmooth(dst, dst, CV_GAUSSIAN, 9, 9);
    
    cvResize(dst, f_den);

    int r, c;
    for (r = 0; r < dst->height; r++) {
        for (c = 0; c < dst->width; c++) {
            cvSetReal2D(dst, r, c, cvGetReal2D(dst, r, c));
        }
    }

    std::stringstream filename;
    filename << path_density_maps << "/" << n_fp;
    //sprintf(filename, "%s/%d.jpeg", path_density_maps, n_fp);
    cvSaveImage(filename.str().c_str(), dst);
    cvReleaseImage(&f_den);
    cvReleaseImage(&dst);
}
    
