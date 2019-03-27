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

#include "n_noise.h"
#include "n_cv_type_converter.h"

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <time.h>

using namespace std;

using namespace noise;

Noise::Noise()
{
    int i;

    f_print1 = cvCreateImage(cvSize(W + margin, H + margin), IPL_DEPTH_32F, 1);
    f_print2 = cvCreateImage(cvSize(W + margin, H + margin), IPL_DEPTH_32F, 1);

    mask = cvCreateImage(cvSize(W + margin, H + margin), IPL_DEPTH_32F, 1);

    f_print1_2Dmat = (int**) malloc((H + margin) * sizeof(int*));
    for (i = 0; i < H + margin; i++)
            f_print1_2Dmat[i] = (int*) malloc((W + margin) * sizeof(int));

    f_print2_2Dmat = (int**) malloc((H + margin) * sizeof(int*));
    for (i = 0; i < H + margin; i++)
            f_print2_2Dmat[i] = (int*) malloc((W + margin) * sizeof(int));


    f_mask_2Dmat = (int**) malloc((H + margin) * sizeof(int*));
    for (i = 0; i < H + margin; i++)
            f_mask_2Dmat[i] = (int*) malloc((W + margin) * sizeof(int));

    transmin  = -3 * 3;
    transmax  =  3 * 3;
    rotmin    = (-pi/60) * 3;
    rotmax    =  (pi/60) * 3;
    max_noiseLevel = 0;
    min_noiseLevel = 0;
}

Noise::~Noise()
{
    int i;
    cvReleaseImage(&f_print1);
    cvReleaseImage(&f_print2);
    cvReleaseImage(&mask);


    for (i = 0; i < H + margin; i++)
            free(f_print1_2Dmat[i]);
    free(f_print1_2Dmat);


    for (i = 0; i < H + margin; i++)
            free(f_print2_2Dmat[i]);
    free(f_print2_2Dmat);


    for (i = 0; i < H + margin; i++)
            free(f_mask_2Dmat[i]);
    free(f_mask_2Dmat);

    //cout << "Noise Distructor" << endl;
}

/*
  Masks fingerprint for impression. Then adds random noise
  to masked fingerprint.
  */
void Noise::random_noise(IplImage *f_print) {
    int r, c;
    int i;

    for (r = 0; r < mask->height; r++) {
        for (c = 0; c < mask->width; c++) {
            cvSetReal2D(mask, r, c, 0);
        }
    }

    int mask_boundry[10000][2];
    int mask_perimeter;

    float a1, a2, b1, b2, c1, sx, sy;
    a1 = W * .45 + ahaq_rand() * W * .15;
    a2 = (W + padding - a1);

    /* Original parameters; produce somewhat small impressions...
    //    b1 = H * .30 + ahaq_rand() * H * .05;
    //    b2 = b1 + b1 * .25 + b1 * ahaq_rand() * .45;
    */
    b1 = H * .40 + ahaq_rand() * H * .15;
    b2 = b1 + b1 * .25 + b1 * ahaq_rand() * .25;
    c1 = (H + padding - (b1 + b2));
    sx = 0;
    sy = 0;
    sx = (W / 2.0);
    sy = (H / 2.0);
    sx = 0;
    sy = 0;

    sx = (W / 2 - a2 + (a1 + a2) / 2) + padding;
    sy = ((b2 + c1 / 2) + (H - (b1 + b2 + c1)) / 2) + padding;

    /* ellipse(Mat& img, Point center, Size axes, double angle,
     *  double startAngle, double endAngle, const Scalar& color,
     *  int thickness=1, int lineType=8, int shift=0)
     */
    cvEllipse(mask, cvPoint(sx, sy - c1 / 2), cvSize(a1, b2), 0, 90, 180,
            cvScalar(255, 255, 255), 1, 8, 0);
    cvEllipse(mask, cvPoint(sx, sy - c1 / 2), cvSize(a2, b2), 0, 0, 90,
            cvScalar(255, 255, 255), 1, 8, 0);
    cvEllipse(mask, cvPoint(sx, sy + c1 / 2), cvSize(a1, b1), 0, 180, 270,
            cvScalar(255, 255, 255), 1, 8, 0);
    cvEllipse(mask, cvPoint(sx, sy + c1 / 2), cvSize(a2, b1), 0, 270, 360,
            cvScalar(255, 255, 255), 1, 8, 0);

    cvLine(mask, cvPoint(sx - a1, sy - c1 / 2), cvPoint(sx - a1, sy + c1 / 2),
            cvScalar(255, 255, 255), 1, CV_AA);
    cvLine(mask, cvPoint(sx + a2, sy - c1 / 2), cvPoint(sx + a2, sy + c1 / 2),
            cvScalar(255, 255, 255), 1, CV_AA);

    float tmp;
    mask_perimeter = 0;

    for (r = 0; r < mask->height; r++) {
        for (c = 0; c < mask->width; c++) {
            tmp = cvGetReal2D(mask, r, c);
            if (tmp == 255) {
                mask_boundry[mask_perimeter][0] = c;
                mask_boundry[mask_perimeter][1] = r;
                mask_perimeter++;
            }
        }
    }

    // Other Variables
    CvPoint seed_point = cvPoint(W / 2 + padding, H / 2 + padding);
    CvScalar color = CV_RGB(255,255,255);
    cvFloodFill(mask, seed_point, color, cvScalarAll(5.0), cvScalarAll(5.0),
            NULL, 4, NULL );

    normalize32Ipl(mask);

    Ipltomat2D(mask, f_mask_2Dmat);

    for (r = 0; r < mask->height; r++) {
        for (c = 0; c < mask->width; c++) {
            tmp = cvGetReal2D(mask, r, c);
            ;
            cvSetReal2D(mask, r, c, tmp * 255);
        }
    }

    //noise generation.....................

    int n_noise = min_noiseLevel * 4000 + (min_noiseLevel - max_noiseLevel + 2) * floor(ahaq_rand() * 4000);
    int x, y, k;
    int ind;

    IplImage* f_noise = cvCreateImage(cvSize(W + margin, H + margin),
            IPL_DEPTH_32F, 1);

    //cout << min_noiseLevel << " " << max_noiseLevel << " " << n_noise << endl;

    for (r = 0; r < f_noise->height; r++) {
        for (c = 0; c < f_noise->width; c++) {
            cvSetReal2D(f_noise, r, c, 0);
        }
    }

    double distance_max = sqrt((W / 2.0) * (W / 2.0) + (H / 2.0) * (H / 2.0));

    int Narea = 100;

    int max_noise_blob[100];

    for (i = 0; i < Narea; i++) {
        max_noise_blob[i] = 4 * 0.0003 * ((i + 1) * (distance_max / Narea))
                * ((i + 1) * (distance_max / Narea)) * sqrt((i + 1)
                * (distance_max / Narea));
    }

    int cur_noise_blob[100];
    for (i = 0; i < Narea; i++)
        cur_noise_blob[i] = 0;

    for (i = 0; i < n_noise; i++) {
        x = floor(ahaq_rand() * (W));
        y = floor(ahaq_rand() * (H));
        k = floor(1.0 + (int) (ahaq_rand() * 100) % 2);

        double distance = sqrt((W / 2.0 - x) * (W / 2 - x) + (H / 2 - y) * (H / 2
                - y));
        int area = floor(distance / (distance_max / Narea));

        //if (cur_noise_blob[area] < max_noise_blob[area]) {
            paste_noise_blob(x + padding, y + padding, k, f_print);
            paste_noise_blob(x + padding, y + padding, k, f_noise);
            cur_noise_blob[area] = cur_noise_blob[area] + 1;
        //}
    }

    cvReleaseImage(&f_noise);

#ifdef FINE_EDGE
    //for outside of the boundry
    int w = 6;

    n_noise = 1000 + floor(ahaq_rand() * 20);

    for (i = 0; i < n_noise; i++) {
        ind = int(ahaq_rand() * mask_perimeter);

        x = mask_boundry[ind][0];

        if (x < (W + margin) / 2)
            x = mask_boundry[ind][0] - int(ahaq_rand() * w);
        else
            x = mask_boundry[ind][0] + int(ahaq_rand() * w);

        y = mask_boundry[ind][1];

        if (y < (H + margin) / 2)
            y = mask_boundry[ind][1] - int(ahaq_rand() * w);
        else
            y = mask_boundry[ind][1] + int(ahaq_rand() * w);

        k = floor(1.0 + (int) (ahaq_rand() * 100) % 4);

        paste_noise_blob(x, y, k, f_print);
        paste_noise_blob(x, y, k, mask);

    }
    //on the boundry
    w = 4;

    n_noise = 1000 + floor(ahaq_rand() * 20);

    for (i = 0; i < n_noise; i++) {
        ind = int(ahaq_rand() * mask_perimeter);
        x = mask_boundry[ind][0];
        x = mask_boundry[ind][0] - w + int(2 * ahaq_rand() * w);

        y = mask_boundry[ind][1];
        y = mask_boundry[ind][1] - w + int(2 * ahaq_rand() * w);
        k = floor(1.0 + (int) (ahaq_rand() * 100) % 4);
        paste_noise_blob(x, y, k, f_print);
        paste_noise_blob(x, y, k, mask);
    }

    //for inside of the boundry
    //very dense
    w = 8;
    n_noise = 600 + floor(ahaq_rand() * 20);

    for (i = 0; i < n_noise; i++) {
        ind = int(ahaq_rand() * mask_perimeter);

        x = mask_boundry[ind][0];

        if (x < (W + margin) / 2)
            x = mask_boundry[ind][0] + int(ahaq_rand() * w);
        else
            x = mask_boundry[ind][0] - int(ahaq_rand() * w);

        y = mask_boundry[ind][1];
        if (y < (H + margin) / 2)
            y = mask_boundry[ind][1] + int(ahaq_rand() * w);
        else
            y = mask_boundry[ind][1] - int(ahaq_rand() * w);

        k = floor(1.0 + (int) (ahaq_rand() * 100) % 7);

        paste_noise_blob(x, y, k, f_print);
        paste_noise_blob(x, y, k, mask);
    }

    //lesser dense
    w = 12;
    n_noise = 1200 + floor(ahaq_rand() * 20);
    for (i = 0; i < n_noise; i++) {
        ind = int(ahaq_rand() * mask_perimeter);

        x = mask_boundry[ind][0];
        if (x < (W + margin) / 2)
            x = mask_boundry[ind][0] + int(ahaq_rand() * w);
        else
            x = mask_boundry[ind][0] - int(ahaq_rand() * w);

        y = mask_boundry[ind][1];
        if (y < (H + margin) / 2)
            y = mask_boundry[ind][1] + int(ahaq_rand() * w);
        else
            y = mask_boundry[ind][1] - int(ahaq_rand() * w);

        k = floor(1.0 + (int) (ahaq_rand() * 100) % 5);

        paste_noise_blob(x, y, k, f_print);
        paste_noise_blob(x, y, k, mask);
    }

    //least dense
    w = 18;
    n_noise = 800 + floor(ahaq_rand() * 20);
    for (i = 0; i < n_noise; i++) {
        ind = int(ahaq_rand() * mask_perimeter);

        x = mask_boundry[ind][0];
        if (x < (W + margin) / 2)
            x = mask_boundry[ind][0] + int(ahaq_rand() * w);
        else
            x = mask_boundry[ind][0] - int(ahaq_rand() * w);

        y = mask_boundry[ind][1];
        if (y < (H + margin) / 2)
            y = mask_boundry[ind][1] + int(ahaq_rand() * w);
        else
            y = mask_boundry[ind][1] - int(ahaq_rand() * w);

        k = floor(1.0 + (int) (ahaq_rand() * 100) % 4);

        paste_noise_blob(x, y, k, f_print);
        paste_noise_blob(x, y, k, mask);
    }

#endif

    /////masking........................

    for (r = 0; r < f_print->height; r++) {
        for (c = 0; c < f_print->width; c++) {
            tmp = f_mask_2Dmat[r][c];
            if (tmp == 1 && f_print1_2Dmat[r][c] == 0)
                cvSetReal2D(f_print, r, c, cvGetReal2D(f_print, r, c));
            else
                cvSetReal2D(f_print, r, c, 255);
        }
    }
    //masking..........................
    cvSmooth(f_print, f_print, CV_GAUSSIAN, 3, 3);

    for (r = 0; r < f_print->height; r++) {
        for (c = 0; c < f_print->width; c++) {

            if (f_print2_2Dmat[r][c] == 255)
                cvSetReal2D(f_print, r, c, 255);
        }
    }
}

/*
  Adds noise to fingerprint using random noise blob.
  */
void Noise::paste_noise_blob(int i, int j, int k, IplImage *f_print) {
    int r, c;
    float tmp;

    int shift = floor(((noise_blob[k])->height) / 2.0);
    for (r = 0; r < (noise_blob[k])->height; r++) {
        for (c = 0; c < (noise_blob[k])->width; c++) {
            tmp = cvGetReal2D(noise_blob[k], r, c);

            if (tmp == 0) {
                if (r + j - shift < H + margin && c + i - shift < W + margin
                        && r + j - shift >= 0 && c + i - shift >= 0) {
                    cvSetReal2D(f_print, r + j - shift, c + i - shift, 255);
                }
            }
        }
    }
}

/*
  Adds fixed noise and is called before masking the fingerprint.
  */
void Noise::fixed_noise(IplImage *f_print) {
    int r, c;
    int i;

    int x, y, k;

    for (i = 0; i < 5000; i++)

    {
        x = floor(ahaq_rand() * (W));
        y = floor(ahaq_rand() * (H));
        k = floor(1.0 + (int) (ahaq_rand() * 100) % 2);

        paste_noise_blob(x + padding, y + padding, k, f_print);
    }
    for (r = 0; r < f_print->height; r++) {
        for (c = 0; c < f_print->width; c++) {
            if (f_print1_2Dmat[r][c] != 0)
                cvSetReal2D(f_print, r, c, 255);
        }
    }
}

/*
  Adds random distortion to masked fingerprint depending on the
  limits on rotation and translation.
  */
void Noise::distortion(const char *filename)
{
    int i, j;


        /*float transmin  = -3 * 3;
        float transmax  =  3 * 3;
        float rotmin    = (-pi/60) * 3;
        float rotmax    =  (pi/60) * 3;*/

        //IplImage* image = cvLoadImage(file_name, CV_LOAD_IMAGE_GRAYSCALE);
        Ipltomat2D(f_print1, f_print1_2Dmat);
        int width = f_print1->width;
        int height = f_print1->height;

        //int **f_mask_2Dmat;
        //f_mask_2Dmat = new int*[height];

        int new_dim = max(width, height);

        int aa = floor(new_dim / 2 - height / 2.0) + 1;
        //int bb = floor(new_dim / 2 + height / 2.0);
        int cc = floor(new_dim / 2 - width / 2.0) + 1;
        //int dd = floor(new_dim / 2 + width / 2.0);
        double shapedist[401][401];
        int **kk;

        kk = new int*[new_dim];

        float** result_image;
        result_image  = new float*[new_dim];

        for (i = 0; i < new_dim; i++)
        {
            kk[i] = new int[new_dim];
            result_image[i]  = new float[new_dim ];

            //if(i < height)
              //      f_mask_2Dmat[i] = new int[width];

            for(j = 0; j < new_dim; j++)
            {
                kk[i][j] = 255;
                result_image[i][j] = 255;

                shapedist[i][j] = -2;
            }
        }

        //Ipltomat2D(image, f_mask_2Dmat);

        for( i = 0; i < height; i++)
        {
            for(j = 0; j < width; j++)
            {
               kk[aa + i - 1][cc + j - 1] = f_print1_2Dmat[i][j];

            }
            //delete []f_mask_2Dmat[i];
        }

        //delete []f_mask_2Dmat;


        int centerX = floor(new_dim/2.0);
        int centerY = floor(new_dim/2.0);

        int a1 = floor(100/3.0);
        int a2 = a1;
        int b1 = floor(122/3.0);
        int b2 = b1;
        int c = 0;
        int h = 0;
        int k = 0;

        float y1[201], y2[201], y3[201], y4[201];

        for(i = 1; i <= 200; i++)
        {
            double temp = 1 - ((double)(i * i) / (a1 * a1));
            temp = temp > 0 ? temp : 0;

            y1[i] = c / 2 + b1 * sqrt(temp);
            y3[i] = -c / 2 - b2 * sqrt(temp);

            j = i - 200;
            temp = 1 - ((double)(j * j) / (a2 * a2));
            temp = temp > 0 ? temp : 0;

            y2[i] = c / 2 + b1 * sqrt(temp);
            y4[i] = -c / 2 - b2 * sqrt(temp);

            y1[i] = y1[i] + 200 + k;
            y2[i] = y2[i] + 200 + k;
            y3[i] = y3[i] + 200 + k;
            y4[i] = y4[i] + 200 + k;
        }

        int x3 = -a2;
        int x4 = a1;
        int y5 = 0;

        x3 = x3 + 200 + h;
        x4 = x4 + 200 + h;
        y5 = y5 + 200 + k;


        int l;
        for( j = 200 - a2; j <= 200; j++)
        {
            //cout << floor(y2[j]) << " " << floor(y4[j]) << endl;
            for (l = floor(y2[j]); l >= floor(y4[j]); l--)
            {
                shapedist[400 - l][j + h] = 0;
            }
        }


        for(j = 1; j <= a1; j++)
        {
            //cout << floor(y1[j]) << " " << floor(y3[j]) << endl;
            for (l = floor(y1[j]); l >= floor(y3[j]); l--)
            {
                shapedist[400 - l][j + 200 + h] = 0;
            }
        }

        double thetha = ahaq_rand();
        thetha = rotmin + (rotmax - rotmin) * thetha;

        int parak = 2;
        double distortionX;
        double distortionY;
        double transX;
        double transY;

        transX = (floor(transmin + (transmax- transmin) * ahaq_rand()));
        transY = (floor(transmin + (transmax- transmin) * ahaq_rand()));

        for(j = 1; j <= new_dim; j++)
        {
            for(k = 1; k <= new_dim; k++)
            {
                double temp1;
                double temp2;
                double temp3;
                double temp4;

                if(shapedist[j][k] == -2)
                {
                    temp1 = j - centerX;
                    temp3 = temp1 / (a1 * a1);
                    temp2 = k - centerY;
                    temp4 = temp2 / (b1 * b1);

                    temp1 = temp1 * temp3 + temp2 * temp4;
                    temp3 = sqrt(temp1) - 1;
                }
                else
                    temp3 = -2;

                temp1 = (double) (j - centerX) * cos(thetha) + (k - centerY) * sin(thetha) + centerX;
                temp1 += transX -j;
                //temp1 += (floor(transmin + (transmax- transmin) * 1) - j);
                temp2 = (double) -(j - centerX) * sin(thetha) + (k - centerY) * cos(thetha) + centerY;
                temp2 += transY - k;
                //temp2 += (floor(transmin + (transmax- transmin) * 1) - k);



                if(temp3 <= 0)
                    temp3 = 0;
                else if (temp3 > 0 && temp3 <= parak)
                    temp3 = ((double)1 / 2) * (1 - cos((temp3 * pi) / parak));
                else
                    temp3 = 1;

                distortionX = j + temp1 * temp3;
                distortionY = k + temp2 * temp3;


                if(!(distortionX <= 1 || distortionY <= 1
                    || distortionX >= 400 || distortionY >= 400 ))
                {
                    int ind1 = distortionX > new_dim ? new_dim : distortionX;
                    int ind2 = distortionY > new_dim ? new_dim : distortionY;

                    result_image[ind1][ind2] = kk[j - 1][k - 1];
                }
            }
        }

        IplImage *img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

        for( i = 0; i < height; i++)
        {
            for(j = 0; j < width; j++)
            {
               f_print1_2Dmat[i][j] = result_image[aa + i - 1][cc + j - 1];

            }
            //delete []f_mask_2Dmat[i];
        }

        mat2DtoIpl(f_print1_2Dmat, img);

        //CvMat* mat = cvCreateMat(400, 400, CV_32FC1 );

        //t2Dtocvmat(result_image, mat);
        //cv32fc1_to_ipl8uc1(mat, img);

        cvSaveImage(filename, img);

        cvReleaseImage(&img);

        try {
        for(j = 0; j < new_dim; j++)
        {
            delete[] kk[j];
            delete[] result_image[j];
        }
        delete[] kk;
        delete[] result_image;
        }
        catch(exception ae)
        {
            cout << "Exception";
        }
   // }
}
