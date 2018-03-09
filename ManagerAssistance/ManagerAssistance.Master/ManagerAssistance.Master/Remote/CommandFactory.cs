using System;
using System.Threading;

namespace ManagerAssistance.Master.Remote
{
    public enum CommandType
    {
        Download,
        Upload
    }

    public static class CommandFactory
    {
        private static long m_nextCommandId;
        public static long NextCommandId => Interlocked.Increment(ref m_nextCommandId);

        public static IRemoteCommand CreateCommand<TIn>(CommandType type,
            Client client,
            string targetText,
            Action<RemoteCommand<TIn>> onExecute,
            Action<TIn> onContinue,
            Action<TIn> onCompleted,
            Action onFaulted)
        {
            switch (type)
            {
                case CommandType.Download:
                    return new RemoteCommand<TIn>(
                        NextCommandId,
                        "Download",
                        client,
                        "Downloading...",
                        targetText,
                        onExecute,
                        onContinue,
                        onCompleted,
                        onFaulted);

                case CommandType.Upload:
                    return new RemoteCommand<TIn>(
                        NextCommandId,
                        "Upload",
                        client,
                        "Uploading...",
                        targetText,
                        onExecute,
                        onContinue,
                        onCompleted,
                        onFaulted);

                default:
                    return null;
            }
        }
    }
}
