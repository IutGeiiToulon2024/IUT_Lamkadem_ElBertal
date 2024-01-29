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
#include "main.h"
#include "UART.h"
#include "CB_TX1.h"
#include "CB_RX1.h"
#include "UART_Protocol.h"
#include "QEI.h"
#include <libpic30.h>

unsigned int * result;
int ADCValue0 = 0;
int ADCValue1 = 0;
int ADCValue2 = 0;

int main(void) {

    /***************************************************************************************************/
    //Initialisation de l?oscillateur

    /****************************************************************************************************/
    InitOscillator();
    InitIO();

    /****************************************************************************************************/
    // Configuration des éentres sorties
    /****************************************************************************************************/
    InitTimer23(); // Initialisation des timers
    InitTimer1();
    InitTimer4();
    InitADC1();
    InitPWM();
    InitUART();
    InitQEI1();
    InitQEI2();
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/

    while (1) {

        //        SendMessage((unsigned char*) "Bonjour", 7);
        //        __delay32(40000000);


        //        for (int i = 0; i < CB_RX1_GetDataSize(); i++) {
        //            unsigned char c = CB_RX1_Get();
        //            SendMessage(&c, 1);
        //        }

        //float x = 58;
      //  int diz1, unit1, diz2, unit2, diz3, unit3;
//        diz = (int) x / 10;
//        unit = x - diz * 10;
//          
//
//        unsigned char payload[] = {diz + '0', unit + '0'};
//
//        UartEncodeAndSendMessage(0x0080, 2, payload);
//        __delay32(40000000);
        
        unsigned char payloadTelemetre1[2], payloadTelemetre2[2], payloadTelemetre3[2] ;
        
        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [0])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtremeDroit = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtremeGauche = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            
            if  (robotState.distanceTelemetreDroit > 100)
                 robotState.distanceTelemetreDroit = 100 ;
            if  (robotState.distanceTelemetreGauche > 100)
                 robotState.distanceTelemetreGauche = 100 ;
            if  (robotState.distanceTelemetreCentre > 100)
                 robotState.distanceTelemetreCentre = 100 ;
            // ---------------------------------------------------Envoi des valeurs sur port serie
//            diz1 = (int) robotState.distanceTelemetreDroit / 10;      // Telemetre Droit
//            unit1 = robotState.distanceTelemetreDroit - diz1 * 10;
//            unsigned char payloadTelemetre1[] = {diz1 + '0', unit1 + '0'};
//            UartEncodeAndSendMessage(0x0031, 2, payloadTelemetre1);
//            
//            diz2 = (int) robotState.distanceTelemetreCentre / 10;      // Telemetre Centre
//            unit2 = robotState.distanceTelemetreCentre - diz2 * 10;
//            unsigned char payloadTelemetre2[] = {diz2 + '0', unit2 + '0'};
//            UartEncodeAndSendMessage(0x0032, 2, payloadTelemetre2);
//            
//            diz3 = (int) robotState.distanceTelemetreGauche / 10;      // Telemetre Gauche
//            unit3 = robotState.distanceTelemetreGauche - diz3 * 10;
//            unsigned char payloadTelemetre3[] = {diz3 + '0', unit3 + '0'};
//            UartEncodeAndSendMessage(0x0033, 2, payloadTelemetre3);
            
            payloadTelemetre1[0] = (char)(int)robotState.distanceTelemetreDroit ;
            payloadTelemetre1[1] = (char)((int)robotState.distanceTelemetreDroit >> 8) ;
            
            payloadTelemetre2[0] = (char)(int)robotState.distanceTelemetreGauche ;
            payloadTelemetre2[1] = (char)((int)robotState.distanceTelemetreGauche >> 8) ;
            
            payloadTelemetre3[0] = (char)(int)robotState.distanceTelemetreCentre ;
            payloadTelemetre3[1] = (char)((int)robotState.distanceTelemetreCentre >> 8) ;
            
            UartEncodeAndSendMessage(0x0031, 2, (unsigned char*)payloadTelemetre1) ;
            UartEncodeAndSendMessage(0x0032, 2, (unsigned char*)payloadTelemetre2) ;
            UartEncodeAndSendMessage(0x0033, 2, (unsigned char*)payloadTelemetre3) ;
            
             __delay32(40000000);
            
            
            // --------------------------------------------------Gestion etat des leds
//            if (robotState.distanceTelemetreExtremeDroit < 30) {
//                LED_ORANGE = 1;
//            } else {
//                LED_ORANGE = 0;
//            }
//            if (robotState.distanceTelemetreExtremeGauche < 30) {
//                LED_BLANCHE = 1;
//            } else {
//                LED_BLANCHE = 0;
//            }
//            if (robotState.distanceTelemetreCentre < 30) {
//                LED_BLEUE = 1;
//            } else {
//                LED_BLEUE = 0;
//            }
        }
    }

} // fin main

unsigned char stateRobot;

void OperatingSystemLoop(void) {
    int vitesseD, vitesseG;
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;
        case STATE_AVANCE:
            PWMSetSpeedConsigne(25, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(13, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-7, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(-7, MOTEUR_DROIT);
            PWMSetSpeedConsigne(13, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_RALENTIS:
            if (robotState.distanceTelemetreGauche > 50) {
                vitesseD = 20;
            } else if (robotState.distanceTelemetreGauche < 20) {
                vitesseD = -15;
            } else {
                vitesseD = 1 / 3 * robotState.distanceTelemetreGauche + 10 / 3;
            }

            if (robotState.distanceTelemetreDroit > 50) {
                vitesseG = 20;
            } else if (robotState.distanceTelemetreDroit < 20) {
                vitesseG = -15;
            } else {
                vitesseG = 1 / 3 * robotState.distanceTelemetreDroit + 10 / 3;
            }

            PWMSetSpeedConsigne(vitesseD, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesseG, MOTEUR_GAUCHE);
            stateRobot = STATE_RALENTIS_EN_COURS;
            break;
        case STATE_RALENTIS_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}
unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;
    //éDtermination de la position des obstacles en fonction des ééètlmtres
    if ((robotState.distanceTelemetreExtremeDroit < 25 ||
            robotState.distanceTelemetreDroit < 25) &&
            robotState.distanceTelemetreCentre > 25 &&
            robotState.distanceTelemetreGauche > 25 &&
            robotState.distanceTelemetreExtremeGauche > 25) //Obstacle à droite
        positionObstacle = OBSTACLE_A_DROITE;
    else if (robotState.distanceTelemetreExtremeDroit > 25 &&
            robotState.distanceTelemetreDroit > 25 &&
            robotState.distanceTelemetreCentre > 25 &&
            (robotState.distanceTelemetreGauche < 25 ||
            robotState.distanceTelemetreExtremeGauche < 25)) //Obstacle à gauche
        positionObstacle = OBSTACLE_A_GAUCHE;
    else if (robotState.distanceTelemetreCentre < 25) //Obstacle en face
        positionObstacle = OBSTACLE_EN_FACE;
    else if ((robotState.distanceTelemetreGauche < 50) || (robotState.distanceTelemetreDroit < 50))
        positionObstacle = OBSTACLE_D_G;
    else if (robotState.distanceTelemetreExtremeDroit > 25 &&
            robotState.distanceTelemetreDroit > 25 &&
            robotState.distanceTelemetreCentre > 30 &&
            robotState.distanceTelemetreGauche > 25 &&
            robotState.distanceTelemetreExtremeGauche > 25) //pas d?obstacle
        positionObstacle = PAS_D_OBSTACLE;
    //éDtermination de lé?tat àvenir du robot
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_D_G)
        nextStateRobot = STATE_RALENTIS;
    else if (positionObstacle == OBSTACLE_EN_FACE) {
        if (robotState.distanceTelemetreExtremeGauche > robotState.distanceTelemetreExtremeDroit) {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        } else {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        }
    }
    //Si l?on n?est pas dans la transition de lé?tape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}
