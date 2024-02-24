/**
 * @file state.cpp
 * @brief
 */

#include "state_machine/state.h"

namespace state_machine {

// clang-format off
std::vector<std::vector<State>> state_list = {
  {
    {.main = main_state::Idle, .sub = idle::sub_state::Idle, .name = "Idle::Idle", .function = idle::idle_process},
    {.main = main_state::Idle, .sub = idle::sub_state::NoConnect, .name = "Idle::NoConnect", .function = idle::no_connect_process},
    {.main = main_state::Idle, .sub = idle::sub_state::ChangingState, .name = "Idle::ChangingState", .function = idle::changing_state_process},
  },
  {
    {.main = main_state::Search, .sub = search::sub_state::Start, .name = "Search::Start", .function = search::start_process},
    {.main = main_state::Search, .sub = search::sub_state::Search, .name = "Search::Search", .function = search::search_process},
    {.main = main_state::Search, .sub = search::sub_state::DetectObstacle, .name = "Search::DetectObstacle", .function = search::detect_obstacle_process},
    {.main = main_state::Search, .sub = search::sub_state::AvoidObstacle, .name = "Search::AvoidObstacle", .function = search::avoid_obstacle_process},
    {.main = main_state::Search, .sub = search::sub_state::AppelToCat, .name = "Search::AppelToCat", .function = search::appel_to_cat_process},
    {.main = main_state::Search, .sub = search::sub_state::Finish, .name = "Search::Finish", .function = search::finish_process},
  },
  {
    {.main = main_state::CaughtByCat, .sub = caught_by_cat::sub_state::Start, .name = "CaughtByCat::Start", .function = caught_by_cat::start_process},
    {.main = main_state::CaughtByCat, .sub = caught_by_cat::sub_state::Caught, .name = "CaughtByCat::Caught", .function = caught_by_cat::caught_process},
    {.main = main_state::CaughtByCat, .sub = caught_by_cat::sub_state::Finish, .name = "CaughtByCat::Finish", .function = caught_by_cat::finish_process},
  },
  {
    {.main = main_state::Feed, .sub = feed::sub_state::Start, .name = "Feed::Start", .function = feed::start_process},
    {.main = main_state::Feed, .sub = feed::sub_state::AppelToCat, .name = "Feed::AppelToCat", .function = feed::appel_to_cat_process},
    {.main = main_state::Feed, .sub = feed::sub_state::Feed, .name = "Feed::Feed", .function = feed::feed_process},
    {.main = main_state::Feed, .sub = feed::sub_state::Finish, .name = "Feed::Start", .function = feed::finish_process},
  },
  {
    {.main = main_state::Manual, .sub = manual::sub_state::Manual, .name = "Manual::Manual", .function = manual::manual_process}
  },
  {
    {.main = main_state::Error, .sub = error::sub_state::Error, .name = "Error::Error", .function = error::error_process}
  }
};
// clang-format on

}  // namespace state_machine
