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
                MessageBox.Show("设置开机启动成功");
            }));
        }

        private void Infect_PE(object sender, RoutedEventArgs e)
        {
            tasks.Add(Task.Factory.StartNew(() =>
            {
                Infect infect = new Infect();
                infect.infectPe("C:\\Users\\etenal\\Downloads\\re的传统.exe");
            }));
        }
    }
}
