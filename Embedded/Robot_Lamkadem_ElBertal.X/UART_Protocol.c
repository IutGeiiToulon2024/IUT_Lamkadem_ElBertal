#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"
#include "IO.h"
#include "Utilities.h"
#include "robot.h"
#include "asservissement.h"
#include "trajectory.h"
#include <math.h>

unsigned char UartCalculateChecksum(int msgFunction,
        int msgPayloadLength, unsigned char* msgPayload) {
    unsigned char checksum = 0;
    checksum ^= 0xFE;
    checksum ^= (unsigned char) (msgFunction >> 8);
    checksum ^= (unsigned char) msgFunction;
    checksum ^= (unsigned char) (msgPayloadLength >> 8);
    checksum ^= (unsigned char) msgPayloadLength;
    for (int n = 0; n < msgPayloadLength; n++)
        checksum ^= msgPayload[n];
    return checksum;
}

void UartEncodeAndSendMessage(int msgFunction,
        int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction d?encodage et d?envoi d?un message
    int taille = 6 + msgPayloadLength;
    unsigned char trame [taille];
    trame[0] = 0xFE;
    trame[1] = (unsigned char) (msgFunction >> 8);
    trame[2] = (unsigned char) (msgFunction);
    trame[3] = (unsigned char) (msgPayloadLength >> 8);
    trame[4] = (unsigned char) (msgPayloadLength);
    for (int i = 0; i < msgPayloadLength; i++)
        trame[5 + i] = msgPayload[i];
    trame[taille - 1] = UartCalculateChecksum(msgFunction,
            msgPayloadLength, msgPayload);
    SendMessage(trame, taille);

}
int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;
unsigned char calculatedChecksum = 0;

int rcvState = WAITING;

void UartDecodeMessage(unsigned char c) {
    //Fonction prenant en entree un octet et servant a reconstituer les trames
    switch (rcvState) {
        case WAITING:
            if (c == 0xFE) {
                msgDecodedPayloadIndex = 0;
                rcvState = FUNCTIONMSB;
            }
            break;
        case FUNCTIONMSB:
            msgDecodedFunction = c << 8;
            rcvState = FUNCTIONLSB;
            break;
        case FUNCTIONLSB:
            msgDecodedFunction |= c;
            rcvState = PAYLOADLENGTHMSB;
            break;
        case PAYLOADLENGTHMSB:
            msgDecodedPayloadLength = c << 8;
            rcvState = PAYLOADLENGTHLSB;
            break;
        case PAYLOADLENGTHLSB:
            msgDecodedPayloadLength |= c;
            rcvState = PAYLOAD;
            break;
        case PAYLOAD:
            msgDecodedPayload[msgDecodedPayloadIndex++] = c;
            if (msgDecodedPayloadIndex == msgDecodedPayloadLength) {
                rcvState = CHECKSUM;
                msgDecodedPayloadIndex = 0;
            }
            break;
        case CHECKSUM:
            calculatedChecksum = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

            if (calculatedChecksum == c) {
                //Success, on a un message valide
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            }
            rcvState = WAITING;
            break;
            //        default:
            //            rcvState = WAITING;
            //break;
    }
}


float correcteurKp, correcteurKd, correcteurKi, consigneLineaire, limitPX, limitIX, limitDX;
float correcteurThetaKp, correcteurThetaKd, correcteurThetaKi, consigneAngulaire, limitPTheta, limitITheta, limitDTheta;

void UartProcessDecodedMessage(int function,
        int payloadLength, unsigned char* payload) {
    //Fonction appelee apres le decodage pour executer l?action
    switch (function) { //correspondant au message recu
        case LED1:
            if (payload[0] == 1)
                LED_BLEUE = 1;
            else
                LED_BLEUE = 0;
            break;

        case CONFIGPIDX:
            correcteurKp = getFloat(payload, 0);
            correcteurKd = getFloat(payload, 4);
            correcteurKi = getFloat(payload, 8);
            limitPX = getFloat(payload, 12);
            limitIX = getFloat(payload, 16);
            limitDX = getFloat(payload, 20);

            getBytesFromFloat((unsigned char*) robotState.correcteursXPayload, 0, (float) (correcteurKp));
            getBytesFromFloat((unsigned char*) robotState.correcteursXPayload, 4, (float) (correcteurKd));
            getBytesFromFloat((unsigned char*) robotState.correcteursXPayload, 8, (float) (correcteurKi));
            getBytesFromFloat((unsigned char*) robotState.correcteursXPayload, 12, (float) (limitPX));
            getBytesFromFloat((unsigned char*) robotState.correcteursXPayload, 16, (float) (limitIX));
            getBytesFromFloat((unsigned char*) robotState.correcteursXPayload, 20, (float) (limitDX));
            
            SetupPidAsservissement(&robotState.PidX, (double)correcteurKp, (double)correcteurKi, (double)correcteurKd,(double)limitPX, (double)limitPX, (double)limitPX);
            break;

        case CONFIGPIDTHETA:
            correcteurThetaKp = getFloat(payload, 0);
            correcteurThetaKd = getFloat(payload, 4);
            correcteurThetaKi = getFloat(payload, 8);
            limitPTheta = getFloat(payload, 12);
            limitITheta = getFloat(payload, 16);
            limitDTheta = getFloat(payload, 20);
            
            getBytesFromFloat((unsigned char*) robotState.correcteursThetaPayload, 0, (float) (correcteurThetaKp));
            getBytesFromFloat((unsigned char*) robotState.correcteursThetaPayload, 4, (float) (correcteurThetaKd));
            getBytesFromFloat((unsigned char*) robotState.correcteursThetaPayload, 8, (float) (correcteurThetaKi));
            getBytesFromFloat((unsigned char*) robotState.correcteursThetaPayload, 12, (float) (limitPTheta));
            getBytesFromFloat((unsigned char*) robotState.correcteursThetaPayload, 16, (float) (limitITheta));
            getBytesFromFloat((unsigned char*) robotState.correcteursThetaPayload, 20, (float) (limitDTheta));
            
            SetupPidAsservissement(&robotState.PidTheta, (double)correcteurThetaKp, (double)correcteurThetaKi, (double)correcteurThetaKd,(double)limitPTheta, (double)limitITheta, (double)limitDTheta);
            break;
            
        case CONSIGNES: 
            //consigneAngulaire = getFloat(payload, 0);
            consigneAngulaire = 0 ;
            consigneLineaire = getFloat(payload, 4);
            getBytesFromFloat((unsigned char*) robotState.consignes, 0, (float) (consigneAngulaire));
            getBytesFromFloat((unsigned char*) robotState.consignes, 4, (float) (consigneLineaire));
            
            robotState.consigneVitesseAngulaire = consigneAngulaire;
            robotState.consigneVitesseLineaire = consigneLineaire;
            break;
            
        case RECEIVE_POS_GHOST:
            ghostPosition.waypointX = getFloat(payload, 0);
            ghostPosition.waypointY = getFloat(payload, 4);
            ghostPosition.thetaWaypoint = atan2(ghostPosition.posY , ghostPosition.posX) ; // voir comment trouver l'angle de la cible et l'angle retant. a quoi sa correspnd et comment les determiner (photo 12/04))
            
            break;
            
        default:
            break;
    }
}



//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/