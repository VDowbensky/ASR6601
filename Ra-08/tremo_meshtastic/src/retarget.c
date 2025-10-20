#include "bsp.h"
#include "retargetserial.h"

/* Receive buffer */
#define RXBUFSIZE    		512                          /**< Buffer size for RX */
static volatile int     rxReadIndex  = 0;       /**< Index in buffer to be read */
static volatile int     rxWriteIndex = 0;       /**< Index in buffer to be written to */
static volatile int     rxCount      = 0;       /**< Keeps track of how much data which are stored in the buffer */
volatile uint8_t 				rxBuffer[RXBUFSIZE];    /**< Buffer to store data */
static bool             initialized = false;    /**< Initialize UART/LEUART */

/**************************************************************************//**
 * @brief Intializes UART/LEUART
 *****************************************************************************/
void RETARGET_SerialInit(void)
{


#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
  setvbuf(stdout, NULL, _IONBF, 0);   /*Set unbuffered mode for stdout (newlib)*/
#endif
	myuart_init(UART0_BR);
  initialized = true;
}

#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#pragma import(__use_no_semihosting)
//struct __FILE
//{
//    int handle;
//};
//FILE __stdout;

PUTCHAR_PROTOTYPE
{
    uart_send_data(UART0, (uint8_t) ch);
    while (uart_get_flag_status(UART0, UART_FLAG_TX_FIFO_EMPTY) == RESET)
    {}
    return ch;
}

/**************************************************************************//**
 * @brief Receive a byte from USART/LEUART and put into global buffer
 * @return -1 on failure, or positive character integer on sucesss
 *****************************************************************************/
int RETARGET_ReadChar(void)
{
  int c = -1;

  if (initialized == false) RETARGET_SerialInit();
  //INT_Disable();
  if (rxCount > 0)
  {
    c = rxBuffer[rxReadIndex];
    rxReadIndex++;
    if (rxReadIndex == RXBUFSIZE) rxReadIndex = 0;
    rxCount--;
  }
  //INT_Enable();
  return c;
}

/**************************************************************************//**
 * @brief Transmit single byte to USART/LEUART
 * @param c Character to transmit
 * @return Transmitted character
 *****************************************************************************/
int RETARGET_WriteChar(char c)
{
  if (initialized == false) RETARGET_SerialInit();
  uart_send_data(UART0, c);
  return c;
}


//int putc(int c, FILE * stream)
int stdout_putchar(int c, FILE * stream)
{
	RETARGET_WriteChar(c);
	return c; //return the character written to denote a successfull write
}

//int getc(FILE * stream)
int stdin_getchar(FILE * stream)
{
	char c = RETARGET_ReadChar();
	return c;
}



void cbRETARGET_Rx(void)
{
	if (uart_get_flag_status(UART0,UART_FLAG_RX_FIFO_EMPTY) == RESET)
  {
		//USART_ClearITPendingBit(USART1, USART_IT_RXNE); //not needed
    rxBuffer[rxWriteIndex] = uart_receive_data(UART0);
    rxWriteIndex++;
    rxCount++;
    if (rxWriteIndex == RXBUFSIZE)
    {
      rxWriteIndex = 0;
    }
    /* Check for overflow - flush buffer */
    if (rxCount > RXBUFSIZE)
    {
      rxWriteIndex = 0;
      rxCount      = 0;
      rxReadIndex  = 0;
    }
  }
}
