/* 
 * File:   UART_Protocol.h
 * Author: TP_EO_6
 *
 * Created on 15 janvier 2024, 08:19
 */

#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

#define WAITING 0
#define FUNCTIONMSB 1
#define FUNCTIONLSB 2
#define PAYLOADLENGTHMSB 3
#define PAYLOADLENGTHLSB 4
#define PAYLOAD 5
#define CHECKSUM 6


#define LED1 0x0021
#define CONFIGPIDX 0x0091
#define CONFIGPIDTHETA 0x0092

unsigned char UartCalculateChecksum(int, int, unsigned char*) ;
void UartEncodeAndSendMessage(int, int, unsigned char*) ;
void UartDecodeMessage(unsigned char) ;
void UartProcessDecodedMessage(int, int, unsigned char*) ;

#endif	/* UART_PROTOCOL_H */

