/**************************************************************************//**
 * @file     main.c
 * @brief    CMSIS ArmChina STAR QCU example to show flash operation on MPS3 board
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
#include "Flash_app.h"

/* Macros declaration */
#define FLASH_ADDRESS   0x00200000
#define FLASH_SIZE      0x00800000
#define PAGE_SIZE       256
#define EMPTY_VALUE     0xFF
#define MANUFACTURER_ID 0xBF
#define DEVICE_Type     0x26
#define DEVICE_ID       0x43
#define READ_8BIT(adr)  (*((volatile uint8_t *)(adr)))

/* Buffer for test */
uint8_t buf[PAGE_SIZE];

/* Static function declaration */
/**
  \fn          int32_t Test_Flash_Id (void)
  \brief       Printf the default status, configuration register of MPS3 Flash, and check JEDEC-ID.
  \return      \ref execution_status
*/
static int32_t Test_Flash_Id(void);

/**
  \fn          int32_t Test_QCU_IndRead (void)
  \brief       Using indirect read mode of QCU to read the flash device memory written the specific value.
  \return      \ref execution_status
*/
static int32_t Test_QCU_IndRead(void);

/**
  \fn          int32_t Test_Flash_Prg (void)
  \brief       Test flash programming algorithm process using QCU driver.
  \return      \ref execution_status
*/
static int32_t Test_Flash_Prg(void);


/**
  \fn          int32_t main (void)
  \brief       main function.
  \return      \ref execution_status
*/
int32_t main(void)
{
    int ret = ARM_DRIVER_OK;
    UartStdOutInit();

    ret = Test_Flash_Id();
    if (ARM_DRIVER_OK != ret) {
        printf("\nSTAR: Flash operation using QCU driver test is failed!\n\n");
        return ret;
    }

    ret |= Test_QCU_IndRead();
    ret |= Test_Flash_Prg();
    if (ARM_DRIVER_OK != ret) {
        printf("\nSTAR: Flash operation using QCU driver test is failed!\n\n");
    } else {
        printf("\nSTAR: Flash operation using QCU driver test is successful!\n\n");
    }

    return ret;
}

/**
  \fn          int32_t Test_Flash_Id (void)
  \brief       Printf the default status, configuration register of MPS3 Flash, and check JEDEC-ID.
  \return      \ref execution_status
*/
static int32_t Test_Flash_Id(void)
{
    int32_t ret = ARM_DRIVER_OK;
    /* Buffer for status, configuration register, and JEDEC-ID */
    FLASH_INFORMATION infor;

    App_Init();

    printf("\n==========Test flash identification using QCU driver start==========\n");
    ret = App_CheckFlash(&infor);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }

    printf("SR of Flash is 0x%x.\n", infor.status_reg);
    printf("CR of Flash is 0x%x.\n", infor.config_reg);
    printf("Manufacturer ID of Flash is 0x%x.\n", infor.jedec_id.manu_id);
    printf("Device Type of Flash is 0x%x.\n", infor.jedec_id.dev_t);
    printf("Device ID of Flash is 0x%x.\n", infor.jedec_id.dev_id);

    if ((MANUFACTURER_ID != infor.jedec_id.manu_id) \
     || (DEVICE_Type != infor.jedec_id.dev_t) \
     || (DEVICE_ID != infor.jedec_id.dev_id)) {
        printf("Flash type is wrong!\n");
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Test_QCU_IndRead (void)
  \brief       Using indirect read mode of QCU to read the flash device memory written the specific value.
  \return      \ref execution_status
*/
static int32_t Test_QCU_IndRead(void)
{
    int32_t ret = ARM_DRIVER_OK;
    uint32_t n;

    /* Prepare programming buffer */
    for (n = 0; n < PAGE_SIZE; n++) {
        buf[n] = (uint8_t)n;
    }
    printf("\n==========Test indirect read mode of QCU start==========\n");
    App_Init();
    n = FLASH_ADDRESS;
    /* Erase sector 0 start */
    ret = App_EraseSector(n-FLASH_ADDRESS);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }

    /* Program page 0 start */
    printf("Write test data to memory...\n");
    ret = App_ProgramPage(n-FLASH_ADDRESS, PAGE_SIZE, buf);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }

    printf("Verify flash content using indirect read mode of QCU...\n");
    ret = App_TestIndRd(n-FLASH_ADDRESS, PAGE_SIZE, buf);
    if (ARM_DRIVER_OK != ret) {
        return ret;
    }

    for (n = 0; n < PAGE_SIZE; n++) {
        if (((uint8_t)n) != buf[n]) {
            printf("Flash content verification is failed!\n");
            ret = ARM_DRIVER_ERROR;
            break;
        }
    }
    printf("Indirect read mode verification is finished.\n");

    return ret;
}

/**
  \fn          int32_t Test_Flash_Prg (void)
  \brief       Test flash programming algorithm process using QCU driver.
  \return      \ref execution_status
*/
static int32_t Test_Flash_Prg(void)
{
    int32_t ret = ARM_DRIVER_OK;
    uint32_t n;

    /* Prepare programming buffer */
    for (n = 0; n < PAGE_SIZE; n++) {
        buf[n] = (uint8_t)n;
    }

    printf("\n==========Test flash programming algorithm process using QCU driver start==========\n");
    printf("\n======Erase chip start...\n");
    App_Init();
    ret |= App_EraseChip();
    printf("Erase chip finish.\n");

    printf("\nVerify flash content after erase chip...\n");
    for (n = 0; n < FLASH_SIZE; n++) {
        if (EMPTY_VALUE != READ_8BIT(FLASH_ADDRESS + n)) {
            printf("Erase chip verification is failed!\n");
            ret |= ARM_DRIVER_ERROR;
            break;
        }
    }
    printf("Erase chip verification is finished.\n");

    ret |= App_UnInit();
    printf("\nTest EraseChip Function finish======\n");

    printf("\n======Program page start...\n");
    App_Init();
    for (n = FLASH_ADDRESS; n < (FLASH_ADDRESS + FLASH_SIZE); n += PAGE_SIZE) {
        printf("Program page 0x%x...\n", n);
        ret |= App_ProgramPage(n-FLASH_ADDRESS, PAGE_SIZE, buf);
    }
    printf("Program page finish.\n");

    printf("\nVerify flash content after program page...\n");
    for (n = 0; n < FLASH_SIZE; n++) {
        if (((uint8_t)n) != READ_8BIT(FLASH_ADDRESS + n)) {
            printf("Program page verification is failed!\n");
            ret |= ARM_DRIVER_ERROR;
            break;
        }
    }
    printf("Program page verification is finished.\n");

    ret |= App_UnInit();
    printf("\nTest ProgramPage Function finish======\n");

    printf("\n======Erase sector start...\n");
    App_Init();
    for (n = FLASH_ADDRESS; n < (FLASH_ADDRESS + FLASH_SIZE); n += PAGE_SIZE) {
        printf("Erase sector 0x%x...\n", n);
        ret |= App_EraseSector(n-FLASH_ADDRESS);
    }
    printf("Erase sector finish.\n");

    printf("\nVerify flash content after erase sector...\n");
    for (n = 0; n < FLASH_SIZE; n++) {
        if (EMPTY_VALUE != READ_8BIT(FLASH_ADDRESS + n)) {
            printf("Erase sector verification is failed!\n");
            ret |= ARM_DRIVER_ERROR;
            break;
        }
    }
    printf("Erase sector verification is finished.\n");

    ret |= App_UnInit();
    printf("\nTest EraseSector Function finish======\n");

    return ret;
}
