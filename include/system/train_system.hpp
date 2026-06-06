#ifndef TRAIN_SYSTEM_HPP
#define TRAIN_SYSTEM_HPP

#include "train.hpp"
#include "order.hpp"
#include "b_plus_tree.hpp"
#include <map>

namespace sjtu {
class TrainSystem {
private:
  // key: train_id, value: Train
  b_plus_tree<my_string, Train> trains_;
  // key: station, value: (train_id, station_index)
  b_plus_tree<my_string, pair<my_string, int>> stations_;
  // key: (train_id, date), value: SeatStatus
  b_plus_tree<pair<my_string, int>, SeatStatus> seats_;

  std::map<my_string, vector<pair<my_string, int>>> station_cache_map_;

  const vector<pair<my_string, int>> &GetStations(const my_string &station);

  struct TicketInfo {
    my_string train_id_;
    int leaving_time_, arriving_date_, arriving_time_, seat_, price_;
    TicketInfo() = delete;
    TicketInfo(const my_string &train_id, int leaving_time, 
      int arriving_date, int arriving_time, int seat, int price) :
      train_id_(train_id), leaving_time_(leaving_time), arriving_date_(arriving_date), 
      arriving_time_(arriving_time), seat_(seat), price_(price) {}
  };

  struct TransferInfo {
    my_string train_id1_, train_id2_, transfer_station_;
    int leaving_time_, transfer_arriving_date_, transfer_arriving_time_;
    int transfer_leaving_date_, transfer_leaving_time_, arriving_date_, arriving_time_;
    int time_, price_;
    TransferInfo() : time_(INT_MAX), price_(INT_MAX) {}
    TransferInfo(const my_string &train_id1, const my_string &train_id2, const my_string &transfer_station,
      int leaving_time, int transfer_arriving_date, int transfer_arriving_time, 
      int transfer_leaving_date, int transfer_leaving_time, int arriving_date, int arriving_time,
      int time, int price) : train_id1_(train_id1), train_id2_(train_id2), transfer_station_(transfer_station),
      leaving_time_(leaving_time), transfer_arriving_date_(transfer_arriving_date), 
      transfer_arriving_time_(transfer_arriving_time), transfer_leaving_date_(transfer_leaving_date), 
      transfer_leaving_time_(transfer_leaving_time), arriving_date_(arriving_date), arriving_time_(arriving_time),
      time_(time), price_(price) {}
  };

  // key: timestamp, value: Order
  b_plus_tree<int, Order> orders_;
  // key: username, value: timestamp
  b_plus_tree<my_string, int> orders_by_user_;
  // waitlist
  // key: (train_id, start_date), value: timestamp
  b_plus_tree<pair<my_string, int>, int> orders_by_train_;

public:
  TrainSystem() : trains_("trains.dat"), stations_("stations.dat"), seats_("seats.dat"), 
    orders_("orders.dat"), orders_by_user_("orders_by_user.dat"), orders_by_train_("orders_by_train.dat"),
    station_cache_map_() {}

  bool add_train(const Train &train);
  bool delete_train(const my_string &train_id);
  bool release_train(const my_string &train_id);
  void query_train(const my_string &train_id, int date);
  // sort_by_time = true means sorting by departure time, otherwise sorting by price
  void query_ticket(const my_string &start_station, const my_string &end_station, int date, bool sort_by_time);
  void query_transfer(const my_string &start_station, const my_string &end_station, int date, bool sort_by_time);

  // the check of whether the user is logged in is not done in this function
  void buy_ticket(int timestamp, const my_string &username, const my_string &train_id, int date, 
    const my_string &from, const my_string &to, int num_tickets, bool waitlist_willness = false);
  void query_order(const my_string &username);
  void refund_ticket(const my_string &username, int nth = 1);

  void clear();
};
}

#endif // TRAIN_SYSTEM_HPP
