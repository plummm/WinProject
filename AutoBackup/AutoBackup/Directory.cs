using System;
using System.Collections.ObjectModel;
using System.Linq;

namespace AutoBackup
{
    public class Directory : ViewModels.ProductViewModel
    {
        public Directory(string fullPath, string name)
        {
            this.FullPath = fullPath;
            this.Name = name;
            this.Children = new ObservableCollection<object>();
        }

        public ObservableCollection<object> Children
        {
            get;
            private set;
        }
    }
}
