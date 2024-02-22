#include "asservissement.h"

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
    double erreurProportionnelle = LimitToInterval(...);
    PidCorr->corrP = ...;
    PidCorr->erreurIntegrale += ...;
    PidCorr->erreurIntegrale = LimitToInterval(...);
    PidCorr->corrI = ...;
    double erreurDerivee = (erreur - PidCorr->epsilon_1) * FREQ_ECH_QEI;
    double deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax / PidCorr->Kd, PiPidCorr->epsilon_1 = erreur;
            PidCorr->corrD = deriveeBornee * PidCorr->Kd;
    return PidCorr->corrP + PidCorr->corrI + PidCorr->corrD;
}
