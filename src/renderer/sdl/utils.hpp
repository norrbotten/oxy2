#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace Oxy::SDL {

  template <typename Callable>
  std::string fmt(Callable callback) {
    std::stringstream ss;
    callback(ss);
    return ss.str();
  }

  inline bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '-';
  }

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
    if (auto pos = str.find('\n'); pos != std::string::npos)
      return pos;

    return -1;
  }

  inline std::string get_indent(int size) {
    std::string res;
    for (int i = 0; i < size; i++)
      res += "  ";
    return res;
  }

  inline std::vector<std::string> explode(const std::string& str, const std::string& delim) {
    std::vector<std::string> tokens;

    size_t prev = 0, pos = 0;

    do {
      pos = str.find(delim, prev);
      if (pos == std::string::npos)
        pos = str.length();

      std::string token = str.substr(prev, pos - prev);

      if (!token.empty())
        tokens.push_back(token);

      prev = pos + delim.length();

    } while (pos < str.length() && prev < str.length());

    return tokens;
  }

  inline bool parse_float(double* result, const std::string& str) {
    std::stringstream ss;
    ss << str;

    double val;

    if (!(ss >> val))
      return false;

    *result = val;

    return true;
  }

  inline bool parse_float_triplet(double* result, const std::string& str) {
    auto trimmed = trim_lr(str);

    if (trimmed.at(0) == '[')
      trimmed = trimmed.substr(1);

    if (trimmed.back() == ']')
      trimmed = trimmed.substr(0, trimmed.size() - 1);

    auto split = explode(trimmed, ",");

    if (split.size() == 1) {
      double val = 0.0;
      if (!parse_float(&val, split.at(0)))
        return false;

      result[0] = val;
      result[1] = val;
      result[2] = val;

      return true;
    }
    else if (split.size() == 3) {
      return parse_float(result + 0, split.at(0)) && parse_float(result + 1, split.at(1)) &&
             parse_float(result + 2, split.at(2));
    }
    else {
      return false;
    }
  }

  inline bool parse_int(int* result, const std::string& str) {
    std::stringstream ss;
    ss << str;

    int val;

    if (!(ss >> val))
      return false;

    *result = val;

    return true;
  }

} // namespace Oxy::SDL
