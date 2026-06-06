#ifndef TICKET_SYSTEM_HPP
#define TICKET_SYSTEM_HPP

#include "mystring.hpp"
#include "parser.hpp"
#include "user_system.hpp"
#include "train_system.hpp"

namespace sjtu {
class TicketSystem {
private:
  UserSystem user_system_;
  TrainSystem train_system_;

  Command current_command_;
  vector<my_string> logged_in_users_;

  bool IsLoggedIn(const my_string &username) const;

public:
  TicketSystem();
  ~TicketSystem();

  void Run();

  void AddUser();
  void Login();
  void Logout();
  void QueryProfile();
  void ModifyProfile();

  void AddTrain();
  void DeleteTrain();
  void ReleaseTrain();
  void QueryTrain();
  void QueryTicket();
  void QueryTransfer();

  void BuyTicket(int timestamp);
  void QueryOrder();
  void RefundTicket();

  void Clean();
  void Exit();
};
}

#endif // TICKET_SYSTEM_HPP
