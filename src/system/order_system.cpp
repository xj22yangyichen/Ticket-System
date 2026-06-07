#include "order.hpp"
#include "train_system.hpp"
#include "algorithm.hpp"
#include "converter.hpp"

namespace sjtu {
  void TrainSystem::buy_ticket(int timestamp, const my_string &username, const my_string &train_id, int date, 
    const my_string &from, const my_string &to, int num_tickets, bool waitlist_willness) {
      int from_index = -1, to_index = -1;
      const Train *train_ptr;
      if (!cache_train(train_id, train_ptr)) {
        std::cout << "-1" << '\n';
        return;
      }
      const auto &train = *train_ptr;
      if (seats_.find({train_id, train.get_sales_date(0)}).empty()) {
        std::cout << "-1" << '\n';
        return;
      }
      if (num_tickets > train.get_seat_num()) {
        std::cout << "-1" << '\n';
        return;
      }
      for (int i = 0; i < train.get_station_num(); ++i) {
        if (train.get_station(i) == from) {
          from_index = i;
        }
        if (train.get_station(i) == to) {
          to_index = i;
        }
      }
      if (from_index == -1 || to_index == -1 || from_index >= to_index) {
        std::cout << "-1" << '\n';
        return;
      }

      int start_date = date - train.get_depart(from_index) / 1440;
      if (start_date < train.get_sales_date(0) || start_date > train.get_sales_date(1)) {
        std::cout << "-1" << '\n';
        return;
      }
      auto seat_status = seats_.find({train_id, start_date})[0];
      int available_seats = 100000;
      for (int i = from_index; i < to_index; ++i) {
        available_seats = min(available_seats, seat_status.remain_seats_[i]);
        if (available_seats < num_tickets) {
          break;
        }
      }
      if (available_seats < num_tickets) {
        if (!waitlist_willness) {
          std::cout << "-1" << '\n';
          return;
        }
        auto order = Order(username, train_id, from, to, start_date, num_tickets, 
          train.get_price(to_index) - train.get_price(from_index), timestamp, from_index, to_index, PENDING);
        orders_.insert(timestamp, order);
        orders_by_user_.insert(username, timestamp);
        orders_by_train_.insert({train_id, start_date}, timestamp);
        std::cout << "queue" << '\n';
      } else {
        seats_.erase({train_id, start_date}, seat_status);
        for (int i = from_index; i < to_index; ++i) {
          seat_status.remain_seats_[i] -= num_tickets;
        }
        seats_.insert({train_id, start_date}, seat_status);
        auto order = Order(username, train_id, from, to, start_date, num_tickets, 
          train.get_price(to_index) - train.get_price(from_index), timestamp, from_index, to_index, SUCCESS);
        orders_.insert(timestamp, order);
        orders_by_user_.insert(username, timestamp);
        std::cout << 1ll * order.get_price() * order.get_num_tickets() << '\n';
      }
    }

  void TrainSystem::query_order(const my_string &username) {
    auto timestamps = orders_by_user_.find(username);
    std::cout << timestamps.size() << '\n';
    if (timestamps.size() == 0) {
      return;
    }
    for (auto it = timestamps.rbegin(); it != timestamps.rend(); ++it) {
      auto order = orders_.find(*it)[0];
      const auto &train = GetTrain(order.get_train_id());
      std::cout << order_status_to_string(order.get_status()) << " " << order.get_train_id() << " "
        << order.get_from() << " " << int_to_date(order.get_start_date() + train.get_depart(order.get_from_index()) / 1440)
        << " " << int_to_time(train.get_depart(order.get_from_index()) % 1440) << " -> "
        << order.get_to() << " " << int_to_date(order.get_start_date() + train.get_arrive(order.get_to_index()) / 1440)
        << " " << int_to_time(train.get_arrive(order.get_to_index()) % 1440)
        << " " << order.get_price() << " " << order.get_num_tickets() << '\n';
    }
  }

  void TrainSystem::refund_ticket(const my_string &username, int nth) {
    auto timestamps = orders_by_user_.find(username);
    if (timestamps.size() < nth) {
      std::cout << "-1" << '\n';
      return;
    }
    auto order = orders_.find(timestamps[timestamps.size() - nth])[0];
    if (order.get_status() == PENDING) {
      orders_.erase(order.get_timestamp(), order);
      orders_by_train_.erase({order.get_train_id(), order.get_start_date()}, order.get_timestamp());
      std::cout << 0 << '\n';
      order.set_status(REFUNDED);
      orders_.insert(order.get_timestamp(), order);
    }

    else if (order.get_status() == SUCCESS) {
      auto seat_status = seats_.find({order.get_train_id(), order.get_start_date()})[0];
      seats_.erase({order.get_train_id(), order.get_start_date()}, seat_status);
      for (int i = order.get_from_index(); i < order.get_to_index(); ++i) {
        seat_status.remain_seats_[i] += order.get_num_tickets();
      }
      seats_.insert({order.get_train_id(), order.get_start_date()}, seat_status);
      orders_.erase(order.get_timestamp(), order);
      orders_by_train_.erase({order.get_train_id(), order.get_start_date()}, order.get_timestamp());
      std::cout << 0 << '\n';
      order.set_status(REFUNDED);
      orders_.insert(order.get_timestamp(), order);

      // process waitlist
      auto waitlist_timestamps = orders_by_train_.find({order.get_train_id(), order.get_start_date()});
      vector<int> to_erase_from_waitlist;
      for (auto it = waitlist_timestamps.begin(); it != waitlist_timestamps.end(); ++it) {
        auto waitlist_order = orders_.find(*it)[0];
        if (waitlist_order.get_status() == PENDING) {
          int available_seats = 100000;
          for (int i = waitlist_order.get_from_index(); i < waitlist_order.get_to_index(); ++i) {
            available_seats = min(available_seats, seat_status.remain_seats_[i]);
            if (available_seats < waitlist_order.get_num_tickets()) {
              break;
            }
          }
          if (available_seats >= waitlist_order.get_num_tickets()) {
            seats_.erase({waitlist_order.get_train_id(), waitlist_order.get_start_date()}, seat_status);
            for (int i = waitlist_order.get_from_index(); i < waitlist_order.get_to_index(); ++i) {
              seat_status.remain_seats_[i] -= waitlist_order.get_num_tickets();
            }
            seats_.insert({waitlist_order.get_train_id(), waitlist_order.get_start_date()}, seat_status);
            orders_.erase(waitlist_order.get_timestamp(), waitlist_order);
            waitlist_order.set_status(SUCCESS);
            orders_.insert(waitlist_order.get_timestamp(), waitlist_order);
            to_erase_from_waitlist.push_back(*it);
          }
        }
      }
      for (auto timestamp : to_erase_from_waitlist) {
        orders_by_train_.erase({order.get_train_id(), order.get_start_date()}, timestamp);
      }
    }
    
    else {
      std::cout << "-1" << '\n';
    }
  }
}