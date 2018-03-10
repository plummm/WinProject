using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Win32;

namespace ClassAssistance.Common
{
    public class Startup
    {
        private static RegistryKey startupKey;
        private string appName;
        private string appPath;

        public Startup(string name, string path)
        {
            appName = name;
            appPath = path;
        }

        public void Start()
        {

            startupKey = Registry.CurrentUser.OpenSubKey("Software\\Microsoft\\Windows\\CurrentVersion\\Run", true);
            string MyValue = appPath + "\\" + appName;
            if (!IsInStartup(appName))
            {
                CreateStartup(appName, MyValue);
            }
        }

        private bool IsInStartup(string KeyName)
        {
            if (startupKey != null)
            {
                Object o = startupKey.GetValue(KeyName);
                if (o != null)
                {
                    return true;
                }

            }
            return false;
        }

        private void DeleteStartup(string KeyName)
        {
            startupKey.DeleteValue(KeyName);
        }

        private void CreateStartup(string KeyName, string KeyValue)
        {
            startupKey.SetValue(KeyName, KeyValue);
        }
    }
}
