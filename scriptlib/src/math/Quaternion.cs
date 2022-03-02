using System.Runtime.InteropServices;

namespace Aderite
{
    /// <summary>
    /// Quaternion struct used to specify rotations
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Quaternion
    {
        public static Quaternion Zero = new Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

        public float w;
        public float x;
        public float y;
        public float z;

        public Quaternion(float v)
        {
            x = v;
            y = v;
            z = v;
            w = v;
        }

        public Quaternion(float w, float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }
    }
}
