#ifndef USER_SYSTEM_HPP
#define USER_SYSTEM_HPP

#include "user.hpp"
#include "b_plus_tree.hpp"

namespace sjtu {
class UserSystem {
private:
  // key: username, value: User
  b_plus_tree<my_string, User> users_;

public:
  UserSystem() : users_("users.dat") {}
  bool add_user(const User &user, bool is_admin = false);
  bool delete_user(const my_string &username);
  bool find_user(const my_string &username, User &user);
  bool modify_user(const User &user);
  bool login(const my_string &username, const my_string &password);
  bool logout(const my_string &username);
  int get_user_count();
  void clear();
};
}

#endif // USER_SYSTEM_HPP
