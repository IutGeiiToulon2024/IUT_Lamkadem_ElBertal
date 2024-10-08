/* 
 * File:   QEI.h
 * Author: TP_EO_6
 *
 * Created on 29 janvier 2024, 14:23
 */

#ifndef QEI_H
#define	QEI_H

#define FREQ_ECH_QEI 250.0

void InitQEI1(void);
void InitQEI2(void);
void QEIUpdateData(void);
void SendPositionData(void);


#endif	/* QEI_H */

