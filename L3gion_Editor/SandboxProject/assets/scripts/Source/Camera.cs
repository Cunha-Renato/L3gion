using L3gion;
using System;

namespace Sandbox
{
    public class Camera : Entity
    {
        public Entity OtherEntity;

        public float DistanceFromPlayer = 20.0f;

        private Entity m_Player;

        void OnCreate()
        {
            m_Player = FindEntityByName("Ball");
        }

        void OnUpdate(float ts)
        {
            if (m_Player != null)
                Translation = new Vec3(m_Player.Translation.xy, DistanceFromPlayer);
            else
            {
                Console.WriteLine("Player is null!");
            }

            float speed = 1.0f;
            Vec3 velocity = Vec3.Zero;

            velocity *= speed;
            Vec3 translation = Translation;
            translation += velocity * ts;
            Translation = translation;
        }
    }
}
