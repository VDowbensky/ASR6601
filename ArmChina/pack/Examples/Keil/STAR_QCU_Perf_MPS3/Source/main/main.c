/**************************************************************************//**
 * @file     main.c
 * @brief    CMSIS ArmChina STAR QCU example to show quad-line SPI mode performance
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
#include "lowlevel_retarget.h"
#include "UART_APB.h"
#include "Flash_api.h"
#include "Mat_mult.h"
#include "timing.h"

/* Static function declaration */
/**
  \fn          int32_t Test_QCUPerf (void)
  \brief       Test the QCU performance in quad-line SPI mode with XIP.
  \return      \ref execution_status
*/
static int32_t Test_QCUPerf(void);


/**
  \fn          int32_t main (void)
  \brief       main function.
  \return      \ref execution_status
*/
int32_t main(void)
{
	int ret = ARM_DRIVER_OK;
	UartStdOutInit();

	ret = Test_QCUPerf();
	if (ARM_DRIVER_OK != ret) {
		printf("\nSTAR: QCU with XIP test is failed!\n\n");
	}
	printf("\nSTAR: QCU with XIP test is successful!\n\n");

    return ret;
}

/**
  \fn          int32_t Test_QCUPerf (void)
  \brief       Test the QCU performance in quad-line SPI mode with XIP.
  \return      \ref execution_status
*/
static int32_t Test_QCUPerf(void)
{
    int32_t ret = ARM_DRIVER_OK;
    uint32_t cnt = 0;
    uint8_t val[2] = {0};

    TIMING_INIT();

    printf("\n==========TEST QCU XIP performance start==========\n");

    /* 1. Default configuration to run matrix multiplciation */
    printf("QCU driver initialization...\n");
    Api_Init();

    printf("\n==========TEST: Default SPI mode run matrix multiplciation==========\n");

    printf("Run the calculation and counting...\n");
    cnt = Test_MatMult();
    if (cnt < 0) {
        return ARM_DRIVER_ERROR;
    } else {
        printf("Matrix multiplciation count is %d in SPI mode without XIP.\n", cnt);
    }

    /* 2. Quad-line SPI configuration with XIP to run matrix multiplciation */
    printf("\n==========TEST: SQI mode with XIP run matrix multiplciation==========\n");
    printf("Change to SQI direct read mode with XIP...\n");
    ret = Api_EnHiSpd_XIP();
    if (ARM_DRIVER_OK != ret) {
        printf("Change to SQI with XIP is failed!!!\n");
        return ret;
    }
    printf("Run the calculation and counting...\n");
    cnt = Test_MatMult();
    if (cnt < 0) {
        return ARM_DRIVER_ERROR;
    } else {
        printf("Matrix multiplciation count is %d in SQI mode with XIP.\n", cnt);
    }

    /* 3. Test finish and return to SPI mode */
    printf("\n==========TEST QCU XIP performance finish==========\n");
	printf("QCU change to SPI mode.\n");
    ret = Api_RstHiSpd_XIP();
    if (ARM_DRIVER_OK != ret) {
        printf("Change to default configuration is failed!!!\n");
        return ret;
    }

    return ARM_DRIVER_OK;
}
