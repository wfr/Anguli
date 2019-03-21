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

#include "orientmap.h"
#include "global.h"
#include "cv_type_converter.h"
#include "ahaque_complex.h"

using namespace anguli;

/*
  Constructor
  */
OrientMap::OrientMap()
{
    int i;
    orient = new float *[H + margin];
    for (i = 0; i < H + margin; i++)
            orient[i] = new float[W + margin];
}

/*
  Destructor
  */
OrientMap::~OrientMap()
{
    int i;

    for (i = 0; i < H + margin; i++)
            delete[] orient[i];
    delete[] orient;
}

/*
  Creates orient map for a fingerprint according to its type
  */
void OrientMap::orientmap(void)//float **orient)
{
    int i, j, i_block, j_block, block = 1;
    float tmp;
    for (i = 0; i < H + margin - block + 1; i = i + block)//i++)
    {
        for (j = 0; j < W + margin - block + 1; j = j + block)//++)
        {
            tmp = get_orient(i - padding, j - padding, singularity_type);

            for (i_block = i; i_block < i + block; i_block++) {
                for (j_block = j; j_block < j + block; j_block++) {
                    orient[i_block][j_block] = tmp;
                }
            }
        }
    }
}

/* 
  Returns orientation of ridge at a perticular point according  
  to fingerprint type.
  */
float OrientMap::get_orient(int i, int j, int singularity_type) {
    float local_orient;
    complex z, v1, u1, v2, u2;

    if (singularity_type == 1) {
        local_orient = atan(max_float(0.0, (k_arch - k_arch * i / (H
                * arch_fact2))) * cos(j * pi / (W * arch_fact1)));
    }

    if (singularity_type == 2 || singularity_type == 3 || singularity_type == 4) {
        z.x = j;
        z.y = i;

        v1.x = z.x - d[1].x;
        v1.y = z.y - d[1].y;

        u1.x = z.x - l[1].x;
        u1.y = z.y - l[1].y;

        local_orient = 0.5 * (g_delta1(arg(&v1)) - g_loop1(arg(&u1)));

    }

    if (singularity_type == 5 || singularity_type == 6) {
        z.x = j;
        z.y = i;

        v1.x = z.x - d[1].x;
        v1.y = z.y - d[1].y;

        u1.x = z.x - l[1].x;
        u1.y = z.y - l[1].y;

        v2.x = z.x - d[2].x;
        v2.y = z.y - d[2].y;

        u2.x = z.x - l[2].x;
        u2.y = z.y - l[2].y;

        local_orient = 0.5 * (g_delta1(arg(&v1)) - g_loop1(arg(&u1)));
        local_orient = local_orient + 0.5 * (g_delta2(arg(&v2)) - g_loop2(arg(
                &u2)));
    }

    int degrees;

    degrees = int(local_orient * rad_deg_fact);


    if (degrees > 0)
        degrees = degrees % (180);
    else if (degrees < 0)
        degrees = -(((-1) * degrees) % (180)) + 180;

    local_orient = degrees * deg_rad_fact;

    return (local_orient);
}
