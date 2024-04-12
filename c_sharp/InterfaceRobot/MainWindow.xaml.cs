using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Security.RightsManagement;
using System.Runtime.Remoting.Messaging;

using ExtendedSerialPort;
using WpfOscilloscopeControl;
using WpfAsservissementDisplay;
using System.Timers;

namespace InterfaceRobot
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>

    
    
    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;
        Timer timerAffichage;

        Robot robot = new Robot();
        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM21", 115200, Parity.None, 8, StopBits.One);
            serialPort1.OnDataReceivedEvent += SerialPort1_OnDataReceivedEvent;
            serialPort1.Open();
            timerAffichage = new Timer();
            timerAffichage.Interval = 50;// new TimeSpan(0, 0, 0, 0, 20);
            timerAffichage.Elapsed += TimerAffichage_Tick;
            timerAffichage.Start();

            oscilloSpeed.AddOrUpdateLine(0, 200, "position");
            oscilloSpeed.ChangeLineColor("position", Colors.Red);
            oscilloSpeed.isDisplayActivated = true;

            oscilloPos.AddOrUpdateLine(0, 200, "Vitesse");
            oscilloPos.ChangeLineColor("Vitesse", Colors.Red);
            oscilloPos.isDisplayActivated = true;

        }

        Random random = new Random();

        

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            //if(robot.receivedText != "" && robot.receivedText != "\r")
            //{
            //    textBoxReception.Text += robot.receivedText;
            //    robot.receivedText = "";
            //}
            //while (robot.byteListReceived.Count > 0)
            //{
            //    //var c = robot.byteListReceived.Dequeue();
            //    // ASCII :
            //    //    textBoxReception.Text += Convert.ToChar(c);

            //    // HEXA :
            //    //    textBoxReception.Text += "0x" + c.ToString("X2") + " ";

            //}

            //oscilloSpeed.AddPointToLine(0, robot.timestamp/1000, robot.vitesseLineaireFromOdometry);

            asservSpeedDisplay.UpdatePolarSpeedConsigneValues(robot.consigneLineaire, robot.consigneAngulaire);
            asservSpeedDisplay.UpdateIndependantOdometrySpeed(robot.vitesseGaucheFromOdometry, robot.vitesseDroitFromOdometry);
            asservSpeedDisplay.UpdatePolarOdometrySpeed(robot.vitesseLineaireFromOdometry, robot.vitesseAngulaireFromOdometry);                   // robot.angleRadianFromOdometry);
            asservSpeedDisplay.UpdateIndependantSpeedConsigneValues(robot.consigneG, robot.consigneD);
            asservSpeedDisplay.UpdatePolarSpeedCorrectionGains(robot.correcteurKp, robot.correcteurThetaKp,
                               robot.correcteurKi, robot.correcteurThetaKi,
                               robot.correcteurKd, robot.correcteurThetaKd);
            asservSpeedDisplay.UpdatePolarSpeedCorrectionValues(robot.correctionPX, robot.correctionPTheta,
                               robot.correctionIX, robot.correctionITheta,
                               robot.correctionDX, robot.correctionDTheta);
            asservSpeedDisplay.UpdatePolarSpeedCorrectionLimits(robot.corrPmaxX, robot.corrPmaxTheta,
                               robot.corrImaxX, robot.corrImaxTheta,
                               robot.corrDmaxX, robot.corrDmaxTheta);
            asservSpeedDisplay.UpdatePolarSpeedCommandValues(robot.commandeX, robot.commandeTheta);
            asservSpeedDisplay.UpdatePolarSpeedErrorValues(robot.erreurX, robot.erreurTheta);

        }

        public void SerialPort1_OnDataReceivedEvent(object sender, DataReceivedArgs e)
        {
            //robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            foreach(byte value in e.Data)
            {
                DecodeMessage(value);
                //robot.byteListReceived.Enqueue(value);
            }
        }

        bool toggle = true ;
        
        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            if(toggle)
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
                toggle = false;
            }
            else
            {
                buttonEnvoyer.Background = Brushes.Beige;
                toggle = true;
            }
            SendMessage();
            //textBoxReception.Text +="Reçu : "+ textBoxEmission.Text +"\n";
            textBoxEmission.Clear();
        }
        bool clear = true;
        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            if (clear)
            {
                buttonClear.Background = Brushes.RoyalBlue;
                clear = false;
            }
            else
            {
                buttonClear.Background = Brushes.Beige;
                clear = true;
            }
            textBoxReception.Clear();
            for (int i = 0;i<100; i++)
                oscilloSpeed.AddPointToLine(0, random.NextDouble(), random.NextDouble());
        }
        bool test = true;
        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            //if (test)
            //{
            //    buttonClear.Background = Brushes.RoyalBlue;
            //    test = false;
            //}
            //else
            //{
            //    buttonClear.Background = Brushes.Beige;
            //    test = true;
            //}

            //byte[] byteList;
            //byteList = new byte[20];

            //for(int i=0; i<20; i++)
            //{
            //    byteList[i] = (byte)(2 * i);
            //    serialPort1.WriteLine(byteList[i].ToString());
            //}

            byte[] array = Encoding.ASCII.GetBytes("Bonjour");
            UartEncodeAndSendMessage(0x0080, 7, array) ;
        }



        private void textBoxEmission_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            oscilloSpeed.AddOrUpdateLine(0, 200, "Ligne1");
        }

        void SendMessage()
        {
            //textBoxReception.Text += "Reçu : " + textBoxEmission.Text;
            //textBoxEmission.Text = "";
            //serialPort1.WriteLine(textBoxEmission.Text.Substring(0, textBoxEmission.Text.Length-2));
            byte[] message = Encoding.ASCII.GetBytes(textBoxEmission.Text) ;
            UartEncodeAndSendMessage(0x80, textBoxEmission.Text.Length, message) ;
            // textBoxReception.Text += "n=" + textBoxEmission.Text.Length ;
        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage() ;
                textBoxEmission.Clear();
            }
            
        }

        private byte CalculateChecksum(int msgFunction,
                int msgPayloadLength, byte[] msgPayload)
        {
            byte checksum = 0;
            checksum ^= 0xFE ;
            checksum ^= (byte)(msgFunction >> 8);
            checksum ^= (byte) msgFunction;
            checksum ^= (byte)(msgPayloadLength >> 8);
            checksum ^= (byte) msgPayloadLength; 
            for (int n=0 ; n<msgPayloadLength ; n++)
                checksum ^= msgPayload[n];
            return checksum;            
        }

        private void UartEncodeAndSendMessage(int msgFunction,
                int msgPayloadLength, byte[] msgPayload)
        {
            int taille = 6 + msgPayloadLength ;
            byte[] trame;
            trame = new byte[taille];
            trame[0] = 0xFE;
            trame[1] = (byte)(msgFunction >> 8);
            trame[2] = (byte)(msgFunction);
            trame[3] = (byte)(msgPayloadLength >> 8) ;
            trame[4] = (byte)(msgPayloadLength) ;
            for (int i = 0; i < msgPayloadLength; i++)
                trame[5 + i] = msgPayload[i];
            trame[taille - 1] = CalculateChecksum(msgFunction,
                msgPayloadLength, msgPayload);

            //textBoxReception.Text += "checksum=" + trame[taille - 1];
            serialPort1.Write(trame,0, taille);
        }

        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }

        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;
        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                    {
                        msgDecodedPayloadIndex = 0;
                        rcvState = StateReception.FunctionMSB;
                    }
                        
                    break;
                case StateReception.FunctionMSB:
                    msgDecodedFunction = c << 8 ;
                    rcvState = StateReception.FunctionLSB;
                    break;
                case StateReception.FunctionLSB:
                    msgDecodedFunction |= c ;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;
                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8 ;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength |= c ;
                    if (msgDecodedPayloadLength == 0)
                        rcvState = StateReception.CheckSum;
                    else if (msgDecodedPayloadLength >= 256)
                        rcvState = StateReception.Waiting;
                    else
                    {
                        msgDecodedPayload = new byte[msgDecodedPayloadLength];
                        rcvState = StateReception.Payload;
                        msgDecodedPayloadIndex = 0;
                    }
                    break;
                case StateReception.Payload:
                    if(msgDecodedPayloadIndex <= msgDecodedPayloadLength)
                    {
                        //textBoxReception.Text += msgDecodedPayloadIndex.ToString();
                        msgDecodedPayload[msgDecodedPayloadIndex++] = c;
                        if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                        {
                            rcvState = StateReception.CheckSum;
                        }
                    }
                    break;
                case StateReception.CheckSum:
                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    //textBoxReception.Text += "checksum recalculé=" + calculatedChecksum;
                    //textBoxReception.Text += "checksum reçu=" + c;
                    if (calculatedChecksum == c)
                    {
                        //Success, on a un message valide
                        ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    }
                    else
                    {
                        Dispatcher.BeginInvoke(new Action(() =>
                        {
                            textBoxReception.Text += " ERROR ";
                        }));
                    }
                    rcvState = StateReception.Waiting;
                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }

        public enum Fonctions
        {
            textTransmission = 0x0080,
            led1Reglage = 0x0021,
            led2Reglage = 0x0022,
            led3Reglage = 0x0023,
            distTelemetre1 = 0x0031,
            distTelemetre2 = 0x0032,
            distTelemetre3 = 0x0033,
            distTelemetre4 = 0x0034,
            distTelemetre5 = 0x0035,
            consigneVitesse1 = 0x0041,
            consigneVitesse2 = 0x0042,
            position = 0x0061,
            mesureVitesse = 0x0062,
            test = 0x0070,
            configPIDX = 0x0091,
            configPIDTheta = 0x0092,
            AsservissementX = 0x0093,
            AsservissementTheta = 0x0094,
            consignes = 0x0095,
            commandeerreur = 0x0096,
            ghost = 0x0011,
            RobotState
        }

        public enum StateRobot
        {
            STATE_ATTENTE = 0,
            STATE_ATTENTE_EN_COURS = 1,
            STATE_AVANCE = 2,
            STATE_AVANCE_EN_COURS = 3,
            STATE_TOURNE_GAUCHE = 4,
            STATE_TOURNE_GAUCHE_EN_COURS = 5,
            STATE_TOURNE_DROITE = 6,
            STATE_TOURNE_DROITE_EN_COURS = 7,
            STATE_TOURNE_SUR_PLACE_GAUCHE = 8,
            STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS = 9,
            STATE_TOURNE_SUR_PLACE_DROITE = 10,
            STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS = 11,
            STATE_ARRET = 12,
            STATE_ARRET_EN_COURS = 13,
            STATE_RECULE = 14,
            STATE_RECULE_EN_COURS = 15
        }


        private void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {

            switch (msgFunction)
            {
                case ((int)Fonctions.textTransmission):
                    textBoxReception.Text += Encoding.UTF8.GetString(msgPayload, 0, msgPayloadLength);
                    break;

                case ((int)Fonctions.led1Reglage):

                    break;

                case ((int)Fonctions.led2Reglage):
                    break;

                case ((int)Fonctions.led3Reglage):
                    break;

                case ((int)Fonctions.distTelemetre1):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        textBoxTelemetres.Clear();
                        textBoxTelemetres.Text += "Télémètre Droit : " + BitConverter.ToInt16(msgPayload, 0).ToString() + " cm\n";
                    }));
                    break;

                case ((int)Fonctions.distTelemetre3):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        textBoxTelemetres.Text += "Télémètre Centre : " + BitConverter.ToInt16(msgPayload, 0).ToString() + " cm\n";
                    }));
                    break;

                case ((int)Fonctions.distTelemetre2):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        textBoxTelemetres.Text += "Télémètre Gauche : " + BitConverter.ToInt16(msgPayload, 0).ToString() + " cm\n";
                    }));
                    break;

                case ((int)Fonctions.distTelemetre4):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        textBoxTelemetres.Text += "Télémètre Extreme Droit : " + BitConverter.ToInt16(msgPayload, 0).ToString() + " cm\n";
                    }));
                    break;

                case ((int)Fonctions.distTelemetre5):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        textBoxTelemetres.Text += "Télémètre Extreme Gauche : " + BitConverter.ToInt16(msgPayload, 0).ToString() + " cm";
                    }));
                    break;


                case ((int)Fonctions.consigneVitesse1):
                    robot.consigneD = BitConverter.ToInt16(msgPayload, 0);

                    break;

                case ((int)Fonctions.consigneVitesse2):
                    robot.consigneG = BitConverter.ToInt16(msgPayload, 0);
                    break;

                case ((int)Fonctions.position):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        textBoxPosition.Clear();
                        textBoxPosition.Text += "trame : 0x" + BitConverter.ToInt16(msgPayload, 0).ToString("X2") + '\n';
                    }));
                    var tab = msgPayload.Skip(0).Take(4).Reverse().ToArray();
                    robot.timestamp = BitConverter.ToUInt32(tab, 0);
                    robot.positionXOdo = BitConverter.ToSingle(msgPayload, 4);
                    robot.positionYOdo = BitConverter.ToSingle(msgPayload, 8);
                    robot.angleRadianFromOdometry = BitConverter.ToSingle(msgPayload, 12);
                    robot.vitesseLineaireFromOdometry = BitConverter.ToSingle(msgPayload, 16);
                    robot.vitesseAngulaireFromOdometry = BitConverter.ToSingle(msgPayload, 20);

                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        textBoxPosition.Text += "Time : " + robot.timestamp + '\n';
                        textBoxPosition.Text += "Position X : " + robot.positionXOdo + '\n';
                        textBoxPosition.Text += "Position Y : " + robot.positionYOdo + '\n';
                        textBoxPosition.Text += "angleRadianFromOdometry : " + robot.angleRadianFromOdometry + '\n';
                        textBoxPosition.Text += "vitesseLineaireFromOdometry : " + robot.vitesseLineaireFromOdometry + '\n';
                        textBoxPosition.Text += "vitesseAngulaireFromOdometry : " + robot.vitesseAngulaireFromOdometry + '\n';
                    }));

                    oscilloSpeed.AddPointToLine(0, robot.positionXOdo, robot.positionYOdo);
                    oscilloPos.AddPointToLine(0, robot.timestamp / 1000.0, robot.vitesseLineaireFromOdometry);
                    break;
                //

                case ((int)Fonctions.mesureVitesse):
                    robot.vitesseDroitFromOdometry = BitConverter.ToSingle(msgPayload, 0);
                    robot.vitesseGaucheFromOdometry = BitConverter.ToSingle(msgPayload, 4);
                    Dispatcher.Invoke(new Action(() =>
                    {
                        textBoxReception.Clear();
                        textBoxPosition.Text += "Moteur D :" + robot.vitesseDroitFromOdometry.ToString() + "\n";
                        textBoxPosition.Text += "Moteur G :" + robot.vitesseGaucheFromOdometry.ToString() + "\n";
                    }));
                    break;

                case ((int)Fonctions.configPIDX):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        robot.correcteurKp = BitConverter.ToSingle(msgPayload, 0) ;
                        robot.correcteurKd = BitConverter.ToSingle(msgPayload, 4);
                        robot.correcteurKi = BitConverter.ToSingle(msgPayload, 8);
                        robot.consigneLineaire = BitConverter.ToSingle(msgPayload, 12);
                    }));
                    break;

                case ((int)Fonctions.configPIDTheta):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        robot.correcteurThetaKp = BitConverter.ToSingle(msgPayload, 0);
                        robot.correcteurThetaKd = BitConverter.ToSingle(msgPayload, 4);
                        robot.correcteurThetaKi = BitConverter.ToSingle(msgPayload, 8);
                        robot.consigneAngulaire = BitConverter.ToSingle(msgPayload, 12);
                    }));
                    break;

                case ((int)Fonctions.test):
                    
                    break;

                case (int)Fonctions.AsservissementX:
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        robot.correctionPX = BitConverter.ToSingle(msgPayload, 0);
                        robot.corrPmaxX = BitConverter.ToSingle(msgPayload, 4);
                        robot.correctionIX = BitConverter.ToSingle(msgPayload, 8);
                        robot.corrImaxX = BitConverter.ToSingle(msgPayload, 12);
                        robot.correctionDX = BitConverter.ToSingle(msgPayload, 16);
                        robot.corrDmaxX = BitConverter.ToSingle(msgPayload, 20);
                        textBoxReception.Clear();
                        textBoxReception.Text += "donnees Pmax :" + robot.corrPmaxX.ToString() + "\n";
                        textBoxReception.Text += "donnees Imax :" + robot.corrImaxX.ToString() + "\n";
                        textBoxReception.Text += "donnees Dmax :" + robot.corrDmaxX.ToString() + "\n";
                    }));
                    break;

                case ((int)Fonctions.AsservissementTheta):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        robot.correctionPTheta = BitConverter.ToSingle(msgPayload, 0);
                        robot.corrPmaxTheta = BitConverter.ToSingle(msgPayload, 4);
                        robot.correctionITheta = BitConverter.ToSingle(msgPayload, 8);
                        robot.corrImaxTheta = BitConverter.ToSingle(msgPayload, 12);
                        robot.correctionDTheta = BitConverter.ToSingle(msgPayload, 16);
                        robot.corrDmaxTheta = BitConverter.ToSingle(msgPayload, 20);
                    }));
                    break;

                case ((int)Fonctions.consignes):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        robot.consigneAngulaire = BitConverter.ToSingle(msgPayload, 0);
                        robot.consigneLineaire = BitConverter.ToSingle(msgPayload, 4);
                    }));
                    break;

                case ((int)Fonctions.commandeerreur):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        robot.commandeX = BitConverter.ToSingle(msgPayload, 0);
                        robot.commandeTheta = BitConverter.ToSingle(msgPayload, 4);
                        robot.erreurX = BitConverter.ToSingle(msgPayload, 8);
                        robot.erreurTheta = BitConverter.ToSingle(msgPayload, 12);
                    }));
                    break;

                case ((int)Fonctions.ghost):
                    Dispatcher.BeginInvoke(new Action(() =>
                    {
                        robot.posX = BitConverter.ToSingle(msgPayload, 0);
                        robot.posY = BitConverter.ToSingle(msgPayload, 4);
                        robot.thetaRobot = BitConverter.ToSingle(msgPayload, 8);
                    }));
                    break;


                    //case ((int)Fonctions.RobotState):
                    //    int instant = (((int)msgPayload[1]) << 24) + (((int)msgPayload[2]) << 16)
                    //    + (((int)msgPayload[3]) << 8) + ((int)msgPayload[4]);
                    //    rtbReception.Text += "\nRobot␣State␣:␣" +
                    //    ((StateRobot)(msgPayload[0])).ToString() + 
                    //    break;
            }

            //if (msgFunction == 0x80)
            //{
            //    textBoxReception.Text += Encoding.UTF8.GetString(msgPayload, 0, msgPayloadLength);
            //}

            //else if (msgFunction == 0x7)
            //{
            //    textBoxReception.Text += " ERROR ";
            //}

        }

        byte[] etat_led1 = new byte[1];

        private void checkLed1_Checked(object sender, RoutedEventArgs e)
        {
            etat_led1[0] = 1;
            UartEncodeAndSendMessage(0x0021, 1, etat_led1);
        }

        private void checkLed1_Unchecked(object sender, RoutedEventArgs e)
        {
            etat_led1[0] = 0;
            UartEncodeAndSendMessage(0x0021, 1, etat_led1);
        }

        private void asservSpeedDisplay_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void buttonAsserv_Click(object sender, RoutedEventArgs e)
        {
            //(float) random.NextDouble() ;
            float KpX = (float)1.5 ;
            float KpTheta = (float)1.2 ;
            float KiX = 100 ;
            float KiTheta = 40 ;
            float KdX = 0 ;
            float KdTheta = 0 ;

            float limitPX = 100 ;
            float limitIX = 100 ;
            float limitDX = 100 ;

            float limitPTheta = 100 ;
            float limitITheta = 100 ;
            float limitDTheta = 100 ;

            byte[] kpByte = BitConverter.GetBytes(KpX);
            byte[] kdByte = BitConverter.GetBytes(KdX);
            byte[] kiByte = BitConverter.GetBytes(KiX);
            byte[] limitPXbyte = BitConverter.GetBytes(limitPX);
            byte[] limitIXbyte = BitConverter.GetBytes(limitIX);
            byte[] limitDXbyte = BitConverter.GetBytes(limitDX);

            byte[] kpThetaByte = BitConverter.GetBytes(KpTheta);
            byte[] kdThetaByte = BitConverter.GetBytes(KdTheta);
            byte[] kiThetaByte = BitConverter.GetBytes(KiTheta);
            byte[] limitPThetabyte = BitConverter.GetBytes(limitPTheta);
            byte[] limitIThetabyte = BitConverter.GetBytes(limitITheta);
            byte[] limitDThetabyte = BitConverter.GetBytes(limitDTheta);

            byte[] correcteursX = new byte[24];
            kpByte.CopyTo(correcteursX, 0);
            kdByte.CopyTo(correcteursX, 4);
            kiByte.CopyTo(correcteursX, 8);
            limitPXbyte.CopyTo(correcteursX, 12);
            limitIXbyte.CopyTo(correcteursX, 16);
            limitDXbyte.CopyTo(correcteursX, 20);

            byte[] correcteursTheta = new byte[24];
            kpThetaByte.CopyTo(correcteursTheta, 0);
            kdThetaByte.CopyTo(correcteursTheta, 4);
            kiThetaByte.CopyTo(correcteursTheta, 8);
            limitPThetabyte.CopyTo(correcteursTheta, 12);
            limitIThetabyte.CopyTo(correcteursTheta, 16);
            limitDThetabyte.CopyTo(correcteursTheta, 20);


            float consigneAngulaire = (float)SliderVitesseAngulaire.Value;
            float consigneLineaire = (float)SliderVitesseLineaire.Value;

            byte[] ConsigneAng = BitConverter.GetBytes(consigneAngulaire);
            byte[] ConsigneLine = BitConverter.GetBytes(consigneLineaire);

            byte[] consignes = new byte[8];

            ConsigneAng.CopyTo(consignes, 0);
            ConsigneLine.CopyTo(consignes, 4);


            UartEncodeAndSendMessage(0x0091, correcteursX.Length, correcteursX);
            UartEncodeAndSendMessage(0x0092, correcteursTheta.Length, correcteursTheta);
            UartEncodeAndSendMessage(0x0095, consignes.Length, consignes);
        }

        int position;

        private void button01_Click(object sender, RoutedEventArgs e)
        {
            position = 0x01;
            byte[] posGhost = BitConverter.GetBytes(position);
            UartEncodeAndSendMessage(0x0010, posGhost.Length, posGhost);
        }

        private void button10_Click(object sender, RoutedEventArgs e)
        {
            position = 0x10;
            byte[] posGhost = BitConverter.GetBytes(position);
            UartEncodeAndSendMessage(0x0010, posGhost.Length, posGhost);
        }

        private void button11_Click(object sender, RoutedEventArgs e)
        {
            position = 0x11;
            byte[] posGhost = BitConverter.GetBytes(position);
            UartEncodeAndSendMessage(0x0010, posGhost.Length, posGhost);
        }

        private void button00_Click(object sender, RoutedEventArgs e)
        {
            position = 0x00;
            byte[] posGhost = BitConverter.GetBytes(position);
            UartEncodeAndSendMessage(0x0010, posGhost.Length, posGhost);
        }
    }
}