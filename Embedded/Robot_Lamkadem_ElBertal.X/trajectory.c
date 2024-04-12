#include "trajectory.h"
#include "robot.h"
#include "Utilities.h"

void ghost() {
    switch (GhostPosition.state) {
        case IDLE:
            GhostPosition.vitesseLineaire = 0;
            GhostPosition.vitesseAngulaire = 0;
            break;

        case ROTATION:
            GhostPosition.thetaRest = GhostPosition.thetaWaypoint - ModuloByAngle(GhostPosition.thetaWaypoint, GhostPosition.thetaRobot);
            GhostPosition.thetaArret = pow(GhostPosition.vitesseAngulaire, 2) / (2 * GhostPosition.accelerationAngulaire);
            if (GhostPosition.vitesseAngulaire < 0) {
                GhostPosition.vitesseAngulaire -= GhostPosition.accelerationAngulaire;
            }else {
                    if (GhostPosition.thetaRestant > ghostPosition.thetaArret) {
                        if (ghostPosition.vitesseAngulaire < VitesseMaxAngulaire) {
                            ghostPosition.vitesseAngulaire += ghostPosition.accelerationAngulaire;
                        } else {
                            ghostPosition.vitesseAngulaire = VitesseMaxAngulaire;
                        }
    }

