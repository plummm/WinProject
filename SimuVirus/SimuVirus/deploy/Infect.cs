using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Win32;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Windows.Input;
using SimuVirus.tools;

namespace SimuVirus.deploy
{
    class Infect
    {
        public void infectPe(string filePath)
        {
            ulong textStart;
            ulong textEnd;
            ulong EntryPoint;

            PEreader peReader = new PEreader();
            peReader.initialize(filePath);
            for (int headerNo = 1; headerNo < peReader.imageSectionHeaders.Length; ++headerNo)
            {
                string name = new string(peReader.imageSectionHeaders[headerNo - 1].Name);
                if (name.Equals(".text\0\0\0"))
                {
                    textStart = peReader.imageBase + peReader.imageSectionHeaders[headerNo - 1].VirtualAddress;
                    EntryPoint = peReader.imageBase + peReader.addressOfEntryPoint;
                    textEnd = peReader.imageBase + peReader.imageSectionHeaders[headerNo].VirtualAddress;
                    break;
                }
            }
        }
    }
}
