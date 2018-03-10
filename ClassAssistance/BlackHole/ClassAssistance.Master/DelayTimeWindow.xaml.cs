using System.IO;
using System.Windows;
using System.Windows.Media.Imaging;
using ClassAssistance.Common;
using ClassAssistance.Common.Helpers;
using ClassAssistance.Common.Network.Protocol;
using ClassAssistance.Master.Extentions;
using System.Diagnostics;
using System;

namespace ClassAssistance.Master
{
    /// <summary>
    /// DelayTimeWindow.xaml 的交互逻辑
    /// </summary>
    public partial class DelayTimeWindow : SlaveWindow
    {
        private string action;
        public DelayTimeWindow()
        {
            InitializeComponent();
        }

        public DelayTimeWindow(string flag,Slave slave) 
            : base(slave)
        {
            InitializeComponent();
            action = flag;
        }

        

        private void SetTimer(object sender, RoutedEventArgs e)
        {
          
            switch(action)
            {
                case "/s":
                    Send(new ShutdownRemoteMessage
                    {
                        DelayTime = (Convert.ToInt32(Hours.Text) * 60 * 60 +
                            Convert.ToInt32(Miniutes.Text) * 60 +
                            Convert.ToInt32(Seconds.Text)).ToString()
                    });
                    break;
                case "/r":
                    Send(new RebootRemoteMessage
                    {
                        DelayTime = (Convert.ToInt32(Hours.Text) * 60 * 60 +
                            Convert.ToInt32(Miniutes.Text) * 60 +
                            Convert.ToInt32(Seconds.Text)).ToString()
                    });
                    break;
                default:
                    break;
            }

            this.Close();
        }
    }
}
