#include <xc.h> // library xc.h inclut tous les uC
#include "IO.h"
#include "PWM.h"
#include "Robot.h"
#include "Utilities.h"
#include "QEI.h"
#include "asservissement.h"
#include "main.h"

#define PWMPER 40.0
#define FREQ_ECH_QEI 250.0
#define DISTROUES 0.281 
#define SPEED_TO_PERCENT 40
float acceleration = 5;
int modeAuto = 0;

void InitPWM(void) {
    PTCON2bits.PCLKDIV = 0b000; //Divide by 1
    PTPER = 100 * PWMPER; //Période en pourcentage

    //Réglage PWM moteur 1 sur hacheur 1
    IOCON1bits.POLH = 1; //High = 1 and active on low =0
    IOCON1bits.POLL = 1; //High = 1
    IOCON1bits.PMOD = 0b01; //Set PWM Mode to Redundant
    FCLCON1 = 0x0003; //Désactive la gestion des faults

    //Reglage PWM moteur 2 sur hacheur 6
    IOCON6bits.POLH = 1; //High = 1
    IOCON6bits.POLL = 1; //High = 1
    IOCON6bits.PMOD = 0b01; //Set PWM Mode to Redundant
    FCLCON6 = 0x0003; //Désactive la gestion des faults

    /* Enable PWM Module */
    PTCONbits.PTEN = 1;
}

//void PWMSetSpeed(float vitesseEnPourcents, int numMoteur)
//{
//    if(numMoteur == 0)
//    {
//        robotState.vitesseGaucheCommandeCourante = vitesseEnPourcents;
//        if(vitesseEnPourcents >= 0)
//            {
//                MOTEUR_GAUCHE_L_PWM_ENABLE = 0; //Pilotage de la pin en mode IO
//                MOTEUR_GAUCHE_L_IO_OUTPUT = 1;
//                MOTEUR_GAUCHE_H_PWM_ENABLE = 1;
//            }
//            else
//            {
//                MOTEUR_GAUCHE_L_PWM_ENABLE = 1; //Pilotage de la pin en mode IO
//                MOTEUR_GAUCHE_H_IO_OUTPUT = 1; //Mise à 1 de la pin
//                MOTEUR_GAUCHE_H_PWM_ENABLE = 0;
//            }
//        MOTEUR_GAUCHE_DUTY_CYCLE = Abs(robotState.vitesseGaucheCommandeCourante*PWMPER);
//    }
//    else
//    {
//        if(numMoteur == 1)
//        {
//            robotState.vitesseDroiteCommandeCourante = vitesseEnPourcents;
//            if (robotState.vitesseDroiteCommandeCourante > 0)
//            {
//                MOTEUR_DROIT_H_PWM_ENABLE = 0; //pilotage de la pin en mode IO
//                MOTEUR_DROIT_H_IO_OUTPUT = 1; //Mise à 1 de la pin
//                MOTEUR_DROIT_L_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
//            }
//            else
//            {
//                MOTEUR_DROIT_L_PWM_ENABLE = 0; //pilotage de la pin en mode IO
//                MOTEUR_DROIT_L_IO_OUTPUT = 1; //Mise à 1 de la pin
//                MOTEUR_DROIT_H_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
//            }
//            MOTEUR_DROIT_DUTY_CYCLE = Abs(robotState.vitesseDroiteCommandeCourante*PWMPER);
//        }
//    }
//}

//void PWMSetSpeedConsigne(float vitesseEnPourcents, int numMoteur) {
//    if (modeAuto == 1) {
//        if (numMoteur == MOTEUR_GAUCHE) {
//            robotState.vitesseGaucheConsigne = vitesseEnPourcents;
//        } else if (numMoteur == MOTEUR_DROIT) {
//            robotState.vitesseDroiteConsigne = vitesseEnPourcents;
//        }
//    }
//}

void PWMUpdateSpeed() {
    // Cette fonction est éappele sur timer et permet de suivre des rampes d?ééacclration

    if (robotState.vitesseDroiteCommandeCourante < robotState.vitesseDroiteConsigne)
        robotState.vitesseDroiteCommandeCourante = Min(robotState.vitesseDroiteCommandeCourante + acceleration,
            robotState.vitesseDroiteConsigne);

    if (robotState.vitesseDroiteCommandeCourante > robotState.vitesseDroiteConsigne)
        robotState.vitesseDroiteCommandeCourante = Max(robotState.vitesseDroiteCommandeCourante - acceleration,
            robotState.vitesseDroiteConsigne);

    if (robotState.vitesseDroiteCommandeCourante > 0) {
        MOTEUR_DROIT_H_PWM_ENABLE = 0; //pilotage de la pin en mode IO
        MOTEUR_DROIT_H_IO_OUTPUT = 1; //Mise à1 de la pin
        MOTEUR_DROIT_L_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
    } else {
        MOTEUR_DROIT_L_PWM_ENABLE = 0; //pilotage de la pin en mode IO
        MOTEUR_DROIT_L_IO_OUTPUT = 1; //Mise à1 de la pin
        MOTEUR_DROIT_H_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
    }
    MOTEUR_DROIT_DUTY_CYCLE = Abs(robotState.vitesseDroiteCommandeCourante) * PWMPER;
    if (robotState.vitesseGaucheCommandeCourante < robotState.vitesseGaucheConsigne)
        robotState.vitesseGaucheCommandeCourante = Min(robotState.vitesseGaucheCommandeCourante + acceleration,
            robotState.vitesseGaucheConsigne);

    if (robotState.vitesseGaucheCommandeCourante > robotState.vitesseGaucheConsigne)
        robotState.vitesseGaucheCommandeCourante = Max(robotState.vitesseGaucheCommandeCourante - acceleration,
            robotState.vitesseGaucheConsigne);

    if (robotState.vitesseGaucheCommandeCourante > 0) {
        MOTEUR_GAUCHE_L_PWM_ENABLE = 0; //pilotage de la pin en mode IO
        MOTEUR_GAUCHE_L_IO_OUTPUT = 1; //Mise à1 de la pin
        MOTEUR_GAUCHE_H_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
    } else {
        MOTEUR_GAUCHE_H_PWM_ENABLE = 0; //pilotage de la pin en mode IO
        MOTEUR_GAUCHE_H_IO_OUTPUT = 1; //Mise à1 de la pin
        MOTEUR_GAUCHE_L_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
    }
    MOTEUR_GAUCHE_DUTY_CYCLE = Abs(robotState.vitesseGaucheCommandeCourante) * PWMPER;

}

void PWMSetSpeedCommandePolaire(double vitesseLineaire, double vitesseAngulaire) {
    if (modeAuto == 0) {
        robotState.vitesseDroiteConsigne = SPEED_TO_PERCENT * (vitesseLineaire + vitesseAngulaire*DISTROUES);
        robotState.vitesseGaucheConsigne = SPEED_TO_PERCENT * (vitesseLineaire - vitesseAngulaire*DISTROUES);
        PWMUpdateSpeed();
    }
}