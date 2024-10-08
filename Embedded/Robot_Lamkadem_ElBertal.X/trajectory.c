#include "trajectory.h"
#include "robot.h"
#include "Utilities.h"
#include "UART_Protocol.h"
#include "math.h"
#include "timer.h"
#include "QEI.h"

volatile GhostPosition ghostPosition;

void InitTrajectory(void) {
    ghostPosition.posX = 0.0;
    ghostPosition.posY = 0.0;
    ghostPosition.thetaRobot = 0.0;
    ghostPosition.vitesseLineaire = 0.0;
    ghostPosition.vitesseAngulaire = 0.0;
    ghostPosition.waypointX = 0.0;
    ghostPosition.waypointY = 0.0;
    ghostPosition.thetaWaypoint = 0.0;
    ghostPosition.distanceToTarget = 0.0;
    ghostPosition.state = IDLE;
}

double lastUpdateTime = 0 ;
double deltaTime = 0 ;

void ghost() {
    switch (ghostPosition.state) {
        case IDLE:
            ghostPosition.vitesseLineaire = 0;
            ghostPosition.vitesseAngulaire = 0;
            break;

        case ROTATION:
            ghostPosition.thetaRestant = ghostPosition.thetaWaypoint - ModuloByAngle(ghostPosition.thetaWaypoint, ghostPosition.thetaRobot);
            ghostPosition.thetaArret = pow(ghostPosition.vitesseAngulaire, 2) / (2 * ghostPosition.accelerationAngulaire);
            ghostPosition.incrementAng = ghostPosition.vitesseAngulaire * 1/FREQ_ECH_QEI;
            if (((ghostPosition.thetaRestant >= 0 && ghostPosition.thetaArret >= 0) || (ghostPosition.thetaRestant <= 0 && ghostPosition.thetaArret <= 0))
                    && Abs(ghostPosition.thetaRestant) >= Abs(ghostPosition.thetaArret))   {
                if (ghostPosition.thetaRestant > 0){
                    if(ghostPosition.thetaRestant < ghostPosition.thetaArret)
                    {
                        if (ghostPosition.vitesseAngulaire < VIT_MAX_ANG) {
                            ghostPosition.vitesseAngulaire += ghostPosition.accelerationAngulaire;
                        } 
                        else {
                            ghostPosition.vitesseAngulaire = VIT_MAX_ANG;
                        }
                    }   
                }
                else if  (ghostPosition.thetaRestant < 0)
                {
                    if(ghostPosition.thetaRestant > ghostPosition.thetaArret)
                    {
                        if (ghostPosition.vitesseAngulaire>-VIT_MAX_ANG) {
                            ghostPosition.vitesseAngulaire -= ghostPosition.accelerationAngulaire;
                        } else {
                            ghostPosition.vitesseAngulaire = -VIT_MAX_ANG;
                        }
                    }
                }
                else {
                    ghostPosition.vitesseAngulaire = 0;
                }
            }
//            else {
//                
//            }
            
            if (Abs(ghostPosition.thetaRestant) < TOLERANCE_ANG) {
                ghostPosition.vitesseAngulaire = 0;
                ghostPosition.thetaRobot = ghostPosition.thetaWaypoint;
                ghostPosition.state = DEPLACEMENTLINEAIRE;
            }
            break;
        case DEPLACEMENTLINEAIRE:
            ghostPosition.distance = sqrt(pow((ghostPosition.waypointY-ghostPosition.posY),2) + pow((ghostPosition.waypointX-ghostPosition.posX ), 2));
             if (ghostPosition.distance < TOLERANCE_DIST) 
             {
                ghostPosition.state = IDLE;
                ghostPosition.vitesseLineaire = 0.0;
                ghostPosition.waypointX = ghostPosition.posX;
                ghostPosition.waypointY = ghostPosition.posY;
                return;
            }
            ghostPosition.distanceToTarget = ghostPosition.distance;
           
            double distanceDecel = pow(ghostPosition.vitesseLineaire, 2) / (2 * ACCELERATION_LIN);
            double distanceAccel = (pow(VIT_MAX_LIN, 2) - pow(ghostPosition.vitesseLineaire, 2))  / (2 * ACCELERATION_LIN);
            double deltaTime = timestamp - lastUpdateTime;

            if (ghostPosition.distance <= (distanceDecel + TOLERANCE_DIST)) 
            {
                ghostPosition.vitesseLineaire -= ACCELERATION_LIN * deltaTime;
                ghostPosition.vitesseLineaire = fmax(ghostPosition.vitesseLineaire, 0); 
            } 
            else if (ghostPosition.distance > distanceDecel + distanceAccel) 
            {
                ghostPosition.vitesseLineaire += ACCELERATION_LIN * deltaTime;
                ghostPosition.vitesseLineaire = fmin(ghostPosition.vitesseLineaire, VIT_MAX_LIN);
            } 
            else 
            {
                double vMedian = sqrt(VIT_MAX_LIN * ghostPosition.distance + ghostPosition.vitesseLineaire / 2);
                ghostPosition.vitesseLineaire += ACCELERATION_LIN * deltaTime;
                ghostPosition.vitesseLineaire = fmin(ghostPosition.vitesseLineaire, vMedian);
            }
            ghostPosition.posX += ghostPosition.vitesseLineaire * cos(ghostPosition.thetaRobot) * deltaTime;
            ghostPosition.posY += ghostPosition.vitesseLineaire * sin(ghostPosition.thetaRobot) * deltaTime;
            lastUpdateTime = timestamp;
            break;
    }
}

void SendGhost() {
    unsigned char GhostPayload[12];
    getBytesFromFloat(GhostPayload, 0, ghostPosition.posX);
    getBytesFromFloat(GhostPayload, 4, ghostPosition.posY);
    getBytesFromFloat(GhostPayload, 8, ghostPosition.thetaRobot);
    UartEncodeAndSendMessage(SEND_GHOST, 12, GhostPayload);
}
