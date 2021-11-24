using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Script system is the main way that the aderite world is manipulated. By inheriting from this class the user is able to
    /// create an arbitrary system that operates in a scene.
    /// </summary>
    public class ScriptSystem
    {
        protected ScriptSystem()
        {}

        /// <summary>
        /// Instantiate a new entity and the specified origin
        /// </summary>
        /// <param name="entity">Entity to clone</param>
        /// <param name="origin">New position of the entity</param>
        /// <returns>Entity instance</returns>
        protected Entity Instantiate(Entity entity, Vector3 origin) 
        {
            return __InstantiateEntity(entity.GetInstance(), origin);
        }

        /// <summary>
        /// Instantiate a new entity and the specified origin
        /// </summary>
        /// <param name="prefab">Prefab to instantiate from</param>
        /// <param name="origin">New position of the entity</param>
        /// <returns>Entity instance</returns>
        protected Entity Instantiate(Prefab prefab, Vector3 origin)
        {
            return __InstantiatePrefab(prefab.GetInstance(), origin);
        }

        /// <summary>
        /// Destroy the specified entity, consequently removing it from the current scene
        /// </summary>
        /// <param name="entity">Entity to destroy</param>
        protected void Destroy(Entity entity)
        {
            __Destroy(entity.GetInstance());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Entity __InstantiateEntity(IntPtr entity, Vector3 origin);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Entity __InstantiatePrefab(IntPtr prefab, Vector3 origin);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __Destroy(IntPtr entity);
    }
}
