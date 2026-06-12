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
  /**
   * Add a new user to the system.
   * @param user The user to add.
   * @param is_admin Whether the user is an administrator.
   * @return True if the user was added successfully, false otherwise.
   */
  bool add_user(const User &user, bool is_admin = false);
  /**
   * Delete a user from the system.
   * @param username The username of the user to delete.
   * @return True if the user was deleted successfully, false otherwise.
   */
  bool delete_user(const my_string &username);
  /**
   * Find a user in the system.
   * @param username The username of the user to find.
   * @param user The user object to store the found user.
   * @return True if the user was found, false otherwise.
   */
  bool find_user(const my_string &username, User &user);
  /**
   * Modify a user in the system.
   * @param user The updated user object.
   * @return True if the user was modified successfully, false otherwise.
   */
  bool modify_user(const User &user);
  /**
   * Log in a user.
   * @param username The username of the user.
   * @param password The password of the user.
   * @return True if the user was logged in successfully, false otherwise.
   */
  bool login(const my_string &username, const my_string &password);
  /**
   * Log out a user.
   * @param username The username of the user.
   * @return True if the user was logged out successfully, false otherwise.
   */
  bool logout(const my_string &username);
  /**
   * Get the number of users in the system.
   * @return The number of users.
   */
  int get_user_count();
  /**
   * Clear all data.
   */
  void clear();
};
}

#endif // USER_SYSTEM_HPP
