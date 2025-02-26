#pragma once

#include <map>
#include <string>
#include <vector>

namespace NJS
{
    class ArgParser
    {
        using ID = unsigned;

        struct Arg
        {
            ID Id;
            std::string Description;
            std::vector<std::string> Patterns;
            bool IsFlag;
        };

        struct ArgPattern
        {
            ID Id;
            std::string Description;
            bool IsFlag;
        };

    public:
        explicit ArgParser(const std::vector<Arg> &patterns);

        void Parse(int argc, const char **argv);

        [[nodiscard]] std::string Executable() const;
        void Values(std::vector<std::string> &values) const;
        [[nodiscard]] bool Flag(ID id) const;
        void Option(ID id, std::string &option, const std::string &default_value = {}) const;

        void Print() const;
        [[nodiscard]] bool IsEmpty() const;

    private:
        std::map<std::string, ArgPattern> m_Patterns;

        std::string m_Executable;
        std::vector<std::string> m_Values;
        std::map<ID, bool> m_Flags;
        std::map<ID, std::string> m_Options;
    };
}
