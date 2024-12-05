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

bool NJS::ArgParser::HasOption(const ID id) const
{
    return m_Options.contains(id);
}

std::string NJS::ArgParser::Option(const ID id) const
{
    if (!m_Options.contains(id))
        return {};
    return m_Options.at(id);
}
