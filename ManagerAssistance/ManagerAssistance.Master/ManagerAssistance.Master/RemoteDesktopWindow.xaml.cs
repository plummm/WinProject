using System.IO;
using System.Windows;
using System.Windows.Media.Imaging;
using ManagerAssistance.Common;
using ManagerAssistance.Common.Helpers;
using ManagerAssistance.Common.Network.Protocol;
using ManagerAssistance.Master.Extentions;

namespace ManagerAssistance.Master
{
    /// <summary>
    /// Logique d'interaction pour RemoteDesktopWindow.xaml
    /// </summary>
    public partial class RemoteDesktopWindow : ClientWindow
    {
        public RemoteDesktopWindow()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="client"></param>
        public RemoteDesktopWindow(Client client)
            : base(client)
        {
            InitializeComponent();

            // in case we leave the window withouth stopping
            Closing += (s, e) => StopCapture();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnStartCapture(object sender, RoutedEventArgs e) =>
            StartCapture(0, int.Parse(TxtBoxQuality.Text), int.Parse(TxtBoxRate.Text));

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnStopCapture(object sender, RoutedEventArgs e) => StopCapture();

        /// <summary>
        /// 
        /// </summary>
        private void StartCapture(int screen = 0, int quality = 10, int rate = 10) => Send(new StartScreenCaptureMessage
        {
            Quality = quality,
            Rate = rate,
            ScreenNumber = screen
        });

        /// <summary>
        /// 
        /// </summary>
        private void StopCapture() => Send(new StopScreenCaptureMessage());

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        private void UpdateScreenCapture(ScreenCaptureMessage message)
        {
            if (ScreenCaptureImage.Source != null)
                ((BitmapImage)ScreenCaptureImage.Source).StreamSource.Dispose();
            ScreenCaptureImage.Source = ImageHelpers.BitmapToImageSource(message.RawImage);
        }

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
                        ev.Data.Match().With<ScreenCaptureMessage>(UpdateScreenCapture);
                        break;
                    }
                }
            });
        }
    }
}
