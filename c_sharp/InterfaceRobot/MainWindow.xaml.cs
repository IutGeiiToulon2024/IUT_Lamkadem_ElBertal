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

namespace InterfaceRobot
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>
    
    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;

        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM4", 115200, Parity.None, 8, StopBits.One);
            serialPort1.OnDataReceivedEvent += SerialPort1_OnDataReceivedEvent;
            serialPort1.Open();
            

        }

        String receivedText ;
        public void SerialPort1_OnDataReceivedEvent(object sender, DataReceivedArgs e)
        {
            receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
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
            textBoxReception.Text +="Reçu : "+ textBoxEmission.Text +"\n";
            textBoxEmission.Text = "";
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
            }

        }
    }
}
