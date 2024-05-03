#include <string.h>
#include "tremo_regs.h"
#include "tremo_qspi.h"
#include "tremo_rcc.h"
#include "tremo_gpio.h"

unsigned int sector_size = 0x1000; // 4Kbyte
unsigned char buf[1024];           // Programming Buffer
unsigned char one_read_buf[256];

extern qspi_flash_device_t const flash_device;

int read_onebyte_check(unsigned char* dst, unsigned char* src, int size)
{
    memset(dst, 0, size);
    int i = 0;
    for (i = 0; i < size; i = i + 1) {
        *dst++ = *src++;
    }
    return 0;
}

void qspi_read_write(unsigned int len)
{
    unsigned long n;
    uint8_t verify_flag    = 0;
    unsigned long end_addr = 0;
    int ret;

    gpio_set_iomux(GPIOC, GPIO_PIN_0, 4); // GP32
    gpio_set_iomux(GPIOC, GPIO_PIN_1, 4); // GP33
    gpio_set_iomux(GPIOC, GPIO_PIN_2, 4); // GP34
    gpio_set_iomux(GPIOC, GPIO_PIN_3, 4); // GP35
    gpio_set_iomux(GPIOC, GPIO_PIN_4, 4); // GP36
    gpio_set_iomux(GPIOC, GPIO_PIN_5, 4); // GP37

    qspi_init(1, 2, 4);

    for (n = 0; n < flash_device.szPage; n++) {
        buf[n] = (unsigned char)n;
    }
    memset(one_read_buf, 0, 256);

    for (n = flash_device.DevAdr; n < (flash_device.DevAdr + len); n += sector_size) {
        ret |= qspi_erase_sector(n); // Sector Erase
    }
    for (n = flash_device.DevAdr; n < (flash_device.DevAdr + len); n += flash_device.szPage) {
        ret |= qspi_program_page(n, flash_device.szPage, buf); // Page Programming
    }

    read_onebyte_check(one_read_buf, (unsigned char*)flash_device.DevAdr, flash_device.szPage);
    for (int i = 0; i < 4; i++) {
        if (one_read_buf[i] == i)
            continue;
        else
            while (1)
                ;
    }
    end_addr = qspi_verify(flash_device.DevAdr, len, buf);
    if (end_addr == flash_device.DevAdr + len) {
        verify_flag++;
    }
}

int main(void)
{
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOC, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_QSPI, true);

    qspi_read_write(0x06000);

    /* Infinite loop */
    while (1) { }
}

#ifdef USE_FULL_ASSERT
void assert_failed(void* file, uint32_t line)
{
    (void)file;
    (void)line;

    while (1) { }
}
#endif
