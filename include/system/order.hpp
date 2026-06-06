#ifndef ORDER_HPP
#define ORDER_HPP

#include <string>
#include "mystring.hpp"

namespace sjtu {
  enum OrderStatus { SUCCESS, REFUNDED, PENDING };
  inline std::string order_status_to_string(OrderStatus status) {
    switch (status) {
      case SUCCESS: return "[success]";
      case REFUNDED: return "[refunded]";
      case PENDING: return "[pending]";
      default: throw std::invalid_argument("Invalid order status");
    }
  }

class Order {
private:
  my_string username_, train_id_, from_, to_;
  int start_date_, num_tickets_, price_, timestamp_, from_index_, to_index_;
  OrderStatus status_;

public:
  Order() : username_(), train_id_(), from_(), to_(), start_date_(0), num_tickets_(0), price_(0), 
    timestamp_(0), from_index_(0), to_index_(0), status_(PENDING) {}
  Order(const my_string &username, const my_string &train_id, const my_string &from, const my_string &to,
    int start_date, int num_tickets, int price, int time, int from_index, int to_index, OrderStatus status)
    : username_(username), train_id_(train_id), from_(from), to_(to), start_date_(start_date), num_tickets_(num_tickets), 
      price_(price), timestamp_(time), from_index_(from_index), to_index_(to_index), status_(status) {}
  Order(const Order &other)
    : username_(other.username_), train_id_(other.train_id_), from_(other.from_), to_(other.to_),
      start_date_(other.start_date_), num_tickets_(other.num_tickets_), price_(other.price_), timestamp_(other.timestamp_), 
      from_index_(other.from_index_), to_index_(other.to_index_), status_(other.status_) {}

  Order &operator=(const Order &other) {
    if (this != &other) {
      username_ = other.username_;
      train_id_ = other.train_id_;
      from_ = other.from_;
      to_ = other.to_;
      start_date_ = other.start_date_;
      num_tickets_ = other.num_tickets_;
      price_ = other.price_;
      timestamp_ = other.timestamp_;
      from_index_ = other.from_index_;
      to_index_ = other.to_index_;
      status_ = other.status_;
    }
    return *this;
  }

  bool operator==(const Order &other) const {
    return timestamp_ == other.timestamp_;
  }
  bool operator!=(const Order &other) const {
    return !(*this == other);
  }
  bool operator<(const Order &other) const {
    return timestamp_ < other.timestamp_;
  }
  bool operator>(const Order &other) const {
    return timestamp_ > other.timestamp_;
  }
  bool operator<=(const Order &other) const {
    return !(*this > other);
  }
  bool operator>=(const Order &other) const {
    return !(*this < other);
  }

  const my_string& get_username() const { return username_; }
  const my_string& get_train_id() const { return train_id_; }
  const my_string& get_from() const { return from_; }
  const my_string& get_to() const { return to_; }
  int get_timestamp() const { return timestamp_; }
  int get_start_date() const { return start_date_; }
  int get_num_tickets() const { return num_tickets_; }
  int get_price() const { return price_; }
  int get_from_index() const { return from_index_; }
  int get_to_index() const { return to_index_; }
  OrderStatus get_status() const { return status_; }

  void set_status(OrderStatus new_status) { status_ = new_status; }
};
}

#endif // ORDER_HPP
