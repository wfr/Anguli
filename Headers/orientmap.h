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

#ifndef _ORIENTMAP_H
#define _ORIENTMAP_H
#include "densitymap.h"
#include "noise.h"
#include "g_cap.h"

namespace anguli
{
    class OrientMap : public DensityMaps, public G_Cap
    {
    public:
        OrientMap();
        ~OrientMap();
        float get_orient(int i, int j, int singularity_type);
        void orientmap(void);
        void draw_orient(float **orient_padded_few);

    public:
        double arch_fact1;
        double arch_fact2;
        float **orient;
        int singularity_type;
    };
}
#endif
