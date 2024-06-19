/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright (c) 2010 Keil - An ARM Company. All rights reserved.     */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Flash Programming Functions adapted                   */
/*               for New Device 256kB Flash                            */
/*                                                                     */
/***********************************************************************/

#include "tremo_qspi.h"
#include "tremo_regs.h"

uint32_t value[32]     = { 0 };
uint32_t sw_reset_flag = 1;

#define M8(adr)  (*((uint8_t*)(adr)))
#define M16(adr) (*((uint16_t*)(adr)))
#define M32(adr) (*((uint32_t*)(adr)))

qspi_flash_device_t const flash_device = { FLASH_DRV_VERS, // Driver Version, do not modify!
    "XT25F16B",                                            // Device Name
    ONCHIP,                                                // Device Type
    0x60000000,                                            // Device Start Address
    0x00200000,                                            // Device Size in Bytes (2MB)
    256,                                                   // Programming Page Size
    0,                                                     // Reserved, must be 0
    0xFF,                                                  // Initial Content of Erased Memory
    3000,                                                  // Program Page Timeout 100 mSec
    6000,                                                  // Erase Sector Timeout 3000 mSec
    { { 0x001000, 0x000000 },                              // Sector Size  4kB (512 Sectors)
        { SECTOR_END } } };

#define FLAH_RESET       *(volatile int*)0x40000950
#define flash_reset_high 0x10
#define flash_reset_low  0xFFFFFFEF

unsigned long base_adr;

int check_busy_flag = 1;

void f_delay(unsigned char cycle)
{
    while (cycle--)
        __asm("nop");
}
int sw_reset()
{
    FLAH_RESET |= flash_reset_high;
    f_delay(10);
    FLAH_RESET &= flash_reset_low;

    return 0;
}

int check_busy(void)
{

    int var_rdata = 0;
    int cnt       = 0;
    /* Add your Code */
    var_rdata = QSPI->QSPI_SR; // read back

    while (((var_rdata >> 5 & 0x01) == 1)) {
        var_rdata = QSPI->QSPI_SR; // read back
        if (cnt < 12000000)        // erase chip time(10s)/(160MHz*2cycle)
            cnt = cnt + 1;
        else {
            check_busy_flag++;
            return (1);
        }
    }
    return (0); // Finished without Errors
}

int check_abort_busy(void)
{

    int var_rdata = 0;
    int cnt       = 0;
    /* Add your Code */
    var_rdata = QSPI->QSPI_SR; // read back

    while (((var_rdata >> 5 & 0x01) == 1)) {
        var_rdata = QSPI->QSPI_SR; // read back
        if (cnt < 200)             // /160MHz*10cycled
            cnt = cnt + 1;
        else {
            // sw_reset();
            // sw_reset_flag = 2;
            // return (1);
        }
    }
    return (0); // Finished without Errors
}

int clr_flg(void)
{
    /* Add your Code */
    QSPI->QSPI_FCR = 0x1B;
    QSPI->QSPI_FCR = 0x0;

    return (0); // Finished without Errors
}

int abort_en(void)
{
    /* Add your Code */
    int var_rdata;
    var_rdata     = QSPI->QSPI_CR;
    QSPI->QSPI_CR = (var_rdata & 0xFFFFFFFD) + 0x2;
    QSPI->QSPI_CR = var_rdata & 0xFFFFFFFD;
    return (0); // Finished without Errors
}

int polling_wip(void)
{
    int var_rdata = 0;
    // dumode=2'b00,fmode=2'b10,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h05;
    QSPI->QSPI_CCR   = 0x9000105;
    var_rdata        = QSPI->QSPI_CR;
    QSPI->QSPI_CR    = (var_rdata & 0xFFBFFFFF) + 0x400000; // QSPI_CR[22],apms= 1'b1;
    QSPI->QSPI_DLR   = 0x0;                                 // one byte
    QSPI->QSPI_PSMKR = 0x1;                                 // mask = 0x1;
    QSPI->QSPI_PSMAR = 0x0;                                 // match = 0x0;
    QSPI->SBUS_START = 0x1;
    f_delay(10);
    check_busy();
    clr_flg();
    return (0); // Finished without Errors
}

int shift_adj(unsigned int gold_data)
{
    int var_rdata;
    int match_flag[8];
    int match_interval[8];
    int sshift_min[8];
    int sshift_max[8];
    int dshift;
    int sshift;
    int i, j;

    for (i = 0; i <= 0x7; i++) {
        dshift        = i;
        sshift_min[i] = 0x7;
        sshift_max[i] = 0x0;
        match_flag[i] = 0x0;
        for (j = 0; j <= 0x7; j++) {
            sshift        = j;
            var_rdata     = QSPI->QSPI_CR;
            QSPI->QSPI_CR = (var_rdata & 0x87FF1FFF) + (dshift << 13) + (sshift << 27); // QSPI_CR[15:13],QSPI_CR[30:27];
            var_rdata     = qspi_read_id();
            if (var_rdata == gold_data) {
                match_flag[i] = 0x1;
                if (j < sshift_min[i]) {
                    sshift_min[i] = j;
                }
                if (j > sshift_max[i]) {
                    sshift_max[i] = j;
                }
            }
        }
    }
    for (i = 0; i <= 0x7; i++) {
        if (match_flag[i] == 0x1) {
            match_interval[i] = sshift_max[i] - sshift_min[i];
        } else {
            match_interval[i] = 0x0;
        }
    }
    dshift = 0x0;
    for (i = 1; i <= 0x7; i++) {
        if (match_interval[i] > match_interval[dshift]) {
            dshift = i;
        }
    }
    sshift        = (sshift_max[dshift] + sshift_min[dshift]) / 2 + (sshift_max[dshift] + sshift_min[dshift]) % 2;
    var_rdata     = QSPI->QSPI_CR;
    QSPI->QSPI_CR = (var_rdata & 0x87FF1FFF) + (dshift << 13) + (sshift << 27); // QSPI_CR[15:13],QSPI_CR[30:27];
    return (0);                                                                 // Finished without Errors
}

int polling_wel(void)
{
    int var_rdata = 0;
    // dumode=2'b00,fmode=2'b10,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h05;
    QSPI->QSPI_CCR   = 0x9000105;
    var_rdata        = QSPI->QSPI_CR;
    QSPI->QSPI_CR    = (var_rdata & 0xFFBFFFFF) + 0x400000; // QSPI_CR[22],apms= 1'b1;
    QSPI->QSPI_DLR   = 0x0;                                 // one byte
    QSPI->QSPI_PSMKR = 0x2;                                 // mask = 0x1;
    QSPI->QSPI_PSMAR = 0x2;                                 // match = 0x0;
    QSPI->SBUS_START = 0x1;
    f_delay(10);
    check_busy();
    clr_flg();
    return (0); // Finished without Errors
}

int set_qe(unsigned char quad)
{
    volatile int var_rdata   = 0;
    volatile int cycle_count = 0;
    abort_en();
    check_abort_busy();
    do {
        QSPI->QSPI_CCR = 0x106; // IMODE=2'b01,INSTRUCTION=WREN
        clr_flg();
        QSPI->SBUS_START = 0x1;
        f_delay(10);
        check_busy();
        QSPI->QSPI_CCR
            = 0x5000105; // dumode=2'b00,fmode=2'b01,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h05;
        clr_flg();
        QSPI->SBUS_START = 0x1;
        f_delay(10);
        check_busy();
        var_rdata = QSPI->QSPI_DR & 0xFF;
        cycle_count++;
        if (cycle_count == 5) {
            return 1;
        }
    } while (((var_rdata >> 1) & 0x01) == 0);

    cycle_count = 0;
    // QSPI->QSPI_CCR =
    // 0x01000101;//dumode=2'b00,fmode=2'b00,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h01;
    // QSPI->QSPI_DLR = 0x1;//two byte
    // QSPI->QSPI_DR = 0x200;
    QSPI->QSPI_CCR
        = 0x01000131; // dumode=2'b00,fmode=2'b00,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h01;
    QSPI->QSPI_DLR = 0x0; // two byte
    QSPI->QSPI_DR  = 0x2;
    clr_flg();
    QSPI->SBUS_START = 0x1;
    f_delay(10);
    check_busy();
    clr_flg();
    polling_wip();

    /* Add your Code */
    return (0); // Finished without Errors
}

void load_reset_reg_cfg()
{
    abort_en();
    check_abort_busy();
    // QSPI->QSPI_CR    = 0x08004801;//div2, 20MHz 0x11004801;
    QSPI->QSPI_CR = (0x2 << 27) | (0x1 << 24) | (0x2 << 13) | 1; // div4, 13MHz //offset 0x00    //0x09004801
    // QSPI->QSPI_CR    = 0x89004801;  //div3,   13.3MHz //offset 0x00
    QSPI->QSPI_DCR = 0x001B0000; // offset 0x04
    QSPI->QSPI_FCR = 0x0;        // offset 0x0C
    QSPI->QSPI_DLR = 0x0;        // offset 0x10
    // QSPI->QSPI_CCR   =
    // 0x4E1C253B;//dumode=2'b10,,sio=1'b0,fmode=2'b11,dmode=2'b10,reserved=1'b0,dcyc=5'h7,absize=2'b00,abmode=2'b00,adsize=2'b10,admode=2'b01,imode=2'b01,instruction=8'h3B;
    // QSPI->QSPI_CCR   =
    // 0x2D1C250B;//dumode=2'b01,,sio=1'b0,fmode=2'b11,dmode=2'b01,reserved=1'b0,dcyc=5'h7,absize=2'b00,abmode=2'b00,adsize=2'b10,admode=2'b01,imode=2'b01,instruction=8'h0B;
    QSPI->QSPI_CCR
        = 0x6F0CEDEB; // dumode=2'b11,,sio=1'b0,fmode=2'b11,dmode=2'b11,reserved=1'b0,dcyc=5'h3,absize=2'b00,abmode=2'b11,adsize=2'b10,admode=2'b11,imode=2'b01,instruction=8'hEB;
    QSPI->QSPI_AR    = 0x0;      // offset 0x18
    QSPI->QSPI_ABR   = 0x0;      // offset 0x1C
    QSPI->QSPI_DR    = 0x0;      // offset 0x20
    QSPI->QSPI_PSMKR = 0x0;      // offset 0x24
    QSPI->QSPI_PSMAR = 0x0;      // offset 0x28
    QSPI->QSPI_PIR   = 0x0;      // offset 0x2C
    QSPI->QSPI_TOR   = 0x12FFFF; // offset 0x30
    QSPI->QSPI_CFGR  = 0x200;    // offset 0x8C
}
int qspi_init(unsigned long adr, unsigned long clk, unsigned long fnc)
{

    abort_en();
    check_abort_busy();
    load_reset_reg_cfg();

    QSPI->QSPI_CFGR |= (1 << 10);
    set_qe(1);
    QSPI->QSPI_CFGR &= ~(1 << 10);
    return (0); // Finished without Errors
}

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int qspi_erase_chip(void)
{
    abort_en();
    check_abort_busy();

    QSPI->QSPI_CCR = 0x106; // IMODE=2'b01,INSTRUCTION=WREN
    clr_flg();
    QSPI->SBUS_START = 0x1;
    f_delay(10);
    check_busy();
    polling_wel();

    QSPI->QSPI_CCR
        = 0x0000160; // dumode=2'b0,fmode=2'b00,dmode=2'b00,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h60;
    clr_flg();
    QSPI->SBUS_START = 0x1;
    f_delay(10);
    check_busy();
    clr_flg();
    polling_wip();

    f_delay(10);
    clr_flg();
    load_reset_reg_cfg();
    /* Add your Code */
    return (0); // Finished without Errors
}

/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int qspi_erase_sector(unsigned long adr)
{
    abort_en();
    check_abort_busy();

    QSPI->QSPI_CCR = 0x106; // IMODE=2'b01,INSTRUCTION=WREN
    clr_flg();
    QSPI->SBUS_START = 0x1;
    f_delay(10);
    check_busy();
    polling_wel();

    QSPI->QSPI_CCR
        = 0x0002520; // fmode=2'b00,dmode=2'b00,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b10,admode=2'b01,imode=2'b01,instruction=8'h20;
    QSPI->QSPI_AR = adr;
    clr_flg();
    QSPI->SBUS_START = 0x1;
    f_delay(10);
    check_busy();
    clr_flg();
    polling_wip();
    f_delay(10);
    clr_flg();
    load_reset_reg_cfg();
    /* Add your Code */
    return (0); // Finished without Errors
}

/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int qspi_program_page(unsigned long adr, unsigned long sz, unsigned char* buf)
{
    unsigned long sz_temp = 0;

    int var_rdata = 0;
    int fthres    = 8;
    int cnt       = 0;

    sz = (sz + 3) & ~3; // Adjust size for Words
    if (sz != 0) {
        sz_temp = sz - 1;
    }
    abort_en();
    check_abort_busy();

    QSPI->QSPI_CCR = 0x106; // IMODE=2'b01,INSTRUCTION=WREN
    clr_flg();
    QSPI->SBUS_START = 0x1;
    f_delay(10);
    check_busy();
    polling_wel();

    QSPI->QSPI_CCR
        = 0x1002502; // fmode=2'b00,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b10,admode=2'b01,imode=2'b01,instruction=8'h02;
    QSPI->QSPI_DLR = sz_temp;
    QSPI->QSPI_AR  = adr;
    while ((sz > 0) && (cnt < fthres)) {
        var_rdata = QSPI->QSPI_SR;
        var_rdata = var_rdata >> 8 & 0x3F;
        if (var_rdata < fthres) {
            QSPI->QSPI_DR = *((unsigned long*)buf); // Program Word
            cnt += 1;
            buf += 4;
            sz -= 4;
        }
    }
    f_delay(10);
    check_busy();
    QSPI->SBUS_START = 0x01; // WRITE
    f_delay(10);
    while (sz) {
        var_rdata = QSPI->QSPI_SR;
        var_rdata = var_rdata >> 8 & 0x3F;
        if (var_rdata < fthres) {
            QSPI->QSPI_DR = *((unsigned long*)buf); // Program Word
            buf += 4;
            sz -= 4;
        }
    }
    f_delay(10);
    check_busy();
    polling_wip();
    load_reset_reg_cfg();
    return (0); // Finished without Errors
}

unsigned long qspi_verify(unsigned long adr, unsigned long sz, unsigned char* buf)
{
    unsigned char* pbuf = buf;
    while (sz) {
        if (M8(adr) != *buf) {
            return (adr); // Verification Failed
        }
        // Go to next Byte
        adr += 1;
        buf += 1;
        if (adr % 256 == 0)
            buf = pbuf;
        sz -= 1;
    }
    return (adr + sz); // Done successfully
}

int qspi_read_sbus(unsigned long adr, unsigned long sz, unsigned long* read_tmp)
{
    int var_rdata = 0;

    if (sz != 0) {
        sz = ((sz + 3) & ~3) - 1; // Adjust size for Words
    }

    QSPI->QSPI_CCR
        = 0x251C250B; // dumode=2'b01,fmode=2'b01,dmode=2'b01,reserved=1'b0,dcyc=5'h7,absize=2'b00,abmode=2'b00,adsize=2'b10,admode=2'b01,imode=2'b01,instruction=8'h0B;
    QSPI->QSPI_DLR = sz;
    QSPI->QSPI_AR  = adr;
    check_busy();
    clr_flg();
    QSPI->SBUS_START = 0x01; // WRITE
    f_delay(10);
    while (sz > 0) {
        var_rdata = QSPI->QSPI_SR;
        if ((((var_rdata >> 1) & 0x01) == 0x01) || (((var_rdata >> 2) & 0x01) == 0x01)) {
            *read_tmp++ = QSPI->QSPI_DR;
            if (sz >= 4) {
                sz -= 4;
            } else {
                sz = 0;
            }
        }
    }
    clr_flg();
    // check_busy();
    return (0); // Finished without Errors
}

int qspi_read_id(void)
{
    int var_rdata;
    int dshift;
    QSPI->QSPI_CCR
        = 0x500019F; // fmode=2'b01,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h9F;
    QSPI->QSPI_DLR = 0x2;
    clr_flg();
    QSPI->SBUS_START = 0x1;
    f_delay(10);
    check_busy();
    var_rdata = QSPI->QSPI_CR;
    dshift    = (var_rdata >> 13) & 0x7;
    f_delay(dshift);

    var_rdata = QSPI->QSPI_DR;
    clr_flg();
    load_reset_reg_cfg();
    return (var_rdata);
}
