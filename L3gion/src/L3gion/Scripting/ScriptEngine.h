#pragma once

#include "L3gion/Scene/Scene.h"
#include "L3gion/Scene/Entity.h"

#include <string>
#include <filesystem>
#include <map>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}


namespace L3gion
{
	enum class ScriptFieldType
	{
		None = 0,
		// Native types
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		
		// L3gion types
		Vec2, Vec3, Vec4,
		Entity
	};

	struct ScriptField
	{
		ScriptFieldType type;
		std::string name;

		MonoClassField* classField;
	};

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

		MonoObject* instantiate();
		MonoMethod* getMethod(const std::string& name, int parameterCount);
		MonoObject* invokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		const std::map<std::string, ScriptField>& getFields() const { return m_Fields; }

	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;
		MonoClass* m_MonoClass = nullptr;

		std::map<std::string, ScriptField> m_Fields;

		friend class ScriptEngine;
	};

//------------------ SCRIPT_INSTANCE ------------------

	class ScriptInstance
	{
	public:
		ScriptInstance(ref<ScriptClass> scriptClass);

		void invokeConstructor(Entity entity);
		void invokeOnCreate();
		void invokeOnUpdate(float ts);

		template<typename T>
		T getFieldValue(const std::string& name, bool retainValue)
		{
			bool success = getFieldValueInternal(name, retainValue);

			if (!success)
				return T();

			return retainValue ? *(T*)m_FieldValues.at(name) : *(T*)m_Buffer;
		}
		template<typename T>
		void setFieldValue(const std::string& name, const T& value, bool retainValue)
		{
			if (setFieldValueInternal(name, &value) && retainValue)
				memcpy(m_FieldValues[name], &value, sizeof(T));
		}

		MonoObject* getManagedObject() { return m_Instance; }
		ref<ScriptClass> getScriptClass() { return m_ScriptClass; }

	private:
		bool getFieldValueInternal(const std::string& name, bool retainValue);
		bool setFieldValueInternal(const std::string& name, const void* value);

	private:
		ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		std::map<std::string, char[8]> m_FieldValues;
		char m_Buffer[8];
	};

//------------------ SCRIPT_ENGINE ------------------

	class ScriptEngine
	{
	public:
		static void init();
		static void shutdown();

		static void loadAssembly(const std::filesystem::path filepath);
		static void loadAppAssembly(const std::filesystem::path filepath);
		
		static void onRuntimeStart(Scene* scene);
		static void onRuntimeStop();

		static bool entityClassExists(const std::string& fullName);
		static void onCreateEntity(Entity entity);
		static void onUpdateEntity(Entity entity, Timestep ts);

		static Scene* getSceneContext();
		static ref<ScriptInstance> getEntityScriptInstance(UUID entityID);

		static std::unordered_map<std::string, ref<ScriptClass>> getEntityClasses();
		static const std::vector<std::string>& getEntityClassesName();

		static MonoImage* getCoreAssemblyImage();

		static void loadEntity(Entity entity);

		static MonoObject* getManagedInstance(UUID entityUUID);
	private:
		static void initMono();
		static void shutdownMono();
		
		static MonoObject* instantiateClass(MonoClass* monoClass);
		static void loadAssemblyClasses();

		friend class ScriptClass;
		friend class ScriptGlue;
	};
}