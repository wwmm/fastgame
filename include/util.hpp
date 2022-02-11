#ifndef UTIL_HPP
#define UTIL_HPP

#include <glib.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
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

void clear_cpu_affinity(const int& pid);

void set_process_scheduler(const int& pid, const int& policy_index, const int& priority);

auto card_is_amdgpu(const int& card_index) -> bool;

auto get_irq_number(const std::string& name) -> int;

auto get_irq_affinity(const int& irq_number) -> int;  // we assume that the irq can use only one cpu core

auto remove_filename_extension(const std::string& basename) -> std::string;

}  // namespace util

#endif
