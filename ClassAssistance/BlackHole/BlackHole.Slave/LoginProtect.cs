using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.IO;
using BlackHole.Common;
using System.Windows;

namespace BlackHole.Slave
{
    public class LoginProtect
    {
        public static Timer shutdownTimer;

        public LoginProtect()
        {
            InitializeTimer();
            CheckForStartup();

            Task.Factory.StartNew(() =>
            {
                while (!CheckDeamon())
                {
                    System.Threading.Thread.Sleep(5000);
                }
                Task.Factory.StartNew(() => Monitor());
            });
                
            
        }

        private void CheckForStartup()
        {
            Startup startup = new Startup("BlackHole.Slave.exe", Directory.GetCurrentDirectory());
            startup.Start();
        }

        private void Monitor()
        {
            while (CheckDeamon())
            {
                System.Threading.Thread.Sleep(5000);
            }
            //MessageBox.Show("Slave Shutdown!");
            ShutdownLocal(null, null);
        }

        private bool CheckDeamon()
        {
            Process[] classAssistance = Process.GetProcessesByName("Deamon");
            if (classAssistance == null || classAssistance.Count() != 1)
            {
                return false;
            }
            else
            {
                return true;
            }
        }


        public static void ShutdownLocal(Object source, ElapsedEventArgs e)
        {
            //MessageBox.Show("Time up Shutdown!");
            
            string param = "/s /t 0";
            var psi = new ProcessStartInfo("shutdown", param)
            {
                CreateNoWindow = true,
                UseShellExecute = false
            };
            Process.Start(psi);
            
        }

        private static void InitializeTimer()
        {
            shutdownTimer = new Timer(60000);
            shutdownTimer.Elapsed += ShutdownLocal;
            shutdownTimer.AutoReset = false;
            shutdownTimer.Enabled = true;
            shutdownTimer.Start();
        }

        public static void TimeStop()
        {
            if (shutdownTimer!=null)
            {
                shutdownTimer.Stop();
            }
        }
    }
}
