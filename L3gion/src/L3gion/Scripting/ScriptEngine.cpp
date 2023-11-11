#include "lgpch.h"
#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

namespace L3gion
{
	struct ScriptEngineData
	{
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;
		MonoAssembly* coreAssembly = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;

	void ScriptEngine::init()
	{
		s_Data = new ScriptEngineData();
		initMono();
	}
	void ScriptEngine::shutdown()
	{
		shutdownMono();
		delete s_Data;
		s_Data = nullptr;
	}

	char* readBytes(const std::string& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			// Failed to open the file
			return nullptr;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = end - stream.tellg();

		if (size == 0)
		{
			// File is empty
			return nullptr;
		}

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}

	MonoAssembly* loadCSharpAssembly(const std::string& assemblyPath)
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

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
		mono_image_close(image);

		// Don't forget to free the file data
		delete[] fileData;

		return assembly;
	}

	void printAssemblyTypes(MonoAssembly* assembly)
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

	void ScriptEngine::initMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("L3gionJITRuntime");
		LG_CORE_ASSERT(rootDomain, "In initMono(): No MonoDomain!");

		// Store the root domain pointer
		s_Data->rootDomain = rootDomain;

		// Create an App Domain
		char appName[] = "L3gionScriptRuntime";
		s_Data->appDomain = mono_domain_create_appdomain(appName, nullptr);
		mono_domain_set(s_Data->appDomain, true);

		// Maybe not here!
		s_Data->coreAssembly = loadCSharpAssembly("resources/scripts/L3gion_ScriptCore.dll");
		printAssemblyTypes(s_Data->coreAssembly);

		MonoImage* assemblyImage = mono_assembly_get_image(s_Data->coreAssembly);

		// Creating an object and call its default constructor
		MonoClass* monoClass = mono_class_from_name(assemblyImage, "L3gion", "Main");
		MonoObject* instance = mono_object_new(s_Data->appDomain, monoClass);
		mono_runtime_object_init(instance);

		// Calling a function
		MonoMethod* printFunc = mono_class_get_method_from_name(monoClass, "printMessage", 0);
		mono_runtime_invoke(printFunc, instance, nullptr, nullptr);


		// With parameters
		MonoMethod* printIntFunc = mono_class_get_method_from_name(monoClass, "printInt", 1);
		int value = 5;
		void* param = &value;
		mono_runtime_invoke(printIntFunc, instance, &param, nullptr);

		MonoString* monoString = mono_string_new(s_Data->appDomain, "Hello world from C++");
		MonoMethod* printCustomFunc = mono_class_get_method_from_name(monoClass, "printCustomMsg", 1);

		void* strParam = monoString;

		mono_runtime_invoke(printCustomFunc, instance, &strParam, nullptr);
	}

	void ScriptEngine::shutdownMono()
	{
		//mono_domain_unload(s_Data->appDomain);
		s_Data->appDomain = nullptr;

		//mono_jit_cleanup(s_Data->rootDomain);
		s_Data->rootDomain = nullptr;
	}
}