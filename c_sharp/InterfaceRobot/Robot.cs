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

        public Robot()
        {

        }
    }
}
