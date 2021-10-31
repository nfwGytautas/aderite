using System.Runtime.InteropServices;

namespace Aderite
{
    /// <summary>
    /// 4 dimensional vector
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector4
    {
        public static Vector4 Zero = new Vector4(0.0f);

        public float x;
        public float y;
        public float z;
        public float w;
        
        public Vector4(float v)
        {
            x = v;
            y = v;
            z = v;
            w = v;
        }

        public Vector4(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }
    }

    /// <summary>
    /// 3 dimensional vector
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public static Vector3 Zero = new Vector3(0.0f);

        public float x;
        public float y;
        public float z;

        public Vector3(float v)
        {
            x = v;
            y = v;
            z = v;
        }

        public Vector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }
    }

    /// <summary>
    /// 2 dimensional vector
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
    {
        public static Vector2 Zero = new Vector2(0.0f);

        public float x;
        public float y;

        public Vector2(float v)
        {
            x = v;
            y = v;
        }

        public Vector2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }
    }
}
