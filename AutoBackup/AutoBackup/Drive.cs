using System;
using System.Collections.ObjectModel;
using System.Linq;

namespace AutoBackup
{
    public class Drive : ViewModels.ProductViewModel
    {
        public Drive(string name, bool isReady)
        {
            this.Name = name;
            this.IsReady = isReady;
            this.Children = new ObservableCollection<object>();
        }
        /*
        public string Name
        {
            get;
            set;
        }
        */
        public bool IsReady
        {
            get;
            set;
        }
        public ObservableCollection<object> Children
        {
            get;
            private set;
        }
    }
}
