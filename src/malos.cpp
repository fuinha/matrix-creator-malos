/*
 * Copyright 2016 <Admobilize>
 * MATRIX Labs  [http://creator.matrix.one]
 * This file is part of MATRIX Creator MALOS
 *
 * MATRIX Creator MALOS is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <chrono>
#include <iostream>
#include <thread>

#include "./driver_imu.h"
#include "./driver_humidity.h"
#include "./driver_everloop.h"

#include "matrix_hal/wishbone_bus.h"

const int kBasePort = 20013;

const char kUnsecureBindScope[] = "*";

namespace matrix_malos {

int RunServer() {
  std::cerr << "**************" << std::endl;
  std::cerr << "MALOS starting" << std::endl;
  std::cerr << "**************" << std::endl;
  std::cerr << std::endl;

  matrix_hal::WishboneBus* wishbone_bus = new matrix_hal::WishboneBus();
  wishbone_bus->SpiInit();

  ImuDriver driver_imu;
  driver_imu.SetupWishboneBus(wishbone_bus);
  if (!driver_imu.Init(kBasePort, kUnsecureBindScope)) {
    return 1;
  }

  HumidityDriver driver_humidity;
  driver_humidity.SetupWishboneBus(wishbone_bus);
  if (!driver_humidity.Init(kBasePort + 4 * 1, kUnsecureBindScope)) {
    return 1;
  }

  HumidityDriver driver_everloop;
  driver_everloop.SetupWishboneBus(wishbone_bus);
  if (!driver_everloop.Init(kBasePort + 4 * 2, kUnsecureBindScope)) {
    return 1;
  }

  // Busy waiting because we are detaching the 0MQ threads we create
  // during driver initializations. Is "for (;;) pause();" better?
  // I don't know how pause changes ZMQ's interaction with signals.
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1000));
  }

  return 0;  // Never reached.
}
}  // namespace matrix_malos

int main(int, char* []) { return matrix_malos::RunServer(); }