using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Aderite
{
    /// <summary>
    /// The main Aderite scripting object, this is the main way objects interact with the runtime.
    /// When a class is marked with this attribute the aderite runtime will automatically connect methods
    /// and invoke them when needed, for more information look at documentation for scripted behaviors
    /// </summary>
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = false, Inherited = false)]
    public class ScriptedBehaviorAttribute : Attribute
    {
        public ScriptedBehaviorAttribute()
        {}
    }
}
