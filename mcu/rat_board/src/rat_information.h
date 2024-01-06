/**
 * @file rat.h
 * @brief
 */

#pragma once

#include <stdint.h>

namespace rat {

// TODO: search_modeとappel_modeをちゃんと考える
struct Information {
private:
  uint8_t search_mode_;
  uint8_t appel_mode_;

public:
  Information() {
    search_mode_ = 0;
    appel_mode_ = 0;
  }
  void setSearchMode(int search_mode) { search_mode_ = search_mode; }
  void setAppelMode(int appel_mode) { appel_mode_ = appel_mode; }
  uint8_t getSearchMode() { return search_mode_; }
  uint8_t getAppelMode() { return appel_mode_; }
};
}  // namespace rat
extern rat::Information rat_information;
