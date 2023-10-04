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

int main(void)
{
    // Initialisation des timers
    InitTimer23();
    InitTimer1();
    /***************************************************************************************************/
    //Initialisation de l?oscillateur

    /****************************************************************************************************/
    InitOscillator();
    InitADC1();
    /****************************************************************************************************/
    // Configuration des éentres sorties
    /****************************************************************************************************/
    InitIO();
    LED_BLANCHE = 1;
    LED_BLEUE = 1;
    LED_ORANGE = 1;

    InitPWM();
    //    PWMSetSpeed(20, MOTEUR_GAUCHE);
    //    PWMSetSpeed(20, MOTEUR_DROIT);
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    unsigned int * result; 
    float val1;
    float val2;
    float val3;
    
    while (1) {
        if (ADCIsConversionFinished()){
            result = ADCGetResult();
            val1 = result[0];
            val2 = result[1];
            val3 = result[2];
            ADCClearConversionFinishedFlag();
            LED_ORANGE = 1;
        }
        
    } // fin main
}

