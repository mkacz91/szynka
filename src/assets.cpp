#include "assets.h"

// For each registered hint, provides a list of asset search directories.
std::map<string, std::vector<string>> Assets::m_directories;

// Adds a search directory under the specified hints. This directory is searched when one of the
// hints is given upon asset request.
void Assets::add_directory(string const& directory, string const& hints)
{
    int hint_start = 0;
    while (hint_start < hints.length())
    {
        // Fetch next hint
        int hint_end = hint_start + 1;
        while (hint_end < hints.length() && hints[hint_end] != ' ')
            ++hint_end;
        auto hint = hints.substr(hint_start, hint_end - hint_start);
        hint_start = hint_end + 1;

        // Append direcotry to the hint's directory list
        auto directories = m_directories.find(hint);
        if (directories == m_directories.end())
            directories = m_directories.emplace(hint, std::vector<string>()).first;
        directories->second.push_back(directory);
    }
}

// Adds search directories specified in a config file.
void Assets::add_config(std::istream& config)
{
    while (!config.eof())
    {
        string line;
        std::getline(config, line);
        auto separator_pos = line.find(" : ");
        if (separator_pos == 0 || separator_pos == string::npos)
            continue;
        string directory = line.substr(0, separator_pos);
        string hints = line.substr(separator_pos + 3);
        add_directory(directory, hints);
    }
}

// Adds search directories specified in a config file.
void Assets::add_config(string const& filename)
{
    std::ifstream config(filename);
    if (!config)
        throw std::ios_base::failure("Unable to open file '" + filename + "'.");
    return add_config(config);
}

// Loads an asset assuming extension as the hint.
std::ifstream Assets::open(string const& name)
{
    auto ext_start = name.find_last_of('.');
    if (ext_start == string::npos || ext_start == 0 || ext_start == name.length() - 1)
        throw std::invalid_argument("Asset name '" + name + "' does not have an extension.");
    return std::move(open(name, name.substr(ext_start + 1, name.length() - ext_start + 1)));
}

// Loads an asset searching the directories registered for given hint.
std::ifstream Assets::open(string const& name, string const& hint)
{
    auto directories_it = m_directories.find(hint);
    if (directories_it == m_directories.end())
        throw std::invalid_argument("Unknown hint: " + hint + ".");
    std::vector<string> const& directories = directories_it->second;

    auto directory = directories.begin();
    std::ifstream result;
    while (!result.is_open() && directory != directories.end())
        result.open(*directory + "/" + name);

    if (!result.is_open())
        throw asset_unavailable(name, hint, directories);
    return std::move(result);
}

string Assets::read_all_text_(std::istream& istr)
{
    int start_pos = istr.tellg();
    istr.seekg(0, std::istream::end);
    int stream_size = (int)istr.tellg() - start_pos;
    string result(stream_size, '\0');
    istr.seekg(start_pos, std::istream::beg);
    istr.read(&result[0], stream_size);
    return std::move(result);
}

asset_unavailable::asset_unavailable(
    string const& name, string const& hint, std::vector<string> const& searched_directories) :
    runtime_error("Unable to locate or open asset '" + name + "' with hint '" + hint + "'."),
    m_name(name), m_hint(hint), m_searched_directories(searched_directories)
{ }
