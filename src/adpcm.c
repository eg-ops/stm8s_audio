#include "adpcm.h"
#include "stm8s_type.h"

// Table of index changes
const s8 IndexTable[16] = {-1,-1,-1,-1,2,4,6,8,-1,-1,-1,-1,2,4,6,8};

// Quantizer step size lookup table
const u16 StepSizeTable[89] =
      {7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,
       45,50,55,60,66,73,80,88,97,107,118,130,143,157,173,190,
       209,230,253,279,307,337,371,408,449,494,544,598,658,724,
       796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,
       2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,
       7132,7845,8630,9493,10442,11487,12635,13899,15289,16818,
       18500,20350,22385,24623,27086,29794,32767};

u16 step;           // Quantizer step size
s32 preSample;			// ADPCM predictor output,
s8  index;          // Index into step size table

void initDecoder(void)
{
	preSample = 0;
	index = 0;
}

// IMA/DVI ADPCM compatible decoder
//*********************************

s16 ADPCMDecoder( u8 dat ){
	
	s32 diff;             // Predicted diff.

	step = StepSizeTable[index];                      // Find quantizer step size
  diff = step >> 3;                                 // Inverse quantize ADPCM code into
  if( dat & 4 ) diff += step;                      // a difference using the quantizer step
  if( dat & 2 ) diff += step >> 1;
  if( dat & 1 ) diff += step >> 2;
  if( dat & 8 ) preSample -= diff;                 // Add the difference to predicted sample
		else        preSample += diff;
  if( preSample > 32767 ) 	      preSample =  32767; // Check for new predicted sample overflow/underflow
		else if( preSample < -32768 ) preSample = -32768;
  index += IndexTable[dat];                          // Find new quantizer step size
  if( index < 0  ) index = 0;                         // Check if overflow of new quantizer step
  if( index > 88 ) index = 88;

  return( (s16)preSample );                   // Return new sample
}

