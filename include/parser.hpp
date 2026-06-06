#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "mystring.hpp"

namespace sjtu {
class Command {
private:
  std::string cmd_;
  int pos_;

public:
  Command() : cmd_(), pos_(0) {}
  Command(const std::string &cmd) : cmd_(cmd), pos_(0) {}
  bool read();
  std::string next_token();
};

char GetKey(const std::string &str);
char GetChar(const std::string &str);
my_string GetString(const std::string &str);
int GetInt(const std::string &str);
int GetDate(const std::string &str);
int GetTime(const std::string &str);
int GetTimestamp(const std::string &str);
void GetIntArray(const std::string &str, int *array);
void GetStringArray(const std::string &str, my_string *array);
void GetDateArray(const std::string &str, int *array);
void GetTimeArray(const std::string &str, int *array);
}

#endif // PARSER_HPP
