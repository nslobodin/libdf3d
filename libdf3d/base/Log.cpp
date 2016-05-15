#include "Log.h"

#ifdef DF3D_ANDROID
#include <android/log.h>
#endif

#if defined(DF3D_WINDOWS_PHONE)
#include <wrl.h>
#include <wrl/client.h>
#endif

#if defined(DF3D_WINDOWS)
#include <Windows.h>
#endif

namespace df3d {

#ifndef DF3D_DISABLE_LOGGING

class LoggerImpl
{
public:
    virtual ~LoggerImpl() = default;

    virtual void writeBuffer(const char *buffer, Log::LogChannel channel) = 0;
};

class StdoutLogger : public LoggerImpl
{
public:
    StdoutLogger() = default;

    void writeBuffer(const char *buffer, Log::LogChannel channel) override
    {
        switch (channel)
        {
        case Log::LogChannel::CHANNEL_DEBUG:
            std::cout << "[debug]: ";
            break;
        case Log::LogChannel::CHANNEL_MESS:
            std::cout << "[message]: ";
            break;
        case Log::LogChannel::CHANNEL_WARN:
            std::cout << "[WARNING]: ";
            break;
        case Log::LogChannel::CHANNEL_CRITICAL:
            std::cout << "[!!!CRITICAL!!!]: ";
            break;
        case Log::LogChannel::CHANNEL_GAME:
            std::cout << "[game]: ";
            break;
        default:
            break;
        }

        std::cout << buffer << std::endl;
    }
};

#ifdef DF3D_WINDOWS

class WindowsLoggerImpl : public StdoutLogger
{
    HANDLE m_consoleHandle = nullptr;
    std::unordered_map<Log::LogChannel, WORD> m_consoleColors;

public:
    WindowsLoggerImpl()
    {
        m_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        m_consoleColors =
        {
            { Log::LogChannel::CHANNEL_DEBUG, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED },
            { Log::LogChannel::CHANNEL_MESS, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY },
            { Log::LogChannel::CHANNEL_WARN, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY },
            { Log::LogChannel::CHANNEL_CRITICAL, FOREGROUND_RED | FOREGROUND_INTENSITY },
            { Log::LogChannel::CHANNEL_GAME, FOREGROUND_GREEN | FOREGROUND_INTENSITY }
        };

        assert(m_consoleColors.size() == (size_t)Log::LogChannel::CHANNEL_COUNT);
    }

    void writeBuffer(const char *buffer, Log::LogChannel channel) override
    {
        if (m_consoleHandle)
            SetConsoleTextAttribute(m_consoleHandle, m_consoleColors[channel]);

        StdoutLogger::writeBuffer(buffer, channel);

        if (m_consoleHandle)
            SetConsoleTextAttribute(m_consoleHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    }
};

#endif

#ifdef DF3D_WINDOWS_PHONE

class WindowsRTLoggerImpl : public LoggerImpl
{
public:
    WindowsRTLoggerImpl()
    {
    }

    virtual void writeBuffer(const std::string &buffer, MessageType type) override
    {
        OutputDebugStringA(buffer.c_str());
        OutputDebugStringA("\n");
    }
};

#endif

#ifdef DF3D_ANDROID

class AndroidLoggerImpl : public LoggerImpl
{
    std::unordered_map<int, android_LogPriority> m_priorities;

public:
    AndroidLoggerImpl()
    {
        m_priorities =
        {
            { (int)MessageType::MT_DEBUG, ANDROID_LOG_DEBUG },
            { (int)MessageType::MT_MESSAGE, ANDROID_LOG_INFO },
            { (int)MessageType::MT_WARNING, ANDROID_LOG_WARN },
            { (int)MessageType::MT_CRITICAL, ANDROID_LOG_FATAL },
            { (int)MessageType::MT_GAME, ANDROID_LOG_INFO },
            { (int)MessageType::MT_SCRIPT, ANDROID_LOG_INFO },
            { (int)MessageType::MT_NONE, ANDROID_LOG_UNKNOWN }
        };

        DF3D_ASSERT(m_priorities.size() == (int)MessageType::COUNT, "sanity check");
    }

    virtual void writeBuffer(const std::string &buffer, MessageType type) override
    {
        __android_log_print(m_priorities[(int)type], "df3d_android", "%s", buffer.c_str());
    }
};

#endif

Log::Log()
{
#if defined(DF3D_WINDOWS)
    m_impl.reset(new WindowsLoggerImpl());
#elif defined(DF3D_WINDOWS_PHONE)
    m_impl.reset(new WindowsRTLoggerImpl());
#elif defined(DF3D_ANDROID)
    m_impl.reset(new AndroidLoggerImpl());
#else
    m_impl.reset(new StdoutLogger());
#endif
}

Log::~Log()
{

}

Log& Log::instance()
{
    static Log m_instance;

    return m_instance;
}

void Log::print(LogChannel channel, const char *fmt, ...)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    va_list argptr;
    va_start(argptr, fmt);

    vprint(channel, fmt, argptr);

    va_end(argptr);
}

void Log::vprint(LogChannel channel, const char *fmt, va_list argList)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    char buf[4096];

    vsnprintf(buf, sizeof(buf), fmt, argList);  // TODO: check count and warn if overflow occurs.

    m_impl->writeBuffer(buf, channel);
}

#endif

Log& glog = Log::instance();

}
