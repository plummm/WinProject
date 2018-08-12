using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimuVirus.deploy
{
    class Info
    {
        public static string currentDirectory { set; get; }

        public void initialize()
        {
            if (currentDirectory == null)
                currentDirectory = this.getCurrentDirectory();
        }

        private string getCurrentDirectory()
        {
            string path = System.IO.Directory.GetCurrentDirectory();
            return path;
        }
    }
}
