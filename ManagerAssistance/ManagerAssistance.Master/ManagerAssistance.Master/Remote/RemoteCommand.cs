using System;
using System.Windows.Media;
using ManagerAssistance.Master.Model;

namespace ManagerAssistance.Master.Remote
{
    public interface IRemoteCommand
    {
        long Id { get; }
        string Name { get; }
        Client Client { get; }
        long Progress { get; set; }
        Brush ProgressColor { get; set; }
        bool Completed { get; set; }

        void DoExecute();
        void DoFault();
        void UpdateProgression(long current, long maximum);
    }

    public sealed class RemoteCommand<TIn> : CommandModel, IRemoteCommand
    {
        public string Name { get; }
        private readonly Action<RemoteCommand<TIn>> m_executeAction;
        private readonly Action m_faultedAction;
        private readonly Action<TIn> m_continueAction;
        private readonly Action<TIn> m_completedAction;

        public RemoteCommand(long id, string name, Client client, string headerText, string targetText,
            Action<RemoteCommand<TIn>> onExecute, Action<TIn> onContinue, Action<TIn> onCompleted, Action onFaulted)
            : base(id, client, headerText, targetText)
        {
            Name = name;
            m_executeAction = WrapAction(ClientEventType.CommandExecuted, onExecute);
            m_continueAction = WrapAction(ClientEventType.CommandContinue, onContinue);
            m_completedAction = WrapAction(ClientEventType.CommandCompleted, onCompleted);
            m_faultedAction = WrapAction(ClientEventType.CommandFaulted, onFaulted);
        }

        private Action WrapAction(ClientEventType type, Action action) => () =>
        {
            action();
            Client.ClientEvents.PostEvent(new ClientEvent(type, Client, this));
        };

        private Action<T> WrapAction<T>(ClientEventType type, Action<T> action) => input =>
        {
            action(input);
            Client.ClientEvents.PostEvent(new ClientEvent(type, Client, this));
        };

        public void DoExecute() => m_executeAction(this);
        public void DoContinue(TIn input) => m_continueAction(input);
        public void DoComplete(TIn input) => m_completedAction(input);
        public void DoFault() => m_faultedAction();
    }
}
