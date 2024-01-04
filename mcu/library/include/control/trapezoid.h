/**
 * @file trapezoid.h
 * @brief
 */

#pragma once

#include <algorithm>
#include <cmath>
namespace control {

/**
 * @class Trapezoid
 * @brief 簡易的な台形加速のクラスのため、加速度は制御周期ごとに加算される
 *
 */
class Trapezoid {
private:
  float v_ = 0.0f;
  float v_max_ = 0.0f;
  float a_ = 0.0f;

  bool is_current_v_nearly_equal_target(float v) {
    if (v_ > v - a_ && v_ < v + a_)
      return true;
    else
      return false;
  }

public:
  Trapezoid(float v_max, float a)
      : v_max_(v_max), a_(a) {}

  void setAccel(float a) {
    if (a > 0.0f) a_ = a;
  }
  void setVelocityMax(float v_max) {
    if (v_max > 0.0f) v_max_ = v_max;
  }
  float calculate(float v) {
    // vが範囲外だった場合は範囲内にする
    v = std::max(std::min(v, v_max_), -v_max_);
    // 目標速度と現在速度の値が近い場合は、目標速度と現在速度を同じにする
    if (is_current_v_nearly_equal_target(v)) {
      v_ = v;
      return v_;
    }
    // 加速or減速
    if (v_ > v)
      v_ -= a_;
    else if (v_ < v)
      v_ += a_;

    return v_;
  }
  float getAccel() { return a_; }
  float getVelocityMax() { return v_max_; }
  float getVelocity() { return v_; }
};

}  // namespace control

/* sample code

Trapezoid trap(1.0f, 0.05f);

void calc(float target) {
  // このwhile分の比較だと、targetが範囲外だとおかしくなるから注意
  while (!(target == trap.getVelocity())) {
    trap.calculate(target);
    printf("current = %+3f, target = %+3f\r\n", trap.getVelocity(), target);
  }
}

int main(void) {
  printf("target = 1.0f\r\n");
  calc(1.0f);
  printf("target = -1.0f\r\n");
  calc(-1.0f);
  printf("target = 0.3f\r\n");
  calc(0.3f);
  printf("target = 0.4f\r\n");
  calc(0.4f);
  printf("target = 0.0f\r\n");
  calc(0.0f);
}
*/
