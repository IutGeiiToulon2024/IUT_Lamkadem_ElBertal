#include "asservissement.h"

#ifndef ROBOT_H
#define ROBOT_H

typedef struct robotStateBITS {

    union {

        struct {
            unsigned char taskEnCours;
            float vitesseGaucheConsigne;
            float vitesseGaucheCommandeCourante;
            float vitesseDroiteConsigne;
            float vitesseDroiteCommandeCourante;

            float distanceTelemetreDroit;
            float distanceTelemetreGauche;
            float distanceTelemetreCentre;
            float distanceTelemetreExtremeDroit;
            float distanceTelemetreExtremeGauche;

            char correcteursXPayload[24];
            char correcteursThetaPayload[24];
            char consignes[8];
                      
            double vitesseDroitFromOdometry;
            double vitesseGaucheFromOdometry;
            double vitesseLineaireFromOdometry;
            double vitesseAngulaireFromOdometry;

            double xPosFromOdometry_1;
            double yPosFromOdometry_1;
            double angleRadianFromOdometry_1;

            double xPosFromOdometry;
            double yPosFromOdometry;
            double angleRadianFromOdometry;
            
            double consigneVitesseLineaire ;
            double consigneVitesseAngulaire ;
            
            double xCorrectionVitesse ;
            double thetaCorrectionVitesse ;
            
            double accelerationAngulaire ;
            
            PidCorrector PidX ;
            PidCorrector PidTheta ;
        };
    };
} ROBOT_STATE_BITS;
extern volatile ROBOT_STATE_BITS robotState;
#endif /* ROBOT_H */


