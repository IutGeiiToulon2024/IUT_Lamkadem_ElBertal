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

int main(void)
{
    // Initialisation des timers
    InitTimer23();
    InitTimer1();
    /***************************************************************************************************/
    //Initialisation de l?oscillateur

    /****************************************************************************************************/
    InitOscillator();
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
    while (1) {
        //    LED_BLANCHE = !LED_BLANCHE;
        //    LED_BLEUE = !LED_BLEUE;
        //    LED_ORANGE = !LED_ORANGE;
    } // fin main
}

