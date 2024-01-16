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
using ExtendedSerialPort;
using System.Windows.Threading;
using System.Security.RightsManagement;

namespace InterfaceRobot
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>

    
    
    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;
        DispatcherTimer timerAffichage;

        Robot robot = new Robot();
        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM12", 115200, Parity.None, 8, StopBits.One);
            serialPort1.OnDataReceivedEvent += SerialPort1_OnDataReceivedEvent;
            serialPort1.Open();
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 1, 0);
            timerAffichage.Tick += TimerAffichage_Tick; ;
            timerAffichage.Start();

        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            //if(robot.receivedText != "" && robot.receivedText != "\r")
            //{
            //    textBoxReception.Text += robot.receivedText;
            //    robot.receivedText = "";
            //}
            while (robot.byteListReceived.Count > 0)
            {
                var c = robot.byteListReceived.Dequeue();
                DecodeMessage(c);
                // ASCII :
                //    textBoxReception.Text += Convert.ToChar(c);

                // HEXA :
                //    textBoxReception.Text += "0x" + c.ToString("X2") + " ";

            }
        }

        public void SerialPort1_OnDataReceivedEvent(object sender, DataReceivedArgs e)
        {
            //robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            foreach(byte value in e.Data)
            {
                robot.byteListReceived.Enqueue(value);
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
                    rcvState |= StateReception.PayloadLengthMSB;
                    break;
                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8 ;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength |= c ;
                    msgDecodedPayload = new byte[msgDecodedPayloadLength];
                    rcvState = StateReception.Payload;
                    break;
                case StateReception.Payload:
                    if(msgDecodedPayloadIndex <= msgDecodedPayloadLength)
                    {
                        //textBoxReception.Text += msgDecodedPayloadIndex.ToString();
                        msgDecodedPayload[msgDecodedPayloadIndex] = c;
                        if (++msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                        {
                            rcvState = StateReception.CheckSum;
                            msgDecodedPayloadIndex = 0;
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
                        textBoxReception.Text += " ERROR ";
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
            consigneVitesse1 = 0x0041,
            consigneVitesse2 = 0x0042
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

                    // textBoxTelemetres.Text +=  Encoding.UTF8.GetString(msgPayload, 0, msgPayloadLength);
                    textBoxTelemetres.Clear();
                    textBoxTelemetres.Text += "Télémètre Droit : " + Encoding.UTF8.GetString(msgPayload, 0, msgPayloadLength) + " cm\n";
                    break;

                case ((int)Fonctions.distTelemetre2):
                    textBoxTelemetres.Text += "Télémètre Centre : " + Encoding.UTF8.GetString(msgPayload, 0, msgPayloadLength) + " cm\n";
                    break;

                case ((int)Fonctions.distTelemetre3):
                    textBoxTelemetres.Text += "Télémètre Gauche : " + Encoding.UTF8.GetString(msgPayload, 0, msgPayloadLength) + " cm";
                    break;

                case ((int)Fonctions.consigneVitesse1):
                    break;

                case ((int)Fonctions.consigneVitesse2):
                    break;
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

    }
}