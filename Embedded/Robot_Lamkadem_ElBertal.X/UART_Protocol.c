#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"
#include "IO.h"
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
unsigned char calculatedChecksum = 0 ;

int rcvState ;

void UartDecodeMessage(unsigned char c) {
    //Fonction prenant en entree un octet et servant a reconstituer les trames
    rcvState = WAITING ;
    switch (rcvState)
    {
        case WAITING:
            if (c == 0xFE)
                rcvState = FUNCTIONMSB;
            break;
        case FUNCTIONMSB:
            msgDecodedFunction = c << 8 ;
            rcvState = FUNCTIONLSB;
            break;
        case FUNCTIONLSB:
            msgDecodedFunction |= c ;
            rcvState |= PAYLOADLENGTHMSB;
            break;
        case PAYLOADLENGTHMSB:
            msgDecodedPayloadLength = c << 8 ;
            rcvState = PAYLOADLENGTHLSB;
            break;
        case PAYLOADLENGTHLSB:
            msgDecodedPayloadLength |= c ;
            rcvState = PAYLOAD;
            break;
        case PAYLOAD:
            msgDecodedPayload[msgDecodedPayloadIndex ++] = c ;
            if (msgDecodedPayloadIndex == msgDecodedPayloadLength)
            {
                rcvState = CHECKSUM;
                msgDecodedPayloadIndex = 0;
            }           
            break;
        case CHECKSUM:
            calculatedChecksum = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            
            if (calculatedChecksum == c)
            {
                //Success, on a un message valide
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            }
            
            rcvState = WAITING;
            break;
        default:
            rcvState = WAITING;
            break;
    }
}

    void UartProcessDecodedMessage(int function,
        int payloadLength, unsigned char* payload) {
                                                                                                    //Fonction appelee apres le decodage pour executer l?action
        switch(function){                                                                                            //correspondant au message recu
            case (int)LED1 :
                if (payload[0]==1)
                    LED_BLEUE = 1;
                else
                    LED_BLEUE = 0;
                break ;
        }
}



//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/