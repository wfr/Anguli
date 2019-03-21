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

#ifndef _N_GLOBAL_H
#define _N_GLOBAL_H

#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
namespace noise
{
#define WRITE
#define GEN
#define ADD_NOISE
#define ADD_MASK
#define ADD_BLUR
#define FINE_EDGE
#define WRITE_REF

#ifdef MAIN
#define EXTERN

bool noise_blobs_loaded = false;
float ahaq_rand()
{
	return(rand()%1000/999.0);
}
#else
#define EXTERN extern
extern float ahaq_rand();
extern bool noise_blobs_loaded;
#endif

//integer variables

#ifdef MAIN
int k = 16;
int k_fact = k;
int block_size = k;

int margin = 30;
int padding = 15;

int W = 275 - margin;
int H = 400 - margin;
#else
extern int k;
extern int k_fact;
extern int block_size;
extern int W;
extern int H;
extern int margin;
extern int padding;
#endif

#ifdef MAIN
float pi=M_PI;
float i_pi=1/pi;
float i_180=1/180.0;
float rad_deg_fact=180.0/M_PI;
float deg_rad_fact=M_PI/180.0;
#else
extern float pi;
extern float i_pi;
extern float i_180;
extern float rad_deg_fact;
extern float deg_rad_fact;
#endif
EXTERN IplImage* noise_blob[11];
}
#endif
