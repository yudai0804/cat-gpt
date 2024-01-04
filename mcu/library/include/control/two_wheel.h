/**
 * @file two_wheel.h
 * @brief
 */

#pragma once

#include <math.h>

#include <cmath>

namespace control {

class TwoWheel {
private:
  float vr_ = 0.0f;
  float vl_ = 0.0f;
  float v_ = 0.0f;
  float omega_ = 0.0f;
  // トレッド幅
  const float d_;
  // 旋回半径
  float turning_radius_ = 0.0f;

public:
  TwoWheel(const float d) : d_(d) {}
  void calculateByVelocity(const float v, const float omega) {
    v_ = v;
    omega_ = omega;
    vr_ = v_ + d_ * omega_;
    vl_ = v_ - d_ * omega_;

    // 速度から求めた、vr,vlを使って旋回半径を計算
    // vr = vlのとき旋回半径は無限大となる
    if (vr_ - vl_ == 0.0f)
      turning_radius_ = INFINITY;
    else
      turning_radius_ = d_ * (vr_ + vl_) / (vr_ - vl_);
  }

  void calculateByOmega(const float omega, const float turning_radius) {
    omega_ = omega;
    turning_radius_ = turning_radius;
    vr_ = (turning_radius_ + d_) * omega_;
    vl_ = (turning_radius_ - d_) * omega_;

    // 角速度から求めた、vr,vlを使って速度を計算
    v_ = (vr_ + vl_) / 2.0f;
  }

  float getVr() { return vr_; }
  float getVl() { return vl_; }
  float getV() { return v_; }
  float getOmega() { return omega_; }
  float getD() { return d_; }
  float getTurningRadius() { return turning_radius_; }
};

}  // namespace control
