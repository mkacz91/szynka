#ifndef SZYNKA_ASSETS_H
#define SZYNKA_ASSETS_H

#include "prefix.h"
#include <map>
#include <vector>
#include <fstream>

namespace szynka {

// Manages application file assets.
class Assets
{
public:
    static void add_directory(string const& directory, string const& hints);
    static void add_config(std::istream& config);
    static void add_config(string const& filename);
    static std::ifstream open(string const& name);
    static std::ifstream open(string const& name, string const& hint);

    template <class... Ts>
    static string read_all_text(Ts&&... args);

private:
    static std::map<string, std::vector<string>> m_directories;

    static string read_all_text_(std::istream& istr);
};

// Exception thrown when an asset can not be loaded.
class asset_unavailable : public std::runtime_error
{
public:
    asset_unavailable(
        string const& name, string const& hint, std::vector<string> const& searched_directories);

    string const& name() const { return m_name; }
    string const& hint() const { return m_hint; }
    std::vector<string> const& searched_directories() { return m_searched_directories; }

private:
    string m_name, m_hint;
    std::vector<string> const& m_searched_directories;
};

template <class... Ts> inline
string Assets::read_all_text(Ts&&... args)
{
    auto istr = open(std::forward<Ts>(args)...);
    return read_all_text_(istr);
}

} // namespace szynka

#endif // SZYNKA_ASSETS_H
