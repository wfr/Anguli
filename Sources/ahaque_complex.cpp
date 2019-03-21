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

#include "ahaque_complex.h"

#define _USE_MATH_DEFINES
#include <math.h>
/*
    Methods for handing complex numbers.
*/
namespace anguli
{
    float length(complex *v) {
        return (sqrt(v->x * v->x + v->y * v->y));
    }


    float dot(complex *v, complex *w) {
        return (v->x * w->x + v->y * w->y);
    }

    float angle(complex *u, complex *v) {
        float result, l1, l2, dotproduct;

        l1 = length(u);
        l2 = length(v);
        dotproduct = dot(u, v);

        result = dotproduct / (l1 * l2);
        result = acos(result);
        
        return (result);
    }

    float arg(complex *u) {
        float result;
        result = atan2(u->y, u->x);

        return (result);
    }
}

