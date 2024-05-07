;**************************************************************************
;* @file     startup_tremo.s
;* @brief    tremo_437 startup file for keil
;* <<< Use Configuration Wizard in Context Menu >>>  
;**************************************************************************
;

; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                        ; Top of Stack
                DCD     Reset_Handler                       ; Reset Handler
                DCD     NMI_Handler                         ; NMI Handler
                DCD     HardFault_Handler                   ; Hard Fault Handler
                DCD     MemManage_Handler                   ; MPU Fault Handler
                DCD     BusFault_Handler                    ; Bus Fault Handler
                DCD     UsageFault_Handler                  ; Usage Fault Handler
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     SVC_Handler                         ; SVCall Handler
                DCD     DebugMon_Handler                    ; Debug Monitor Handler
                DCD     0                                   ; Reserved
                DCD     PendSV_Handler                      ; PendSV Handler
                DCD     SysTick_Handler                     ; SysTick Handler
                ; External Interrupts
                DCD     SEC_IRQHandler                     	; SEC 0
                DCD     RTC_IRQHandler                      ; RTC 1
                DCD     WDG_IRQHandler               		; WDG 2
                DCD     EFC_IRQHandler                		; EFC 3
                DCD     UART3_IRQHandler                    ; UART3 4
				DCD     I2C2_IRQHandler             		; I2C2 5
                DCD     UART0_IRQHandler                    ; UART0 6
                DCD     UART1_IRQHandler                   	; UART1 7
                DCD     UART2_IRQHandler                   	; UART2 8
                DCD     LPUART_IRQHandler                   ; LPUART 9
                DCD     SSP0_IRQHandler                   	; SSP0 10 
                DCD     SSP1_IRQHandler                   	; SSP1 11
                DCD     QSPI_IRQHandler             		; QSPI 12
                DCD     I2C0_IRQHandler             		; I2C0 13
				DCD     I2C1_IRQHandler             		; I2C1 14
				DCD		SCC_IRQHandler						; SCC 15
                DCD     ADC_IRQHandler             			; ADC 16
                DCD     AFEC_IRQHandler             		; AFEC 17
                DCD     SSP2_IRQHandler             		; SSP2 18
                DCD     DMA1IRQHandler             			; DMA1 19
                DCD     DMA1_IRQHandler                 	; DAC 20
                DCD     LORA_IRQHandler                  	; LORA 21
                DCD     GPIO_IRQHandler                 	; GPIO 22
                DCD     TIMER0_IRQHandler                 	; TIMER0 23
                DCD     TIMER1_IRQHandler                  	; TIMER1 24
                DCD     TIMER2_IRQHandler                 	; TIMER2 25
                DCD     TIMER3_IRQHandler            		; TIMER3 26
                DCD     BSTIMER0_IRQHandler           		; BSTIMER0 27
                DCD     BSTIMER1_IRQHandler      			; BSTIMER1 28
                DCD     LPTIMER0_IRQHandler                 ; LPTIMER0 29
				DCD		SAC_IRQHandler						; SAC 30	
                DCD     DMA0_IRQHandler              		; DMA0 31
                DCD     I2S_IRQHandler              		; I2S 32
                DCD     LCD_IRQHandler              		; LCD 33
                DCD     PWR_IRQHandler                 		; PWR 34
                DCD     LPTIMER1_IRQHandler                 ; LPTIMER1 35
                DCD     IWDG_IRQHandler                 	; IWDG 36

__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler
Reset_Handler   PROC
                EXPORT  Reset_Handler                       [WEAK]
                IMPORT  __main
                IMPORT  SystemInit
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                         [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler                   [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler                   [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler                    [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler                  [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                         [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler                    [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler                      [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler                     [WEAK]
                B       .
                ENDP

Default_Handler PROC
					
                EXPORT  SEC_IRQHandler                     	[WEAK]
                EXPORT  RTC_IRQHandler                      [WEAK]
                EXPORT  WDG_IRQHandler               		[WEAK]
                EXPORT  EFC_IRQHandler                		[WEAK]
                EXPORT  UART3_IRQHandler                    [WEAK]
				EXPORT  I2C2_IRQHandler						[WEAK] ;
			    EXPORT  UART0_IRQHandler                    [WEAK]
				EXPORT  UART1_IRQHandler                   	[WEAK]
				EXPORT  UART2_IRQHandler                   	[WEAK]
				EXPORT  LPUART_IRQHandler                   [WEAK]
				EXPORT  SSP0_IRQHandler                   	[WEAK]
				EXPORT  SSP1_IRQHandler                   	[WEAK]
				EXPORT  QSPI_IRQHandler             		[WEAK]
				EXPORT  I2C0_IRQHandler             		[WEAK]
				EXPORT  I2C1_IRQHandler             		[WEAK]
				EXPORT  SCC_IRQHandler						[WEAK] ;
				EXPORT  ADC_IRQHandler             			[WEAK]
				EXPORT  AFEC_IRQHandler             		[WEAK]
				EXPORT  SSP2_IRQHandler             		[WEAK]
				EXPORT  DMA1IRQHandler             			[WEAK]
				EXPORT  DMA1_IRQHandler                 	[WEAK]
				EXPORT  LORA_IRQHandler                  	[WEAK]
				EXPORT  GPIO_IRQHandler                 	[WEAK]
				EXPORT  TIMER0_IRQHandler                 	[WEAK]
				EXPORT  TIMER1_IRQHandler                  	[WEAK]
				EXPORT  TIMER2_IRQHandler                 	[WEAK]
				EXPORT  TIMER3_IRQHandler            		[WEAK]
				EXPORT  BSTIMER0_IRQHandler           		[WEAK]
				EXPORT  BSTIMER1_IRQHandler      			[WEAK]
				EXPORT  LPTIMER0_IRQHandler                 [WEAK]
				EXPORT  SAC_IRQHandler						[WEAK] ;	
				EXPORT  DMA0_IRQHandler              		[WEAK]
				EXPORT  I2S_IRQHandler              		[WEAK]
				EXPORT  LCD_IRQHandler              		[WEAK]
				EXPORT  PWR_IRQHandler                 		[WEAK]
				EXPORT  LPTIMER1_IRQHandler                 [WEAK]
				EXPORT  IWDG_IRQHandler                 	[WEAK]
				
SEC_IRQHandler
RTC_IRQHandler
WDG_IRQHandler
EFC_IRQHandler
UART3_IRQHandler
I2C2_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
LPUART_IRQHandler
SSP0_IRQHandler
SSP1_IRQHandler
QSPI_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
SCC_IRQHandler
ADC_IRQHandler
AFEC_IRQHandler
SSP2_IRQHandler
DMA1IRQHandler
DMA1_IRQHandler
LORA_IRQHandler
GPIO_IRQHandler
TIMER0_IRQHandler
TIMER1_IRQHandler
TIMER2_IRQHandler
TIMER3_IRQHandler
BSTIMER0_IRQHandler
BSTIMER1_IRQHandler
LPTIMER0_IRQHandler
SAC_IRQHandler
DMA0_IRQHandler
I2S_IRQHandler
LCD_IRQHandler
PWR_IRQHandler
LPTIMER1_IRQHandler
IWDG_IRQHandler
                B       .

                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB

                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit

                 ELSE

                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap

__user_initial_stackheap

                 LDR     R0, = Heap_Mem
                 LDR     R1, = (Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END
