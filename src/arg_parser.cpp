#include <iostream>
#include <ranges>
#include <NJS/ArgParser.hpp>

NJS::ArgParser::ArgParser(const std::vector<Arg>& args)
{
    for (const auto& [id, patterns, is_flag] : args)
        for (const auto& pat : patterns)
            m_Args[pat] = {id, is_flag};
}

void NJS::ArgParser::Parse(const int argc, const char** argv)
{
    m_Executable = argv[0];

    for (int i = 1; i < argc; ++i)
    {
        const std::string pat(argv[i]);

        if (!m_Args.contains(pat))
        {
            m_Values.push_back(pat);
            continue;
        }

        const auto& [id, is_flag] = m_Args[pat];

        if (is_flag)
        {
            m_Flags[id] = true;
            continue;
        }

        m_Options[id] = argv[++i];
    }
}

std::string NJS::ArgParser::Executable() const
{
    return m_Executable;
}

void NJS::ArgParser::Values(std::vector<std::string>& values) const
{
    values = m_Values;
}

bool NJS::ArgParser::Flag(const ID id) const
{
    if (!m_Flags.contains(id))
        return false;
    return m_Flags.at(id);
}

void NJS::ArgParser::Option(const ID id, std::string& option, const std::string& alt) const
{
    if (m_Options.contains(id))
    {
        option = m_Options.at(id);
        return;
    }
    option = alt;
}

void NJS::ArgParser::Print() const
{
    std::map<ID, std::vector<std::string>> options;
    std::map<ID, std::vector<std::string>> flags;

    for (const auto& [pat_, arg_] : m_Args)
    {
        if (arg_.second)
            flags[arg_.first].push_back(pat_);
        else options[arg_.first].push_back(pat_);
    }

    std::cerr << m_Executable << " [OPTION|FLAG|VALUE]..." << std::endl;
    std::cerr << "OPTION" << std::endl;
    for (const auto& pat_ : options | std::ranges::views::values)
    {
        for (unsigned i = 0; i < pat_.size(); ++i)
        {
            if (i > 0) std::cerr << ", ";
            std::cerr << pat_[i];
        }
        std::cerr << std::endl;
    }
    std::cerr << "FLAG" << std::endl;
    for (const auto& pat_ : flags | std::ranges::views::values)
    {
        for (unsigned i = 0; i < pat_.size(); ++i)
        {
            if (i > 0) std::cerr << ", ";
            std::cerr << pat_[i];
        }
        std::cerr << std::endl;
    }
}

bool NJS::ArgParser::IsEmpty() const
{
    return m_Options.empty()
        && m_Flags.empty()
        && m_Values.empty();
}
