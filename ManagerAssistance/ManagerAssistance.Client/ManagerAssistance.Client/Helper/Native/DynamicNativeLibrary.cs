using System;
using System.Runtime.ConstrainedExecution;
using System.Runtime.InteropServices;
using System.Security.Permissions;
using ManagerAssistance.Common;
using Microsoft.Win32.SafeHandles;

namespace ManagerAssistance.Client.Helper.Native.Impl
{
    [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode = true)]
    public sealed class SafeLibraryHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        private SafeLibraryHandle() : base(true) { }
        protected override bool ReleaseHandle() => DynamicNativeLibraryHelper.FreeLibrary(handle);
    }

    public static class DynamicNativeLibraryHelper
    {
        public const string KERNEL_LIBRARY = "kernel32";
        [DllImport(KERNEL_LIBRARY, CharSet = CharSet.Auto, BestFitMapping = false, SetLastError = true)]
        public static extern SafeLibraryHandle LoadLibrary(string fileName);

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        [DllImport(KERNEL_LIBRARY, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool FreeLibrary(IntPtr module);

        [DllImport(KERNEL_LIBRARY)]
        public static extern IntPtr GetProcAddress(SafeLibraryHandle module, string procName);
    }

    public class DynamicNativeLibrary<T> : Singleton<T>, IDisposable
        where T : DynamicNativeLibrary<T>, new()
    {

        private readonly SafeLibraryHandle m_libraryHandle;


        public DynamicNativeLibrary(string fileName)
        {
            m_libraryHandle = DynamicNativeLibraryHelper.LoadLibrary(fileName);
            if (m_libraryHandle.IsInvalid)
                Marshal.ThrowExceptionForHR(Marshal.GetHRForLastWin32Error());
        }


        public TT FindUmanagedFunction<TT>(string functionName) where TT : class
        {
            var ptr = DynamicNativeLibraryHelper.GetProcAddress(m_libraryHandle, functionName);
            if (ptr == IntPtr.Zero)
                return null;

            return Marshal.GetDelegateForFunctionPointer(ptr, typeof(TT)) as TT;
        }


        public void Dispose()
        {
            if (!m_libraryHandle.IsClosed)
                m_libraryHandle.Close();
        }
    }
}
