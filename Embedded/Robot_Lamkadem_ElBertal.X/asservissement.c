#include "asservissement.h"
#include "QEI.h"
#include "robot.h"
#include "Utilities.h"

void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double proportionelleMax, double integralMax, double deriveeMax) {
    PidCorr->Kp = Kp;
    PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
    PidCorr->Ki = Ki;
    PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
    PidCorr->Kd = Kd;
    PidCorr->erreurDeriveeMax = deriveeMax;
}

double Correcteur(volatile PidCorrector* PidCorr, double erreur) {
    PidCorr->erreur = erreur;
    double erreurProportionnelle = LimitToInterval(erreur, -PidCorr->erreurProportionelleMax / PidCorr->Kp, PidCorr->erreurProportionelleMax / PidCorr->Kp);
    PidCorr->corrP = PidCorr->Kp * erreurProportionnelle;

    PidCorr->erreurIntegrale += erreur / FREQ_ECH_QEI;
    PidCorr->erreurIntegrale = LimitToInterval(PidCorr->erreurIntegrale, -PidCorr->erreurIntegraleMax / PidCorr->Ki, PidCorr->erreurIntegraleMax / PidCorr->Ki);
    PidCorr->corrI = PidCorr->Ki * PidCorr->erreurIntegrale;

    double erreurDerivee = (erreur - PidCorr->epsilon_1) * FREQ_ECH_QEI;
    double deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax / PidCorr->Kd, PidCorr->erreurDeriveeMax / PidCorr->Kd);
    PidCorr->epsilon_1 = erreur;
    PidCorr->corrD = deriveeBornee * PidCorr->Kd;

//    getBytesFromFloat((unsigned char*) robotState.correcteursThetaPayload, 0, (float) (correcteurThetaKp));
//    getBytesFromFloat((unsigned char*) robotState.correcteursThetaPayload, 4, (float) (correcteurThetaKd));
//    getBytesFromFloat((unsigned char*) robotState.correcteursThetaPayload, 8, (float) (correcteurThetaKi));
//    getBytesFromFloat((unsigned char*) robotState.correcteursThetaPayload, 12, (float) (consigneAngulaire));
// On veut recup les corrections pour ensuite les envoyer sur l'interface dans le main
    
    return PidCorr->corrP + PidCorr->corrI + PidCorr->corrD;
}

void UpdateAsservissement() {
    robotState.PidX.erreur = robotState.consigneX - robotState.vitesseLineaireFromOdometry;
    robotState.PidTheta.erreur = robotState.consigneTheta - robotState.vitesseAngulaireFromOdometry;
    robotState.xCorrectionVitessePourcent =
            Correcteur(&robotState.PidX, robotState.PidX.erreur);
    robotState.thetaCorrectionVitessePourcent = Correcteur(&robotState.PidTheta, robotState.PidTheta.erreur);
    PWMSetSpeedCommandePolaire(robotState.xCorrectionVitessePourcent,
            robotState.thetaCorrectionVitessePourcent);
}

