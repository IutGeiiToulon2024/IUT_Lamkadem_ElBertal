#include "trajectory.h"
#include "robot.h"
#include "Utilities.h"
#include "math.h"

void ghost() {
    switch (GhostPosition.state) {
        case IDLE:
            GhostPosition.vitesseLineaire = 0;
            GhostPosition.vitesseAngulaire = 0;
            break;

        case ROTATION:
            GhostPosition.thetaRestant = GhostPosition.thetaWaypoint - ModuloByAngle(GhostPosition.thetaWaypoint, GhostPosition.thetaRobot);
            GhostPosition.thetaArret = pow(GhostPosition.vitesseAngulaire, 2) / (2 * GhostPosition.accelerationAngulaire);
            if (GhostPosition.thetaRestant > 0) {
                if (GhostPosition.vitesseAngulaire < 0) {
                    GhostPosition.vitesseAngulaire -= GhostPosition.accelerationAngulaire;
                } else {
                    if (GhostPosition.thetaRestant > GhostPosition.thetaArret) {
                        if (GhostPosition.vitesseAngulaire < VitesseMaxAngulaire) {
                            GhostPosition.vitesseAngulaire += GhostPosition.accelerationAngulaire;
                        } else {
                            GhostPosition.vitesseAngulaire = VitesseMaxAngulaire;
                        }
                    } else {
                        GhostPosition.vitesseAngulaire -= GhostPosition.accelerationAngulaire;
                    }
                }
            }
            else {
                if (GhostPosition.vitesseAngulaire > 0) {
                    GhostPosition.vitesseAngulaire -= GhostPosition.accelerationAngulaire;
                } else {
                    if (Abs(GhostPosition.thetaRestant) > GhostPosition.thetaArret) {
                        if (GhostPosition.vitesseAngulaire>-VitesseMaxAngulaire) {
                            GhostPosition.vitesseAngulaire -= GhostPosition.accelerationAngulaire;
                        } else {
                            GhostPosition.vitesseAngulaire = VitesseMaxAngulaire;
                        }
                    } else {
                        GhostPosition.vitesseAngulaire += GhostPosition.accelerationAngulaire;
                    }
                }
            }
            if (Abs(GhostPosition.thetaRestant) < ToleranceAngulaire) {
                GhostPosition.vitesseAngulaire = 0;
                GhostPosition.thetaRobot = GhostPosition.thetaWaypoint;
                GhostPosition.state = DEPLACEMENTLINEAIRE;
            }
            break;
        case DEPLACEMENTLINEAIRE:

            break;
    }
}

void SendGhost() {

    unsigned char GhostPayload[12];
    getBytesFromFloat(GhostPayload, 0, GhostPosition.posX);
    getBytesFromFloat(GhostPayload, 4, GhostPosition.posY);
    getBytesFromFloat(GhostPayload, 8, GhostPosition.thetaRobot);
    UartEncodeAndSendMessage(SEND_GHOST, 12, GhostPayload);
}
