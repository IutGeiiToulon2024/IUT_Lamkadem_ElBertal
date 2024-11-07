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
#include "asservissement.h"
#include "Utilities.h"
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
    // Configuration des �entres sorties
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
    
    InitTrajectoryGenerator();
    
    SetupPidAsservissement(&robotState.PidX, 2.9f, 39.0f, 0.0f,100.0f, 100.0f, 100.0f);
    SetupPidAsservissement(&robotState.PidTheta, 3.1f, 42.0f, 0.0f,100.0f, 100.0f, 100.0f);

    int subSamplingSendCounter = 0;
    while (1) {
        unsigned char payloadTelemetre1[2], payloadTelemetre2[2], payloadTelemetre3[2], payloadTelemetre4[2], payloadTelemetre5[2];
        unsigned char payloadVitesse[8];

        while (CB_RX1_IsDataAvailable()) {
            UartDecodeMessage(CB_RX1_Get());
        }
        //        char payloadAsservissementX[24], payloadAsservissementTheta[24];

        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [3])* 3.3 / 4096 ;
            robotState.distanceTelemetreExtremeDroit = 34 / volts - 5;
            volts = ((float) result [0])* 3.3 / 4096 ;
            robotState.distanceTelemetreExtremeGauche = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096 ;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096 ;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096 ;
            robotState.distanceTelemetreDroit = 34 / volts - 5;

            if (robotState.distanceTelemetreDroit > 100)
                robotState.distanceTelemetreDroit = 100;
            if (robotState.distanceTelemetreGauche > 100)
                robotState.distanceTelemetreGauche = 100;
            if (robotState.distanceTelemetreCentre > 100)
                robotState.distanceTelemetreCentre = 100;
            if (robotState.distanceTelemetreExtremeGauche > 100)
                robotState.distanceTelemetreExtremeGauche = 100;
            if (robotState.distanceTelemetreExtremeDroit > 100)
                robotState.distanceTelemetreExtremeDroit = 100;
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

            subSamplingSendCounter++;
            if (subSamplingSendCounter >= 10) {
                payloadTelemetre1[0] = (char) (int) robotState.distanceTelemetreDroit;
                payloadTelemetre1[1] = (char) ((int) robotState.distanceTelemetreDroit >> 8);

                payloadTelemetre2[0] = (char) (int) robotState.distanceTelemetreGauche;
                payloadTelemetre2[1] = (char) ((int) robotState.distanceTelemetreGauche >> 8);

                payloadTelemetre3[0] = (char) (int) robotState.distanceTelemetreCentre;
                payloadTelemetre3[1] = (char) ((int) robotState.distanceTelemetreCentre >> 8);

                payloadTelemetre4[0] = (char) (int) robotState.distanceTelemetreExtremeDroit;
                payloadTelemetre4[1] = (char) ((int) robotState.distanceTelemetreExtremeDroit >> 8);

                payloadTelemetre5[0] = (char) (int) robotState.distanceTelemetreExtremeGauche;
                payloadTelemetre5[1] = (char) ((int) robotState.distanceTelemetreExtremeGauche >> 8);

                UartEncodeAndSendMessage(0x0031, 2, (unsigned char*) payloadTelemetre1);
                UartEncodeAndSendMessage(0x0032, 2, (unsigned char*) payloadTelemetre2);
                UartEncodeAndSendMessage(0x0033, 2, (unsigned char*) payloadTelemetre3);
                UartEncodeAndSendMessage(0x0034, 2, (unsigned char*) payloadTelemetre4);
                UartEncodeAndSendMessage(0x0035, 2, (unsigned char*) payloadTelemetre5);


                getBytesFromFloat(payloadVitesse, 0, robotState.vitesseDroiteConsigne);
                getBytesFromFloat(payloadVitesse, 4, robotState.vitesseGaucheConsigne);

                UartEncodeAndSendMessage(0x0041, 8, (unsigned char*) payloadVitesse);

                UartEncodeAndSendMessage(CONFIGPIDX, 24, (unsigned char*) robotState.correcteursXPayload);
                UartEncodeAndSendMessage(CONFIGPIDTHETA, 24, (unsigned char*) robotState.correcteursThetaPayload);
                UartEncodeAndSendMessage(CONSIGNES, 8, (unsigned char*) robotState.consignes);

                SendPidX();
                SendPidTheta();

                SendCommandeErreur();

                subSamplingSendCounter = 0;
            }

            //__delay32(40000000);


            // --------------------------------------------------Gestion etat des leds
            if (robotState.distanceTelemetreExtremeDroit < 20) {
                LED_VERTE_1 = 1;
            } else {
                LED_VERTE_1 = 0;
            }
            if (robotState.distanceTelemetreDroit < 20) {
                LED_ROUGE_1 = 1;
            } else {
                LED_ROUGE_1 = 0;
            }
            if (robotState.distanceTelemetreCentre < 20) {
                LED_ORANGE_1 = 1;
            } else {
                LED_ORANGE_1 = 0;
            }
            if (robotState.distanceTelemetreGauche < 20) {
                LED_BLEUE_1 = 1;
            } else {
                LED_BLEUE_1 = 0;
            }
            if (robotState.distanceTelemetreExtremeGauche < 20) {
                LED_BLANCHE_1 = 1;
            } else {
                LED_BLANCHE_1 = 0;
            }
        }
    }
} // fin main

unsigned char stateRobot;

void OperatingSystemLoop(void) {
    int vitesseD, vitesseG;
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            //PWMSetSpeedCommandePolaire(0, 0);
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;
        case STATE_AVANCE:
            //PWMSetSpeedCommandePolaire(1, 0);
            PWMSetSpeedConsigne(25, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_GAUCHE:
            //PWMSetSpeedCommandePolaire(0.5, 2);
            PWMSetSpeedConsigne(13, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-7, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE:
            //PWMSetSpeedCommandePolaire(0.5, -2);
            PWMSetSpeedConsigne(-7, MOTEUR_DROIT);
            PWMSetSpeedConsigne(13, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            //                robotState.consigneVitesseLineaire = 0;
            //                robotState.consigneVitesseAngulaire = -3;            
            //PWMSetSpeedCommandePolaire(0, 2);
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE:
            //PWMSetSpeedCommandePolaire(0, -2);
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
            //PWMSetSpeedCommandePolaire(0, 0);
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
    //�Dtermination de la position des obstacles en fonction des ���tlmtres
    if ((robotState.distanceTelemetreExtremeDroit < 25 ||
            robotState.distanceTelemetreDroit < 25) &&
            robotState.distanceTelemetreCentre > 25 &&
            robotState.distanceTelemetreGauche > 25 &&
            robotState.distanceTelemetreExtremeGauche > 28) //Obstacle � droite
        positionObstacle = OBSTACLE_A_DROITE;
    else if (robotState.distanceTelemetreExtremeDroit > 25 &&
            robotState.distanceTelemetreDroit > 25 &&
            robotState.distanceTelemetreCentre > 25 &&
            (robotState.distanceTelemetreGauche < 25 ||
            robotState.distanceTelemetreExtremeGauche < 28)) //Obstacle � gauche
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
    //�Dtermination de l�?tat �venir du robot
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
    //Si l?on n?est pas dans la transition de l�?tape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}
