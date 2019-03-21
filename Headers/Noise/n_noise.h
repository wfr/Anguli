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

#ifndef _N_NOISE_H
#define _N_NOISE_H

#include "global.h"
namespace noise
{
    class Noise
    {
    public:
        Noise();
        ~Noise();
        void paste_noise_blob(int i, int j, int k, IplImage *f_print);
        void random_noise(IplImage *f_print);
        void fixed_noise(IplImage *f_print);
        void distortion(const char *filename);
    public:
        int **f_print1_2Dmat;
        int **f_print2_2Dmat;
        int **f_mask_2Dmat;
        IplImage* f_print1;
        IplImage* f_print2;
        IplImage* mask;
        float transmin;
        float transmax;
        float rotmin;
        float rotmax;
        int min_noiseLevel;
        int max_noiseLevel;
    };
}
#endif
