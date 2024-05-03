/**************************************************************************//**
 * @file     Mat_mult.c
 * @brief    CMSIS ArmChina STAR Square Matrix Multiplication Template C File
 * @version  V1.0.0
 * @date     20. January 2022
 ******************************************************************************/
/*
 * Copyright (c) 2018-2021 ArmChina. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Timing.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#define CAL_LOOPS       5
#define SQUARE_MAT_SIZE 10


/* Static functions declaration */
/**
  \fn          void Cal_SquareMatMult (uint32_t n, float *a, float *b, float *c)
  \brief       Calculate the matrix multiplciation c = a * b + c.
  \param[in]   n  The size of square matrix.
  \param[in]   a  Pointer to a buffer storing the square matrix a.
  \param[in]   b  Pointer to a buffer storing the square matrix b.
  \param[in]   c  Pointer to a buffer storing the square matrix c.
  \return      none
*/
static void Cal_SquareMatMult(uint32_t n, float *a, float *b, float *c);

/**
  \fn          void Test_MatMult (uint32_t n, float *a)
  \brief       Generate the random square matrix.
  \param[in]   n  The size of square matrix.
  \param[in]   a  Pointer to a buffer storing the square matrix.
  \return      none
*/
static void Random_SquareMat(uint32_t n, float *a);


/**
  \fn          int32_t Test_MatMult (void)
  \brief       Test the matrix multiplciation and timing.
  \param[in]   none
  \return      spent cycle count or error.
*/
int32_t Test_MatMult(void)
{
    uint32_t n;
    uint32_t i;
    uint32_t cnt;
    float *a = NULL;
    float *b = NULL;
    float *c = NULL;
    
    n = SQUARE_MAT_SIZE;

    a = (float *)malloc(n * n * sizeof(float));
    b = (float *)malloc(n * n * sizeof(float));
    c = (float *)malloc(n * n * sizeof(float));

    if ((a == NULL) || (b == NULL) || (c == NULL)) {
        if (a) {
            printf("Matrix multiplication malloc a failed ! \n");
            free(a);
        }
        if (b) {
            printf("Matrix multiplication malloc b failed ! \n");
            free(b);
        }
        if (c) {
            printf("Matrix multiplication malloc c failed ! \n");
            free(c);
        }
        return -1;
    }

    /* Generate random square matrices a, b, c */
    Random_SquareMat(n, a);
    Random_SquareMat(n, b);
    Random_SquareMat(n, c);

    /* Timing the calculation */
    TIMING_START();
    for (i=0; i<CAL_LOOPS; i++) {
        Cal_SquareMatMult(n, a, b, c);
    }
    cnt = TIMING_GET();
    TIMING_STOP();

    free(a);
    free(b);
    free(c);
    a = NULL;
    b = NULL;
    c = NULL;

    /* Return the timing result */
    return (cnt/CAL_LOOPS);
}

/**
  \fn          void Cal_SquareMatMult (uint32_t n, float *a, float *b, float *c)
  \brief       Calculate the matrix multiplciation c = a * b + c.
  \param[in]   n  The size of square matrix.
  \param[in]   a  Pointer to a buffer storing the square matrix a.
  \param[in]   b  Pointer to a buffer storing the square matrix b.
  \param[in]   c  Pointer to a buffer storing the square matrix c.
  \return      none
*/
static void Cal_SquareMatMult(uint32_t n, float *a, float *b, float *c)
{
    uint32_t i, j, k;
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            for (k=0; k<n; k++) {
                c[i*n+j] = a[i*n+k] * b[k*n+j] + c[i*n+j];
            }
        }
    }
}

/**
  \fn          void Test_MatMult (uint32_t n, float *a)
  \brief       Generate the random square matrix.
  \param[in]   n  The size of square matrix.
  \param[in]   a  Pointer to a buffer storing the square matrix.
  \return      none
*/
static void Random_SquareMat(uint32_t n, float *a)
{
    uint32_t i,j;
    srand(0x5A5A);
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            a[i*n+j] = 2.0 * (float)rand() / (double)(RAND_MAX) - 1.0;
        }
    }
}

#ifdef  __cplusplus
}
#endif
