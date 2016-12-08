using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Net;
using System.Threading;
using Newtonsoft.Json;
using System.IO;

namespace LibreNMS_XmasLights
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("");
            Console.Write("Enter the COM Port to use: ");
            string comPort = Console.ReadLine();
            Console.Write("Enter the LibreNMS URL: ");
            string api = Console.ReadLine();
            Console.Write("Enter the LibreNMS API Token: ");
            string token = Console.ReadLine();
            Console.WriteLine("");
            //
            SerialPort serialPort = new SerialPort();
            serialPort.PortName = comPort;
            serialPort.DataBits = 8;
            serialPort.Parity = Parity.None;
            serialPort.StopBits = StopBits.One;
            serialPort.BaudRate = 9600;
            //
            serialPort.Open();
            serialPort.Write("w");
            Console.WriteLine("Serial Port Opened.");
            // setup URL getter
            while (true) {
                WebRequest wr = WebRequest.Create(api + "/api/v0/alerts");
                wr.Headers["X-Auth-Token"] = token;
                WebResponse response = wr.GetResponse();
                Stream dataStream = response.GetResponseStream();
                StreamReader reader = new StreamReader(dataStream);
                string responseFromServer = reader.ReadToEnd();
                // parse
                Console.WriteLine(responseFromServer);
                var results = JsonConvert.DeserializeObject<dynamic>(responseFromServer);
                if (results.count > 0)
                {
                    int warning = 0;
                    int critical = 0;
                    foreach(var x in results.alerts)
                    {
                        if (x.state == "1")
                        {
                            if (x.severity == "warning")
                            {
                                warning = warning + 1;
                            } else
                            {
                                critical = critical + 1;
                            }
                        }
                    }
                    //
                    if (critical > 0)
                    {
                        serialPort.Write("r");
                    }
                    else
                    {
                        serialPort.Write("o");
                    }
                } else
                {
                    serialPort.Write("g");
                }
                // end parse
                reader.Close();
                response.Close();
                Thread.Sleep(5000);
            }
        }
    }
}
