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

#ifndef _N_CV_TYPE_CONVERTER_H
#define _N_CV_TYPE_CONVERTER_H

#include "global.h"

namespace noise
{

void ipl8uc3_to_cv32fc1(IplImage *src, int chn, CvMat *dst);
void ipl8uc1_to_cv32fc1(IplImage *src, CvMat *dst);
void ipl8uc3_to_cv8uc1(IplImage *src, int chn, CvMat *dst);
void ipl8uc1_to_cv8uc1(IplImage *src, CvMat *dst);
void cv32fc1_to_cv8uc1(CvMat *src, CvMat *dst);
void cv8uc1_to_cv32fc1(CvMat *src, CvMat *dst);
unsigned char min8(CvMat *img);
float min32(CvMat *img);
unsigned char max8(CvMat *img);
float max32(CvMat *img);
void add32(CvMat *img, float val);
void subtract32(CvMat *img, float val);
void divide32(CvMat *img, float val);
void normalize32(CvMat *img);
void normalize32contrast(CvMat *img);
float max32Ipl(IplImage *img);
float min32Ipl(IplImage *img);
void add32Ipl(IplImage *img, float val);
void subtract32Ipl(IplImage *img, float val);
void divide32Ipl(IplImage *img, float val);
void normalize32Ipl(IplImage *img);
void binarize32Ipl(IplImage *img, float val);
void copy2Df_print(int **f_print1, int **f_print2);
int max2Df_print(int **f_print);
int min2Df_print(int **f_print);
void add2Df_print(int **f_print, int val);
void subtract2Df_print(int **f_print, int val);
void divide2Df_print(int **f_print, float val);
void normalize2Df_print(int **f_print);
void binarize2Df_print(int** f_print, int val);
void invert32Ipl(IplImage *img);
void invert2Df_print(int** f_print);
void Ipltomat2D(IplImage *img, int **dst);
void Ipltomat2D_float(IplImage *img, float **dst);
void Ipltomat2D_bin(IplImage *img, float **dst);
void mat2DtoIpl(float **dst, IplImage *img);
void mat2DtoIpl(int **dst, IplImage *img);
void cvmattomat2D(CvMat *img, int **dst);
void mat2Dtocvmat(float **dst, CvMat *img);
void multiply32(CvMat *img, float val);
void cv32fc1_to_ipl8uc1(CvMat *src, IplImage *dst);
void save_normalize_32fc1(CvMat* m, char *filename);
void save_32fc1(CvMat *m, char *filename);
void save_8uc1(CvMat *m, char *filename);
void muls(CvMat* src, double s, CvMat* dst);
void divs(CvMat* src, double s, CvMat* dst);
void picturize(float **f_print);
float max_float(float x, float y);
}
#endif
