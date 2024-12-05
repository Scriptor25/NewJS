#pragma once

#include <map>
#include <string>
#include <vector>

namespace NJS
{
    class ArgParser
    {
        typedef unsigned ID;

        struct Arg
        {
            ID Id;
            std::vector<std::string> Patterns;
            bool IsFlag;
        };

    public:
        explicit ArgParser(const std::vector<Arg>& args);

        void Parse(int argc, const char** argv);

        [[nodiscard]] std::string Executable() const;
        void Values(std::vector<std::string>&) const;
        [[nodiscard]] bool Flag(ID) const;
        void Option(ID, std::string&, const std::string& = {}) const;

    private:
        std::map<std::string, std::pair<ID, bool>> m_Args;

        std::string m_Executable;
        std::vector<std::string> m_Values;
        std::map<ID, bool> m_Flags;
        std::map<ID, std::string> m_Options;
    };
}
