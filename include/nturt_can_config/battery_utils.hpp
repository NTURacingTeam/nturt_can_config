/**
 * @file state_of_charge.hpp
 * @author QuantumSpawner jet22854111@gmail.com
 * @brief Utilities function to calculate the essential state of the battery.
 *
 * For some reason I have to do this.
 */

#ifndef STATE_OF_CHARGE_HPP
#define STATE_OF_CHARGE_HPP

// stl include
#include <array>

// nturt include
#include "nturt_can_config.h"

/* macro ---------------------------------------------------------------------*/
// prameters
#define NUM_BATTERY_SEGMENT 7
#define NUM_BATTERY_CELL_PER_SEGMENT 12
#define NUM_BATTERY_CELL_PER_FRAME 3

/// @brief Number of the valid battery cell, some segments are not fully
/// populated.
#define NUM_VALID_CELL 78

#define NUM_VALID_TEMPERATURE 73

#define CURRENT_TO_C 0.0606

/* typedef -------------------------------------------------------------------*/
// battery data
template <typename T>
using battery_data_t = std::array<std::array<T, NUM_BATTERY_CELL_PER_SEGMENT>,
                                  NUM_BATTERY_SEGMENT>;

struct BatteryData {
  BatteryData();

  void update(BMS_Cell_Stats_t *frame);

  double average_voltage();

  double average_temperature();

  double maximum_temperature();

  const static battery_data_t<bool> valid_cell;

  const static battery_data_t<bool> valid_temperature;

  battery_data_t<double> voltage = {0};

  battery_data_t<double> temperature = {0};
};

/**
 * @brief Calcutate the state of charge of the battery.
 *
 * @param voltage Voltage of a single battery cell in [V].
 * @param current Discharge current of the battery in [A].
 * @return double State of charge in [0 ~ 1].
 */
double state_of_charge(double voltage, double current);

#endif  // STATE_OF_CHARGE_HPP
