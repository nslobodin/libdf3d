#pragma once

#include <df3d/lib/Utils.h>

namespace tb { class TBWidget; }

namespace df3d {

extern const DF3D_DLL std::string CVAR_DEBUG_DRAW;
// TODO:
// More cvars.

struct DF3D_DLL ConsoleCommand
{
    std::string name;
    std::string help;
    std::function<std::string(const std::vector<std::string> &params)> handler;
};

class CVarsContainer : NonCopyable
{
    std::unordered_map<std::string, std::string> m_cvars;

public:
    CVarsContainer() = default;
    ~CVarsContainer() = default;

    template<typename T>
    void set(const std::string &key, const T &val)
    {
        m_cvars[key] = utils::to_string(val);
    }

    template<typename T>
    T get(const std::string &key) const
    {
        auto found = m_cvars.find(key);
        if (found == m_cvars.end())
            return T();

        return utils::from_string<T>(found->second);
    }
};

class DF3D_DLL DebugConsole : NonCopyable
{
    friend class ConsoleWindow;

    CVarsContainer m_cvars;
    std::unordered_map<std::string, ConsoleCommand> m_consoleCommands;

    tb::TBWidget *m_root;

    std::string m_history;

    void registerDefaultCommands();
    void onConsoleInput(const std::string &str);
    void updateHistory(const std::string &commandResult);

public:
    DebugConsole();
    ~DebugConsole();

    bool isVisible() const;
    void show();
    void hide();
    void toggle() { isVisible() ? hide() : show(); }

    void registerCommand(const ConsoleCommand &command);
    void unregisterCommand(const ConsoleCommand &command);

    CVarsContainer& getCVars() { return m_cvars; }
    const CVarsContainer& getCVars() const { return m_cvars; }
};

}