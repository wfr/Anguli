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

#include "noise.h"
#include "cv_type_converter.h"
#include <string>
#include <iostream>
using namespace std;

using namespace anguli;
Noise::Noise()
{
    int i;
    f_print1 = cvCreateImage(cvSize(W + margin, H + margin), IPL_DEPTH_32F, 1);
    f_print2 = cvCreateImage(cvSize(W + margin, H + margin), IPL_DEPTH_32F, 1);

    f_print1_2Dmat = new int *[H + margin];
    for (i = 0; i < H + margin; i++)
            f_print1_2Dmat[i] = new int[W + margin];

    f_print2_2Dmat = new int *[H + margin];
    for (i = 0; i < H + margin; i++)
            f_print2_2Dmat[i] = new int[W + margin];

}

Noise::~Noise()
{
    int i;
    cvReleaseImage(&f_print1);
    cvReleaseImage(&f_print2);

    for (i = 0; i < H + margin; i++)
            delete[] f_print1_2Dmat[i];
    delete[] f_print1_2Dmat;

    for (i = 0; i < H + margin; i++)
            delete[] f_print2_2Dmat[i];
    delete[] f_print2_2Dmat;


    //printf("distructor Noise \n");
}

#ifdef WIN32
/*
  rand_r function for windows.
  */
int Noise::rand_r (unsigned int *seed)
{
    unsigned int next = *seed;
    int result;

    next *= 1103515245;
    next += 12345;
    result = (unsigned int) (next / 65536) % 2048;

    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (unsigned int) (next / 65536) % 1024;

    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (unsigned int) (next / 65536) % 1024;

    *seed = next;

    return result;
}
#endif

/*
  Generates random number from 0-1.
  */
float Noise::ahaq_rand()
{
    return((float)(rand_r(&rand_seed) % 1000)/999.0);
}
