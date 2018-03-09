
namespace ManagerAssistance.Master.Model
{
    public sealed class ClientMonitorModel : ViewModel
    {
        public string ListeningState
        {
            get;
            private set;
        }

        public string OnlineClients
        {
            get;
            private set;
        }

        public ClientMonitorModel()
        {
            SetListeningState("Not bound");
            SetOnlineClients(0);
        }

        public void SetListeningState(string state)
        {
            ListeningState = $"State : {state}";
            NotifyPropertyChange("ListeningState");
        }

        public void SetOnlineClients(int count)
        {
            OnlineClients = $"Online Clients : {count}";
            NotifyPropertyChange("OnlineClients");
        }
    }
}
