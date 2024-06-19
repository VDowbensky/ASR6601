/*!
 * \file      nvmm.c
 *
 * \brief     None volatile memory management module
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2020 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 */

#include <stdint.h>
#include <string.h>
#include "utilities.h"
#include "nvmm.h"
#include "tremo_flash.h"

uint16_t NvmmWrite( uint8_t* src, uint16_t size, uint16_t offset )
{
    uint8_t buf[CONFIG_NVM_FLASH_LEN];
    int status = 0;

    memcpy(buf, (void *)CONFIG_NVM_FLASH_ADDR, CONFIG_NVM_FLASH_LEN);
    memcpy(buf+offset, src, size);
    
    flash_erase_page(CONFIG_NVM_FLASH_ADDR);
    status = flash_program_bytes(CONFIG_NVM_FLASH_ADDR, buf, CONFIG_NVM_FLASH_LEN);
    if(status != 0){
        return 0;
    }

    return size;
}

uint16_t NvmmRead( uint8_t* dest, uint16_t size, uint16_t offset )
{
    memcpy((void *)dest, (void *)(CONFIG_NVM_FLASH_ADDR+offset), size);

    return size;
}

bool NvmmCrc32Check( uint16_t size, uint16_t offset )
{
    uint8_t data = 0;
    uint32_t calculatedCrc32 = 0;
    uint32_t readCrc32 = 0;

    if( NvmmRead( ( uint8_t* ) &readCrc32, sizeof( readCrc32 ),
                  ( offset + ( size - sizeof( readCrc32 ) ) ) ) == sizeof( readCrc32 ) )
    {
        // Calculate crc
        calculatedCrc32 = Crc32Init( );
        for( uint16_t i = 0; i < ( size - sizeof( readCrc32 ) ); i++ )
        {
            if( NvmmRead( &data, 1, offset + i ) != 1 )
            {
                return false;
            }
            calculatedCrc32 = Crc32Update( calculatedCrc32, &data, 1 );
        }
        calculatedCrc32 = Crc32Finalize( calculatedCrc32 );

        if( calculatedCrc32 != readCrc32 )
        {
            return false;
        }
    }
    return true;
}

bool NvmmReset( uint16_t size, uint16_t offset )
{
    uint32_t crc32 = 0;

    if (NvmmWrite( ( uint8_t* ) &crc32, sizeof( crc32 ), offset + size - sizeof( crc32 ) ) != sizeof( crc32 ))
    {
        return false;
    }

    return true;
}
