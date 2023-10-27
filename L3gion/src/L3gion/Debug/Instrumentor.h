#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <thread>

namespace L3gion
{
    using floatingPointMicroseconds = std::chrono::duration<double, std::micro>;
    
    struct ProfileResult
    {
        std::string name;
        floatingPointMicroseconds start;
        std::chrono::microseconds elapsedTime;
        uint32_t threadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class Instrumentor
    {
    public:
        Instrumentor()
            : m_CurrentSession(nullptr), m_ProfileCount(0), m_Active(true)
        {
        }
        
        void beginSession(const std::string& name, const std::string& filepath = "results.json")
        {
            setActive(true);
            m_OutputStream.open(filepath);
            writeHeader();
            m_CurrentSession = new InstrumentationSession{ name };
        }

        void endSession()
        {
            writeFooter();
            m_OutputStream.close();
            delete m_CurrentSession;
            m_CurrentSession = nullptr;
            m_ProfileCount = 0;
            setActive(false);
        }

        void writeProfile(const ProfileResult& result)
        {
            if (!m_Active)
                return;
            
            std::string name = result.name;
            std::replace(name.begin(), name.end(), '"', '\'');

            if (m_ProfileCount++ > 0)
                m_OutputStream << ",";

            m_OutputStream << std::setprecision(3) << std::fixed;
            m_OutputStream << "{";
            m_OutputStream << "\"cat\":\"function\",";
            m_OutputStream << "\"dur\":" << (result.elapsedTime.count()) << ',';
            m_OutputStream << "\"name\":\"" << name << "\",";
            m_OutputStream << "\"ph\":\"X\",";
            m_OutputStream << "\"pid\":0,";
            m_OutputStream << "\"tid\":" << result.threadID << ",";
            m_OutputStream << "\"ts\":" << result.start.count();
            m_OutputStream << "}";

            m_OutputStream.flush();
            
        }

        void writeHeader()
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
            m_OutputStream.flush();
        }

        void writeFooter()
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        static Instrumentor& get()
        {
            static Instrumentor instance;
            return instance;
        }

        void setActive(bool option = true) { m_Active = option; }
        const bool getActive() const { return m_Active; }

    private:
        InstrumentationSession* m_CurrentSession;
        std::ofstream m_OutputStream;
        int m_ProfileCount;
    
        bool m_Active;
    };

    class InstrumentationTimer
    {
    public:
        InstrumentationTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::steady_clock::now();
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                stop();
        }

        void stop()
        {
            auto endTimepoint = std::chrono::steady_clock::now();
            auto highResStart = floatingPointMicroseconds
            { 
                m_StartTimepoint.time_since_epoch() 
            };

            auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

            uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
            Instrumentor::get().writeProfile({ m_Name, highResStart, elapsedTime, threadID });

            m_Stopped = true;
        }

    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
        bool m_Stopped;
    };

    namespace InstrumentorUtils 
    {

        template <size_t N>
        struct ChangeResult
        {
            char data[N];
        };

        template <size_t N, size_t K>
        constexpr auto cleanupOutputString(const char(&expr)[N], const char(&remove)[K])
        {
            ChangeResult<N> result = {};

            size_t srcIndex = 0;
            size_t dstIndex = 0;
            while (srcIndex < N)
            {
                size_t matchIndex = 0;
                while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
                    matchIndex++;
                if (matchIndex == K - 1)
                    srcIndex += matchIndex;
                result.data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
                srcIndex++;
            }
            return result;
        }
    }
}

#define LG_PROFILE 1
#if LG_PROFILE
// Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts, so the syntax highlighting
// could mark the wrong one in your editor!
    #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
        #define LG_FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(__DMC__) && (__DMC__ >= 0x810)
        #define LG_FUNC_SIG __PRETTY_FUNCTION__
    #elif (defined(__FUNCSIG__) || (_MSC_VER))
        #define LG_FUNC_SIG __FUNCSIG__
    #elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
        #define LG_FUNC_SIG __FUNCTION__
    #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
        #define LG_FUNC_SIG __FUNC__
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
        #define LG_FUNC_SIG __func__
    #elif defined(__cplusplus) && (__cplusplus >= 201103)
        #define LG_FUNC_SIG __func__
    #else
        #define LG_FUNC_SIG "LG_FUNC_SIG unknown!"
    #endif

    #define LG_PROFILE_BEGIN_SESSION(name, filepath) ::L3gion::Instrumentor::get().beginSession(name, filepath)
    #define LG_PROFILE_END_SESSION() ::L3gion::Instrumentor::get().endSession()

    #define LG_PROFILE_SCOPE(name) constexpr auto fixedName = ::L3gion::InstrumentorUtils::cleanupOutputString(name, "__cdecl ");\
									::L3gion::InstrumentationTimer timer##__LINE__(fixedName.data)
    #define LG_PROFILE_FUNCTION() LG_PROFILE_SCOPE(LG_FUNC_SIG)
    #define LG_PROFILE_IS_ACTIVE() ::L3gion::Instrumentor::get().getActive()
#else
    #define LG_PROFILE_BEGIN_SESSION(name, filepath)
    #define LG_PROFILE_END_SESSION()
    #define LG_PROFILE_SCOPE(name)
    #define LG_PROFILE_FUNCTION()
#endif