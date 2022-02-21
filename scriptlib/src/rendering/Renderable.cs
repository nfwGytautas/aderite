using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Renderable component is used to control the visual rendering of the game object during runtime
    /// </summary>
    public class Renderable
    {
        /// <summary>
        /// Mesh of the game object
        /// </summary>
        public Mesh Mesh { get { return __GetMesh(Instance); } set { __SetMesh(Instance, value); } }

        /// <summary>
        /// Material of the game object
        /// </summary>
        public Material Material { get { return __GetMaterial(Instance); } set { __SetMaterial(Instance, value); } }

        // The C++ instance of this object
        IntPtr Instance = IntPtr.Zero;

        internal Renderable(IntPtr instance)
        {
            Instance = instance;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Mesh __GetMesh(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetMesh(IntPtr instance, Mesh value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Material __GetMaterial(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetMaterial(IntPtr instance, Material value);
    }
}
