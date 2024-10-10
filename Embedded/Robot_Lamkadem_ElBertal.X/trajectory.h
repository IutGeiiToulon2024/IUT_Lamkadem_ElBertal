/* 
 * File:   trajectory.h
 * Author: TP_EO_6
 *
 * Created on 9 avril 2024, 17:28
 */

#ifndef TRAJECTORY_H
#define	TRAJECTORY_H

#define VIT_MAX_ANG 3
#define TOLERANCE_ANG 0.5
#define TOLERANCE_DIST 0.1
#define ACCELERATION_LIN 0.1
#define VIT_MAX_LIN 1

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
    float thetaGhost ;
    float incrementAng;
    float accelerationAngulaire;
    float vitesseLineaire;
    float vitesseAngulaire;
    float posX;
    float posY;
    float waypointX ;
    float waypointY ;
    float waypoint ;
    float distanceToTarget ;
    float distance ;
    float lastWaypointX ;
    float lastWaypointY ;
    
    StateGhost state;

} GhostPosition;

extern volatile GhostPosition ghostPosition;

void ghost();
void SendGhost();
void InitTrajectory() ;

#endif	/* TRAJECTORY_H */

