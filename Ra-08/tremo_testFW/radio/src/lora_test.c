#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "tremo_uart.h"
#include "tremo_lpuart.h"
#include "tremo_gpio.h"
#include "tremo_pwr.h"
#include "delay.h"
#include "timer.h"
#include "sx126x-board.h"
#include "sx126x.h"
#include "radio.h"

#define TRANS_STR_MAX_LEN 128

#define RF_FREQUENCY 470000000
#define TX_OUTPUT_POWER 22       // dBm
#define LORA_BANDWIDTH 0         // [0: 125 kHz,
                                 //  1: 250 kHz,
                                 //  2: 500 kHz,
                                 //  3: Reserved]
#define LORA_SPREADING_FACTOR 12 // [SF7..SF12]
#define LORA_CODINGRATE 1        // [1: 4/5,
                                 //  2: 4/6,
                                 //  3: 4/7,
                                 //  4: 4/8]
#define LORA_PREAMBLE_LENGTH 8   // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 5    // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false

typedef int(ATTestHandler)(int argc, char *argv[]);
typedef struct TestCaseSt_
{
    char name[32];
    ATTestHandler *fn;
} TestCaseSt;

static bool lora_listen = false;

typedef enum
{
    at_command = 0,
    transparent_command,
} uart_command_t;

static uint16_t local_addr = 0, target_addr = 1;

static uart_command_t uart_command = at_command;

int test_case_ctxcw(int argc, char *argv[]);
int test_case_ctx(int argc, char *argv[]);
int test_case_crx(int argc, char *argv[]);
int test_case_crxs(int argc, char *argv[]);
int test_case_csleep(int argc, char *argv[]);
int test_case_cstdby(int argc, char *argv[]);
int test_case_addr_set(int argc, char *argv[]);
int test_case_txaddr_set(int argc, char *argv[]);

static TestCaseSt gCases[] = {
    {"AT+CTXCW=", &test_case_ctxcw},
    {"AT+CTX=", &test_case_ctx},
    {"AT+CRXS=", &test_case_crxs},
    {"AT+CRX=", &test_case_crx},
    {"AT+CADDRSET=", &test_case_addr_set},
    {"AT+CTXADDRSET=", &test_case_txaddr_set},
    {"AT+CSLEEP=", &test_case_csleep},
    {"AT+CSTDBY=", &test_case_cstdby}};
static RadioEvents_t TestRadioEvents;
static uint32_t g_fcnt_send = 1;
static uint32_t g_freq = RF_FREQUENCY;
static uint8_t g_sf = LORA_SPREADING_FACTOR;
static uint8_t g_bw = LORA_BANDWIDTH;
static uint8_t g_cr = LORA_CODINGRATE;
static uint8_t g_iqinverted = LORA_IQ_INVERSION_ON;
static uint8_t g_tx_power = TX_OUTPUT_POWER;
static bool g_lora_test_rxs = false;
static bool g_lora_tx_done = false;

gpio_t *g_test_gpiox = GPIOA;
uint32_t g_test_pin = GPIO_PIN_11;

TimerTime_t ts_rxdone_pre = 0;

extern SX126x_t SX126x;

uint8_t CheckSum8(uint8_t *pData, uint8_t len)
{
    uint8_t ucCheckSum = 0;
    uint8_t ucNum;
    // 以1byte为单位依次相加
    for (ucNum = 0; ucNum < len; ucNum++)
    {
        ucCheckSum += pData[ucNum];
    }
    // 二进制求反码，并加1，得到校验和
    ucCheckSum = ~ucCheckSum;
    ucCheckSum++;

    return ucCheckSum;
}

void OnTxDone(void)
{
    printf("OnTxDone\r\n");
    g_lora_tx_done = true;
}

// 处理接收到的数据
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    static uint16_t rx_to_addr = 0, rx_from_addr = 1;
    uint8_t check_sum = 0;
    int i = 0;
    rx_from_addr = ((uint16_t)payload[0] << 8) + (uint16_t)payload[1];
    rx_to_addr = ((uint16_t)payload[2] << 8) + (uint16_t)payload[3];
    check_sum = CheckSum8(payload, size - 1);
    if (local_addr == rx_to_addr && check_sum == payload[size - 1])
    {
        payload[size - 1] = '\0';
        printf("OnRxDone\r\n");
        printf("Recv:\r\n");
        if (false == g_lora_test_rxs)
        {
            for (i = 4; i < size - 1; i++)
            {
                printf("%02x ", payload[i]);
            }
        }
        else
        {
            printf("%s", (char *)(payload + 4));
        }
        printf("\r\nfrom: %d\r\n", rx_from_addr);
        printf("rssi = %d, snr = %d\r\n", rssi, snr);
    }
}

void OnTxTimeout(void)
{
    printf("OnTxTimeout\r\n");
}

void OnRxTimeout(void)
{
    printf("OnRxTimeout\r\n");
}

void OnRxError(void)
{
    printf("OnRxError\r\n");
}

void test_stop3_wfi()
{
    gpio_init(g_test_gpiox, g_test_pin, GPIO_MODE_INPUT_PULL_UP);
    gpio_config_interrupt(g_test_gpiox, g_test_pin, GPIO_INTR_FALLING_EDGE);
    gpio_config_stop3_wakeup(g_test_gpiox, g_test_pin, true, GPIO_LEVEL_LOW);

    /* NVIC config */
    NVIC_EnableIRQ(GPIO_IRQn);
    NVIC_SetPriority(GPIO_IRQn, 2);

    printf("enter deepsleep...\r\n");
    while (uart_get_flag_status(CONFIG_DEBUG_UART, UART_FLAG_BUSY))
        ;

    gpio_set_iomux(GPIOB, GPIO_PIN_1, 0);
    pwr_deepsleep_wfi(PWR_LP_MODE_STOP3);
    gpio_set_iomux(GPIOB, GPIO_PIN_1, 1);

    printf("leave deepsleep...\r\n");
}

int test_case_ctxcw(int argc, char *argv[])
{
    uint8_t opt = 0;
    uint32_t freq = strtol(argv[0], NULL, 0);
    uint8_t pwr = strtol(argv[1], NULL, 0);
    if (argc > 2)
        opt = strtol(argv[2], NULL, 0);

    TestRadioEvents.TxDone = OnTxDone;
    TestRadioEvents.RxDone = OnRxDone;
    TestRadioEvents.TxTimeout = OnTxTimeout;
    TestRadioEvents.RxTimeout = OnRxTimeout;
    TestRadioEvents.RxError = OnRxError;

    SX126xSetPaOpt(opt);
    Radio.Init(&TestRadioEvents);
    SX126xAntSwOff();

    printf("Start to txcw (freq: %lu, power: %udb, opt: %u)\r\n", freq, pwr, opt);
    Radio.SetTxContinuousWave(freq, pwr, 0xffff);

    return 0;
}

int test_case_cstdby(int argc, char *argv[])
{
    uint8_t stdby_mode = 0;
    stdby_mode = strtol((const char *)argv[0], NULL, 0);

    TestRadioEvents.TxDone = OnTxDone;
    TestRadioEvents.RxDone = OnRxDone;
    TestRadioEvents.TxTimeout = OnTxTimeout;
    TestRadioEvents.RxTimeout = OnRxTimeout;
    TestRadioEvents.RxError = OnRxError;

    Radio.Init(&TestRadioEvents);

    // lora standby
    if (stdby_mode == 0)
        SX126xSetStandby(STDBY_RC);
    else
        SX126xSetStandby(STDBY_XOSC);

    test_stop3_wfi();

    return 0;
}

// 设置进入睡眠模式，启动分两种：0：热启动；1：冷启动
int test_case_csleep(int argc, char *argv[])
{
    uint8_t sleep_mode = 0;
    sleep_mode = strtol((const char *)argv[0], NULL, 0);

    TestRadioEvents.TxDone = OnTxDone;
    TestRadioEvents.RxDone = OnRxDone;
    TestRadioEvents.TxTimeout = OnTxTimeout;
    TestRadioEvents.RxTimeout = OnRxTimeout;
    TestRadioEvents.RxError = OnRxError;

    Radio.Init(&TestRadioEvents);

    // lora sleep
    if (sleep_mode == 0)
        Radio.Sleep();
    else
    {
        SleepParams_t params = {0};
        params.Fields.WarmStart = 0;
        SX126xSetSleep(params);
    }

    test_stop3_wfi();

    return 0;
}

// 接收数据，并以字符串格式打印出来
int test_case_crxs(int argc, char *argv[])
{
    if (argc != 5)
        return -1;
    uint32_t freq = strtol(argv[0], NULL, 0);
    uint8_t dr = strtol(argv[1], NULL, 0);
    uint8_t bw = strtol(argv[2], NULL, 0);
    uint8_t cr = strtol(argv[3], NULL, 0);
    uint8_t iqInverted = strtol(argv[4], NULL, 0);

    g_freq = freq;
    g_sf = 12 - dr;
    g_bw = bw;
    g_cr = cr;
    g_iqinverted = iqInverted;

    TestRadioEvents.TxDone = OnTxDone;
    TestRadioEvents.RxDone = OnRxDone;
    TestRadioEvents.TxTimeout = OnTxTimeout;
    TestRadioEvents.RxTimeout = OnRxTimeout;
    TestRadioEvents.RxError = OnRxError;

    Radio.Init(&TestRadioEvents);

    Radio.SetChannel(g_freq);
    Radio.SetRxConfig(MODEM_LORA, g_bw, g_sf,
                      g_cr, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                      0, true, 0, 0, g_iqinverted, true);
    g_lora_test_rxs = true;
    Radio.Rx(0);

    printf("params num: %d ,start to recv package (freq: %lu, dr:%u, bw: %d, cr: %d)\r\n", argc, freq, dr, g_bw, g_cr);
    lora_listen = true;
    return 0;
}

// 接收数据，并以十六进制格式打印出来
int test_case_crx(int argc, char *argv[])
{
    if (argc != 5)
        return -1;
    uint32_t freq = strtol(argv[0], NULL, 0);
    uint8_t dr = strtol(argv[1], NULL, 0);
    uint8_t bw = strtol(argv[2], NULL, 0);
    uint8_t cr = strtol(argv[3], NULL, 0);
    uint8_t iqInverted = strtol(argv[4], NULL, 0);

    g_freq = freq;
    g_sf = 12 - dr;
    g_bw = bw;
    g_cr = cr;
    g_iqinverted = iqInverted;

    TestRadioEvents.TxDone = OnTxDone;
    TestRadioEvents.RxDone = OnRxDone;
    TestRadioEvents.TxTimeout = OnTxTimeout;
    TestRadioEvents.RxTimeout = OnRxTimeout;
    TestRadioEvents.RxError = OnRxError;

    Radio.Init(&TestRadioEvents);

    Radio.SetChannel(g_freq);
    Radio.SetRxConfig(MODEM_LORA, g_bw, g_sf,
                      g_cr, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                      0, true, 0, 0, g_iqinverted, true);
    g_lora_test_rxs = false;
    Radio.Rx(0);

    printf("params num: %d ,start to recv package (freq: %lu, dr:%u, bw: %d, cr: %d)\r\n", argc, freq, dr, g_bw, g_cr);
    lora_listen = true;
    return 0;
}

// 配置发送数据的参数，并进入透传模式
int test_case_ctx(int argc, char *argv[])
{
    if (argc != 6)
        return -1;
    uint32_t freq = strtol(argv[0], NULL, 0);
    uint8_t dr = strtol(argv[1], NULL, 0);
    uint8_t bw = strtol(argv[2], NULL, 0);
    uint8_t cr = strtol(argv[3], NULL, 0);
    uint8_t pwr = strtol(argv[4], NULL, 0);
    uint8_t iqInverted = strtol(argv[5], NULL, 0);

    g_freq = freq;
    g_sf = 12 - dr;
    g_bw = bw;
    g_cr = cr;
    g_tx_power = pwr;
    g_iqinverted = iqInverted;

    TestRadioEvents.TxDone = OnTxDone;
    TestRadioEvents.RxDone = OnRxDone;
    TestRadioEvents.TxTimeout = OnTxTimeout;
    TestRadioEvents.RxTimeout = OnRxTimeout;
    TestRadioEvents.RxError = OnRxError;
    Radio.Init(&TestRadioEvents);

    printf("config radio params data(freq: %lu, dr: %u, bw:%d, cr: %d, power: %u): %lu\r\n", g_freq, 12 - g_sf, g_bw, g_cr, g_tx_power, g_fcnt_send);
    Radio.SetChannel(g_freq);
    Radio.SetTxConfig(MODEM_LORA, g_tx_power, 0, g_bw,
                      g_sf, g_cr,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, 0, 0, g_iqinverted, 60000);
    lora_listen = true;
    printf(">");
    uart_command = transparent_command;
    return 0;
}

// 设置本地地址
int test_case_addr_set(int argc, char *argv[])
{
    if (argc != 1)
        return -1;
    local_addr = strtol(argv[0], NULL, 0);
    printf("set local address: %d \r\n", local_addr);
    return 0;
}

// 设置发送数据的目标设备地址
int test_case_txaddr_set(int argc, char *argv[])
{
    if (argc != 1)
        return -1;
    target_addr = strtol(argv[0], NULL, 0);
    printf("set target address: %d \r\n", target_addr);
    return 0;
}

// 串口数据处理，数据类型有两种：AT指令以及透传数据
char cmd_str[64] = {0};
uint8_t tran_str[TRANS_STR_MAX_LEN] = {0};
int cmd_index = 0;
int tc_lora_test(void)
{
    uint8_t addr[4] = {0};
    int ret = -1;
    int i = 0;
    char *ptr = NULL;
    char ch;

    int case_num = sizeof(gCases) / sizeof(gCases[0]);
    int argc = 0;
    char *argv[16];
    char *str = NULL;
    char resetFlag = 1;

    while (1)
    {
        cmd_index = 0;
        argc = 0;
        if (resetFlag == 1)
        {
            memset(cmd_str, 0, sizeof(cmd_str));
            resetFlag = 0;
        }
        ch = '\0';
        while (1)
        {
            resetFlag = 1;
            while (!lpuart_get_rx_status(LPUART, LPUART_SR0_RX_DONE_STATE))
            {
                if (true == lora_listen)
                {
                    Radio.IrqProcess();
                }
            }

            ch = lpuart_receive_data(LPUART);
            lpuart_clear_rx_status(LPUART, LPUART_SR0_RX_DONE_STATE);

            if (ch != '\n')
            {
                cmd_str[cmd_index++] = ch;
            }
            else
            {
                if (cmd_str[cmd_index - 1] == '\r')
                {
                    cmd_index--;
                    break;
                }
                else
                    cmd_str[cmd_index++] = ch;
            }
        }

        if (cmd_index == 0)
            continue;

        cmd_str[cmd_index] = '\0';

        if (at_command == uart_command)
        {
            if (('A' != cmd_str[0]) && ('T' != cmd_str[1]))
            {
                ret = -1;
                goto end;
            }
            printf("Get CMD: %s \n", cmd_str);
            for (i = 0; i < case_num; i++)
            {
                int cmd_len = strlen(gCases[i].name);
                if (!strncmp((const char *)cmd_str, gCases[i].name, cmd_len))
                {
                    ptr = (char *)cmd_str + cmd_len;
                    break;
                }
            }

            if (i >= case_num || !gCases[i].fn)
                goto end;

            str = strtok((char *)ptr, ",");
            while (str)
            {
                argv[argc++] = str;
                str = strtok((char *)NULL, ",");
            }
            ret = gCases[i].fn(argc, argv);
        }
        else
        {
            if (0 == strcmp(cmd_str, "+++"))
            {
                printf("Quit transparent\r\n");
                uart_command = at_command;
                continue;
            }
            addr[0] = (uint8_t)(local_addr >> 8 & 0xFF);
            addr[1] = (uint8_t)(local_addr & 0xFF);
            addr[2] = (uint8_t)(target_addr >> 8 & 0xFF);
            addr[3] = (uint8_t)(target_addr & 0xFF);
            for (i = 0; i < 4; i++)
            {
                tran_str[i] = addr[i];
            }
            for (i = 0; i < cmd_index; i++)
            {
                tran_str[i + 4] = cmd_str[i];
            }
            tran_str[cmd_index + 4] = CheckSum8(tran_str, (cmd_index + 4));
            Radio.Send((uint8_t *)tran_str, (cmd_index + 5));
            printf("send data: ");
            for (i = 0; i < (cmd_index + 5); i++)
            {
                printf("%02X ", tran_str[i]);
            }
            printf("\r\n");
            memset(tran_str, '\0', (cmd_index + 5));
        }
        memset(cmd_str, '\0', cmd_index);

    end:
        if (ret == -1)
            printf("\r\n+CME ERROR:1\r\n");
    }
}
