#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "mystring.hpp"

namespace sjtu {
struct SeatStatus {
  int station_num_, remain_seats_[35];
  SeatStatus() : station_num_(0) {}
  SeatStatus(int station_num, int seat_num) : station_num_(station_num) {
    for (int i = 0; i < station_num_; ++i) {
      remain_seats_[i] = seat_num;
    }
  }
  bool operator==(const SeatStatus &other) const {
    if (station_num_ != other.station_num_) {
      return false;
    }
    for (int i = 0; i < station_num_; ++i) {
      if (remain_seats_[i] != other.remain_seats_[i]) {
        return false;
      }
    }
    return true;
  }
  bool operator<(const SeatStatus &other) const {
    if (station_num_ != other.station_num_) {
      return station_num_ < other.station_num_;
    }
    for (int i = 0; i < station_num_; ++i) {
      if (remain_seats_[i] != other.remain_seats_[i]) {
        return remain_seats_[i] < other.remain_seats_[i];
      }
    }
    return false;
  }
};

class Train {
private:
  char type_;
  bool released_ = false;
  my_string train_id_, stations_[35];
  // prices_[i] is the price from station 0 to station i
  int station_num_, seat_num_, prices_[35], sales_date_[2];
  // travel_times_[i] is the travel time from station 0 to station i (including stopover time)
  int start_time_, travel_times_[35], stopover_times_[35];

public:
  Train() : type_(), train_id_(), station_num_(0), seat_num_(0), start_time_(0) {
    for (int i = 0; i < 35; ++i) {
      stations_[i] = my_string();
      prices_[i] = 0;
      travel_times_[i] = 0;
      stopover_times_[i] = 0;
    }
    sales_date_[0] = 0;
    sales_date_[1] = 0;
  }
  Train(const my_string &train_id, const int station_num, const int seat_num, 
    const my_string stations[], const int prices[], const int start_time, 
    const int travel_times[], const int stopover_times[], const int sales_date[], const char type) :
    train_id_(train_id), station_num_(station_num), seat_num_(seat_num), start_time_(start_time), type_(type) {
    for (int i = 0; i < station_num_; ++i) {
      stations_[i] = stations[i];
    }
    if (station_num_ > 0) {
      stopover_times_[0] = 0;
    }
    for (int i = 1; i < station_num_ - 1; ++i) {
      stopover_times_[i] = stopover_times[i - 1];
    }
    if (station_num_ > 1) {
      stopover_times_[station_num_ - 1] = 0;
    }
    prices_[0] = 0;
    for (int i = 1; i < station_num_; ++i) {
      prices_[i] = prices_[i - 1] + prices[i - 1];
    }
    travel_times_[0] = 0;
    for (int i = 1; i < station_num_; ++i) {
      travel_times_[i] = travel_times_[i - 1] + travel_times[i - 1] + stopover_times_[i - 1];
    }
    sales_date_[0] = sales_date[0];
    sales_date_[1] = sales_date[1];
  }
  Train(const Train &other) : type_(other.type_), train_id_(other.train_id_), 
    station_num_(other.station_num_), seat_num_(other.seat_num_), start_time_(other.start_time_) {
    for (int i = 0; i < station_num_; ++i) {
      stations_[i] = other.stations_[i];
      prices_[i] = other.prices_[i];
      travel_times_[i] = other.travel_times_[i];
    }
    for (int i = 0; i < station_num_; ++i) {
      stopover_times_[i] = other.stopover_times_[i];
    }
    sales_date_[0] = other.sales_date_[0];
    sales_date_[1] = other.sales_date_[1];
  }

  Train &operator=(const Train &other) {
    if (this != &other) {
      type_ = other.type_;
      train_id_ = other.train_id_;
      station_num_ = other.station_num_;
      seat_num_ = other.seat_num_;
      start_time_ = other.start_time_;
      for (int i = 0; i < station_num_; ++i) {
        stations_[i] = other.stations_[i];
        prices_[i] = other.prices_[i];
        travel_times_[i] = other.travel_times_[i];
        stopover_times_[i] = other.stopover_times_[i];
      }
      sales_date_[0] = other.sales_date_[0];
      sales_date_[1] = other.sales_date_[1];
    }
    return *this;
  }

  bool operator==(const Train &other) const {
    return train_id_ == other.train_id_;
  }
  bool operator!=(const Train &other) const {
    return !(*this == other);
  }
  bool operator<(const Train &other) const {
    return train_id_ < other.train_id_;
  }
  bool operator>(const Train &other) const {
    return train_id_ > other.train_id_;
  }
  bool operator<=(const Train &other) const {
    return !(*this > other);
  }
  bool operator>=(const Train &other) const {
    return !(*this < other);
  }

  const char get_type() const { return type_; }
  const my_string &get_train_id() const { return train_id_; }
  int get_station_num() const { return station_num_; }
  int get_seat_num() const { return seat_num_; }
  const my_string &get_station(int index) const { return stations_[index]; }
  int get_price(int index) const { return prices_[index]; }
  int get_start_time() const { return start_time_; }
  int get_travel_time(int index) const { return travel_times_[index]; }
  int get_stopover_time(int index) const { return stopover_times_[index]; }
  int get_sales_date(int index) const { return sales_date_[index]; }
  bool is_released() const { return released_; }

  void set_type(char type) { type_ = type; }
  void set_train_id(const my_string &train_id) { train_id_ = train_id; }
  void set_station_num(int station_num) { station_num_ = station_num; }
  void set_seat_num(int seat_num) { seat_num_ = seat_num; }
  void set_station(int index, const my_string &station) { stations_[index] = station; }
  void set_price(int index, int price) { prices_[index] = price; }
  void set_start_time(int start_time) { start_time_ = start_time; }
  void set_travel_time(int index, int travel_time) { travel_times_[index] = travel_time; }
  void set_stopover_time(int index, int stopover_time) { stopover_times_[index] = stopover_time; }
  void set_sales_date(int index, int sales_date) { sales_date_[index] = sales_date; }
  void release() { released_ = true; }
};
}

#endif // TRAIN_HPP
