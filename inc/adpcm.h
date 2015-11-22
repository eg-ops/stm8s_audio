#ifndef __ADPCM_H
#define __ADPCM_H

#include "stm8s.h"

#define SAMPLE_RATE_DIV 4   // 16000000/256/4 = 15625 Hz
#define MAX_CIRC_BUFF 2*512 // it has to be power of two
#define CIRC_BUFF_MSK  (MAX_CIRC_BUFF-1)


#define ADPCMDATA_DIM (7755L)//83332L //18112L
//#define PCMDATA_DIM 22050L

typedef union
{
   u8			uBytes[2];
   u16		uShort;
} tTwoByte;

char ADPCMEncoder( signed short sample );
s16 ADPCMDecoder( u8 code );
void initDecoder( void );

#endif /* __ADPCM_H */