using System;
using System.Runtime.CompilerServices;

namespace L3gion
{
    public struct Vec3
    {
        public float x, y, z;

        public static Vec3 Zero => new Vec3(0.0f);

        public Vec3(float values)
        {
            x = values;
            y = values;
            z = values;
        }
        public Vec3(float _x, float _y, float _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }
    
        public static Vec3 operator*(Vec3 vec, float scalar)
        {
            return new Vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
        }

        public static Vec3 operator+(Vec3 vec, float scalar) 
        {
            return new Vec3(vec.x + scalar, vec.y + scalar, vec.z + scalar);
        }
        public static Vec3 operator+(Vec3 vec1, Vec3 vec2)
        {
            return new Vec3(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
        }
    }

    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetTranslation(ulong entityID, out Vec3 tranlation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetTranslation(ulong entityID, ref Vec3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IskeyDown(LgKey keycode);
    }

    public class Entity
    {
        protected Entity() { ID = 0; } 
        internal Entity(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;

        public Vec3 m_Translation
        {
            get
            {
                InternalCalls.Entity_GetTranslation(ID, out Vec3 translation);
                return translation;
            }
            set
            {
                InternalCalls.Entity_SetTranslation(ID, ref value);
            }
        }
    }
}
