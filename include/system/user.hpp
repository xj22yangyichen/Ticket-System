#ifndef USER_HPP
#define USER_HPP

#include "mystring.hpp"

namespace sjtu {
class User {
private:
  my_string username_, password_, name_, mail_addr_;
  int privilege_, ticket_num_;
  bool logged_in_;

public:
  User() : username_(), password_(), name_(), mail_addr_(), privilege_(-1), ticket_num_(0), logged_in_(false) {}
  User(const my_string& username, const my_string& password, 
    const my_string& name, const my_string& mail_addr, int privilege)
    : username_(username), password_(password), name_(name), 
    mail_addr_(mail_addr), privilege_(privilege), ticket_num_(0), logged_in_(false) {}
  User(const User& other)
    : username_(other.username_), password_(other.password_), 
    name_(other.name_), mail_addr_(other.mail_addr_), 
    privilege_(other.privilege_), ticket_num_(other.ticket_num_), logged_in_(other.logged_in_) {}

  User& operator=(const User& other) {
    if (this != &other) {
      username_ = other.username_;
      password_ = other.password_;
      name_ = other.name_;
      mail_addr_ = other.mail_addr_;
      privilege_ = other.privilege_;
      ticket_num_ = other.ticket_num_;
      logged_in_ = other.logged_in_;
    }
    return *this;
  }
  bool operator==(const User& other) const {
    return username_ == other.username_;
  }
  bool operator!=(const User& other) const {
    return !(*this == other);
  }
  bool operator<(const User& other) const {
    return username_ < other.username_;
  }
  bool operator>(const User& other) const {
    return username_ > other.username_;
  }
  bool operator<=(const User& other) const {
    return !(*this > other);
  }
  bool operator>=(const User& other) const {
    return !(*this < other);
  }
  
  const my_string& get_username() const { return username_; }
  const my_string& get_password() const { return password_; }
  const my_string& get_name() const { return name_; }
  const my_string& get_mail_addr() const { return mail_addr_; }
  int get_privilege() const { return privilege_; }
  bool is_logged_in() const { return logged_in_; }

  void set_password(const my_string& password) { password_ = password; }
  void set_name(const my_string& name) { name_ = name; }
  void set_mail_addr(const my_string& mail_addr) { mail_addr_ = mail_addr; }
  void set_privilege(int privilege) { privilege_ = privilege; }
  void set_logged_in(bool logged_in) { logged_in_ = logged_in; }
};
}

#endif // USER_HPP
