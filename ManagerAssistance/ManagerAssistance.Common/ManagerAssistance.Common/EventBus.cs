using System;
using System.Collections.Generic;

namespace ManagerAssistance.Common
{
    public interface IEventListener<TEvent, TSource>
        where TEvent : Event<TSource>
    {
        void OnEvent(TEvent ev);
    }


    public abstract class Event<T>
    {
        public int EventType { get; }
        public T Source { get; }
        public object Data { get; }

        protected Event(int eventType, T source, object data)
        {
            EventType = eventType;
            Source = source;
            Data = data;
        }
    }


    public sealed class EventBus<TEvent, TSource>
        where TEvent : Event<TSource>
    {
        private class Subscriber
        {
            public Predicate<TEvent> Guard
            {
                get;
            }
            public IEventListener<TEvent, TSource> Listener
            {
                get;
            }

            public Subscriber(Predicate<TEvent> guard, IEventListener<TEvent, TSource> listener)
            {
                Guard = guard;
                Listener = listener;
            }
        }


        private readonly List<Subscriber> m_subscriber;


        public EventBus()
        {
            m_subscriber = new List<Subscriber>();
        }


        public void Subscribe(IEventListener<TEvent, TSource> listener) => Subscribe(e => true, listener);


        public void Subscribe(Predicate<TEvent> guard, IEventListener<TEvent, TSource> listener)
            => m_subscriber.Add(new Subscriber(guard, listener));


        public void Unsubscribe(IEventListener<TEvent, TSource> listener)
            => m_subscriber.RemoveAll(s => s.Listener == listener);


        public void PostEvent(TEvent ev) =>
            m_subscriber.ForEach(s =>
            {
                if (s.Guard(ev))
                {
                    try
                    {
                        s.Listener.OnEvent(ev);
                    }
                    catch (Exception e)
                    {
                    }
                }
            });
    }
}
