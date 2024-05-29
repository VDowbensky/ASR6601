#include "adc.h"

float Vcc;
float T;

const float Vref =  1200.0f; //mV
static uint8_t phase;

float gain_value;
float dco_value;

void myadc_init(void)
{
	rcc_set_adc_clk_source(RCC_ADC_CLK_SOURCE_PCLK1);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_ADC, true);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_AFEC, true);
	
	adc_get_calibration_value(false, &gain_value, &dco_value);
	dco_value *= 1000.0;
	adc_init();
	adc_config_clock_division(8);
	
	adc_config_sample_sequence(0, 13); //temp.sensor
  adc_config_sample_sequence(1, 15); //Vcc
	
	adc_config_conv_mode(ADC_CONV_MODE_SINGLE);
  adc_enable(true);
	phase = 0;
  //adc_start(true);
	
	//enable interrupt
	adc_config_interrupt(ADC_IER_EOC,true); //ADC_IER_EOS
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_IRQHandler(void)
{
	uint16_t v;
	
	adc_clear_interrupt_status(ADC_ISR_EOC);//ADC_ISR_EOS
	v = adc_get_data();
	if (phase == 0)
	{
		T = ((Vref/4096.0) * v - dco_value) / gain_value;
		phase = 1;
		//adc_start(true);
	}
	else
	{
		Vcc = ((Vref/4096.0) * v - dco_value) / gain_value;
		phase = 0;
		adc_start(false);
	}
}
