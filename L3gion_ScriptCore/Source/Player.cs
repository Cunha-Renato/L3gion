using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

using L3gion;
using static System.Runtime.CompilerServices.RuntimeHelpers;

namespace Sandbox
{
    public class Player : Entity
    {
        void OnCreate()
        {
            Console.WriteLine($"Player created! - {ID}");
        }

        void OnUpdate(float ts)
        {
            Console.WriteLine($"Player update: {ts}");

            float speed = 1.0f;
            Vec3 velocity = Vec3.Zero;

            if (Input.IsKeyDown(LgKey.W))
                velocity.y = 1.0f;
            else if (Input.IsKeyDown(LgKey.S))
                velocity.y = -1.0f;
            if (Input.IsKeyDown(LgKey.A))
                velocity.x = -1.0f;
            else if (Input.IsKeyDown(LgKey.D))
                velocity.x = 1.0f;

            velocity *= speed;   

            Vec3 translation = m_Translation;
            translation += velocity * ts;
            m_Translation = translation;
        }
    }
}
