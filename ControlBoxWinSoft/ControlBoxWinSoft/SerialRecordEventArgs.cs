using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ControlBoxWinSoft
{
    public class SerialRecordEventArgs : System.EventArgs
    {
        private char[] separatorsAllowed=new char[]{' ','\r','\n'};

        public string debugMessage;

        private string receivedString;
        public string ReceivedString
        {
            get { return receivedString; }
            set { receivedString = value; }
        }

        public char channel, command, direction;
        public int nValues;
        public int[] values;
        public int crc;

        public SerialRecordEventArgs(SerialRecordEventArgs arg)
        {
            debugMessage = arg.debugMessage;
            receivedString = arg.ReceivedString;
            channel = arg.channel;
            command = arg.command;
            direction = arg.direction;
            nValues = arg.nValues;
            values = arg.values;
            crc = arg.crc;
        }

        public SerialRecordEventArgs()
        {
            Init("");
        }

        public SerialRecordEventArgs(string s)
        {
            Init(s);
        }

        private void Init(string s)
        {
            values = new int[10];
            receivedString = s;
        }

        public bool ParseRecord()
        {
            int i, n, old_n;
            string s;

            i = 0;
            while (i < receivedString.Length)
            {
                if (receivedString[i++] == '@')
                {
                    command = receivedString[i++];
                    channel = receivedString[i++];
                    direction = receivedString[i++];
                    n = receivedString.IndexOfAny(separatorsAllowed);
                    s = receivedString.Substring(i, n-i+1);
                    try
                    {
                        crc = Convert.ToInt32(s);
                    }
                    catch (Exception ex)
                    {
                        System.Windows.Forms.MessageBox.Show("Parsing error", ex.Message + ":" + s + ".");
                        return false;
                    }

                    i = 0;
                    //here n is either space or \n - in which case it means no more parameters.
                    while (receivedString[n] == ' ')
                    {
                        old_n = n;
                        n=receivedString.IndexOfAny(separatorsAllowed, old_n+1);
                        s = receivedString.Substring(old_n + 1, n - old_n - 1);
//                        values[i] = Convert.ToInt32(receivedString.Substring(old_n + 1, n - old_n - 1));
                        try
                        {
                            values[i++] = Convert.ToInt32(s);
                        }
                        catch (Exception ex)
                        {
                            System.Windows.Forms.MessageBox.Show("(2)Parsing error", ex.Message + ":" + s + ":");
                            return false; 
                        }

                    }
                    nValues = i;
                    debugMessage = "Found " + command + " " + channel + " " + direction + " " + s + " " + crc.ToString() + "pars=" + nValues.ToString();
                    return true;

                }
            }
            return false;
        }
    }
}
