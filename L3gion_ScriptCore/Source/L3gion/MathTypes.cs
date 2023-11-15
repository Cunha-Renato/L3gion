namespace L3gion
{
    public struct Vec2
    {
        public float x, y;

        public static Vec2 Zero => new Vec2(0.0f);

        public Vec2(float values)
        {
            x = values;
            y = values;
        }
        public Vec2(float _x, float _y)
        {
            x = _x;
            y = _y;
        }
        public Vec2(Vec2 vec)
        {
            x = vec.x;
            y = vec.y;
        }

        public static Vec2 operator *(Vec2 vec, float scalar)
        {
            return new Vec2(vec.x * scalar, vec.y * scalar);
        }

        public static Vec2 operator +(Vec2 vec, float scalar)
        {
            return new Vec2(vec.x + scalar, vec.y + scalar);
        }
        public static Vec2 operator +(Vec2 vec1, Vec2 vec2)
        {
            return new Vec2(vec1.x + vec2.x, vec1.y + vec2.y);
        }
    }

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
        public Vec3(Vec3 vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
        }
        public Vec3(Vec2 vec, float _z)
        {
            x = vec.x;
            y = vec.y;
            z = _z;
        }

        public Vec2 xy
        {
            get => new Vec2(x, y);
            set
            {
                x = value.x;
                y = value.y;
            }
        }
        public Vec2 xz
        {
            get => new Vec2(x, z);
            set
            {
                x = value.x;
                z = value.y;
            }
        }
        public Vec2 yz
        {
            get => new Vec2(y, z);
            set
            {
                y = value.x;
                z = value.y;
            }
        }

        public static Vec3 operator *(Vec3 vec, float scalar)
        {
            return new Vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
        }

        public static Vec3 operator +(Vec3 vec, float scalar)
        {
            return new Vec3(vec.x + scalar, vec.y + scalar, vec.z + scalar);
        }
        public static Vec3 operator +(Vec3 vec1, Vec3 vec2)
        {
            return new Vec3(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
        }
    }

    public struct Vec4
    {
        public float x, y, z, w;

        public static Vec4 Zero => new Vec4(0.0f);

        public Vec4(float values)
        {
            x = values;
            y = values;
            z = values;
            w = values;
        }
        public Vec4(float _x, float _y, float _z, float _w)
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
        }
        public Vec4(Vec4 vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            w = vec.w;
        }
        public Vec4(Vec3 vec, float _w)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            w = _w;
        }
        public Vec4(Vec2 vec, float _z, float _w)
        {
            x = vec.x;
            y = vec.y;
            z = _z;
            w = _w;
        }
        public Vec4(Vec2 vec1, Vec2 vec2)
        {
            x = vec1.x;
            y = vec1.y;
            z = vec2.x;
            w = vec2.y;
        }

        public Vec3 xyz
        {
            get => new Vec3(x, y, z);
            set
            {
                x = value.x;
                y = value.y;
                z = value.z;
            }
        }
        public Vec3 xyw
        {
            get => new Vec3(x, y, w);
            set
            {
                x = value.x;
                y = value.y;
                w = value.z;
            }
        }
        public Vec3 xzw
        {
            get => new Vec3(x, z, w);
            set
            {
                x = value.x;
                z = value.y;
                w = value.z;
            }
        }
        public Vec3 yzw
        {
            get => new Vec3(y, z, w);
            set
            {
                y = value.x;
                z = value.y;
                w = value.z;
            }
        }

        public Vec2 xy
        {
            get => new Vec2(x, y);
            set
            {
                x = value.x;
                y = value.y;
            }
        }
        public Vec2 xz
        {
            get => new Vec2(x, z);
            set
            {
                x = value.x;
                z = value.y;
            }
        }
        public Vec2 xw
        {
            get => new Vec2(x, w);
            set
            {
                x = value.x;
                w = value.y;
            }
        }
        public Vec2 yz
        {
            get => new Vec2(y, z);
            set
            {
                y = value.x;
                z = value.y;
            }
        }
        public Vec2 yw
        {
            get => new Vec2(y, w);
            set
            {
                y = value.x;
                w = value.y;
            }
        }
        public Vec2 zw
        {
            get => new Vec2(z, w);
            set
            {
                z = value.x;
                w = value.y;
            }
        }

        public static Vec4 operator *(Vec4 vec, float scalar)
        {
            return new Vec4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar);
        }

        public static Vec4 operator +(Vec4 vec, float scalar)
        {
            return new Vec4(vec.x + scalar, vec.y + scalar, vec.z + scalar, vec.w + scalar);
        }
        public static Vec4 operator +(Vec4 vec1, Vec4 vec2)
        {
            return new Vec4(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z, vec1.w + vec2.w);
        }
    }
}
