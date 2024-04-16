/* 
 * File:   trajectory.h
 * Author: TP_EO_6
 *
 * Created on 9 avril 2024, 17:28
 */

#ifndef TRAJECTORY_H
#define	TRAJECTORY_H

#define VitesseMaxAngulaire 3
#define ToleranceAngulaire 0.5

typedef enum {
    IDLE,
    ROTATION,
    DEPLACEMENTLINEAIRE,
} StateGhost;

typedef struct {
    float thetaWaypoint;
    float thetaRobot;
    float thetaRestant;
    float thetaArret;
    float accelerationAngulaire;
    float vitesseLineaire;
    float vitesseAngulaire;
    float posX;
    float posY;

    StateGhost state;

} GhostPosition;

void ghost();
void SendGhost();

#endif	/* TRAJECTORY_H */

