#include "lgpch.h"
#include "ScriptEngine.h"

#include "ScriptGlue.h"
#include "L3gion/Core/UUID.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

#include <glm/glm.hpp>

namespace L3gion
{
	namespace Utils
	{
		static void printAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				LG_CORE_INFO("{0}.{1}\n", nameSpace, name);
			}
		}
		
		static char* readBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint64_t size = end - stream.tellg();

			if (size == 0)
			{
				// File is empty
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = (uint32_t)size;
			return buffer;
		}
		
		static MonoAssembly* loadMonoAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = readBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			std::string pathStr = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathStr.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}
	}

	struct ScriptEngineData
	{
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;

		MonoAssembly* coreAssembly = nullptr;
		MonoImage* coreAssemblyImage = nullptr;

		ScriptClass entityClass;

		std::unordered_map<std::string, ref<ScriptClass>> entityClasses;
		std::unordered_map<UUID, ref<ScriptInstance>> entityInstances;

		// Runtime
		Scene* sceneContext = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;

	void ScriptEngine::init()
	{
		s_Data = new ScriptEngineData();
		initMono();
		loadAssembly("resources/scripts/L3gion_ScriptCore.dll");
		loadAssemblyClasses(s_Data->coreAssembly);

		ScriptGlue::registerFunctions();

		s_Data->entityClass = ScriptClass("L3gion", "Entity");
	}
	void ScriptEngine::shutdown()
	{
		shutdownMono();
		delete s_Data;
		s_Data = nullptr;
	}

	void ScriptEngine::initMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("L3gionJITRuntime");
		LG_CORE_ASSERT(rootDomain, "In initMono(): No MonoDomain!");

		// Store the root domain pointer
		s_Data->rootDomain = rootDomain;
	}

	void ScriptEngine::shutdownMono()
	{
		//mono_domain_unload(s_Data->appDomain);
		s_Data->appDomain = nullptr;

		//mono_jit_cleanup(s_Data->rootDomain);
		s_Data->rootDomain = nullptr;
	}

	void ScriptEngine::loadAssembly(const std::filesystem::path filepath)
	{
		// Create an App Domain
		char appName[] = "L3gionScriptRuntime";
		s_Data->appDomain = mono_domain_create_appdomain(appName, nullptr);
		mono_domain_set(s_Data->appDomain, true);

		// Maybe not here!
		s_Data->coreAssembly = Utils::loadMonoAssembly(filepath);
		s_Data->coreAssemblyImage = mono_assembly_get_image(s_Data->coreAssembly);
		//printAssemblyTypes(s_Data->coreAssembly);
	}

	void ScriptEngine::onRuntimeStart(Scene* scene)
	{
		s_Data->sceneContext = scene;
	}
	void ScriptEngine::onRuntimeStop()
	{
		s_Data->sceneContext = nullptr;

		s_Data->entityInstances.clear();
	}

	void ScriptEngine::onCreateEntity(Entity entity)
	{
		const auto& sc = entity.getComponent<ScriptComponent>();
		if (entityClassExists(sc.name))
		{
			ref<ScriptInstance> instance = createRef<ScriptInstance>(s_Data->entityClasses[sc.name], entity);
			s_Data->entityInstances[entity.getUUID()] = instance;
			
			instance->invokeOnCreate();
		}
	}
	void ScriptEngine::onUpdateEntity(Entity entity, Timestep ts)
	{
		UUID entityUUID = entity.getUUID();

		LG_CORE_ASSERT(s_Data->entityInstances.find(entityUUID) != s_Data->entityInstances.end(), "Instance doesn't exists!");

		auto& instance = s_Data->entityInstances[entityUUID];
		instance->invokeOnUpdate((float)ts);
	}

	Scene* ScriptEngine::getSceneContext()
	{
		return s_Data->sceneContext;
	}

	bool ScriptEngine::entityClassExists(const std::string& fullName)
	{
		return s_Data->entityClasses.find(fullName) != s_Data->entityClasses.end();
	}

	std::unordered_map<std::string, ref<ScriptClass>> ScriptEngine::getEntityClasses()
	{
		return s_Data->entityClasses;
	}

	void ScriptEngine::loadAssemblyClasses(MonoAssembly* assembly)
	{
		s_Data->entityClasses.clear();

		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(s_Data->coreAssemblyImage, "L3gion", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			
			std::string fullName;
			if (strlen(nameSpace) != 0)	
				fullName = fmt::format("{}.{}", nameSpace, name);
			else
				fullName = name;

			MonoClass* monoClass = mono_class_from_name(s_Data->coreAssemblyImage, nameSpace, name);

			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);

			if (isEntity)
				s_Data->entityClasses[fullName] = createRef<ScriptClass>(nameSpace, name);
		}
	}

	MonoObject* ScriptEngine::instantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->appDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

// ---------------- ScriptClass ----------------

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(s_Data->coreAssemblyImage, m_ClassNamespace.c_str(), m_ClassName.c_str());
	}

	MonoObject* ScriptClass::instantiate()
	{
		return ScriptEngine::instantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::getMethod(const std::string& name, int parameterCount)
	{
		auto str = name;
		return mono_class_get_method_from_name(m_MonoClass, str.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::invokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
	}

// ---------------- ScriptInstance ----------------

	ScriptInstance::ScriptInstance(ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = m_ScriptClass->instantiate();
		
		m_Constructor = s_Data->entityClass.getMethod(".ctor", 1);
		m_OnCreateMethod = m_ScriptClass->getMethod("OnCreate", 0);
		m_OnUpdateMethod = m_ScriptClass->getMethod("OnUpdate", 1);

		// Calling the Entity constructor with UUID
		{
			uint64_t entityUUID = entity.getUUID();
			void* param = &entityUUID;
			m_ScriptClass->invokeMethod(m_Instance, m_Constructor, &param);
		}
	}

	void ScriptInstance::invokeOnCreate()
	{
		m_ScriptClass->invokeMethod(m_Instance, m_OnCreateMethod);
	}
	void ScriptInstance::invokeOnUpdate(float ts)
	{
		void* param = &ts;
		m_ScriptClass->invokeMethod(m_Instance, m_OnUpdateMethod, &param);
	}
}