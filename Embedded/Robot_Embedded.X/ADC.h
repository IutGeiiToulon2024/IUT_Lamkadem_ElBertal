/* 
 * File:   ADC.h
 * Author: TP_EO_6
 *
 * Created on 4 octobre 2023, 10:01
 */

#ifndef ADC_H
#define	ADC_H
unsigned int * ADCGetResult(void) ;
unsigned char ADCIsConversionFinished(void) ;
void InitADC1(void);
void ADC1StartConversionSequence();
void ADCClearConversionFinishedFlag(void);
#endif	/* ADC_H */

