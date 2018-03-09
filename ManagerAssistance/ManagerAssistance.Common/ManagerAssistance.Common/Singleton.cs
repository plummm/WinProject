using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ManagerAssistance.Common
{
    public class Singleton<T>
        where T : class, new()
    {
        public static T Instance { get; } = new T();
    }
}
