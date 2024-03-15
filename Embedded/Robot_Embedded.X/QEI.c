#include <xc.h>
#include "QEI.h"
#include "robot.h"
#include "IO.h"
#include "Utilities.h"
#include "main.h"
#include "timer.h"
#include "UART_Protocol.h"
#include <math.h>  // <cmath> en C++

#define DISTROUES 281.2
//#define PI 3.14159265358979323846
#define POSITION_DATA 0x0061
#define MESURE_VIT 0x0062

double delta_d, delta_g, QeiDroitPosition_T_1, QeiDroitPosition, QeiGauchePosition_T_1, QeiGauchePosition;

void InitQEI1() {
    QEI1IOCbits.SWPAB = 1; //QEAx and QEBx are swapped
    QEI1GECL = 0xFFFF;
    QEI1GECH = 0xFFFF;
    QEI1CONbits.QEIEN = 1; // Enable QEI Module
}

void InitQEI2() {
    QEI2IOCbits.SWPAB = 1; //QEAx and QEBx are not swapped
    QEI2GECL = 0xFFFF;
    QEI2GECH = 0xFFFF;
    QEI2CONbits.QEIEN = 1; // Enable QEI Module
}

void QEIUpdateData() {
    //On sauvegarde les anciennes valeurs
    QeiDroitPosition_T_1 = QeiDroitPosition;
    QeiGauchePosition_T_1 = QeiGauchePosition;
    //On actualise les valeurs des positions
    long QEI1RawValue = POS1CNTL;
    QEI1RawValue += ((long) POS1HLD << 16);
    long QEI2RawValue = POS2CNTL;
    QEI2RawValue += ((long) POS2HLD << 16);
    //Conversion en mm (regle pour la taille des roues codeuses)
    QeiDroitPosition =  0.00001620*QEI1RawValue;
    QeiGauchePosition = -0.00001620*QEI2RawValue;
    //Calcul des deltas de position
    delta_d = QeiDroitPosition - QeiDroitPosition_T_1;
    delta_g = QeiGauchePosition - QeiGauchePosition_T_1;
    //Calcul des vitesses
    //attention a remultiplier par la frequence d echantillonnage
    robotState.vitesseDroitFromOdometry = delta_d*FREQ_ECH_QEI;
    robotState.vitesseGaucheFromOdometry = delta_g*FREQ_ECH_QEI;
    robotState.vitesseLineaireFromOdometry = (delta_d * FREQ_ECH_QEI + delta_g * FREQ_ECH_QEI) / 2;
    robotState.vitesseAngulaireFromOdometry = (delta_d * FREQ_ECH_QEI - delta_g * FREQ_ECH_QEI) / DISTROUES;
    //Mise a jour du positionnement terrain a t-1
    robotState.xPosFromOdometry_1 = robotState.xPosFromOdometry;
    robotState.yPosFromOdometry_1 = robotState.yPosFromOdometry;
    robotState.angleRadianFromOdometry_1 = robotState.angleRadianFromOdometry;
    //Calcul des positions dans le referentiel du terrain
    robotState.angleRadianFromOdometry = robotState.vitesseAngulaireFromOdometry * 1 / FREQ_ECH_QEI + robotState.angleRadianFromOdometry_1;
    robotState.xPosFromOdometry = robotState.xPosFromOdometry_1 + robotState.vitesseLineaireFromOdometry * cosf((float) (robotState.angleRadianFromOdometry_1 + robotState.angleRadianFromOdometry) / 2) * 1 / FREQ_ECH_QEI;
    robotState.yPosFromOdometry = robotState.yPosFromOdometry_1 + robotState.vitesseLineaireFromOdometry * sinf((float) (robotState.angleRadianFromOdometry_1 + robotState.angleRadianFromOdometry) / 2) * 1 / FREQ_ECH_QEI;

    if (robotState.angleRadianFromOdometry > PI)
        robotState.angleRadianFromOdometry -= 2 * PI;
    if (robotState.angleRadianFromOdometry < -PI)
        robotState.angleRadianFromOdometry += 2 * PI;
}

void SendPositionData() {
    unsigned char positionPayload[24] ;
    unsigned char measureVitesse[8];
    
    getBytesFromInt32(positionPayload, 0, timestamp);
    getBytesFromFloat(positionPayload, 4, (float) (robotState.xPosFromOdometry));
    getBytesFromFloat(positionPayload, 8, (float) (robotState.yPosFromOdometry));
    getBytesFromFloat(positionPayload, 12, (float) (robotState.angleRadianFromOdometry));
    getBytesFromFloat(positionPayload, 16, (float) (robotState.vitesseLineaireFromOdometry));
    getBytesFromFloat(positionPayload, 20, (float) (robotState.vitesseAngulaireFromOdometry));
    
    getBytesFromFloat(measureVitesse, 0, (float) (robotState.vitesseDroitFromOdometry));
    getBytesFromFloat(measureVitesse, 4, (float) (robotState.vitesseGaucheFromOdometry));
    
    UartEncodeAndSendMessage(POSITION_DATA, 24, positionPayload);
    UartEncodeAndSendMessage(MESURE_VIT , 8 , measureVitesse) ;
}