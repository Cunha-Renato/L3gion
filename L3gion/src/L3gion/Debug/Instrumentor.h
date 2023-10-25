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
            : m_CurrentSession(nullptr), m_ProfileCount(0)
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
}

#define LG_PROFILING 1
#if LG_PROFILING == 1
    #define CONCAT(x, y) x ## y    
    #define LG_PROFILE_BEGIN_SESSION(name, filepath) ::L3gion::Instrumentor::get().beginSession(name, filepath)
    #define LG_PROFILE_END_SESSION() ::L3gion::Instrumentor::get().endSession()
    #define LG_PROFILE_SCOPE(name) ::L3gion::InstrumentationTimer CONCAT(timer, __LINE__)(name)
    #define LG_PROFILE_FUNCTION() LG_PROFILE_SCOPE(__FUNCSIG__)
    #define LG_PROFILE_ACTIVATE() ::L3gion::Instrumentor::get().setActive(true)
    #define LG_PROFILE_DEACTIVATE() ::L3gion::Instrumentor::get().setActive(false)
    #define LG_PROFILE_IS_ACTIVE() ::L3gion::Instrumentor::get().getActive()
#else
    #define LG_PROFILE_BEGIN_SESSION(name, filepath) 
    #define LG_PROFILE_END_SESSION()
    #define LG_PROFILE_SCOPE(name)
    #define LG_PROFILE_FUNCTION()
#endif