#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <string>
#include <string.h>

// date format: MM-DD
// 06-01 is the first day, represented by 1, and 08-31 is the last day, represented by 92
inline int date_to_int(const std::string &date_str) {
  // if (date_str == "xx-xx") return 0;
  int month = (date_str[0] - '0') * 10 + (date_str[1] - '0');
  int day = (date_str[3] - '0') * 10 + (date_str[4] - '0');
  // Dates before June are invalid for queries (map to 0, which will fail sales-date checks)
  if (month < 6) {
    return 0;
  }
  if (month == 8) {
    return 61 + day;
  }
  if (month == 9) {
    return 92 + day;
  }
  return (month - 6) * 30 + day;
}
inline std::string int_to_date(int date_int) {
  // if (date_int == 0) return "xx-xx";
  if (date_int <= 0) return "xx-xx";
  int month = 0;
  int day = 0;
  if (date_int <= 30) {
    month = 6;
    day = date_int;
  } else if (date_int <= 61) {
    month = 7;
    day = date_int - 30;
  } else if (date_int <= 92) {
    month = 8;
    day = date_int - 61;
  } else {
    month = 9;
    day = date_int - 92;
  }
  char buffer[6];
  snprintf(buffer, sizeof(buffer), "%02d-%02d", month, day);
  return std::string(buffer);
}

// time format: HH:MM
// 00:00 is represented by 0, and 23:59 is represented by 1439
inline int time_to_int(const std::string &time_str) {
  // if (time_str == "xx:xx") return 0;
  int hour = (time_str[0] - '0') * 10 + (time_str[1] - '0');
  int minute = (time_str[3] - '0') * 10 + (time_str[4] - '0');
  return hour * 60 + minute;
}
inline std::string int_to_time(int time_int) {
  // if (time_int == 0) return "xx:xx";
  int hour = time_int / 60;
  int minute = time_int % 60;
  char buffer[6];
  snprintf(buffer, sizeof(buffer), "%02d:%02d", hour, minute);
  return std::string(buffer);
}

#endif // CONVERTER_HPP
