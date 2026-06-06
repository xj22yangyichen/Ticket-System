#include "parser.hpp"
#include "mystring.hpp"
#include "converter.hpp"

namespace sjtu {
  bool Command::read() {
    std::getline(std::cin, cmd_);
    pos_ = 0;
    return !cmd_.empty();
  }

  std::string Command::next_token() {
    while (pos_ < cmd_.size() && cmd_[pos_] == ' ') {
      ++pos_;
    }
    if (pos_ >= cmd_.size()) {
      return "";
    }
    int start = pos_;
    while (pos_ < cmd_.size() && cmd_[pos_] != ' ') {
      ++pos_;
    }
    return cmd_.substr(start, pos_ - start);
  }

  char GetKey(const std::string &str) {
    return str[1];
  }

  char GetChar(const std::string &str) {
    return str[0];
  }

  my_string GetString(const std::string &str) {
    return my_string(str.c_str());
  }

  int GetInt(const std::string &str) {
    int sign = 1;
    int i = 0;
    if (!str.empty() && str[0] == '-') {
      sign = -1;
      i = 1;
    }
    int value = 0;
    for (; i < static_cast<int>(str.size()); ++i) {
      value = value * 10 + (str[i] - '0');
    }
    return value * sign;
  }

  int GetDate(const std::string &str) {
    return date_to_int(str);
  }

  int GetTime(const std::string &str) {
    return time_to_int(str);
  }

  int GetTimestamp(const std::string &str) {
    int value = 0;
    for (int i = 1; i + 1 < static_cast<int>(str.size()); ++i) {
      value = value * 10 + (str[i] - '0');
    }
    return value;
  }

  void GetIntArray(const std::string &str, int *array) {
    int size = 0;
    int pos = 0;
    while (pos < str.size()) {
      while (pos < str.size() && str[pos] == '|') {
        ++pos;
      }
      if (pos >= str.size()) {
        break;
      }
      int start = pos;
      while (pos < str.size() && str[pos] != '|') {
        ++pos;
      }
      array[size++] = std::stoi(str.substr(start, pos - start));
    }
  }

  void GetStringArray(const std::string &str, my_string *array) {
    int size = 0;
    int pos = 0;
    while (pos < str.size()) {
      while (pos < str.size() && str[pos] == '|') {
        ++pos;
      }
      if (pos >= str.size()) {
        break;
      }
      int start = pos;
      while (pos < str.size() && str[pos] != '|') {
        ++pos;
      }
      array[size++] = my_string(str.substr(start, pos - start).c_str());
    }
  }

  void GetDateArray(const std::string &str, int *array) {
    int size = 0;
    int pos = 0;
    while (pos < str.size()) {
      while (pos < str.size() && str[pos] == '|') {
        ++pos;
      }
      if (pos >= str.size()) {
        break;
      }
      int start = pos;
      while (pos < str.size() && str[pos] != '|') {
        ++pos;
      }
      array[size++] = date_to_int(str.substr(start, pos - start));
    }
  }

  void GetTimeArray(const std::string &str, int *array) {
    if (str == "_" || str.empty()) {
      return;
    }
    int size = 0;
    int pos = 0;
    while (pos < str.size()) {
      while (pos < str.size() && str[pos] == '|') {
        ++pos;
      }
      if (pos >= str.size()) {
        break;
      }
      int start = pos;
      while (pos < str.size() && str[pos] != '|') {
        ++pos;
      }
      array[size++] = time_to_int(str.substr(start, pos - start));
    }
  }
}