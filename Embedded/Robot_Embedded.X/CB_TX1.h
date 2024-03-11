/* 
 * File:   CB_TX1.h
 * Author: TP_EO_6
 *
 * Created on 12 décembre 2023, 09:50
 */

#ifndef CB_TX1_H
#define	CB_TX1_H

void SendMessage(unsigned char* message, int length);
int CB_TX1_GetRemainingSize(void) ;
void CB_TX1_Add(unsigned char) ;
unsigned char CB_TX1_Get(void) ;
void SendOne(void)  ;
unsigned char CB_TX1_IsTranmitting(void) ;
int CB_TX1_GetDataSize(void) ;

#endif	/* CB_TX1_H */

