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

#include "g_cap.h"
#include "global.h"
#include "cv_type_converter.h"

using namespace anguli;

/*
  Parameters for various class of fingerprints.
  */

G_Cap::G_Cap()
{

}

G_Cap::~G_Cap()
{

}

float G_Cap::g_loop1(float alpha) {
    float alpha_temp = M_PI + alpha;
    int q = floor(4 * alpha_temp / pi);

    if (q == 8)
        q = 7;

    float alpha_i = -M_PI + (M_PI * q) / 4;

    return (g_cap[1][1][q + 1] + ((4 * (alpha - alpha_i)) / pi)
            * (g_cap[1][1][q + 2] - g_cap[1][1][q + 1]));
}

float G_Cap::g_delta1(float alpha) {
    float alpha_temp = M_PI + alpha;
    int q = floor(4 * alpha_temp / pi);

    if (q == 8)
        q = 7;

    float alpha_i = -M_PI + (M_PI * q) / 4;

    return (g_cap[1][2][q + 1] + ((4 * (alpha - alpha_i)) / pi)
            * (g_cap[1][2][q + 2] - g_cap[1][2][q + 1]));

}

float G_Cap::g_loop2(float alpha) {
    float alpha_temp = M_PI + alpha;
    int q = floor(4 * alpha_temp / pi);

    if (q == 8)
        q = 7;

    float alpha_i = -M_PI + (M_PI * q) / 4;

    return (g_cap[2][1][q + 1] + ((4 * (alpha - alpha_i)) / pi)
            * (g_cap[2][1][q + 2] - g_cap[2][1][q + 1]));
}

float G_Cap::g_delta2(float alpha) {
    float alpha_temp = M_PI + alpha;
    int q = floor(4 * alpha_temp / pi);

    if (q == 8)
        q = 7;

    float alpha_i = -M_PI + (M_PI * q) / 4;
    return (g_cap[2][2][q + 1] + ((4 * (alpha - alpha_i)) / pi)
            * (g_cap[2][2][q + 2] - g_cap[2][2][q + 1]));
}
