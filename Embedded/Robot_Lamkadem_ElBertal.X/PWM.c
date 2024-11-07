#include <xc.h> // library xc.h inclut tous les uC
#include "IO.h"
#include "PWM.h"
#include "Robot.h"
#include "Utilities.h"
#include "QEI.h"
#include "asservissement.h"
#include "main.h"

#define PWMPER 24.0
#define FREQ_ECH_QEI 250.0
#define DISTROUES 0.216
#define SPEED_TO_PERCENT 40
float acceleration = 5;
int modeAuto = 0;

void InitPWM(void) {
    PTCON2bits.PCLKDIV = 0b000; //Divide by 1
    PTPER = 100 * PWMPER; //éPriode en pourcentage
    //éRglage PWM moteur 1 sur hacheur 1
    IOCON1bits.PMOD = 0b11; //PWM I/O pin pair is in the True Independent Output mode
    IOCON1bits.PENL = 1;
    IOCON1bits.PENH = 1;
    FCLCON1 = 0x0003; //éDsactive la gestion des faults
    IOCON2bits.PMOD = 0b11; //PWM I/O pin pair is in the True Independent Output mode
    IOCON2bits.PENL = 1;
    IOCON2bits.PENH = 1;
    FCLCON2 = 0x0003; //éDsactive la gestion des faults
    /* Enable PWM Module */
    PTCONbits.PTEN = 1;
}
double talon = 50;

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

void PWMSetSpeedConsigne(float vitesseEnPourcents, int numMoteur) {
    if (modeAuto == 1) {
        if (numMoteur == MOTEUR_GAUCHE) {
            robotState.vitesseGaucheConsigne = -vitesseEnPourcents;
        } else if (numMoteur == MOTEUR_DROIT) {
            robotState.vitesseDroiteConsigne = -vitesseEnPourcents;
        }
    }
}

void PWMUpdateSpeed() {
    // Cette fonction est appelee sur timer et permet de suivre des rampes d acceleration
    if (robotState.vitesseDroiteCommandeCourante < robotState.vitesseDroiteConsigne)
        robotState.vitesseDroiteCommandeCourante = Min(
            robotState.vitesseDroiteCommandeCourante + acceleration,
            robotState.vitesseDroiteConsigne);
    if (robotState.vitesseDroiteCommandeCourante > robotState.vitesseDroiteConsigne)
        robotState.vitesseDroiteCommandeCourante = Max(
            robotState.vitesseDroiteCommandeCourante - acceleration,
            robotState.vitesseDroiteConsigne);
    if (robotState.vitesseDroiteCommandeCourante >= 0) {
        PDC2 = robotState.vitesseDroiteCommandeCourante * PWMPER + talon;
        SDC2 = talon;
    } else {
        PDC2 = talon;
        SDC2 = -robotState.vitesseDroiteCommandeCourante * PWMPER + talon;
    }
    if (robotState.vitesseGaucheCommandeCourante < robotState.vitesseGaucheConsigne)
        robotState.vitesseGaucheCommandeCourante = Min(
            robotState.vitesseGaucheCommandeCourante + acceleration,
            robotState.vitesseGaucheConsigne);
    if (robotState.vitesseGaucheCommandeCourante > robotState.vitesseGaucheConsigne)
        robotState.vitesseGaucheCommandeCourante = Max(
            robotState.vitesseGaucheCommandeCourante - acceleration,
            robotState.vitesseGaucheConsigne);
    if (robotState.vitesseGaucheCommandeCourante > 0) {
        PDC1 = robotState.vitesseGaucheCommandeCourante * PWMPER + talon;
        SDC1 = talon;
    } else {
        PDC1 = talon;
        SDC1 = -robotState.vitesseGaucheCommandeCourante * PWMPER + talon;
    }
}


void PWMSetSpeedCommandePolaire(double vitesseLineaire, double vitesseAngulaire) {
    if (modeAuto == 0) {
        robotState.vitesseDroiteConsigne = -SPEED_TO_PERCENT * (vitesseLineaire + vitesseAngulaire*DISTROUES);
        robotState.vitesseGaucheConsigne = SPEED_TO_PERCENT * (vitesseLineaire - vitesseAngulaire*DISTROUES); // divise distroues par 2 si problème !
        PWMUpdateSpeed();
    }
}