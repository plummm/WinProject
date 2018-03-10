using System;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Media.Imaging;
using ClassAssistance.Common;
using ClassAssistance.Common.Network.Protocol;
using ClassAssistance.Master.Extentions;
using System.Collections.Generic;
using System.Collections;
using System.Windows.Input;

namespace ClassAssistance.Master
{
    /// <summary>
    /// 
    /// </summary>
    public partial class TasksWindow : SlaveWindow
    {
        public TasksWindow()
        {
            InitializeComponent();
            Send(new StartTasksMessage());
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="slave"></param>
        public TasksWindow(Slave slave)
            : base(slave)
        {
            InitializeComponent();
            Send(new StartTasksMessage());
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ev"></param>
        public override async void OnEvent(SlaveEvent ev)
        {
            base.OnEvent(ev);
            await this.ExecuteInDispatcher(() =>
            {
                switch ((SlaveEventType)ev.EventType)
                {
                    case SlaveEventType.IncommingMessage:
                    {
                        ev.Data.Match()
                            .With<TasksMessage>(message =>
                            {
                                TasksList.ItemsSource = message.Tasks.Select(c => c.Dictionary).ToArray();
                            });
                        break;
                    }
                }
            });
        }

        private void KillProcess(object sender, RoutedEventArgs e)
        {
            if (TasksList.SelectedItem != null)
            {
                Dictionary<string, string> selectedItem = TasksList.SelectedItem as Dictionary<string, string>;
                //string processName = (new Dictionary<string, string>(TasksList.SelectedItem).Items[0]).Value;

                Send(new KillProcessMessage
                {
                    PID = selectedItem["PID"]
                });
            }
        }

        protected override void OnClosed(EventArgs e)
        {
            Send(new StopTasksMessage());
            base.OnClosed(e);
        }
    }
}
