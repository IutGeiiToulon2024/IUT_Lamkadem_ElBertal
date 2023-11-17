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


    InitADC1();
    InitTimer23(); // Initialisation des timers
    InitTimer1();
    InitTimer4();
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


            if (robotState.distanceTelemetreExtremeDroit > 30) {
                LED_ORANGE = 1;
            } else {
                LED_ORANGE = 0;
            }
            if (robotState.distanceTelemetreExtremeGauche > 30) {
                LED_BLANCHE = 1;
            } else {
                LED_BLANCHE = 0;
            }
            if (robotState.distanceTelemetreCentre > 30) {
                LED_BLEUE = 1;
            } else {
                LED_BLEUE = 0;
            }
        }
    }

} // fin main

unsigned char stateRobot;

void OperatingSystemLoop(void)
{
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
            PWMSetSpeedConsigne(18, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(18, MOTEUR_GAUCHE);
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
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}
unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode()
{
    unsigned char positionObstacle = PAS_D_OBSTACLE;
    //éDtermination de la position des obstacles en fonction des ééètlmtres
    if ((robotState.distanceTelemetreExtremeDroit < 35 ||
            robotState.distanceTelemetreDroit < 45) &&
            robotState.distanceTelemetreCentre > 35 &&
            robotState.distanceTelemetreGauche > 45 &&
            robotState.distanceTelemetreExtremeGauche > 35) //Obstacle à droite
        positionObstacle = OBSTACLE_A_DROITE;
    else if (robotState.distanceTelemetreExtremeDroit > 35 &&
            robotState.distanceTelemetreDroit > 45 &&
            robotState.distanceTelemetreCentre > 35 &&
            (robotState.distanceTelemetreGauche < 45 ||
            robotState.distanceTelemetreExtremeGauche < 35)) //Obstacle à gauche
        positionObstacle = OBSTACLE_A_GAUCHE;
    else if (robotState.distanceTelemetreCentre < 35) //Obstacle en face
        positionObstacle = OBSTACLE_EN_FACE;
    else if (robotState.distanceTelemetreExtremeDroit > 35 &&
            robotState.distanceTelemetreDroit > 45 &&
            robotState.distanceTelemetreCentre > 35 &&
            robotState.distanceTelemetreGauche > 45 &&
            robotState.distanceTelemetreExtremeGauche > 35) //pas d?obstacle
        positionObstacle = PAS_D_OBSTACLE;
    //éDtermination de lé?tat àvenir du robot
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_EN_FACE)
        if(robotState.distanceTelemetreExtremeGauche > robotState.distanceTelemetreExtremeGauche)
        {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        }
        else
        {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE ;
        }
        
    //Si l?on n?est pas dans la transition de lé?tape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}
