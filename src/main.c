/* MAIN.C file
 * 
 *
 * Copyright (c) 2002-2005 STMicroelectronics
 * 
 */

#include "stm8s.h"
#include "adpcm.h"
#include "stm8s_type.h"
#include <stdio.h>
#include "stm8s_tim1.h"

#ifdef __CSMC__ /* COSMIC */
  extern const @far unsigned char rawData[ADPCMDATA_DIM]; 
#elseif _RAISONANCE_
	extern fcode const unsigned char rawData[ADPCMDATA_DIM];
#else 
    const unsigned char rawData[ADPCMDATA_DIM];
#endif /* _COSMIC_ */

u32 position = 0;

#ifdef __CSMC__ /* COSMIC */
  @near u8 circSampleBuffer[MAX_CIRC_BUFF];
#else /* _RAISONANCE_ */
  u8 circSampleBuffer[MAX_CIRC_BUFF];
#endif /* _COSMIC_ */

vu16 firstToRead = 0;
vu16 nextToWrite = 0;

void Delay(u16 nCount)
{
    /* Decrement nCount value */
    while (nCount != 0) nCount--;
}

//-----------------------------------------------------------------------------
void ClockInit (void)
//-----------------------------------------------------------------------------
{
	// HSI 16 MHz
	CLK->CKDIVR = 0x00; CLK->CKDIVR |= 0x00;
	return;
}


//-----------------------------------------------------------------------------
void TIM1_Init (void)
//-----------------------------------------------------------------------------
{
    // control registers settings
    TIM1->CR1 = TIM1_CR1_ARPE;
    // prescaler to fclk/1
    TIM1->PSCRH = 0x00;
		TIM1->PSCRL = 0x00;

		// auto-reload value set to 256 pulse of HSI
		TIM1->ARRH = 0x00; TIM1->ARRL = 0xFF;
    		 
		TIM1->RCR = 0x03; // every fourth PWN half period will be updated i.e. 16MHz/256/4 = 15625 kHz
		//TIM1->RCR = 0x02; // every third PWN half period will be updated i.e. 16MHz/256/3
		
		// capture/compare enable registers
    TIM1->CCER1 = 0x00;
    TIM1->CCER2 = 0x00;
    // capture/compare mode registers (CC3, CC4 to PWM mode 1, preload enabled)
    TIM1->CCMR1 = TIM1_CCMR1_RESET_VALUE;
    TIM1->CCMR2 = TIM1_CCMR2_RESET_VALUE;
    TIM1->CCMR3 = TIM1_OCMODE_PWM1 | TIM1_CCMR_OCxPE;
    TIM1->CCMR4 = TIM1_OCMODE_PWM1 | TIM1_CCMR_OCxPE;

		//clear the counter
    TIM1->EGR |= TIM1_EGR_COMG | TIM1_EGR_UG;

		// clear status registers
    TIM1->SR1 = 0x00;
    TIM1->SR2 = 0x00;

    // enable update interrupt (overflow)
    TIM1->IER = TIM1_IER_UIE;

		// enable CC3, CC4 output
    SetBit(TIM1->CCER2,0);
		SetBit(TIM1->CCER2,4);

    // capture/compare registers (CC1 PWM duty 50%)
    TIM1->CCR1H = 0x00; TIM1->CCR1L = 0x80;
    TIM1->CCR2H = 0x00; TIM1->CCR2L = 0x80;
    TIM1->CCR3H = 0x00; TIM1->CCR3L = 0x80;
		TIM1->CCR4H = 0x00; TIM1->CCR2L = 0x80;
		
		TIM1->BKR |= 0x80; // master output enable

    // start the timer
    SetBit(TIM1->CR1,0);
    return;
}


unsigned char get_input_data(unsigned char * dat)
{
	if (position < ADPCMDATA_DIM){
		*dat = rawData[position];
		position++;
		return SUCCESS;
	}
	else {
	  position = 0;
		return ERROR;
	}
}

void main(void)
{
	u8 inData; 
	u16 tempIndex;
	
	firstToRead = 0;
	nextToWrite = 0;

	ClockInit();
	TIM1_Init();

	GPIOB->DDR |= (0x01);
	GPIOB->CR1 |= (0x01);   
	GPIOB->CR2 |= (0x01);

  initDecoder();
  
	// decode first sample and put it to the circular buffer
	get_input_data(&inData);
	circSampleBuffer[nextToWrite] = inData;
	nextToWrite++;
	
	rim(); // enable decoding in interrupt routine
	
	{
		u8 cnt;
		for (cnt=0; cnt<12; cnt++){
			
			while(get_input_data( &inData )){									// get data to be put in circular buffer
				tempIndex = (nextToWrite + 1) & CIRC_BUFF_MSK;  
				while (tempIndex == firstToRead) {									
					nop();
					} //wait till the buffer will be released
				circSampleBuffer[nextToWrite] = inData;
				nextToWrite++;
				nextToWrite &= CIRC_BUFF_MSK;
			}
		}
	}
	
	sim();
	while(1);
}
