/* 
 * File:   QEI.h
 * Author: TP_EO_6
 *
 * Created on 29 janvier 2024, 14:23
 */

#ifndef QEI_H
#define	QEI_H

void InitQEI1(void);
void InitQEI2(void);
void QEIUpdateData(void);
void SendPositionData(void);

#define FREQ_ECH_QEI 250

#endif	/* QEI_H */

