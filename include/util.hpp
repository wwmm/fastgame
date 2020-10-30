#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <tuple>
#include <vector>

namespace util {

void debug(const std::string& s);
void error(const std::string& s);
void critical(const std::string& s);
void warning(const std::string& s);
void info(const std::string& s);

auto read_system_file_options(const std::string& path_str) -> std::tuple<std::vector<std::string>, std::string>;

}  // namespace util

#endif
