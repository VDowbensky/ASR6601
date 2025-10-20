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
	
	//test pin
	gpio_init(GPIOA, GPIO_PIN_11, GPIO_MODE_ANALOG);
	
	adc_get_calibration_value(false, &gain_value, &dco_value);
	dco_value *= 1000.0;
	adc_init();
	adc_config_clock_division(8);
	adc_config_ref_voltage(ADC_INTERNAL_REF_VOLTAGE);
	//It not works now. Temp sensor and Vcc divider must be enabled in AFEC!
	adc_enable_vbat31(true);
	//adc_config_sample_sequence(0, 13); //temp.sensor
  //adc_config_sample_sequence(0, 15); //Vcc
	adc_config_sample_sequence(0, 1); //PA11
	adc_config_conv_mode(ADC_CONV_MODE_SINGLE); //(ADC_CONV_MODE_SINGLE);
	
  adc_enable(true);
	phase = 0;
  adc_start(true);
	
	//enable interrupt
	adc_config_interrupt(ADC_IER_EOC,true); //ADC_IER_EOS
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_IRQHandler(void)
{
	uint16_t adc_val;
	float mv;
	
	if(adc_get_interrupt_status(ADC_ISR_EOC))
	{
		adc_clear_interrupt_status(ADC_ISR_EOC);//ADC_ISR_EOS
		//adc_start(false);
		adc_val = adc_get_data();
		mv = ((Vref/4096.0) * adc_val - dco_value) / gain_value;
		//mv = (Vref/4096.0) * adc_val;
		Vcc = mv;
		//adc_enable(false);
		//if (phase == 0)
		//{
		//	T = mv;
		//	adc_config_sample_sequence(0, 15); 
		//	phase = 1;
		//}
		//else
		//{
		//	Vcc = mv;
		//	adc_config_sample_sequence(0, 13); 
		//	phase = 0;
		//}
		//adc_enable(true);
	}
}
