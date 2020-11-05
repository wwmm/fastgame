#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <vector>

namespace util {

void debug(const std::string& s);
void error(const std::string& s);
void critical(const std::string& s);
void warning(const std::string& s);
void info(const std::string& s);

auto read_system_setting(const std::string& path_str) -> std::vector<std::string>;

auto get_selected_value(const std::vector<std::string>& list) -> std::string;

auto find_hwmon_index(const int& card_index) -> int;

void apply_cpu_affinity(const int& pid, const std::vector<int>& cpu_affinity);

void set_process_scheduler(const int& pid, const int& policy_index);

}  // namespace util

#endif
