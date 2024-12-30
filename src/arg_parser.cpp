#include <iostream>
#include <ranges>
#include <NJS/ArgParser.hpp>

NJS::ArgParser::ArgParser(const std::vector<Arg>& args)
{
    for (auto& [id_, description_, patterns_, is_flag_] : args)
        for (const auto& pattern : patterns_)
            m_Args[pattern] = {id_, description_, is_flag_};
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

        auto& [id_, description_, is_flag_] = m_Args[pat];

        if (is_flag_)
        {
            m_Flags[id_] = true;
            continue;
        }

        m_Options[id_] = argv[++i];
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
    constexpr auto print_description = [](const std::string& description)
    {
        unsigned i = 0;
        for (const auto c : description)
        {
            if (i % 100 == 0)
                std::cerr << std::endl << "    ";
            std::cerr << description[i++];
        }
        std::cerr << std::endl;
    };

    std::map<ID, std::pair<std::vector<std::string>, std::string>> options;
    std::map<ID, std::pair<std::vector<std::string>, std::string>> flags;

    for (auto& [pat_, arg_] : m_Args)
    {
        if (arg_.IsFlag)
        {
            flags[arg_.Id].first.push_back(pat_);
            flags[arg_.Id].second = arg_.Description;
        }
        else
        {
            options[arg_.Id].first.push_back(pat_);
            options[arg_.Id].second = arg_.Description;
        }
    }

    std::cerr << m_Executable << " [OPTION <VALUE> | FLAG | FILENAME]..." << std::endl;
    std::cerr << "OPTION" << std::endl;
    for (const auto& [pat_, description_] : options | std::ranges::views::values)
    {
        for (unsigned i = 0; i < pat_.size(); ++i)
        {
            if (i > 0) std::cerr << ", ";
            std::cerr << pat_[i];
        }
        print_description(description_);
        std::cerr << std::endl;
    }
    std::cerr << "FLAG" << std::endl;
    for (const auto& [pat_, description_] : flags | std::ranges::views::values)
    {
        for (unsigned i = 0; i < pat_.size(); ++i)
        {
            if (i > 0) std::cerr << ", ";
            std::cerr << pat_[i];
        }
        print_description(description_);
        std::cerr << std::endl;
    }
}

bool NJS::ArgParser::IsEmpty() const
{
    return m_Options.empty()
        && m_Flags.empty()
        && m_Values.empty();
}
