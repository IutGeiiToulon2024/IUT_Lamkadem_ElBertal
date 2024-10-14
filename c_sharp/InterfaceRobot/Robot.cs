using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InterfaceRobot
{
    public class Robot
    {
        public Queue<byte> byteListReceived = new Queue<byte>();
        public string receivedText = "";
        public float distanceTelemetreDroit;
        public float distanceTelemetreCentre;
        public float distanceTelemetreGauche;

        public float positionXOdo ;
        public float positionYOdo ;
        public long timestamp;

        public float angleRadianFromOdometry;
        public float vitesseLineaireFromOdometry;
        public float vitesseAngulaireFromOdometry;

        public float consigneD;
        public float consigneG;

        public float vitesseDroitFromOdometry;
        public float vitesseGaucheFromOdometry;

        public float correcteurKp ;
        public float correcteurKi ;
        public float correcteurKd ;
        public float consigneLineaire;

        public float correcteurThetaKp ;
        public float correcteurThetaKi ;
        public float correcteurThetaKd ;
        public float consigneAngulaire;

        public float limitPX;
        public float limitIX;
        public float limitDX;

        public float limitPTheta;
        public float limitITheta;
        public float limitDTheta;

        public float correctionPX;
        public float correctionIX;
        public float correctionDX;

        public float corrPmaxX;
        public float corrImaxX;
        public float corrDmaxX;

        public float correctionPTheta;
        public float correctionITheta;
        public float correctionDTheta;

        public float corrPmaxTheta;
        public float corrImaxTheta;
        public float corrDmaxTheta;

        public float commandeX;
        public float commandeTheta;

        public float erreurX;
        public float erreurTheta;

        public float posX;
        public float posY;
        public float thetaRobot;
        public float ghostSpeed;

        public Robot()
        {

        }
    }
}
