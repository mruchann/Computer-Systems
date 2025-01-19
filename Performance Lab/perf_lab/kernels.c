/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

team_t team = {
    "e252215",
    "Mehmet Rüçhan Yavuzdemir"
};


/********************
 * NORMALIZATION KERNEL
 ********************/

/****************************************************************
 * Your different versions of the normalization functions go here
 ***************************************************************/

/*
 * naive_normalize - The naive baseline version of convolution
 */
char naive_normalize_descr[] = "naive_normalize: Naive baseline implementation";
void naive_normalize(int dim, float *src, float *dst) {
    float min, max;
    min = src[0];
    max = src[0];
    int i, j;

    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
	
            if (src[RIDX(i, j, dim)] < min) {
                min = src[RIDX(i, j, dim)];
            }
            if (src[RIDX(i, j, dim)] > max) {
                max = src[RIDX(i, j, dim)];
            }
        }
    }

    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            dst[RIDX(i, j, dim)] = (src[RIDX(i, j, dim)] - min) / (max - min);
        }
    }
}

/*
 * normalize - Your current working version of normalization
 * IMPORTANT: This is the version you will be graded on
 */
char normalize_descr[] = "Normalize: Current working version";
void normalize(int dim, float *src, float *dst)
{
    float min = src[0], max = src[0];
    int n = dim * dim;

    float val1, val2, val3, val4;
    float range_inv;

    int i;

    for (i = 0; i < n; i += 4) {
        val1 = src[i];
        val2 = src[i + 1];
        val3 = src[i + 2];
        val4 = src[i + 3];

        min = val1 < min ? val1 : min;
        max = val1 > max ? val1 : max;

        min = val2 < min ? val2 : min;
        max = val2 > max ? val2 : max;

        min = val3 < min ? val3 : min;
        max = val3 > max ? val3 : max;

        min = val4 < min ? val4 : min;
        max = val4 > max ? val4 : max;
    }

    range_inv = 1 / (max - min);
    for (i = 0; i < n; i += 16) {
        dst[i] = (src[i] - min) * range_inv;
        dst[i + 1] = (src[i + 1] - min) * range_inv;
        dst[i + 2] = (src[i + 2] - min) * range_inv;
        dst[i + 3] = (src[i + 3] - min) * range_inv;
        dst[i + 4] = (src[i + 4] - min) * range_inv;
        dst[i + 5] = (src[i + 5] - min) * range_inv;
        dst[i + 6] = (src[i + 6] - min) * range_inv;
        dst[i + 7] = (src[i + 7] - min) * range_inv;

        dst[i + 8] = (src[i + 8] - min) * range_inv;
        dst[i + 9] = (src[i + 9] - min) * range_inv;
        dst[i + 10] = (src[i + 10] - min) * range_inv;
        dst[i + 11] = (src[i + 11] - min) * range_inv;
        dst[i + 12] = (src[i + 12] - min) * range_inv;
        dst[i + 13] = (src[i + 13] - min) * range_inv;
        dst[i + 14] = (src[i + 14] - min) * range_inv;
        dst[i + 15] = (src[i + 15] - min) * range_inv;
    }
}

/*********************************************************************
 * register_normalize_functions - Register all of your different versions
 *     of the normalization functions  with the driver by calling the
 *     add_normalize_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_normalize_functions() {
    add_normalize_function(&naive_normalize, naive_normalize_descr);
    add_normalize_function(&normalize, normalize_descr);
    /* ... Register additional test functions here */
}




/************************
 * KRONECKER PRODUCT KERNEL
 ************************/

/********************************************************************
 * Your different versions of the kronecker product functions go here
 *******************************************************************/

/*
 * naive_kronecker_product - The naive baseline version of k-hop neighbours
 */
char naive_kronecker_product_descr[] = "Naive Kronecker Product: Naive baseline implementation";
void naive_kronecker_product(int dim1, int dim2, float *mat1, float *mat2, float *prod) {
    int N = dim1 * dim2;
    int i, j, k, l;
    for (i = 0; i < dim1; i++) {
        for (j = 0; j < dim1; j++) {
            for (k = 0; k < dim2; k++) {
                for (l = 0; l < dim2; l++) {
                    prod[(i * dim2 + k) * N + (j * dim2 + l)] = mat1[i * dim1 + j] * mat2[k * dim2 + l];
                }
            }
        }
    }
}



/*
 * kronecker_product - Your current working version of kronecker_product
 * IMPORTANT: This is the version you will be graded on
 */
char kronecker_product_descr[] = "Kronecker Product: Current working version";
void kronecker_product(int dim1, int dim2, float *mat1, float *mat2, float *prod)
{
    int i, j, k, l;
    int iB, jB, iE, jE;
    int idim1, idim2, jdim2, kdim2;
    register float c;

    int N = dim1 * dim2;
    const int B = 64;

    register const float* row;
    register float* output;

    for (iB = 0; iB < dim1; iB += B) {
        iE = (iB + B > dim1) ? dim1 : iB + B;
        for (jB = 0; jB < dim1; jB += B) {
            jE = (jB + B > dim1) ? dim1 : jB + B;

            idim1 = 0; idim2 = 0;
            for (i = iB; i < iE; ++i) {
                jdim2 = 0;
                for (j = jB; j < jE; ++j) {
                    c = mat1[idim1 + j];
                    kdim2 = 0;
                    for (k = 0; k < dim2; ++k) {
                        row = mat2 + kdim2;
                        output = prod + ((idim2 + k) * N + jdim2);
                        for (l = 0; l < dim2; l += 4) {
                            *output++ = c * *row++;
                            *output++ = c * *row++;
                            *output++ = c * *row++;
                            *output++ = c * *row++;
                        }
                        kdim2 += dim2;
                    }
                    jdim2 += dim2;
                }
                idim1 += dim1;
                idim2 += dim2;
            }
        }
    }
}

/******************************************************************************
 * register_kronecker_product_functions - Register all of your different versions
 *     of the kronecker_product with the driver by calling the
 *     add_kronecker_product_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 ******************************************************************************/

void register_kronecker_product_functions() {
    add_kronecker_product_function(&naive_kronecker_product, naive_kronecker_product_descr);
    add_kronecker_product_function(&kronecker_product, kronecker_product_descr);
    /* ... Register additional test functions here */
}

