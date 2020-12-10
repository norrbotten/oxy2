#pragma once

#include <sstream>

namespace Oxy::SDL {

  template <typename Callable>
  std::string fmt(Callable callback) {
    std::stringstream ss;
    callback(ss);
    return ss.str();
  }

  inline bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

  inline bool is_alphanumeric(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
  }

  inline bool is_numeric(char c) { return (c >= '0' && c <= '9'); }

  inline bool is_whitespace(char c) { return c == ' ' || c == '\t'; }

  inline bool is_newline(char c) { return c == '\n' || c == '\r'; }

  inline std::string trim_lr(const std::string& original) {
    int n    = original.size();
    int left = 0, right = n - 1;

    while (left < n && is_whitespace(original.at(left)))
      left++;

    while (right > left && is_whitespace(original.at(right)))
      right--;

    return original.substr(left, right - left + 1);
  }

  inline int contains_newline(const std::string& str) {
    if (int pos = str.find('\n'); pos != std::string::npos)
      return pos;

    return -1;
  }

  inline std::string get_indent(int size) {
    std::string res;
    for (int i = 0; i < size; i++)
      res += "  ";
    return res;
  }

} // namespace Oxy::SDL
