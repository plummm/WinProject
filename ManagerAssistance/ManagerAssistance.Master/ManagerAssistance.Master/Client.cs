using ManagerAssistance.Common;
using ManagerAssistance.Common.Network.Protocol;
using NetMQ;

namespace ManagerAssistance.Master
{
    public enum ClientEventType : int
    {
        Connected,
        Disconnected,
        IncommingMessage,
        OutgoingMessage,
        CommandExecuted,
        CommandContinue,
        CommandFaulted,
        CommandCompleted,
        FileDownloaded,
        FileUploaded
    }

    /// <summary>
    /// 
    /// </summary>
    public class ClientEvent : Event<Client>
    {
        public ClientEvent(ClientEventType eventType, Client client, object data = null)
            : base((int)eventType, client, data)
        {
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public sealed class Client
    {
        /// <summary>
        /// 
        /// </summary>
        public static EventBus<ClientEvent, Client> ClientEvents => Singleton<EventBus<ClientEvent, Client>>.Instance;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ev"></param>
        public static void PostEvent(ClientEvent ev) => ClientEvents.PostEvent(ev);

        /// <summary>
        /// 
        /// </summary>
        public byte[] Identity { get; }

        /// <summary>
        /// 
        /// </summary>
        public int Id { get; }

        /// <summary>
        /// 
        /// </summary>
        public string Ip { get; private set; }

        /// <summary>
        /// 
        /// </summary>
        public string OperatingSystem { get; private set; }

        /// <summary>
        /// 
        /// </summary>
        public string MachineName { get; private set; }

        /// <summary>
        /// 
        /// </summary>
        public string UserName { get; private set; }

        /// <summary>
        /// 
        /// </summary>
        public int PingTimeout { get; set; }

        /// <summary>
        /// 
        /// </summary>
        public bool IsInitialized { get; set; }

        /// <summary>
        /// 
        /// </summary>
        public string OutputDirectory => MachineName;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="identity"></param>
        /// <param name="id"></param>
        public Client(byte[] identity, int id)
        {
            Identity = identity;
            Id = id;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ip"></param>
        /// <param name="os"></param>
        /// <param name="machine"></param>
        /// <param name="user"></param>
        public bool Initialize(string ip, string os, string machine, string user)
        {
            if (IsInitialized)
                return false;
            IsInitialized = true;
            Ip = ip;
            OperatingSystem = os;
            MachineName = machine;
            UserName = user;
            return true;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        public bool Send(NetMessage message)
        {
            var frames = new NetMQMessage();
            frames.Append(Identity);
            frames.Append(message.Serialize());
            FireClientOutgoingMessage(this, message);
            return NetworkService.Instance.Send(frames);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="client"></param>
        /// <param name="message"></param>
        private void FireClientOutgoingMessage(Client client, NetMessage message) =>
            PostEvent(new ClientEvent(ClientEventType.OutgoingMessage, client, message));

        /// <summary>
        /// 
        /// </summary>
        public bool PingAndIncrementTimeout()
        {
            PingTimeout++;
            return Send(new PingMessage());
        }

        /// <summary>
        /// 
        /// </summary>
        public void DecrementPingTimeout() => PingTimeout--;

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public override string ToString() =>
            $"ip={Ip} id={Id} machine={MachineName} user={UserName} os={OperatingSystem}";
    }
}
