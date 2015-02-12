using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace testRF21x
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("RF21x started...");

            RF21xHidSerialNumbers sn = rf21x.getHidSerialNumber();
            for (int i = 0; i < sn.Count; ++i)
            {
                sn[i] = String.Format("hid://{0}", sn[i]);
                Console.Write("HID Device {0}: {1}\n", i, sn[i]);
            }
            Console.Write("Finish listing all current HID device.\n\n");

            if (sn.Count < 1)
            {
                Console.Write("No HID device.\n");
                return;
            }

            RF21xDevice[] rf = new RF21xDevice[sn.Count];

            for (int i = 0; i < sn.Count; ++i)
            {
                rf[i] = new RF21xDevice();
                if (rf[i].open(rf21x.RF21X_DT_RF219, sn[i], 0, 40))
                {
                    Console.Write("Success to open HID device {0}: {1}\n", i, sn[i]);
                    System.Threading.Thread.Sleep(300);
                }
                else
                {
                    Console.Write("Fail to open HID device {0}: {1}\n", i, sn[i]);
                    continue;
                }
                if (rf[i].startQuiz(rf21x.RF21X_QT_Single))
                {
                    Console.Write("Success to start a quiz\n");
                }
            }

            Console.Write("Getting message from device, Press Ctrl+C to break...\n");
            RF21xMessage message = new RF21xMessage();
            while (true)
            {
                for (int i = 0; i < sn.Count; ++i)
                {
                    if (rf[i].getMessage(message))  
                    {
                        if (message.messageType == rf21x.RF21X_MT_Teacher)
                        {
                            Console.WriteLine("Teacher: {0}", message.data);
                        }
                        else if (message.messageType == rf21x.RF21X_MT_Student)
                        {
                            Console.WriteLine("Student {0} for current question: {1}", message.keypadId, message.data);
                        }
                        else if (message.messageType == rf21x.RF21X_MT_SetId)
                        {
                            Console.WriteLine("Set ID {0}", message.keypadId);
                        }
                        Console.Write("Raw data: ");
                        for (int j = 0; j < message.rawData.Count; ++j)
                        {
                            Console.Write("{0:X2} ", message.rawData[j]);
                        }
                        Console.WriteLine("");
                    }
                    else
                    {
                        System.Threading.Thread.Sleep(100);
                    }
                }
            }

            Console.WriteLine("RF21x finished...");
        }
    }
}
