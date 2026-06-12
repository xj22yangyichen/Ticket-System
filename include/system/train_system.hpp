#ifndef TRAIN_SYSTEM_HPP
#define TRAIN_SYSTEM_HPP

#include "train.hpp"
#include "order.hpp"
#include "b_plus_tree.hpp"
#include "map.hpp"

namespace sjtu {
class TrainSystem {
private:
  // key: train_id, value: Train
  b_plus_tree<my_string, Train> trains_;
  // key: station, value: (train_id, station_index)
  b_plus_tree<my_string, pair<my_string, int>> stations_;
  // key: (train_id, date), value: SeatStatus
  b_plus_tree<pair<my_string, int>, SeatStatus, 48> seats_;

  map<my_string, vector<pair<my_string, int>>> station_cache_;
  map<my_string, Train> train_cache_;

  /**
   * Get the list of trains that stop at a given station and the index of the station in the train's route.
   */
  vector<pair<my_string, int>> GetStations(const my_string &station);
  /**
   * Get the train with the given ID.
   */
  const Train &GetTrain(const my_string &train_id);
  /**
   * Cache the train with the given ID.
   */
  bool cache_train(const my_string &train_id, const Train *&out_train);

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
  b_plus_tree<int, Order, 48> orders_;
  // key: username, value: timestamp
  b_plus_tree<my_string, int, 32> orders_by_user_;
  // waitlist
  // key: (train_id, start_date), value: timestamp
  b_plus_tree<pair<my_string, int>, int, 32> orders_by_train_;

public:
  TrainSystem() : trains_("trains.dat"), stations_("stations.dat"), seats_("seats.dat"),
    orders_("orders.dat"), orders_by_user_("orders_by_user.dat"), orders_by_train_("orders_by_train.dat") {}

  /**
   * Add a new train to the system.
   */
  bool add_train(const Train &train);
  /**
   * Delete a train from the system.
   */
  bool delete_train(const my_string &train_id);
  /**
   * Release a train from the system.
   */
  bool release_train(const my_string &train_id);
  /**
   * Query information about a specific train.
   * @param train_id The ID of the train to query.
   * @param date The date for which to query information.
   */
  void query_train(const my_string &train_id, int date);
  /**
   * Query tickets between two stations.
   * @param start_station The starting station.
   * @param end_station The destination station.
   * @param date The date when the train departs from the starting station.
   * @param sort_by_time - True: sort by departure time
   *                     - False: sort by price
   */
  void query_ticket(const my_string &start_station, const my_string &end_station, int date, bool sort_by_time);
  /**
   * Query transfer options between two stations.
   * @param start_station The starting station.
   * @param end_station The destination station.
   * @param date The date when the train departs from the starting station.
   * @param sort_by_time - True: sort by departure time
   *                     - False: sort by price
   */
  void query_transfer(const my_string &start_station, const my_string &end_station, int date, bool sort_by_time);

  /**
   * Buy a ticket.
   * @param timestamp The timestamp of the purchase.
   * @param username The username of the buyer.
   * @param train_id The ID of the train.
   * @param date The date when the train departs from the starting station.
   * @param from The starting station.
   * @param to The destination station.
   * @param num_tickets The number of tickets to buy.
   * @param waitlist_willness Whether to add to the waitlist if no tickets are available.
   * @note The check of whether the user is logged in is not done in this function.
   */
  void buy_ticket(int timestamp, const my_string &username, const my_string &train_id, int date, 
    const my_string &from, const my_string &to, int num_tickets, bool waitlist_willness = false);
  /**
   * Query the orders of a user.
   * @param username The username of the user.
   */
  void query_order(const my_string &username);
  /**
   * Refund a ticket.
   * @param username The username of the user.
   * @param nth The nth ticket (the order of query_order) to refund.
   */
  void refund_ticket(const my_string &username, int nth = 1);

  /**
   * Clear all data.
   */
  void clear();
  /**
   * Clear all caches.
   */
  void clear_caches();
};
}

#endif // TRAIN_SYSTEM_HPP
