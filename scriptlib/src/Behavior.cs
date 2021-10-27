namespace Aderite
{
    /// <summary>
    /// The main Aderite scripting object, this is the main way objects interact with the runtime.
    /// When a class inherits this class the aderite runtime will automatically connect methods
    /// and invoke them when needed, for more information look at documentation for scripted behaviors
    /// </summary>
    public class ScriptedBehavior
    {
        protected Entity Entity = null;

        public ScriptedBehavior()
        {}
    }
}
