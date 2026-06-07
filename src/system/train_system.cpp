#include "train_system.hpp"
#include "converter.hpp"
#include "algorithm.hpp"

namespace sjtu {
  static const int kMaxTrainCache = 1500;
  static const int kMaxStationCache = 800;

  const vector<pair<my_string, int>> &TrainSystem::GetStations(const my_string &station) {
    auto it = station_cache_map_.find(station);
    if (it != station_cache_map_.end()) {
      return it->second;
    }
    if (station_cache_map_.size() >= kMaxStationCache) {
      station_cache_map_.clear();
    }
    auto result = stations_.find(station);
    auto inserted = station_cache_map_.insert({station, result});
    return inserted.first->second;
  }

  const Train &TrainSystem::GetTrain(const my_string &train_id) {
    auto it = train_cache_.find(train_id);
    if (it != train_cache_.end()) {
      return it->second;
    }
    if (train_cache_.size() >= kMaxTrainCache) {
      train_cache_.clear();
    }
    auto result = trains_.find(train_id);
    auto inserted = train_cache_.insert({train_id, result[0]});
    return inserted.first->second;
  }

  bool TrainSystem::cache_train(const my_string &train_id, const Train *&out_train) {
    auto it = train_cache_.find(train_id);
    if (it != train_cache_.end()) {
      out_train = &it->second;
      return true;
    }
    if (train_cache_.size() >= kMaxTrainCache) {
      train_cache_.clear();
    }
    auto result = trains_.find(train_id);
    if (result.empty()) {
      return false;
    }
    auto inserted = train_cache_.insert({train_id, result[0]});
    out_train = &inserted.first->second;
    return true;
  }

  bool TrainSystem::add_train(const Train &train) {
    if (!trains_.find(train.get_train_id()).empty()) {
      return false;
    }
    trains_.insert(train.get_train_id(), train);
    return true;
  }

  bool TrainSystem::delete_train(const my_string &train_id) {
    const Train *train_ptr;
    if (!cache_train(train_id, train_ptr)) {
      return false;
    }
    const auto &train = *train_ptr;
    if (!seats_.find({train_id, train.get_sales_date(0)}).empty()) {
      return false;
    }
    trains_.erase(train_id, train);
    auto cache_it = train_cache_.find(train_id);
    if (cache_it != train_cache_.end()) train_cache_.erase(cache_it);
    return true;
  }

  bool TrainSystem::release_train(const my_string &train_id) {
    const Train *train_ptr;
    if (!cache_train(train_id, train_ptr)) {
      return false;
    }
    auto train = *train_ptr;
    if (!seats_.find({train_id, train.get_sales_date(0)}).empty()) {
      return false;
    }

    Train old_train = train;
    train.release();
    trains_.erase(train_id, old_train);
    trains_.insert(train_id, train);
    auto cache_it2 = train_cache_.find(train_id);
    if (cache_it2 != train_cache_.end()) train_cache_.erase(cache_it2);
    station_cache_map_.clear();
    for (int i = 0; i < train.get_station_num(); ++i) {
      stations_.insert(train.get_station(i), {train_id, i});
    }
    for (int i = train.get_sales_date(0); i <= train.get_sales_date(1); ++i) {
      seats_.insert({train_id, i}, SeatStatus(train.get_station_num(), train.get_seat_num()));
    }
    return true;
  }

  void TrainSystem::query_train(const my_string &train_id, int date) {
    const Train *train_ptr;
    if (!cache_train(train_id, train_ptr)) {
      std::cout << "-1" << '\n';
      return;
    }
    const auto &train = *train_ptr;
    if (date < train.get_sales_date(0) || date > train.get_sales_date(1)) {
      std::cout << "-1" << '\n';
      return;
    }

    int station_num = train.get_station_num();
    std::cout << train.get_train_id() << " " << train.get_type() << '\n';
    auto seats_for_date = seats_.find({train_id, date});
    if (seats_for_date.empty()) {
      std::cout << train.get_station(0) << " xx-xx xx:xx -> " << int_to_date(date) << " "
        << int_to_time(train.get_depart(0)) << " " << train.get_price(0) << " " << train.get_seat_num() << '\n';
      for (int i = 1; i < station_num - 1; ++i) {
        std::cout << train.get_station(i) << " "
          << int_to_date(date + train.get_arrive(i) / 1440) << " "
          << int_to_time(train.get_arrive(i) % 1440) << " -> "
          << int_to_date(date + train.get_depart(i) / 1440) << " "
          << int_to_time(train.get_depart(i) % 1440) << " "
          << train.get_price(i) << " " << train.get_seat_num() << '\n';
      }
      std::cout << train.get_station(station_num - 1) << " "
        << int_to_date(date + train.get_arrive(station_num - 1) / 1440) << " "
        << int_to_time(train.get_arrive(station_num - 1) % 1440) << " -> xx-xx xx:xx "
        << train.get_price(station_num - 1) << " x" << '\n';
    } else {
      auto seats = seats_for_date[0];
      std::cout << train.get_station(0) << " xx-xx xx:xx -> " << int_to_date(date) << " "
        << int_to_time(train.get_depart(0)) << " " << train.get_price(0) << " " << seats.remain_seats_[0] << '\n';
      for (int i = 1; i < station_num - 1; ++i) {
        std::cout << train.get_station(i) << " "
          << int_to_date(date + train.get_arrive(i) / 1440) << " "
          << int_to_time(train.get_arrive(i) % 1440) << " -> "
          << int_to_date(date + train.get_depart(i) / 1440) << " "
          << int_to_time(train.get_depart(i) % 1440) << " "
          << train.get_price(i) << " " << seats.remain_seats_[i] << '\n';
      }
      std::cout << train.get_station(station_num - 1) << " "
        << int_to_date(date + train.get_arrive(station_num - 1) / 1440) << " "
        << int_to_time(train.get_arrive(station_num - 1) % 1440) << " -> xx-xx xx:xx "
        << train.get_price(station_num - 1) << " x" << '\n';
    }
  }

  void TrainSystem::query_ticket(const my_string &start_station, const my_string &end_station, int date, bool sort_by_time) {
    const auto &starts = GetStations(start_station);
    const auto &ends = GetStations(end_station);
    
    // use two pointers to find all trains that pass through start_station and end_station in order
    vector<TicketInfo> tickets;
    int n = starts.size(), m = ends.size();
    int i = 0, j = 0;
    while (i < n && j < m) {
      auto [train_id1, index1] = starts[i];
      auto [train_id2, index2] = ends[j];
      if (train_id1 == train_id2) {
        const Train *train_ptr;
        if (!cache_train(train_id1, train_ptr)) {
          ++i;
          continue;
        }
        const auto &train = *train_ptr;
        if (index1 >= index2) {
          ++i;
          continue;
        }
        /*
        note that the date of departure from start_station may be different from 
        the date of departure from the origin station, so we need to calculate 
        the date of departure from start_station and check if it is within the sales date
        */
        int departure_offset = train.get_depart(index1);
        int start_date = date - departure_offset / 1440;
        if (start_date < train.get_sales_date(0) || start_date > train.get_sales_date(1)) {
          ++i;
          continue;
        }
        auto seats_list = seats_.find({train_id1, start_date});
        if (seats_list.empty()) {
          ++i;
          continue;
        }
        auto seats = seats_list[0];
        int available_seats = 100000;
        for (int k = index1; k < index2; ++k) {
          available_seats = min(available_seats, seats.remain_seats_[k]);
          if (available_seats == 0) {
            break;
          }
        }
        // if (available_seats == 0) {
        //   ++i;
        //   continue;
        // }
        int leaving_time = departure_offset % 1440;
        int arriving_offset = train.get_arrive(index2);
        int arriving_date = start_date + arriving_offset / 1440;
        int arriving_time = arriving_offset % 1440;
        tickets.push_back(TicketInfo(train_id1, leaving_time, arriving_date, arriving_time, 
          available_seats, train.get_price(index2) - train.get_price(index1)));
        ++i;
        ++j;
      } else if (train_id1 < train_id2) {
        ++i;
      } else {
        ++j;
      }
    }

    if (sort_by_time) {
      tickets.sort([date](const TicketInfo &a, const TicketInfo &b) {
        if ((a.arriving_date_ - date) * 1440 + (a.arriving_time_ - a.leaving_time_) != 
          (b.arriving_date_ - date) * 1440 + (b.arriving_time_ - b.leaving_time_)) {
          return (a.arriving_date_ - date) * 1440 + (a.arriving_time_ - a.leaving_time_) < 
            (b.arriving_date_ - date) * 1440 + (b.arriving_time_ - b.leaving_time_);
        }
        return a.train_id_ < b.train_id_;
      });
    } else {
      tickets.sort([](const TicketInfo &a, const TicketInfo &b) {
        if (a.price_ != b.price_) {
          return a.price_ < b.price_;
        }
        return a.train_id_ < b.train_id_;
      });
    }

    std::cout << tickets.size() << '\n';
    for (const auto &ticket : tickets) {
      std::cout << ticket.train_id_ << " " << start_station << " " << int_to_date(date) << " " 
        << int_to_time(ticket.leaving_time_) << " -> " << end_station << " "
        << int_to_date(ticket.arriving_date_) << " " << int_to_time(ticket.arriving_time_) << " "
        << ticket.price_ << " " << ticket.seat_ << '\n';
    }
  }

  void TrainSystem::query_transfer(const my_string &start_station, const my_string &end_station, int date, bool sort_by_time) {
    const auto &starts = GetStations(start_station);
    const auto &ends = GetStations(end_station);
    if (starts.empty() || ends.empty()) {
      std::cout << "0" << '\n';
      return;
    }
    vector<Train> end_trains;
    vector<char> end_valid;
    for (int i = 0; i < ends.size(); ++i) {
      const auto &train_id = ends[i].first;
      const Train *train_ptr;
      if (cache_train(train_id, train_ptr)) {
        end_trains.push_back(*train_ptr);
        end_valid.push_back(1);
      } else {
        end_trains.push_back(Train());
        end_valid.push_back(0);
      }
    }
    auto find_end_index = [&ends](const my_string &train_id) -> int {
      if (ends.empty()) {
        return -1;
      }
      int l = 0;
      int r = ends.size() - 1;
      while (l <= r) {
        int mid = (l + r) >> 1;
        if (ends[mid].first == train_id) {
          return mid;
        }
        if (ends[mid].first < train_id) {
          l = mid + 1;
        } else {
          r = mid - 1;
        }
      }
      return -1;
    };
    struct TransferResult {
      my_string train_id1, train_id2, transfer_station;
      int leaving_time, transfer_arriving_date, transfer_arriving_time;
      int transfer_leaving_date, transfer_leaving_time;
      int arriving_date, arriving_time;
      int seat1, seat2;
      int price1, price2;
      int total_time, total_price;
      bool valid;
      TransferResult() : leaving_time(0), transfer_arriving_date(0), transfer_arriving_time(0),
        transfer_leaving_date(0), transfer_leaving_time(0), arriving_date(0), arriving_time(0),
        seat1(0), seat2(0), price1(0), price2(0), total_time(INT_MAX), total_price(INT_MAX), valid(false) {}
    } best;

    for (auto &[start_train_id, start_index] : starts) {
      const Train *train_ptr;
      if (!cache_train(start_train_id, train_ptr)) {
        continue;
      }
      const auto &start_train = *train_ptr;

      int start_departure_offset = start_train.get_depart(start_index);
      int start_date = date - start_departure_offset / 1440;
      if (start_date < start_train.get_sales_date(0) || start_date > start_train.get_sales_date(1)) {
        continue;
      }

      auto start_seats_list = seats_.find({start_train_id, start_date});
      if (start_seats_list.empty()) {
        continue;
      }
      auto start_seats = start_seats_list[0];
      int leaving_time = start_departure_offset % 1440;

      int seat1_min = 100000;
      for (int i = start_index + 1; i < start_train.get_station_num(); ++i) {
        seat1_min = min(seat1_min, start_seats.remain_seats_[i - 1]);
        my_string transfer_station = start_train.get_station(i);

        const auto &transfers = GetStations(transfer_station);
        for (int p = 0; p < transfers.size(); ++p) {
          auto [train_id1, transfer_index] = transfers[p];
          if (train_id1 == start_train_id) {
            continue;
          }
          int end_pos = find_end_index(train_id1);
          if (end_pos < 0) {
            continue;
          }
          int end_index = ends[end_pos].second;
          if (!end_valid[end_pos] || transfer_index >= end_index) {
            continue;
          }
          auto &end_train = end_trains[end_pos];
          int transfer_arriving_offset = start_train.get_arrive(i);
          int transfer_arriving_date = start_date + transfer_arriving_offset / 1440;
          int transfer_arriving_time = transfer_arriving_offset % 1440;

          int transfer_departure_offset = end_train.get_depart(transfer_index);
          int transfer_departure_time = transfer_departure_offset % 1440;

          int candidate_start_date = transfer_arriving_date - transfer_departure_offset / 1440;
          if (transfer_departure_time < transfer_arriving_time) {
            ++candidate_start_date;
          }
          if (candidate_start_date < end_train.get_sales_date(0)) {
            candidate_start_date = end_train.get_sales_date(0);
          }
          int transfer_leaving_date = candidate_start_date + transfer_departure_offset / 1440;
          if (transfer_leaving_date < transfer_arriving_date ||
              (transfer_leaving_date == transfer_arriving_date && transfer_departure_time < transfer_arriving_time)) {
            ++candidate_start_date;
            transfer_leaving_date = candidate_start_date + transfer_departure_offset / 1440;
          }
          if (candidate_start_date > end_train.get_sales_date(1)) {
            continue;
          }

          int arriving_offset = end_train.get_arrive(end_index);
          int arriving_date = candidate_start_date + arriving_offset / 1440;
          int arriving_time = arriving_offset % 1440;

          int price1 = start_train.get_price(i) - start_train.get_price(start_index);
          int price2 = end_train.get_price(end_index) - end_train.get_price(transfer_index);
          int total_price = price1 + price2;
          int total_time = (arriving_date - date) * 1440 + arriving_time - leaving_time;

          bool better = false;
          if (!best.valid) {
            better = true;
          } else if (sort_by_time) {
            if (total_time < best.total_time ||
                (total_time == best.total_time && (total_price < best.total_price ||
                (total_price == best.total_price && (start_train_id < best.train_id1 ||
                (start_train_id == best.train_id1 && train_id1 < best.train_id2)))))) {
              better = true;
            }
          } else {
            if (total_price < best.total_price ||
                (total_price == best.total_price && (total_time < best.total_time ||
                (total_time == best.total_time && (start_train_id < best.train_id1 ||
                (start_train_id == best.train_id1 && train_id1 < best.train_id2)))))) {
              better = true;
            }
          }

          if (better) {
            auto end_seats_list = seats_.find({train_id1, candidate_start_date});
            if (end_seats_list.empty()) {
              continue;
            }
            auto end_seats = end_seats_list[0];
            int seat2_min = 100000;
            for (int k = transfer_index; k < end_index; ++k) {
              seat2_min = min(seat2_min, end_seats.remain_seats_[k]);
              if (seat2_min == 0) {
                break;
              }
            }

            best.valid = true;
            best.train_id1 = start_train_id;
            best.train_id2 = train_id1;
            best.transfer_station = transfer_station;
            best.leaving_time = leaving_time;
            best.transfer_arriving_date = transfer_arriving_date;
            best.transfer_arriving_time = transfer_arriving_time;
            best.transfer_leaving_date = transfer_leaving_date;
            best.transfer_leaving_time = transfer_departure_time;
            best.arriving_date = arriving_date;
            best.arriving_time = arriving_time;
            best.seat1 = seat1_min;
            best.seat2 = seat2_min;
            best.price1 = price1;
            best.price2 = price2;
            best.total_time = total_time;
            best.total_price = total_price;
          }
        }
      }
    }

    if (!best.valid) {
      std::cout << "0" << '\n';
      return;
    }

    std::cout << best.train_id1 << " " << start_station << " " << int_to_date(date) << " "
      << int_to_time(best.leaving_time) << " -> " << best.transfer_station << " "
      << int_to_date(best.transfer_arriving_date) << " " << int_to_time(best.transfer_arriving_time) << " "
      << best.price1 << " " << best.seat1 << '\n';
    std::cout << best.train_id2 << " " << best.transfer_station << " "
      << int_to_date(best.transfer_leaving_date) << " " << int_to_time(best.transfer_leaving_time) << " -> "
      << end_station << " " << int_to_date(best.arriving_date) << " " << int_to_time(best.arriving_time) << " "
      << best.price2 << " " << best.seat2 << '\n';
  }

  void TrainSystem::clear() {
    trains_.clear();
    stations_.clear();
    seats_.clear();
    orders_.clear();
    orders_by_user_.clear();
    orders_by_train_.clear();
    station_cache_map_.clear();
    train_cache_.clear();
  }

  void TrainSystem::clear_caches() {
    station_cache_map_.clear();
    train_cache_.clear();
  }
}