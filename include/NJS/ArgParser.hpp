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
            std::string_view Description;
            std::vector<std::string_view> Patterns;
            bool IsFlag;
        };

        struct ArgPattern
        {
            ID Id;
            std::string_view Description;
            bool IsFlag;
        };

    public:
        explicit ArgParser(const std::vector<Arg> &args);

        void Parse(int argc, const char **argv);

        [[nodiscard]] std::string Executable() const;
        void Values(std::vector<std::string> &values) const;
        [[nodiscard]] bool Flag(ID id) const;
        void Option(ID id, std::string &option, const std::string_view &default_value = {}) const;

        void Print() const;
        [[nodiscard]] bool IsEmpty() const;

    private:
        std::map<std::string_view, ArgPattern> m_Args;

        std::string m_Executable;
        std::vector<std::string> m_Values;
        std::map<ID, bool> m_Flags;
        std::map<ID, std::string> m_Options;
    };
}
