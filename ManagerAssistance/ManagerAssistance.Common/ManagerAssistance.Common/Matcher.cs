using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ManagerAssistance.Common
{
    public static class ObjectExtensions
    {
        public static Matcher<T> Match<T>(this T obj) where T : class => new Matcher<T>(obj);
    }


    public sealed class Matcher<T>
        where T : class
    {

        private bool m_matched;


        public bool WasHandled => m_matched;


        private readonly T m_obj;


        public Matcher(T obj)
        {
            m_obj = obj;
        }


        public Matcher<T> With<V>(Action<V> action)
            where V : class, T
        {
            if (!m_matched)
            {
                V v = m_obj as V;
                if (v != null)
                {
                    action(v);
                    m_matched = true;
                }
            }
            return this;
        }


        public void Default(Action<object> action)
        {
            if (!m_matched)
                action(m_obj);
        }
    }
}
