using System;
using System.Collections.Generic;
using System.Linq;

namespace AutoBackup.ViewModels
{
    public class ProductViewModel
    {
        // Data generation.
        private string _name;
        private string _fullpath;


        private static ProductViewModel GetResult()
        {
            return new ProductViewModel();
        }

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
    }
}
