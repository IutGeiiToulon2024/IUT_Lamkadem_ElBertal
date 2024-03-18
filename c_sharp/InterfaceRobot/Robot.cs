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

        public float correctionP;
        public float correctionI;
        public float correctionD;

        public float corrPmax;
        public float corrImax;
        public float corrDmax;

        public Robot()
        {

        }
    }
}
