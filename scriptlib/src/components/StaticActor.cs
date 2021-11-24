using System;

namespace Aderite
{
    /// <summary>
    /// Static physics actor, that should ideally not be moved
    /// </summary>
    public class StaticActor : PhysicsActor
    {
        internal StaticActor(IntPtr instance) : base(instance)
        {
        }
    }
}
