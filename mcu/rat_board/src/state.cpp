/**
 * @file state_process.cpp
 * @brief
 */

#include "state_machine/state.h"

#include "rat.h"
#include "timer/timer_1ms.h"

namespace state_machine {

static State idle_state_list[] = {
    {.main = main_state::Idle, .sub = idle::sub_state::Idle, .name = "Idle::Idle", .function = idle::idle_process},
    {.main = main_state::Idle, .sub = idle::sub_state::WaitForConnection, .name = "Idle::WaitForConnection", .function = idle::wait_for_connection_process}};

static State search_state_list[] = {
    {.main = main_state::Search, .sub = search::sub_state::Start, .name = "Search::Start", .function = search::start_process},
    {.main = main_state::Search, .sub = search::sub_state::Search, .name = "Search::Search", .function = search::search_process},
    {.main = main_state::Search, .sub = search::sub_state::DetectObstacle, .name = "Search::DetectObstacle", .function = search::detect_obstacle_process},
    {.main = main_state::Search, .sub = search::sub_state::AvoidObstacle, .name = "Search::AvoidObstacle", .function = search::avoid_obstacle_process},
    {.main = main_state::Search, .sub = search::sub_state::AppelToCat, .name = "Search::AppelToCat", .function = search::appel_to_cat_process},
    {.main = main_state::Search, .sub = search::sub_state::Finish, .name = "Search::Finish", .function = search::finish_process},
};

static State caught_by_cat_state_list[] = {
    {.main = main_state::CaughtByCat, .sub = caught_by_cat::sub_state::Start, .name = "CaughtByCat::Start", .function = caught_by_cat::start_process},
    {.main = main_state::CaughtByCat, .sub = caught_by_cat::sub_state::Caught, .name = "CaughtByCat::Caught", .function = caught_by_cat::caught_process},
    {.main = main_state::CaughtByCat, .sub = caught_by_cat::sub_state::Finish, .name = "CaughtByCat::Finish", .function = caught_by_cat::finish_process},
};

static State feed_state_list[] = {
    {.main = main_state::Feed, .sub = feed::sub_state::Start, .name = "Feed::Start", .function = feed::start_process},
    {.main = main_state::Feed, .sub = feed::sub_state::AppelToCat, .name = "Feed::AppelToCat", .function = feed::appel_to_cat_process},
    {.main = main_state::Feed, .sub = feed::sub_state::Feed, .name = "Feed::Feed", .function = feed::feed_process},
    {.main = main_state::Feed, .sub = feed::sub_state::Finish, .name = "Feed::Start", .function = feed::finish_process},
};

static State error_state_list[] = {
    {.main = main_state::Error, .sub = error::sub_state::Error, .name = "Error::Error", .function = error::error_process}};
State *state_list[] = {idle_state_list, search_state_list, caught_by_cat_state_list, feed_state_list, error_state_list};

namespace idle {

void idle_process() {
}

void wait_for_connection_process() {
}

}  // namespace idle

namespace search {

void start_process() {
}

void search_process() {
}

void detect_obstacle_process() {
}

void avoid_obstacle_process() {
}

void appel_to_cat_process() {
}

void finish_process() {
}

}  // namespace search

namespace caught_by_cat {

void start_process() {
}

void caught_process() {
}

void finish_process() {
}

}  // namespace caught_by_cat

namespace feed {

void start_process() {
}

// 使用するのは給餌器側のみ
void appel_to_cat_process() {
}

void feed_process() {
}

void finish_process() {
}

}  // namespace feed

namespace error {

void error_process() {
}

}  // namespace error

}  // namespace state_machine
