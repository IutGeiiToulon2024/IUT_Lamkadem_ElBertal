/* 
 * File:   main.c
 * Author: TP_EO_6
 *
 * Created on 13 septembre 2023, 14:12
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "robot.h"

unsigned int * result; 
int ADCValue0 = 0;
int ADCValue1 = 0;
int ADCValue2 = 0;

int main(void)
{
    
    /***************************************************************************************************/
    //Initialisation de l?oscillateur

    /****************************************************************************************************/
    InitOscillator();
    
    InitIO();
    
    
    InitADC1();// Initialisation des timers
    InitTimer23();
    InitTimer1();
    /****************************************************************************************************/
    // Configuration des éentres sorties
    /****************************************************************************************************/
    
    LED_BLANCHE = 0;
    LED_BLEUE = 0;
    LED_ORANGE = 0;

    InitPWM();
    //    PWMSetSpeed(20, MOTEUR_GAUCHE);
    //    PWMSetSpeed(20, MOTEUR_DROIT);
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    
    
    while (1) {
//        if (ADCIsConversionFinished()){
//            result = ADCGetResult();
//            ADCValue0 = result[0];
//            ADCValue1 = result[1];
//            ADCValue2 = result[2];
//            LED_ORANGE = !LED_ORANGE;
//            for(int i = 0; i < 2; i++){
//                
//            }
            
//            ADCClearConversionFinishedFlag();
        
        if (ADCIsConversionFinished() == 1)
        {
            ADCClearConversionFinishedFlag();

            unsigned int * result = ADCGetResult();
            float volts = ((float) result [2])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche = 34/volts - 5;
            volts = ((float) result [1])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreCentre = 34/volts - 5;
            volts = ((float) result [0])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreDroit = 34/volts - 5;
            
            if(robotState.distanceTelemetreDroit > 30)
            {
                LED_ORANGE = 1 ;
            }
            else
            {
                LED_ORANGE = 0;
            }
            if(robotState.distanceTelemetreGauche > 30)
            {
                LED_BLANCHE = 1 ;
            }
            else
            {
                LED_BLANCHE = 0 ;
            }
            if(robotState.distanceTelemetreCentre > 30)
            {
                LED_BLEUE = 1 ;
            }
            else
            {
                LED_BLEUE = 0 ;
            }
        }
      }
        
    } // fin main


