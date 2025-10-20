#include "ustimer.h"

#include "tremo_rcc.h"
#include "tremo_timer.h"
#include "radio.h"

void ustimer_init(void)
{
	timer_init_t timerx_init;

	rcc_enable_peripheral_clk(RCC_PERIPHERAL_TIMER0, true);  
	timer_config_interrupt(TIMER0, TIMER_DIER_UIE, ENABLE);
	timerx_init.prescaler          = 31;  //sysclock 32MHz, is divided by (prescaler + 1) to 1MHz
	timerx_init.counter_mode       = TIMER_COUNTERMODE_UP;
	timerx_init.period             = 1000;   //1 ms as default
	timerx_init.clock_division     = TIMER_CKD_FPCLK_DIV1;
	//timerx_init.autoreload_preload = false;
	timerx_init.autoreload_preload = true;
	timer_init(TIMER0, &timerx_init);
	timer_config_interrupt(TIMER0, TIMER_DIER_UIE, ENABLE);
	timer_generate_event(TIMER0, TIMER_EGR_UG, ENABLE);
	timer_clear_status(TIMER0, TIMER_SR_UIF);
	NVIC_ClearPendingIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);
	//timer_cmd(TIMER0, true);
	timer_cmd(TIMER0, false);
}

void ustimer_start(void)
{
	timer_cmd(TIMER0, true);
}

void ustimer_stop(void)
{
	timer_cmd(TIMER0, false);
}

void ustimer_setinterval(uint16_t interval)
{
	TIMER0->ARR = (uint32_t)interval;
}

void TIMER0_IRQHandler(void)
{
	bool state;
	
	timer_get_status(TIMER0, TIMER_SR_UIF, &state);
	if(state)
	{
		timer_clear_status(TIMER0, TIMER_SR_UIF);
		sweepflag = true;
	}
}

