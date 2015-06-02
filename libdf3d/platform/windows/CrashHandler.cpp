#include "df3d_pch.h"
#include "CrashHandler.h"

#include <Windows.h>
#include <DbgHelp.h>

namespace df3d { namespace platform {

LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* excInfo)
{
    std::cerr << "Unhandled exception occurred\n";

    auto dbgHelp = LoadLibrary("dbghelp.dll");
    if (dbgHelp)
    {
        typedef BOOL(WINAPI * MiniDumpWriteDumpProc)(
            HANDLE hProcess,
            DWORD ProcessId,
            HANDLE hFile,
            MINIDUMP_TYPE DumpType,
            PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
            PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
            PMINIDUMP_CALLBACK_INFORMATION CallbackParam
            );

        auto miniDumpWriteDump = (MiniDumpWriteDumpProc)GetProcAddress(dbgHelp, "MiniDumpWriteDump");
        if (miniDumpWriteDump)
        {
            auto dumpFile = CreateFile("crashdump.dmp", GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (dumpFile != INVALID_HANDLE_VALUE)
            {
                MINIDUMP_EXCEPTION_INFORMATION miniDumpInfo;
                miniDumpInfo.ThreadId = GetCurrentThreadId();
                miniDumpInfo.ExceptionPointers = excInfo;
                miniDumpInfo.ClientPointers = TRUE;

                auto miniDumpType = _MINIDUMP_TYPE(MiniDumpWithDataSegs | MiniDumpWithHandleData);

                if (miniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), dumpFile, miniDumpType, &miniDumpInfo, nullptr, nullptr))
                {
                    std::cerr << "Written minidump\n";
                    CloseHandle(dumpFile);

                    return EXCEPTION_EXECUTE_HANDLER;
                }
                else
                {
                    CloseHandle(dumpFile);
                    DeleteFile("crashdump.dmp");
                    std::cerr << "Failed to write minidump\n";
                }
            }
        }
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

void CrashHandler::setup(const char *dumpFileName)
{
    base::glog << "Setting up exception handler" << base::logmess;
    SetUnhandledExceptionFilter(UnhandledExceptionFilter);
}

} }