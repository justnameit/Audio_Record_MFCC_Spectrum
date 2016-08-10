

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <stdint.h>
#include "libmfcc.h"
#include "bmp.h"
#include "wav.h"

#define T 64

complex V[2*NUMBINHALF];

extern float FilterBank[NUMFILTERBANK][NUMBINHALF];
extern float fNorm[NUMFILTERBANK];


main()
{
    float mfcc_result[NUMFILTERBANK];
    RGB_data BMP[NUMFILTERBANK-1][T];
    float Min=0.0f,Max=0.0f;
    int16_t *samples = NULL;
    uint16_t numFrame = 0;
    uint16_t i,j,k;
    int32_t Val_RGB;
    uint32_t idxCoeff;

    memset(BMP, 0.0, sizeof(BMP));
    PreCalcFilterBank(FilterBank,fNorm, NUMBINHALF, NUMFILTERBANK);

    wavread("record.wav", &samples);
    printf("No. of channels: %d\n", header->num_channels);
    printf("Sample rate: %d\n", header->sample_rate);
    printf("Bit rate: %dkbps\n", header->byte_rate*8 / 1000);
    printf("Bits per sample: %d\n\n", header->bps);
    //printf("Sample 0: %d\n", samples[0]);
    //printf("Sample 1: %d\n", samples[1]);
    // Modify the header values & samples before writing the new file
    wavwrite("track2.wav", samples);

    numFrame = sizeof(samples)/(2*NUMBINHALF) ;     
    for (i = 0; i < T; i++) 
    {
       for (j=0; j< 2*NUMBINHALF;j++)
       {
           V[j].Re = (float)(samples[i*2*NUMBINHALF + j]/32768.0); 
           V[j].Im = (float)0.0f;
       } 
       MFCC(V, FilterBank ,fNorm, mfcc_result);

        printf("MFCC:");
        for(idxCoeff = 1; idxCoeff < NUMFILTERBANK; idxCoeff++)
        {	     
            printf(" %f ", mfcc_result[idxCoeff]);
            Val_RGB = (int)(20*(mfcc_result[idxCoeff]+3.064632));
            if (Val_RGB < 0)  Val_RGB = 0;

            BMP[NUMFILTERBANK - idxCoeff-1][i].g =  (BYTE)(Val_RGB );
            BMP[NUMFILTERBANK - idxCoeff-1][i].b =  (BYTE)(Val_RGB);
            BMP[NUMFILTERBANK - idxCoeff-1][i].r =  (BYTE)(Val_RGB ); 
            //BMP[idxCoeff][i].g = (BYTE)((Val_RGB & 0x00FF00)>>8);
            //BMP[idxCoeff][i].b = (BYTE)((Val_RGB & 0xFF0000)>>16); 
            //printf(" %d  ", Val_RGB); 
            if (mfcc_result[idxCoeff] < Min) Min=mfcc_result[idxCoeff];
            if (mfcc_result[idxCoeff] > Max) Max=mfcc_result[idxCoeff];  
        }
        printf("\n");
    }

    printf("Min: %f  Max: %f \n", Min, Max);

    bmp_generator("./record.bmp",T, NUMFILTERBANK -1 , (BYTE*)BMP); 

    free(header);
    free(samples);

    exit (0);
}
