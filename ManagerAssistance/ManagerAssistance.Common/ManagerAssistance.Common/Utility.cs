using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;

namespace ManagerAssistance.Common
{
    public static class Utility
    {
        static readonly string IPIFY = "https://api.ipify.org/";

        public static void ExecuteComplexOperation<T>(Func<T> operation, Action<T> onSuccess, Action<Exception> onError)
        {
            try
            {
                onSuccess(operation());
            }
            catch (Exception e)
            {
                onError(e);
            }
        }

        public static string GetWanIp()
        {
            try
            {
                using (var client = new WebClient())
                    return client.DownloadString(IPIFY);
            }
            catch
            {
                return "?.?.?.?";
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="length"></param>
        public static string FormatFileSize(double length)
        {
            int order = 0;
            string[] sizes = { "B", "KB", "MB", "GB" };
            while (length >= 1024 && order + 1 < sizes.Length)
            {
                order++;
                length = length / 1024;
            }
            return $"{length:0.##} {sizes[order]}";
        }
    }
}
