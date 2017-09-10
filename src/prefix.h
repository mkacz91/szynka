#ifndef SZYNKA_PREFIX_H
#define SZYNKA_PREFIX_H

#include <cassert>
#include <utility>
#include <string>
#include <sstream>
#include <stdexcept>

namespace szynka {

using std::string;

inline string quote(const string& s) { return "\"" + s + "\""; }
inline string squote(const string& s) { return "'" + s + "'"; }

template <typename T> inline
std::string to_string(const T& t)
{
    std::ostringstream sout;
    sout << t;
    return sout.str();
}

} // namespace szynka

#endif // SZYNKA_PREFIX_H
