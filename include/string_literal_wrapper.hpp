#pragma once

#include <array>
#include <cstddef>

template <size_t N>
struct StringLiteralWrapper {
  constexpr StringLiteralWrapper(const char (&str)[N]) : msg(std::to_array(str)) {}

  std::array<char, N> msg;
};