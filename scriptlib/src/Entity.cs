using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Object representing an entity in the game world
    /// </summary>
    public class Entity
    {
        private IntPtr scene = IntPtr.Zero;
        private UIntPtr entity = UIntPtr.Zero;

        /// <summary>
        /// Returns a component of T
        /// </summary>
        /// <typeparam name="T">Component type</typeparam>
        /// <returns>T instance for this entity or null</returns>
        public T GetComponent<T>() where T : class
        {
            Type type = typeof(T);

            // Locate
            if (type.Equals(typeof(Transform)))
            {
                return new Transform(scene, entity) as T;
            }
            else if (type.Equals(typeof(MeshRenderer)))
            {
                return new MeshRenderer(scene, entity) as T;
            }
            else if (type.Equals(typeof(DynamicActor)))
            {
                return new DynamicActor(scene, entity) as T;
            }
            else if (type.Equals(typeof(StaticActor)))
            {
                return new StaticActor(scene, entity) as T;
            }
            else
            {
                // Try to resolve as a behavior
                if (!type.IsSubclassOf(typeof(ScriptedBehavior)))
                {
                    return null;
                }
                else
                {
                    return __ResolveBehavior(scene, entity, type) as T;
                }
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static object __ResolveBehavior(IntPtr scene, UIntPtr entity, Type type);
    }
}
