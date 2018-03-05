using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BlackHole.Common;
using System.Diagnostics;

namespace BlackHole.Slave.Malicious
{
    public sealed class TaskManager : Singleton<TaskManager>, IMalicious
    {
        public void Initialize()
        {
            GetProcessList();
            GetNetworkList();
        }

        private void GetProcessList()
        {
            Process[] processlist = Process.GetProcesses();

            foreach (Process theprocess in processlist)
            {
                Console.WriteLine("Process: {0} ID: {1}", theprocess.ProcessName, theprocess.Id);
            }
        }

        private void GetNetworkList()
        {

        }
    }
}
