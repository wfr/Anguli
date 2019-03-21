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

#ifndef _FILTER_IMAGE_H
#define _FILTER_IMAGE_H

#include "global.h"
#include "orientmap.h"

namespace anguli
{
    class Filter_Image : public OrientMap
    {
    public:
        Filter_Image();
        ~Filter_Image();
        void pre_filtering(void);
        void filter_image(void);
        void filter_image_firstpass(void);
        void filter_image_withmap(void);
        void set_filter_area(void);
        float get_filtered_val(float **src1, float **src2, int size);
    public:
        int **orient_ind_2Dmat;
        int **freq_ind_2Dmat;
        float **filt_map;
        int mul_mat[101][101];
        CvMat* mat;
        IplImage* filter;
    };
}
#endif
