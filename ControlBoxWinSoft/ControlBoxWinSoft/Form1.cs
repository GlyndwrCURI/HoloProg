using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using AMS.Profile;

namespace ControlBoxWinSoft
{
    public partial class Form1 : Form
    {
        Thread serialReadThread;
        public delegate void SerialReceivedCharsDelegate(SerialRecordEventArgs e);

        //bool bOdd = true;

        private void SerialCharsProcess(SerialRecordEventArgs e)
        {
            textBoxMemo1.AppendText(e.ReceivedString+"Debug+"+e.debugMessage+"\r\n");
            switch (e.command)
            {
/*          SendGetSerial('f');
            SendGetSerial('r');
            SendGetSerial('l');
            SendGetSerial('w');
            SendGetSerial('v');
            SendGetSerial('p');  */
                case 'f':
                    numericUpDownPixelFrequency.Value = e.values[0];
                    numericUpDownPixelFrequency.BackColor = SystemColors.Window;
                    break;

                case 'r':
                    numericUpDownPixelDivideRatio.Value = e.values[0];
                    numericUpDownPixelDivideRatio.BackColor = SystemColors.Window;
                    break;

                case 'l':
                    numericUpDownLineStartDelay.Value = e.values[0];
                    numericUpDownLineStartDelay.BackColor = SystemColors.Window;
                    break;

                case 'w':
                    numericUpDownLineStartWidth.Value = e.values[0];
                    numericUpDownLineStartWidth.BackColor = SystemColors.Window;
                    break;

                case 'v':
                    numericUpDownPixelWidth.Value = e.values[0];
                    numericUpDownPixelWidth.BackColor = SystemColors.Window;
                    break;

                case 'p':
                    numericUpDownPixelDelay.Value = e.values[0];
                    numericUpDownPixelDelay.BackColor = SystemColors.Window;
                    break;

                case 'm':
                    SetTriggerRadioButtonsValue(e.values[0]);
                    break;

                case 'g':
                    if (e.nValues == 2)
                    {
                        numericUpDownStatusX.Value = e.values[1];
                        numericUpDownStatusY.Value = e.values[0];
                    }
                    else
                    {
                        numericUpDownStatusX.BackColor = Color.Red;
                        numericUpDownStatusY.BackColor = Color.Red;
                    }
                    break;
            }
        }

        private int GetTriggerRadioButtonsValue()
        {
            if (radioButtonInternalSimulator.Checked) return 0;
            if (radioButtonLcosRealTrigger.Checked) return 1;
            return -1;
        }

        private void SetTriggerRadioButtonsValue(int i)
        {
            if (i == 0)
            {
                radioButtonInternalSimulator.Checked = true;
                radioButtonLcosRealTrigger.Checked = false;

                radioButtonInternalSimulator.BackColor = SystemColors.Control;
                radioButtonLcosRealTrigger.BackColor = SystemColors.Control;
            }
            else if (i == 1)
            {
                radioButtonInternalSimulator.Checked = false;
                radioButtonLcosRealTrigger.Checked = true;

                radioButtonInternalSimulator.BackColor = SystemColors.Control;
                radioButtonLcosRealTrigger.BackColor = SystemColors.Control;
            }
            else
            {
                radioButtonInternalSimulator.BackColor = Color.Red;
                radioButtonLcosRealTrigger.BackColor = Color.Red;
            }
        }

        private void SerialCharsDoProcessing(SerialRecordEventArgs e)
        {
            if (this.InvokeRequired == false)
            {
                SerialCharsProcess(e);
            }
            else
            {
                SerialReceivedCharsDelegate d1 = new SerialReceivedCharsDelegate(SerialCharsDoProcessing);
                SerialRecordEventArgs e1 = new SerialRecordEventArgs(e);
                BeginInvoke(d1, e1);
            }

        }

        public Form1()
        {
            InitializeComponent();
        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            string s="";
            int i = 0;
            char c='@';

            int bytesToRead;

//            Thread.Sleep(5);
            bytesToRead = serialPort1.BytesToRead;
            System.Diagnostics.Debug.Write(bytesToRead.ToString());
            try
            {
                while (serialPort1.BytesToRead > 0)
                {
                    i = serialPort1.ReadByte();
                    c = System.Convert.ToChar(i);
                    s += c;
                }

                SerialRecordEventArgs eargs = new SerialRecordEventArgs(s);
                if (bytesToRead!=14)
                {
                    System.Diagnostics.Debug.WriteLine("***c=" + i.ToString());
                }
                else
                {
                    System.Diagnostics.Debug.WriteLine(" " + s);
                }
                SerialCharsDoProcessing(eargs);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        static bool readThreadContinue;

        public  void ReadSerialData()
        {
            string sLine;

            sLine = "";
            Thread.Sleep(100);
            while (readThreadContinue)
            {
                sLine = "";
                try
                {
                    sLine = serialPort1.ReadLine();
                }
                catch (Exception Ex)
                {
                };
                if (sLine != "")
                {
                    SerialRecordEventArgs eargs = new SerialRecordEventArgs(sLine+"\r\n");
                    eargs.ParseRecord();

                    SerialCharsDoProcessing(eargs);
                }
            }
        }

        private void buttonOpen_Click(object sender, EventArgs e)
        {
            serialPort1.PortName = textBoxComPortName.Text;

            try
            {
                serialPort1.Open();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Problem with opening " + serialPort1.PortName);
                return;
            }
            readThreadContinue = true;
            serialReadThread = new Thread(ReadSerialData);
            serialReadThread.Start();
            
            buttonOpen.Enabled = false;
            buttonClose.Enabled = true;
            buttonLineStartDelay.Enabled = true;
            buttonLineStartWidth.Enabled = true;
            buttonLineTrigger.Enabled = true;
            buttonPixelDelay.Enabled = true;
            buttonPixelDivideRatio.Enabled = true;
            buttonPixelFrequency.Enabled = true;
            buttonAutoTrigger.Enabled = true;
            buttonPixelWidth.Enabled = true;
            buttonSendCustomString.Enabled = true;


            buttonPixFreqGet.Enabled = true;
            buttonPixDivRatioGet.Enabled = true;
            buttonLineStartDelayGet.Enabled = true;
            buttonLineStartWidthGet.Enabled = true;
            buttonPixelWidthGet.Enabled = true;
            buttonPixelDelayGet.Enabled = true;
            buttonTriggerGet.Enabled = true;
            buttonGetAll.Enabled = true;
            buttonPixTrigSend.Enabled = true;
            buttonSendAll.Enabled = true;
            buttonSendLaserDelays.Enabled = true;
            buttonTriggerSeries.Enabled = true;
            buttonStartPrint.Enabled = true;
            buttonGetStatus.Enabled = true;
            buttonAutoGetStatus.Enabled = true;
            button1.Enabled = true;

            numericUpDownPixelFrequency.BackColor = Color.LightYellow;
            numericUpDownPixelDivideRatio.BackColor = Color.LightYellow;
            numericUpDownLineStartDelay.BackColor = Color.LightYellow;
            numericUpDownLineStartWidth.BackColor = Color.LightYellow;
            numericUpDownPixelWidth.BackColor = Color.LightYellow;
            numericUpDownPixelDelay.BackColor = Color.LightYellow;
            radioButtonInternalSimulator.BackColor = Color.LightYellow;
            radioButtonLcosRealTrigger.BackColor = Color.LightYellow;
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            readThreadContinue = false;
            serialReadThread.Join();

            timer1.Enabled = false;
            timer2.Enabled = false;
            buttonOpen.Enabled = true;
            buttonClose.Enabled = false;

            buttonLineStartDelay.Enabled = false;
            buttonLineStartWidth.Enabled = false;
            buttonLineTrigger.Enabled = false;
            buttonPixelDelay.Enabled = false;
            buttonPixelDivideRatio.Enabled = false;
            buttonPixelFrequency.Enabled = false;
            buttonAutoTrigger.Enabled = false;
            buttonPixelWidth.Enabled = false;
            buttonSendCustomString.Enabled = false;

            buttonPixFreqGet.Enabled = false;
            buttonPixDivRatioGet.Enabled = false;
            buttonLineStartDelayGet.Enabled = false;
            buttonLineStartWidthGet.Enabled = false;
            buttonPixelWidthGet.Enabled = false;
            buttonPixelDelayGet.Enabled = false;
            buttonTriggerGet.Enabled = false;
            buttonGetAll.Enabled = false;

            buttonPixTrigSend.Enabled = false;
            buttonSendAll.Enabled = false;
            buttonSendLaserDelays.Enabled = false;
            buttonTriggerSeries.Enabled = false;
            buttonStartPrint.Enabled = false;
            buttonGetStatus.Enabled = false;
            buttonAutoGetStatus.Enabled = false;
            button1.Enabled = false;


            buttonAutoTrigger.BackColor = Control.DefaultBackColor;
            serialPort1.Close();

        }

        private void SendGetSerial(char command)
        {
            string s;

            s = "@" + command + "0G-1";
            textBoxMemo1.AppendText("Sent: " + s);
            if (serialPort1.IsOpen)
              serialPort1.WriteLine(s);

        }

        private void SendSetSerial(char command, Decimal value)
        {
            string s;

            s = "@" + command + "0S-1 " + value.ToString();
            textBoxMemo1.AppendText("Sent: " + s);
            if (serialPort1.IsOpen)
              serialPort1.WriteLine(s);
        }

        private void SendSetSerial(char command, Decimal value1, Decimal value2)
        {
            string s;

            s = "@" + command + "0S-1 " + value1.ToString() + " " + value2.ToString();
            textBoxMemo1.AppendText("Sent: " + s);
            if (serialPort1.IsOpen)
              serialPort1.WriteLine(s);
        }

        private void SendSetSerial(char command, Decimal value1, Decimal value2, Decimal value3, Decimal value4)
        {
            string s;

            s = "@" + command + "0S-1 " + value1.ToString() + " " + value2.ToString() + " " + value3.ToString() + " " + value4.ToString();
            textBoxMemo1.AppendText("Sent: " + s);
            if (serialPort1.IsOpen)
              serialPort1.WriteLine(s);
        }

        private void SendSetSerial(char command)
        {
            string s;
            s = "@" + command + "0S-1";
            textBoxMemo1.AppendText("Sent: " + s);
            if (serialPort1.IsOpen)
              serialPort1.WriteLine(s);
        }

        private void buttonPixelFrequency_Click(object sender, EventArgs e)
        {
            SendSetSerial('f', numericUpDownPixelFrequency.Value);
        }

        private void buttonPixelDivideRatio_Click(object sender, EventArgs e)
        {
            SendSetSerial('r', numericUpDownPixelDivideRatio.Value);
//            numericUpDownPixelDivideRatio.BackColor = SystemColors.Window;
        }

        private void buttonLineStartDelay_Click(object sender, EventArgs e)
        {
            SendSetSerial('l', numericUpDownLineStartDelay.Value);
//            numericUpDownLineStartDelay.BackColor = SystemColors.Window;
        }

        private void buttonLineStartWidth_Click(object sender, EventArgs e)
        {
            SendSetSerial('w', numericUpDownLineStartWidth.Value);
//            numericUpDownLineStartWidth.BackColor = SystemColors.Window;
        }

        private void buttonPixelWidth_Click(object sender, EventArgs e)
        {
            SendSetSerial('v', numericUpDownPixelWidth.Value);
//            numericUpDownPixelWidth.BackColor = SystemColors.Window;
        }

        private void buttonPixelDelay_Click(object sender, EventArgs e)
        {
            SendSetSerial('p', numericUpDownPixelDelay.Value);
//            numericUpDownPixelDelay.BackColor = SystemColors.Window;
        }

        private void buttonLineTrigger_Click(object sender, EventArgs e)
        {
            SendSetSerial('t');
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            SendSetSerial('t');
        }

        private void buttonAutoTrigger_Click(object sender, EventArgs e)
        {
            if (timer1.Enabled)
            {
                timer1.Enabled = false;
                buttonAutoTrigger.BackColor = Control.DefaultBackColor;
            }
            else
            {
                timer1.Enabled = true;
                buttonAutoTrigger.BackColor = Color.Red;
            }
        }

        private void numericUpDownAutoTrigger_ValueChanged(object sender, EventArgs e)
        {
            timer1.Interval = Convert.ToInt16(numericUpDownAutoTrigger.Value);
        }

        private void numericUpDownPixelFrequency_ValueChanged(object sender, EventArgs e)
        {
            numericUpDownPixelFrequency.BackColor = Color.LightYellow;
        }

        private void numericUpDownPixelDivideRatio_ValueChanged(object sender, EventArgs e)
        {
            numericUpDownPixelDivideRatio.BackColor = Color.LightYellow;
        }

        private void numericUpDownLineStartDelay_ValueChanged(object sender, EventArgs e)
        {
            numericUpDownLineStartDelay.BackColor = Color.LightYellow;
        }

        private void numericUpDownLineStartWidth_ValueChanged(object sender, EventArgs e)
        {
            numericUpDownLineStartWidth.BackColor = Color.LightYellow;
        }

        private void numericUpDownPixelDelay_ValueChanged(object sender, EventArgs e)
        {
            numericUpDownPixelDelay.BackColor = Color.LightYellow;
        }

        private void numericUpDownPixelWidth_ValueChanged(object sender, EventArgs e)
        {
            numericUpDownPixelWidth.BackColor = Color.LightYellow;
        }

        private void buttonSendCustomString_Click(object sender, EventArgs e) //"SendCustomString" button
        {
            if (serialPort1.IsOpen)
              serialPort1.WriteLine(textBoxCustomString.Text);
        }

        private void buttonTriggerSeries_Click(object sender, EventArgs e)
        {
            SendSetSerial('b', numericUpDownNumberOfLines.Value, numericUpDownDelayMs.Value);
        }

        private void buttonStartPrint_Click(object sender, EventArgs e)
        {
            //if(bOdd)
              SendSetSerial('s', 0, numericUpDownDelayMs.Value);
            //else
            //  SendGetSerial('s');
            //bOdd = !bOdd;
        }

        private void buttonSendAll_Click(object sender, EventArgs e)
        {
            SendSetSerial('f', numericUpDownPixelFrequency.Value);
            Thread.Sleep(100);

            SendSetSerial('r', numericUpDownPixelDivideRatio.Value);
            Thread.Sleep(100);

            SendSetSerial('l', numericUpDownLineStartDelay.Value);
            Thread.Sleep(100);

            SendSetSerial('w', numericUpDownLineStartWidth.Value);
            Thread.Sleep(100);

            SendSetSerial('v', numericUpDownPixelWidth.Value);
            Thread.Sleep(100);

            SendSetSerial('p', numericUpDownPixelDelay.Value);
            Thread.Sleep(100);

            SendPixTrigCommand();
            Thread.Sleep(100);
        }

        private void button1_Click(object sender, EventArgs e)  //mj - this is the "Send Print" button
        {
            string cmd;
//@a0s-1 50 10 10 40 1 10 1
            cmd = "@a0s-1";
            cmd += " " + numericUpDownPrintLineLength.Value.ToString();
            cmd += " " + numericUpDownPrintTotalLines.Value.ToString();
            cmd += " " + numericUpDownPrintShutterStartPixel.Value.ToString();
            cmd += " " + numericUpDownPrintShutterStopPixel.Value.ToString();
            cmd += " " + numericUpDownPrintShutterRedStartLine.Value.ToString();
            cmd += " " + numericUpDownPrintShutterRedStoplLine.Value.ToString();
            cmd += " " + numericUpDownPrintColorOffset.Value.ToString();

            textBoxPrintCommand.Text = cmd;
            if (serialPort1.IsOpen)
              serialPort1.WriteLine(textBoxPrintCommand.Text);


        }

        private void SendPixTrigCommand()
        {
            if (radioButtonInternalSimulator.Checked)
            {
                SendSetSerial('m',0);
            }
            else if (radioButtonLcosRealTrigger.Checked)
            {
                SendSetSerial('m',1);
            }
//            SendSetSerial('m', i);
        }

        private void buttonPixTrigSend_Click(object sender, EventArgs e)
        {
            SendPixTrigCommand();
            radioButtonInternalSimulator.BackColor = SystemColors.Control;
            radioButtonLcosRealTrigger.BackColor = SystemColors.Control;

        }

        private void radioButtonInternalSimulator_CheckedChanged(object sender, EventArgs e)
        {
            radioButtonInternalSimulator.BackColor = Color.LightYellow;
            radioButtonLcosRealTrigger.BackColor = Color.LightYellow;

        }

        private void radioButtonLcosRealTrigger_CheckedChanged(object sender, EventArgs e)
        {
            radioButtonInternalSimulator.BackColor = Color.LightYellow;
            radioButtonLcosRealTrigger.BackColor = Color.LightYellow;

        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            XmlSaveConfig();
            if (readThreadContinue)
            {
                readThreadContinue = false;
                serialReadThread.Join();
                serialPort1.Close();
            }
        }

        private void buttonSendLaserDelays_Click(object sender, EventArgs e)
        {
            SendSetSerial('d', numericUpDownRedLaserDelay.Value, numericUpDownGreenLaserDelay.Value,
                numericUpDownBlueLaserDelay.Value, numericUpDownLaserPulseWidth.Value);

        }

        private void buttonPixFreqGet_Click(object sender, EventArgs e)
        {
            SendGetSerial('f');
        }

        private void buttonPixDivRatioGet_Click(object sender, EventArgs e)
        {
            SendGetSerial('r');
        }

        private void buttonLineStartDelayGet_Click(object sender, EventArgs e)
        {
            SendGetSerial('l');

        }

        private void buttonLineStartWidthGet_Click(object sender, EventArgs e)
        {
            SendGetSerial('w');

        }

        private void buttonPixelWidthGet_Click(object sender, EventArgs e)
        {
            SendGetSerial('v');

        }

        private void buttonPixelDelayGet_Click(object sender, EventArgs e)
        {
            SendGetSerial('p');
        }

        private void buttonGetStatus_Click(object sender, EventArgs e)
        {
            SendGetSerial('g');
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            SendGetSerial('g');
        }

        private void numericUpDown3_ValueChanged(object sender, EventArgs e)
        {
            timer2.Interval = Convert.ToInt16(numericUpDownGetStatusUpdate.Value);
        }

        private void buttonAutoGetStatus_Click(object sender, EventArgs e)
        {
            if (timer2.Enabled)
            {
                timer2.Enabled = false;
                buttonAutoGetStatus.BackColor = Control.DefaultBackColor;
            }
            else
            {
                timer2.Enabled = true;
                buttonAutoGetStatus.BackColor = Color.Red;
            }

        }

        private Profile XmlConfig = new Xml();

//        private string currentConfigName;

        private void XmlReadNamedConfig(string configName)
        {
            textBoxComPortName.Text = XmlConfig.GetValue(configName, "ComPort", "COM1");

            numericUpDownPixelFrequency.Value = XmlConfig.GetValue(configName, "PixelFrequency", 83333);
            numericUpDownPixelDivideRatio.Value = XmlConfig.GetValue(configName, "PixelDivideRatio", 63);
            numericUpDownLineStartDelay.Value = XmlConfig.GetValue(configName, "LineStartDelay", 10);
            numericUpDownLineStartWidth.Value = XmlConfig.GetValue(configName, "LineStartWidth", 100000);
            numericUpDownPixelWidth.Value = XmlConfig.GetValue(configName, "PixelWidth", 100000);
            numericUpDownPixelDelay.Value = XmlConfig.GetValue(configName, "PixelDelay", 10);
            numericUpDownRedLaserDelay.Value = XmlConfig.GetValue(configName, "RedLaserDelay", 1000);
            numericUpDownGreenLaserDelay.Value = XmlConfig.GetValue(configName, "GreenLaserDelay", 1000);
            numericUpDownBlueLaserDelay.Value = XmlConfig.GetValue(configName, "BlueLaserDelay", 1000);
            numericUpDownLaserPulseWidth.Value = XmlConfig.GetValue(configName, "LaserPulseWidth", 1000);
            numericUpDownPrintLineLength.Value = XmlConfig.GetValue(configName, "PrintLineLength", 50);
            numericUpDownPrintTotalLines.Value = XmlConfig.GetValue(configName, "PrintTotalLines", 21);
            numericUpDownPrintShutterStartPixel.Value = XmlConfig.GetValue(configName, "PrintShutterStartPixel", 10);
            numericUpDownPrintShutterStopPixel.Value = XmlConfig.GetValue(configName, "PrintShutterStopPixel", 40);
            numericUpDownPrintShutterRedStartLine.Value = XmlConfig.GetValue(configName, "PrintShutterRedStartLine", 1);
            numericUpDownPrintShutterRedStoplLine.Value = XmlConfig.GetValue(configName, "PrintShutterRedStopLine", 8);
            numericUpDownPrintColorOffset.Value = XmlConfig.GetValue(configName, "PrintColorOffset", 1);
            numericUpDownAutoTrigger.Value = XmlConfig.GetValue(configName, "AutoTrigger", 500);
            numericUpDownGetStatusUpdate.Value = XmlConfig.GetValue(configName, "GetStatusUpdate", 300);
            numericUpDownNumberOfLines.Value = XmlConfig.GetValue(configName, "NumberOfLines", 5);
            numericUpDownDelayMs.Value = XmlConfig.GetValue(configName, "DelayMs", 100);
            SetTriggerRadioButtonsValue(XmlConfig.GetValue(configName, "LcosTriggerSource", 0));
            textBoxCurrentConfig.Text = configName;

            for(int i=0; i<listBoxConfigNames.Items.Count; i++)
            {
                if (listBoxConfigNames.Items[i].ToString() == configName)
                {
                    listBoxConfigNames.SetSelected(i, true);
                }
            }
        }

        private void XmlReadConfig()
        {
            textBoxCurrentConfig.Text = XmlConfig.GetValue("AppConfig", "CurrentConfigName", "DefaultConfig");


            string[] configs = XmlConfig.GetSectionNames();
            foreach (string s in configs)
            {
                if(s!="AppConfig")
                listBoxConfigNames.Items.Add(s);
            }
            XmlReadNamedConfig(textBoxCurrentConfig.Text);
        }

        private void XmlSaveConfig()
        {
            XmlConfig.SetValue("AppConfig", "CurrentConfigName", textBoxCurrentConfig.Text);
            XmlSaveNamedConfig(textBoxCurrentConfig.Text);
        }

        private void XmlSaveNamedConfig(string configName)
        {

            XmlConfig.SetValue(configName, "ComPort", textBoxComPortName.Text);

            XmlConfig.SetValue(configName, "PixelFrequency", numericUpDownPixelFrequency.Value);
            XmlConfig.SetValue(configName, "PixelDivideRatio", numericUpDownPixelDivideRatio.Value);
            XmlConfig.SetValue(configName, "LineStartDelay", numericUpDownLineStartDelay.Value);
            XmlConfig.SetValue(configName, "LineStartWidth", numericUpDownLineStartWidth.Value);
            XmlConfig.SetValue(configName, "PixelWidth", numericUpDownPixelWidth.Value);
            XmlConfig.SetValue(configName, "PixelDelay", numericUpDownPixelDelay.Value);
            XmlConfig.SetValue(configName, "RedLaserDelay", numericUpDownRedLaserDelay.Value);
            XmlConfig.SetValue(configName, "GreenLaserDelay", numericUpDownGreenLaserDelay.Value);
            XmlConfig.SetValue(configName, "BlueLaserDelay", numericUpDownBlueLaserDelay.Value);
            XmlConfig.SetValue(configName, "LaserPulseWidth", numericUpDownLaserPulseWidth.Value);
            XmlConfig.SetValue(configName, "PrintLineLength", numericUpDownPrintLineLength.Value);
            XmlConfig.SetValue(configName, "PrintTotalLines", numericUpDownPrintTotalLines.Value);
            XmlConfig.SetValue(configName, "PrintShutterStartPixel", numericUpDownPrintShutterStartPixel.Value);
            XmlConfig.SetValue(configName, "PrintShutterStopPixel", numericUpDownPrintShutterStopPixel.Value);
            XmlConfig.SetValue(configName, "PrintShutterRedStartLine", numericUpDownPrintShutterRedStartLine.Value);
            XmlConfig.SetValue(configName, "PrintShutterRedStopLine", numericUpDownPrintShutterRedStoplLine.Value);
            XmlConfig.SetValue(configName, "PrintColorOffset", numericUpDownPrintColorOffset.Value);
            XmlConfig.SetValue(configName, "AutoTrigger", numericUpDownAutoTrigger.Value);
            XmlConfig.SetValue(configName, "GetStatusUpdate", numericUpDownGetStatusUpdate.Value);
            XmlConfig.SetValue(configName, "NumberOfLines", numericUpDownNumberOfLines.Value);
            XmlConfig.SetValue(configName, "DelayMs", numericUpDownDelayMs.Value);
            XmlConfig.SetValue(configName, "LcosTriggerSource", GetTriggerRadioButtonsValue());
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            //here I want to load config data from Xml
            XmlReadConfig();

        }

        private void buttonSaveConfig_Click(object sender, EventArgs e)
        {
            XmlSaveConfig();
        }

        private void numericUpDownPrintLineLength_ValueChanged(object sender, EventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {
            string configToLoad;

            configToLoad = listBoxConfigNames.SelectedItem.ToString();
            XmlReadNamedConfig(configToLoad);
        }

        private void buttonAddNewConfig_Click(object sender, EventArgs e)
        {
            string[] configNames=XmlConfig.GetSectionNames();

            foreach (string s in configNames)
            {
                if (s == textBoxCurrentConfig.Text)
                {
                    MessageBox.Show("Config name " + s + " is already defined", "Error");
                    return;
                }
            }
            listBoxConfigNames.Items.Add(textBoxCurrentConfig.Text);
            XmlSaveNamedConfig(textBoxCurrentConfig.Text);
        }

        private void buttonSaveConfig_Click_1(object sender, EventArgs e)
        {
            XmlSaveNamedConfig(listBoxConfigNames.SelectedItem.ToString());
        }

        private void numericUpDownStatusY_ValueChanged(object sender, EventArgs e)
        {

        }


    }
}
