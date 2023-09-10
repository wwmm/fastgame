#ifndef UTIL_HPP
#define UTIL_HPP

#include <glib.h>
#include <algorithm>
#include <array>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <source_location>
#include <thread>
#include <vector>

#ifdef __clang__
#include <experimental/source_location>
#endif

namespace util {

#ifdef __clang__
using source_location = std::experimental::source_location;
#else
using source_location = std::source_location;
#endif

void debug(const std::string& s, source_location location = source_location::current());

void error(const std::string& s, source_location location = source_location::current());

void critical(const std::string& s, source_location location = source_location::current());

void warning(const std::string& s, source_location location = source_location::current());

void info(const std::string& s, source_location location = source_location::current());

auto read_system_setting(const std::string& path_str) -> std::vector<std::string>;

auto get_selected_value(const std::vector<std::string>& list) -> std::string;

auto find_hwmon_index(const int& card_index) -> int;

void apply_cpu_affinity(const int& pid, const std::vector<int>& cpu_affinity);

void clear_cpu_affinity(const int& pid);

void set_process_scheduler(const int& pid, const int& policy_index, const int& priority);

auto card_is_amdgpu(const int& card_index) -> bool;

auto get_amdgpu_indices() -> std::vector<int>;

auto remove_filename_extension(const std::string& basename) -> std::string;

template <typename T>
auto to_string(const T& num, const std::string def = "0") -> std::string {
  // This is used to replace `std::to_string` as a locale independent
  // number conversion using `std::to_chars`.
  // An additional string parameter could be eventually provided with a
  // default value to return in case the conversion fails.

  // Max buffer length:
  // number of base-10 digits that can be represented by the type T without change +
  // number of base-10 digits that are necessary to uniquely represent all distinct
  // values of the type T (meaningful only for real numbers) +
  // room for other characters such as "+-e,."
  const size_t max = std::numeric_limits<T>::digits10 + std::numeric_limits<T>::max_digits10 + 10U;

  std::array<char, max> buffer;

  const auto p_init = buffer.data();

  const auto result = std::to_chars(p_init, p_init + max, num);

  return (result.ec == std::errc()) ? std::string(p_init, result.ptr - p_init) : def;
}

template <typename T>
auto str_to_num(const std::string& str, T& num) -> bool {
  // This is a more robust implementation of `std::from_chars`
  // so that we don't have to do every time with `std::from_chars_result` structure.
  // We don't care of error types, so a simple bool is returned on success/fail.
  // A left trim is performed on strings so that the conversion could success
  // even if there are leading whitespaces and/or the plus sign.

  auto first_char = str.find_first_not_of(" +\n\r\t");

  if (first_char == std::string::npos) {
    return false;
  }

  const auto result = std::from_chars(str.data() + first_char, str.data() + str.size(), num);

  return (result.ec == std::errc());
}

}  // namespace util

#endif
