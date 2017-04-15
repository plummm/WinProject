using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AutoBackup
{
    public class File : ViewModels.ProductViewModel
    {

        public File(string fullPath, string name)
        {
            this.FullPath = fullPath;
            this.Name = name;
        }
        /*
        public string Name
        {
            get { return this._name; }
            set { this._name = value; }
        }
        public string FullPath
        {
            get { return this._fullpath; }
            set { this._fullpath = value; }
        }
        */
        /*
        public string FullPath
        {
            get;
            set;
        }

        public string Name
        {
            get;
            set;
        }
        */
    }
}
