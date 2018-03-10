using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ClassAssistance.Common;
using System.Diagnostics;
using System.Threading;
using ClassAssistance.Common.Network.Protocol;

namespace ClassAssistance.Slave.Malicious
{
    public sealed class TaskManager : Singleton<TaskManager>, IMalicious
    {
        public static Process[] processlist;
        public void Initialize()
        {
            //GetTasksList();
        }

        public static Dictionary<string, string>[] GetTasksList(Dictionary<string, List<PerformanceCounter>> taskInfo)
        {
            processlist = Process.GetProcesses();
            var counters = new List<PerformanceCounter>();
            List<Dictionary<string, string>> taskslist = new List<Dictionary<string, string>>();

            
            foreach (Process theprocess in processlist)
            {
                //Console.WriteLine(theprocess.ProcessName);
                if (!taskInfo.ContainsKey(theprocess.ProcessName))
                {
                    PerformanceCounter ramCounter = new PerformanceCounter("Process", "Working Set", theprocess.ProcessName);
                    PerformanceCounter cpuCounter = new PerformanceCounter("Process", "% Processor Time", theprocess.ProcessName);
                    PerformanceCounter rbsCounter = new PerformanceCounter("Process", "IO Data Bytes/sec", theprocess.ProcessName);
                    taskInfo[theprocess.ProcessName] = new List<PerformanceCounter>(new PerformanceCounter[] { ramCounter, cpuCounter, rbsCounter});
                }
                double ram = taskInfo[theprocess.ProcessName][0].NextValue();
                double cpu = taskInfo[theprocess.ProcessName][1].NextValue();
                double rbs = taskInfo[theprocess.ProcessName][2].NextValue();
                taskslist.Add(new Dictionary<string, string>
                {
                    ["TaskName"] = theprocess.ProcessName,
                    ["PID"] = theprocess.Id.ToString(),
                    ["CPU"] = Math.Round(cpu, 2).ToString() + " %",
                    ["Memory"] = Math.Round((ram / 1024 / 1024), 2).ToString() + " MB",
                    ["Network"] = Math.Round((rbs / 1024), 2).ToString() + " kb/s"
                });
                //Console.WriteLine("RAM: " + (ram / 1024 / 1024) + " MB; CPU: " + (cpu) + " %; Network: " + (rbs / 1024 / 1024));
            }

            return taskslist.ToArray();
            
        }
    }
}
