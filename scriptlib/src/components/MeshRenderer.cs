using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Aderite
{
    /// <summary>
    /// Mesh renderer component
    /// </summary>
    public class MeshRenderer
    {
        IntPtr scene = IntPtr.Zero;
        UIntPtr entity = UIntPtr.Zero;

        public Mesh Mesh { get { return new Mesh(__GetMesh(scene, entity)); } set { __SetMesh(scene, entity, value.GetInstance()); } }
        public Material Material { get { return new Material(__GetMaterial(scene, entity)); } set { __SetMaterial(scene, entity, value.GetInstance()); } }

        internal MeshRenderer(IntPtr scene, UIntPtr entity) 
        {
            this.scene = scene;
            this.entity = entity;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static IntPtr __GetMesh(IntPtr scene, UIntPtr entity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static IntPtr __GetMaterial(IntPtr scene, UIntPtr entity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetMesh(IntPtr scene, UIntPtr entity, IntPtr mesh);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetMaterial(IntPtr scene, UIntPtr entity, IntPtr material);
    }
}
