#include "ticket_system.hpp"
#include "mystring.hpp"
#include "parser.hpp"
#include <iostream>

namespace sjtu {
  TicketSystem::TicketSystem() : user_system_(), train_system_() {
    logged_in_users_.clear();
  }

  bool TicketSystem::IsLoggedIn(const my_string &username) const {
    for (const auto &user : logged_in_users_) {
      if (user == username) {
        return true;
      }
    }
    return false;
  }
  
  TicketSystem::~TicketSystem() {
    for (const auto &username : logged_in_users_) {
      user_system_.logout(username);
    }
  }

  void TicketSystem::Run() {
    while (true) {
      bool has_command = current_command_.read();
      std::string cmd = current_command_.next_token();
      if (cmd.empty()) {
        if (!has_command) {
          break;
        }
        continue;
      }

      int timestamp = GetTimestamp(cmd);
      std::cout << "[" << timestamp << "] ";
      cmd = current_command_.next_token();

      if (cmd == "add_user") {
        AddUser();
      } else if (cmd == "login") {
        Login();
      } else if (cmd == "logout") {
        Logout();
      } else if (cmd == "query_profile") {
        QueryProfile();
      } else if (cmd == "modify_profile") {
        ModifyProfile();
      } else if (cmd == "add_train") {
        AddTrain();
      } else if (cmd == "delete_train") {
        DeleteTrain();
      } else if (cmd == "release_train") {
        ReleaseTrain();
      } else if (cmd == "query_train") {
        QueryTrain();
      } else if (cmd == "query_ticket") {
        QueryTicket();
      } else if (cmd == "query_transfer") {
        QueryTransfer();
      } else if (cmd == "buy_ticket") {
        BuyTicket(timestamp);
      } else if (cmd == "query_order") {
        QueryOrder();
      } else if (cmd == "refund_ticket") {
        RefundTicket();
      } else if (cmd == "clean") {
        Clean();
      } else if (cmd == "exit") {
        Exit();
        break;
      } else {
        std::cout << "Invalid command: " << cmd << '\n';
      }

      if (!has_command) {
        break;
      }
    }
  }
  
  void TicketSystem::AddUser() {
    my_string current_username, username, password, name, email;
    int privilege;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'c') {
        current_username = GetString(token);
      } else if (key == 'u') {
        username = GetString(token);
      } else if (key == 'p') {
        password = GetString(token);
      } else if (key == 'n') {
        name = GetString(token);
      } else if (key == 'm') {
        email = GetString(token);
      } else if (key == 'g') {
        privilege = GetInt(token);
      }
    }
    if (user_system_.get_user_count() == 0) {
      privilege = 10;
      user_system_.add_user(User(username, password, name, email, privilege));
      std::cout << "0" << '\n';
    } else {
      User current_user;
      if (!user_system_.find_user(current_username, current_user) || !IsLoggedIn(current_username) ||
          !(current_user.get_privilege() > privilege)) {
        std::cout << "-1" << '\n';
        return;
      } else if (user_system_.add_user(User(username, password, name, email, privilege))) {
        std::cout << "0" << '\n';
      } else {
        std::cout << "-1" << '\n';
      }
    }
  }

  void TicketSystem::Login() {
    my_string username, password;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'u') {
        username = GetString(token);
      } else if (key == 'p') {
        password = GetString(token);
      }
    }
    if (IsLoggedIn(username)) {
      std::cout << "-1" << '\n';
      return;
    }
    if (user_system_.login(username, password)) {
      std::cout << "0" << '\n';
      logged_in_users_.push_back(username);
    } else {
      std::cout << "-1" << '\n';
    }
  }

  void TicketSystem::Logout() {
    my_string username;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'u') {
        username = GetString(token);
      }
    }
    if (!IsLoggedIn(username)) {
      std::cout << "-1" << '\n';
      return;
    }
    if (user_system_.logout(username)) {
      std::cout << "0" << '\n';
      for (auto it = logged_in_users_.begin(); it != logged_in_users_.end(); ++it) {
        if (*it == username) {
          logged_in_users_.erase(it);
          break;
        }
      }
    } else {
      std::cout << "-1" << '\n';
    }
  }

  void TicketSystem::QueryProfile() {
    my_string current_username, username;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'c') {
        current_username = GetString(token);
      } else if (key == 'u') {
        username = GetString(token);
      }
    }
    User current_user, user;
    if (!user_system_.find_user(current_username, current_user) || !IsLoggedIn(current_username) ||
      !user_system_.find_user(username, user) || 
      !(current_user.get_privilege() > user.get_privilege() || current_username == username)) {
      std::cout << "-1" << '\n';
      return;
    }
    std::cout << user.get_username() << " " << user.get_name() << " " 
      << user.get_mail_addr() << " " << user.get_privilege() << '\n';
  }

  void TicketSystem::ModifyProfile() {
    my_string current_username, username, password, name, email;
    int privilege = -1;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'c') {
        current_username = GetString(token);
      } else if (key == 'u') {
        username = GetString(token);
      } else if (key == 'p') {
        password = GetString(token);
      } else if (key == 'n') {
        name = GetString(token);
      } else if (key == 'm') {
        email = GetString(token);
      } else if (key == 'g') {
        privilege = GetInt(token);
      }
    }
    User current_user, user;
    if (!user_system_.find_user(current_username, current_user) || !IsLoggedIn(current_username) ||
      !user_system_.find_user(username, user) || 
      !(current_user.get_privilege() > user.get_privilege() || current_username == username) || 
      (privilege != -1 && !(current_user.get_privilege() > privilege))) {
      std::cout << "-1" << '\n';
      return;
    }
    if (password != "") {
      user.set_password(password);
    }
    if (name != "") {
      user.set_name(name);
    }
    if (email != "") {
      user.set_mail_addr(email);
    }
    if (privilege != -1) {
      user.set_privilege(privilege);
    }
    user_system_.modify_user(user);
    std::cout << user.get_username() << " " << user.get_name() << " " 
      << user.get_mail_addr() << " " << user.get_privilege() << '\n';
  }

  void TicketSystem::AddTrain() {
    my_string train_id;
    char type;
    int station_num = 0, seat_num = 0, start_time = 0;
    int sales_date[2] = {0, 0};
    int travel_time[100] = {0}, stopover_time[100] = {0}, price[100] = {0};
    my_string station[100];
    for (int i = 0; i < 100; ++i) {
      station[i] = my_string();
    }
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'i') {
        train_id = GetString(token);
      } else if (key == 'y') {
        type = GetChar(token);
      } else if (key == 'n') {
        station_num = GetInt(token);
      } else if (key == 'm') {
        seat_num = GetInt(token);
      } else if (key == 'x') {
        start_time = GetTime(token);
      } else if (key == 'd') {
        GetDateArray(token, sales_date);
      } else if (key == 't') {
        GetIntArray(token, travel_time);
      } else if (key == 'o') {
        GetIntArray(token, stopover_time);
      } else if (key == 'p') {
        GetIntArray(token, price);
      } else if (key == 's') {
        GetStringArray(token, station);
      }
    }
    if (train_system_.add_train(Train(train_id, station_num, seat_num, station, price, 
      start_time, travel_time, stopover_time, sales_date, type))) {
      std::cout << "0" << '\n';
    } else {
      std::cout << "-1" << '\n';
    }
  }

  void TicketSystem::DeleteTrain() {
    my_string train_id;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'i') {
        train_id = GetString(token);
      }
    }
    if (train_system_.delete_train(train_id)) {
      std::cout << "0" << '\n';
    } else {
      std::cout << "-1" << '\n';
    }
  }

  void TicketSystem::ReleaseTrain() {
    my_string train_id;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'i') {
        train_id = GetString(token);
      }
    }
    if (train_system_.release_train(train_id)) {
      std::cout << "0" << '\n';
    } else {
      std::cout << "-1" << '\n';
    }
  }

  void TicketSystem::QueryTrain() {
    my_string train_id;
    int date;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'i') {
        train_id = GetString(token);
      } else if (key == 'd') {
        date = GetDate(token);
      }
    }
    train_system_.query_train(train_id, date);
  }

  void TicketSystem::QueryTicket() {
    my_string start_station, end_station;
    int date;
    bool sort_by_time = true;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 's') {
        start_station = GetString(token);
      } else if (key == 't') {
        end_station = GetString(token);
      } else if (key == 'd') {
        date = GetDate(token);
      } else if (key == 'p') {
        if (token == "time") {
          sort_by_time = true;
        } else if (token == "cost") {
          sort_by_time = false;
        }
      }
    }
    train_system_.query_ticket(start_station, end_station, date, sort_by_time);
  }

  void TicketSystem::QueryTransfer() {
    my_string start_station, end_station;
    int date;
    bool sort_by_time = true;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 's') {
        start_station = GetString(token);
      } else if (key == 't') {
        end_station = GetString(token);
      } else if (key == 'd') {
        date = GetDate(token);
      } else if (key == 'p') {
        if (token == "time") {
          sort_by_time = true;
        } else if (token == "cost") {
          sort_by_time = false;
        }
      }
    }
    train_system_.query_transfer(start_station, end_station, date, sort_by_time);
  }

  void TicketSystem::BuyTicket(int timestamp) {
    my_string username, train_id, start_station, end_station;
    int date, ticket_num;
    bool waitlist_willness = false;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'u') {
        username = GetString(token);
      } else if (key == 'i') {
        train_id = GetString(token);
      } else if (key == 'f') {
        start_station = GetString(token);
      } else if (key == 't') {
        end_station = GetString(token);
      } else if (key == 'd') {
        date = GetDate(token);
      } else if (key == 'n') {
        ticket_num = GetInt(token);
      } else if (key == 'q') {
        if (token == "true") {
          waitlist_willness = true;
        } else if (token == "false") {
          waitlist_willness = false;
        }
      }
    }
    User user;
    if (!user_system_.find_user(username, user) || !IsLoggedIn(username)) {
      std::cout << "-1" << '\n';
      return;
    }
    train_system_.buy_ticket(timestamp, username, train_id, date, start_station, end_station, ticket_num, waitlist_willness);
  }

  void TicketSystem::QueryOrder() {
    my_string username;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'u') {
        username = GetString(token);
      }
    }
    User user;
    if (!user_system_.find_user(username, user) || !IsLoggedIn(username)) {
      std::cout << "-1" << '\n';
      return;
    }
    train_system_.query_order(username);
  }

  void TicketSystem::RefundTicket() {
    my_string username;
    int nth = 1;
    char key;
    std::string token;
    while (!(token = current_command_.next_token()).empty()) {
      key = GetKey(token);
      token = current_command_.next_token();
      if (key == 'u') {
        username = GetString(token);
      } else if (key == 'n') {
        nth = GetInt(token);
      }
    }
    User user;
    if (!user_system_.find_user(username, user) || !IsLoggedIn(username)) {
      std::cout << "-1" << '\n';
      return;
    }
    train_system_.refund_ticket(username, nth);
  }

  void TicketSystem::Clean() {
    user_system_.clear();
    train_system_.clear();
    logged_in_users_.clear();
    std::cout << "0" << '\n';
  }

  void TicketSystem::Exit() {
    for (const auto &username : logged_in_users_) {
      user_system_.logout(username);
    }
    logged_in_users_.clear();
    std::cout << "bye" << '\n';
  }
}