using System;

namespace L3gion
{
    public class Entity
    {
        protected Entity() { ID = 0; }
        public Entity(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;
        public Vec3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(ID, out Vec3 result);
                return result;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(ID, ref value);
            }
        }
        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }
        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            // TODO: Add caching for this
            T component = new T() { Entity = this };

            return component;
        }


        public ulong CreateEntity()
        {
            return InternalCalls.Entity_CreateEntity();
        }
        public void DuplicateEntity(ulong id)
        {
            InternalCalls.Entity_DuplicateEntity(id);
        }
        public Entity FindEntityByName(string name)
        {
            ulong entityID = InternalCalls.Entity_FindEntityByName(name);

            if (entityID == 0)
                return null;

            return new Entity(entityID);
        }
        public T As<T>() where T : Entity, new()
        {
            object instance = InternalCalls.GetScriptInstance(ID);
            
            return instance as T;
        }
    }
}
