/* 
 * File:   PWM.h
 * Author: TP_EO_6
 *
 * Created on 27 septembre 2023, 08:38
 */

#ifndef PWM_H
#define	PWM_H
#define MOTEUR_DROIT 1
#define MOTEUR_GAUCHE 0
void initPWM(void);
// void PWMSetSpeed(float, int);
void PWMUpdateSpeed();
void PWMSetSpeedConsigne(float, int);
#endif	/* PWM_H */

