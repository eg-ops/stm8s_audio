/**
  ******************************************************************************
  * @file stm8s_it.c
  * @brief This file contains all the interrupt routines.
  * @author STMicroelectronics - MCD Application Team
  * @version V1.0.1
  * @date 10/13/2008
  ******************************************************************************
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2008 STMicroelectronics</center></h2>
  * @image html logo.bmp
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
//#include "stm8s_it.h"
#include "adpcm.h"


#ifdef __CSMC__ /* COSMIC */
	extern @near u8 circSampleBuffer[];
#else /* _RAISONANCE_ */
	extern	u8 circSampleBuffer[];
#endif /* _COSMIC_ */

extern u16 firstToRead;
extern u16 nextToWrite;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

tTwoByte newSample;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

/** @addtogroup IT_Functions
  * @{
  */
/**
  * @brief Dummy interrupt routine
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
*/

#ifdef __CSMC__ /* COSMIC */
	@far @interrupt void NonHandledInterrupt(void) {	
		/* In order to detect unexpected events during development,
			it is recommended to set a breakpoint on the following instruction.
		*/
		return;
	}
#else /* _RAISONANCE_ */
#endif /* COSMIC */


//@far void f__stext 

/* ---------------------------------------------------------------------------*/
/**
  * @brief TIM4 external interrupt routine
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
*/


#ifdef __CSMC__ /* COSMIC */
	@svlreg @far @interrupt void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void) {
#else /* _RAISONANCE_ */
	void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void) interrupt 11 {
#endif /* COSMIC */
		u16 pcmSample;
		u8  adpcmSample;
		static u8 nibble = 1;
			
			SetBit(GPIOB->ODR,0);
	
			// clear update flag
			ClrBit(TIM1->SR1,0);
			
			if (firstToRead != nextToWrite){ // check if some data is available in the buffer
	
				if (nibble) {
						adpcmSample = (u8)(circSampleBuffer[firstToRead] >> 4);
					}
					else {
						adpcmSample = (u8)(circSampleBuffer[firstToRead] & 0x0F);
						firstToRead = (++firstToRead) & (u16)CIRC_BUFF_MSK; 
					}
				nibble = (u8)(!nibble);
				pcmSample = ADPCMDecoder(adpcmSample);
				// update sample
				newSample.uShort = (u16)32768 + pcmSample;
				TIM1->CCR3L = newSample.uBytes[1]; //LSB
				TIM1->CCR4L = newSample.uBytes[0]; //MSB
			}
			else { //end of buffer, try to update sample value next interrupt;
			firstToRead;
			}
	
			ClrBit(GPIOB->ODR,0);
		return;
	}



/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

