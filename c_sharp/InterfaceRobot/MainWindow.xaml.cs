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
            serialPort1 = new ReliableSerialPort("COM6", 115200, Parity.None, 8, StopBits.One);
            serialPort1.OnDataReceivedEvent += SerialPort1_OnDataReceivedEvent;
            serialPort1.Open();
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick; ;
            timerAffichage.Start();

        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            if(robot.receivedText != "" && robot.receivedText != "\r")
            {
                textBoxReception.Text += robot.receivedText;
                robot.receivedText = "";
            }
        }

        public void SerialPort1_OnDataReceivedEvent(object sender, DataReceivedArgs e)
        {
            robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
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
            if (test)
            {
                buttonClear.Background = Brushes.RoyalBlue;
                test = false;
            }
            else
            {
                buttonClear.Background = Brushes.Beige;
                test = true;
            }

            byte[] byteList;
            byteList = new byte[20];

            for(int i=0; i<20; i++)
            {
                byteList[i] = (byte)(2 * i);
                serialPort1.WriteLine(byteList[i].ToString());
            }
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
            serialPort1.WriteLine(textBoxEmission.Text);
        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage() ;
                textBoxEmission.Clear();
            }
            
        }
    }
}
