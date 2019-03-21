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

#include "filter_image.h"
#include "global.h"
#include "cv_type_converter.h"
using namespace anguli;

/*
  Constructor
  */
Filter_Image::Filter_Image()
{
    int i, j;

    freq_ind_2Dmat = new int *[H + margin];
    for (i = 0; i < H + margin; i++)
            freq_ind_2Dmat[i] = new int[W + margin];

    orient_ind_2Dmat = new int *[H + margin];
    for (i = 0; i < H + margin; i++)
            orient_ind_2Dmat[i] = new int[W + margin];

    filt_map = new float *[H + margin];
    for (i = 0; i < H + margin; i++)
            filt_map[i] = new float[W + margin];


    for (i = 0; i < H + margin; i++)
            for (j = 0; j < W + margin; j++)
                    filt_map[i][j] = 0;

    for (i = 0; i < 101; i++)
            for (j = 0; j < 101; j++)
                    mul_mat[i][j] = (i) * (j - 46);
}

/*
  Destructor
  */
Filter_Image::~Filter_Image()
{
    int i;

    for (i = 0; i < H + margin; i++)
            delete[] orient_ind_2Dmat[i];
    delete[] orient_ind_2Dmat;

    for (i = 0; i < H + margin; i++)
            delete[] freq_ind_2Dmat[i];
    delete[] freq_ind_2Dmat;

    for (i = 0; i < H + margin; i++)
            delete[] filt_map[i];
    delete[] filt_map;
}

/* Filters the image with gabor filters.
   */
void Filter_Image::pre_filtering(void) {
    int i, j;

    for (i = 0; i < H + margin; i++) {
        for (j = 0; j < W + margin; j++) {
            orient_ind_2Dmat[i][j] = (int(orient[i][j] * 180 / M_PI)) % distnct_o;
            freq_ind_2Dmat[i][j] = 100 - (int(f_den_2Dmat[i][j] * 100)) % distnct_f;
        }
    }
}

/* 
  Applies Gabor Filter.
 */
void Filter_Image::filter_image(void) {
    int i, j;
    int filt_size_half;
    int r, c;
    int tmp1;
    int filter_size;

    //for each point in image
    for (i = floor(max_filter_size / 2.0);
                        i < H + margin - ceil(max_filter_size / 2.0); i++) {
                        
        for (j = floor(max_filter_size / 2.0);
                        j < W + margin - ceil(max_filter_size / 2.0); j++) {
            {
                filter_size
                        = filter_size_2Dmat[freq_ind_2Dmat[i][j]][orient_ind_2Dmat[i][j]];
                filt_size_half = (int) floor(filter_size / 2.0);

                tmp1 = 0;
                for (r = 0; r < filter_size; r++) {
                    for (c = 0; c < filter_size; c++) {
#ifndef IDX
                        int ind1 = (i + r - filt_size_half) > 0 ? (i + r - filt_size_half) : 0;
                        int ind2 = (j + c - filt_size_half) > 0 ? (j + c - filt_size_half) : 0;
                        ind1 = ind1 < H + margin ? ind1 : H + margin - 1;
                        ind2 = ind2 < W + margin ? ind2 : W + margin - 1;
                        tmp1 = tmp1 + f_print1_2Dmat[ind1][ind2]
                                    * filterbank_4Dmat[freq_ind_2Dmat[i][j]][orient_ind_2Dmat[i][j]][r][c];
#endif

#ifdef IDX
                        tmp1=tmp1+mul_mat[f_print1_2Dmat[i+r-filt_size_half][j+c-filt_size_half]]
                                    [filterbank_4Dmat[freq_ind_2Dmat[i][j]][orient_ind_2Dmat[i][j]][r][c] + 46];
#endif
                    }
                }
                f_print2_2Dmat[i][j] = tmp1;
            }
        }
    }

    copy2Df_print(f_print2_2Dmat, f_print1_2Dmat);
    normalize2Df_print(f_print1_2Dmat);//[0..100]

    binarize2Df_print(f_print1_2Dmat, 55);
    normalize2Df_print(f_print1_2Dmat);//[0..100]
}


void Filter_Image::filter_image_firstpass(void) {
    int i, j;
    int filt_size_half;
    int r, c;
    int tmp1;
    int filter_size;

    binarize2Df_print(f_print1_2Dmat, 65);
    normalize2Df_print(f_print1_2Dmat);//[0..100]

    for (i = floor(max_filter_size / 2.0);
            i < H + margin - ceil(max_filter_size / 2.0); i++) {
        for (j = floor(max_filter_size / 2.0);
            j < W + margin - ceil(max_filter_size / 2.0); j++) {
            
            if (filt_map[i][j] == 1) {
                filter_size = filter_size_2Dmat[freq_ind_2Dmat[i][j]][orient_ind_2Dmat[i][j]];
                filt_size_half = (int) floor(filter_size / 2.0);

                tmp1 = 0;
                for (r = 0; r < filter_size; r++) {
                    for (c = 0; c < filter_size; c++) {
#ifndef IDX
                        int ind1 = (i + r - filt_size_half) > 0 ? (i + r - filt_size_half) : 0;
                        int ind2 = (j + c - filt_size_half) > 0 ? (j + c - filt_size_half) : 0;
                        ind1 = ind1 < H + margin ? ind1 : H + margin - 1;
                        ind2 = ind2 < W + margin ? ind2 : W + margin - 1;
                        tmp1 = tmp1 + f_print1_2Dmat[ind1][ind2]
                                      * filterbank_4Dmat[freq_ind_2Dmat[i][j]][orient_ind_2Dmat[i][j]][r][c];
#endif

#ifdef IDX
                        tmp1=tmp1+mul_mat[f_print1_2Dmat[i+r-filt_size_half][j+c-filt_size_half]]
                                    [filterbank_4Dmat[freq_ind_2Dmat[i][j]][orient_ind_2Dmat[i][j]][r][c] + 46];
#endif
                    }
                }

                f_print2_2Dmat[i][j] = tmp1;
            }
        }
    }
    
    copy2Df_print(f_print2_2Dmat, f_print1_2Dmat);
    normalize2Df_print(f_print1_2Dmat);//[0..100]
    binarize2Df_print(f_print1_2Dmat, 55);
}

void Filter_Image::filter_image_withmap(void) {
    int i, j;
    int filt_size_half;
    int r, c;
    int tmp1;
    int filter_size;

        for (i = floor(max_filter_size / 2.0); i < H + margin - ceil(max_filter_size / 2.0); i++) {
                for (j = floor(max_filter_size / 2.0); j < W + margin - ceil(max_filter_size / 2.0); j++) {
            if (filt_map[i][j] == 1) {
                filter_size = filter_size_2Dmat[freq_ind_2Dmat[i][j]][orient_ind_2Dmat[i][j]];
                filt_size_half = (int) floor(filter_size / 2.0);

                tmp1 = 0;
                for (r = 0; r < filter_size; r++) {
                    for (c = 0; c < filter_size; c++) {
#ifndef IDX
                        int ind1 = (i + r - filt_size_half) > 0 ? (i + r - filt_size_half) : 0;
                        int ind2 = (j + c - filt_size_half) > 0 ? (j + c - filt_size_half) : 0;
                        ind1 = ind1 < H + margin ? ind1 : H + margin - 1;
                        ind2 = ind2 < W + margin ? ind2 : W + margin - 1;
                        tmp1 = tmp1 + f_print1_2Dmat[ind1][ind2]
                                   * filterbank_4Dmat[freq_ind_2Dmat[i][j]][orient_ind_2Dmat[i][j]][r][c];
#endif

#ifdef IDX
                        tmp1=tmp1+mul_mat[f_print1_2Dmat[i+r-filt_size_half][j+c-filt_size_half]]
                                [filterbank_4Dmat[freq_ind_2Dmat[i][j]][orient_ind_2Dmat[i][j]][r][c] + 46];
#endif
                    }
                }
                f_print2_2Dmat[i][j] = tmp1;
            }
        }
    }

    copy2Df_print(f_print2_2Dmat, f_print1_2Dmat);
    normalize2Df_print(f_print1_2Dmat);//[0..100]

    binarize2Df_print(f_print1_2Dmat, 55);
}

/*
  Initializes the are which is to be filtered.
  */
void Filter_Image::set_filter_area(void) {
    int i, j, k;
    int flag_in;

    for (i = 0; i < H + margin; i++) {
        flag_in = 0;
        for (j = 0; j < W + margin; j++) {
            if (filt_map[i][j] == 1 && flag_in == 0) {
                for (k = j - floor(max_filter_size / 2.0); k < j; k++) {
                    if (k > 0)
                    {
                        filt_map[i][k] = 1;
                    }
                }
                flag_in = 1;
            }

            if (filt_map[i][j] == 0 && flag_in == 1) {
                for (k = j; k <= j + floor(max_filter_size / 2.0); k++) {
                    if (k < W + margin) {
                        filt_map[i][k] = 1;
                    }
                }

                j = j + floor(max_filter_size / 2.0) + 1;
                flag_in = 0;
                continue;
            }
        }
    }

    //for each column .. do modification
    for (j = 0; j <= W + margin; j++) {
        flag_in = 0;

        for (i = 0; i < H + margin; i++) {

            if (filt_map[i][j] == 1 && flag_in == 0) {
                for (k = i - floor(max_filter_size / 2.0); k < i; k++) {
                    if (k > 0)
                    {
                        filt_map[k][j] = 1;
                    }
                }
                flag_in = 1;
            }

            if (filt_map[i][j] == 0 && flag_in == 1) {
                for (k = i; k <= i + floor(max_filter_size / 2.0); k++) {
                    if (k < H + margin) {
                        filt_map[k][j] = 1;
                    }
                }
                
                i = i + floor(max_filter_size / 2.0) + 1;
                flag_in = 0;
                                continue;
            }
        }
    }
}

/*
  Returns the Gabor filter value from the filters loaded.
  */
float Filter_Image::get_filtered_val(float **src1, float **src2, int size) {
    int r, c;
    float tmp;
    tmp = 0;
    for (r = 0; r < size; r++) {
        for (c = 0; c < size; c++) {
            tmp = tmp + src1[r][c] * src2[r][c];
        }
    }
    return tmp;
}
