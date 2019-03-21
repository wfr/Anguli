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

#ifndef _DENSITYMAP_H
#define _DENSITYMAP_H

#include "global.h"
#include "noise.h"

namespace anguli
{
    class DensityMaps : public Noise
    {
    public:
        DensityMaps();
        ~DensityMaps();
        void sel_n_merg_densitymap(void);
        void random_densitymap(void);
        void densitymap();
    public:
        int **f_den_int_2Dmat;
        float **f_den_2Dmat;
        CvPoint2D32f l[3];
        CvPoint2D32f d[3];
        int n_fp, n_fo;
        //char filename[150];
    };
}
#endif
