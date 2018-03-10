using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.IO;
using System.Diagnostics;
using System.Threading;
using ClassAssistance.Common;

namespace Deamon
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            //this.Hide();
            CheckForStartup();
            while (!CheckClassAssistance())
            {
                Thread.Sleep(3000);
            }
            Monitor();
        }

        private void CheckForStartup()
        {
            Startup startup = new Startup("Deamon.exe", Directory.GetCurrentDirectory());
            startup.Start();
        }

        private bool CheckClassAssistance()
        {
            Process[] classAssistance = Process.GetProcessesByName("ClassAssistance.Slave");
            if (classAssistance.Count() == 1)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        private static void ShutdownLocal()
        {
            //MessageBox.Show("Deamon Shutdown!");
            
            string param = "/s /t 0";
            var psi = new ProcessStartInfo("shutdown", param)
            {
                CreateNoWindow = true,
                UseShellExecute = false
            };
            Process.Start(psi);
            
        }

        private void Monitor()
        {
            while(CheckClassAssistance())
            {
                Thread.Sleep(5000);
            }
            ShutdownLocal();
        }
    }
}
