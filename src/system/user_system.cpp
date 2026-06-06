#include "user_system.hpp"

namespace sjtu {
  bool UserSystem::add_user(const User &user, bool is_admin) {
    if (!users_.find(user.get_username()).empty()) {
      return false;
    }
    users_.insert(user.get_username(), user);
    return true;
  }

  bool UserSystem::delete_user(const my_string &username) {
    if (users_.find(username).empty()) {
      return false;
    }
    User old_user = *users_.find(username).begin();
    users_.erase(username, old_user);
    return true;
  }

  bool UserSystem::find_user(const my_string &username, User &user) {
    auto res = users_.find(username);
    if (res.empty()) {
      return false;
    }
    user = *res.begin();
    return true;
  }

  bool UserSystem::modify_user(const User &user) {
    if (users_.find(user.get_username()).empty()) {
      return false;
    }
    User old_user = *users_.find(user.get_username()).begin();
    users_.erase(user.get_username(), old_user);
    users_.insert(user.get_username(), user);
    return true;
  }

  bool UserSystem::login(const my_string &username, const my_string &password) {
    auto res = users_.find(username);
    if (res.empty()) {
      return false;
    }
    User user = *res.begin();
    if (user.get_password() != password) {
      return false;
    }
    return true;
  }

  bool UserSystem::logout(const my_string &username) {
    auto res = users_.find(username);
    if (res.empty()) {
      return false;
    }
    return true;
  }

  int UserSystem::get_user_count() {
    return users_.size();
  }

  void UserSystem::clear() {
    users_.clear();
  }
}