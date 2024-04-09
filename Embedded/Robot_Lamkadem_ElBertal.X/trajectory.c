#include "trajectory.h"
#include "robot.h"
#include "Utilities.h"

void ghost()
{
    robotState.thetaRest = robotState.thetaWaypoint - ModuloByAngle(robotState.thetaWaypoint, robotState.angleRadianFromOdometry);
    robotState.thetaArret = (robotState.vitesseAngulaireFromOdometry)^2/(2*robotState.accelerationAngulaire) ;
    
    if(robotState.thetaRest > 0)
    {
        if(robotState.vitesseAngulaireFromOdometry < 0)
        {
            robotState.vitesseAngulaireFromOdometry = Max(robotState.vitesseAngulaireFromOdometry + robotState.accelerationAngulaire , 0); // NON c pas le max ici, essaie de voir ce qu'il faut mettre
        }
    }    
}

