// File: seq_gain.hpp
// Project: examples
// Created Date: 20/07/2021
// Author: Shun Suzuki
// -----
// Last Modified: 10/09/2021
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2021 Hapis Lab. All rights reserved.
//

#pragma once

#include <iostream>
#include <vector>

#include "autd3.hpp"
#include "autd3/gain/eigen_backend.hpp"
#include "autd3/gain/holo.hpp"

using autd::NUM_TRANS_X, autd::NUM_TRANS_Y, autd::TRANS_SPACING_MM;
using autd::gain::holo::EigenBackend;

inline void seq_gain_test(const autd::ControllerPtr& autd) {
  autd->silent_mode() = false;

  const auto m = autd::modulation::Static::create();
  autd->send(m);

  const auto seq = autd::sequence::GainSequence::create();


  const autd::Vector3 center1(TRANS_SPACING_MM * ((NUM_TRANS_X - 1) / 2.0), TRANS_SPACING_MM * ((NUM_TRANS_Y - 1) / 2.0), 200.0);
  const autd::Vector3 center2(TRANS_SPACING_MM * ((NUM_TRANS_X - 1) / 2.0) + 15.0, TRANS_SPACING_MM * ((NUM_TRANS_Y - 1) / 2.0), 250.0);
  int a, b, c, d;
  a = 50; b = 50; c = 20; d = 50;
  constexpr auto point_num = 170;  // a+b+c+d
  const double rate = 2;
  for (auto i = 0; i < point_num; i++) {
    double radius;
    const auto theta1 = 2.0 * M_PI * static_cast<double>(i * rate) / static_cast<double>(point_num);
    const auto theta2 = 2.0 * M_PI * (static_cast<double>(i * rate) + static_cast<double>(1))/ static_cast<double>(point_num);
    const auto theta3 = 2.0 * M_PI * (static_cast<double>(i * rate) + static_cast<double>(2)) / static_cast<double>(point_num);
    const auto theta4 = 2.0 * M_PI * (static_cast<double>(i * rate) + static_cast<double>(3)) / static_cast<double>(point_num);
    double z;
    if (static_cast<double>(i % (a + b + c + d)) < a) {
        // ‹ß‚Ã‚­
        radius = 3.0;
        z = static_cast<double>(i % (a + b + c + d))* static_cast<double>(d) / static_cast<double>(a);
    }
    else if (a <= static_cast<double>(i % (a + b + c + d)) && static_cast<double>(i % (a + b + c + d)) < static_cast<double>(a + b)) {
        // —¯‚Ü‚é
        radius = static_cast<double>(3) - static_cast<double>(2) * (static_cast<double>(i % (a + b + c + d) - a) / static_cast<double>(b));
        z = static_cast<double>(c);
    }
    else if (static_cast<double>(a + b) <= static_cast<double>(i % (a + b + c + d)) && static_cast<double>(i % (a + b + c + d)) < static_cast<double>(a + b + c)) {
        radius = 1.0;
        z = static_cast<double>(c);
    }
    else {
        // —£‚ê‚é
        radius = 1.0;
        z = (static_cast<double>(a + b + c) - static_cast<double>(i % (a + b + c)));
    }
    // const auto z = static_cast<double>(i % 100) / static_cast<double>(4);
    const autd::Vector3 p1(radius * std::cos(theta1), radius * std::sin(theta1), z);
    const autd::Vector3 p2(radius * std::cos(theta2), radius * std::sin(theta2), z);
    const autd::Vector3 p3(radius * std::cos(theta3), radius * std::sin(theta3), z);
    const autd::Vector3 p4(radius * std::cos(theta4), radius * std::sin(theta4), z);
    std::vector<autd::Vector3> foci = {center1 + p1, center1 + p2, center1 + p3, center1 + p4};
    std::vector<double> amps = {1, 1, 1, 1};
    const auto g = autd::gain::holo::SDP::create(EigenBackend::create(), foci, amps);
    // const auto g = autd::gain::FocalPoint::create(center + p);
    seq->add_gain(g);
  }

  const auto actual_freq = seq->set_frequency(1);

  std::cout << "Actual frequency is " << actual_freq << " Hz\n";
  autd->send(seq);
}
