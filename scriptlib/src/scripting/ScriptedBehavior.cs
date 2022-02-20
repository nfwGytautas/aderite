using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// The main class used to create scripted behaviors for game objects.
    /// There is a couple hidden methods that one can override that are found using reflection.
    /// 
    /// void Update(float) - this function is called once every frame with the float argument being the delta time of the frame
    /// void OnTriggerEnter(TriggerEvent) - this function is called when the object enters a trigger zone
    /// void OnTriggerLeave(TriggerEvent) - this function is called when the object leave a trigger zone
    /// void OnTriggerWasEntered(TriggerEvent) - this function is called when another object enters a trigger collider attached to this game object
    /// void OnTriggerWasLeft(TriggerEvent) - this function is called when another object leaves a trigger collider attached to this game object
    /// 
    /// </summary>
    class ScriptedBehavior
    {
        // The C++ instance of this object
        IntPtr Instance = IntPtr.Zero;

        /// <summary>
        /// The game object of this behavior
        /// </summary>
        protected GameObject GameObject { get { return __GetGameObject(Instance); } }

        internal ScriptedBehavior(IntPtr instance)
        {

        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static GameObject __GetGameObject(IntPtr instance);
    }
}
