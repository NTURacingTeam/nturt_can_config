#include "nturt_can_config/battery_utils.hpp"

// glibc include
#include <string.h>

// stl include
#include <algorithm>
#include <limits>
#include <numeric>
#include <vector>

// nturt include
#include "nturt_can_config.h"

#define CURRENT_TO_C 1

/* Private typedef -----------------------------------------------------------*/
struct VoltageToSOC {
  double voltage;
  double soc;
};

struct CurrentToVoltageToSOC {
  double current;
  std::vector<VoltageToSOC> voltage_to_soc;
};

/*Static vataible ------------------------------------------------------------*/
static const std::vector<VoltageToSOC> data_1C = {
    {2.8, 0.0},
    {3.3318996415770608, 0.0379384395132428},
    {3.4924731182795696, 0.22004294917680753},
    {3.6379928315412187, 0.5327129563350036},
    {3.7985663082437275, 0.7234073013600573},
    {4.079569892473119, 0.9707945597709378},
    {4.159856630824373, 1.0},
};

static const std::vector<VoltageToSOC> data_2C = {
    {2.8, 0.0},
    {3.261648745519713, 0.06370794559770944},
    {3.382078853046595, 0.12727272727272726},
    {3.5376344086021505, 0.4313528990694346},
    {3.6379928315412187, 0.608303507516106},
    {4.039426523297491, 0.9776664280601288},
    {4.1046594982078854, 1.0017179670722978},
};

static const std::vector<VoltageToSOC> data_3C = {
    {2.81505376344086, 0.02075876879026493},
    {3.186379928315412, 0.07229778095919838},
    {3.311827956989247, 0.1375805297065141},
    {3.3620071684587813, 0.2011453113815319},
    {3.592831541218638, 0.6168933428775949},
    {3.9440860215053766, 0.9725125268432355},
    {4.049462365591398, 0.9982820329277022},
};

static const std::vector<VoltageToSOC> data_5C = {
    {2.825089605734767, 0.02591267000715826},
    {3.171326164874552, 0.08947745168217609},
    {3.306810035842294, 0.1805297065139586},
    {3.3519713261648745, 0.2526843235504654},
    {3.5275985663082436, 0.5911238367931282},
    {3.878853046594982, 0.9742304939155334},
    {3.999283154121864, 1.0},
};

static const std::vector<VoltageToSOC> data_8C = {
    {2.81505376344086, 0.024194702934860535},
    {3.151254480286738, 0.10150322118826063},
    {3.1913978494623656, 0.12899069434502516},
    {3.281720430107527, 0.22863278453829644},
    {3.683154121863799, 0.9347172512526842},
    {3.7584229390681, 0.9725125268432355},
    {4.004301075268817, 1.0},
};

static const std::vector<VoltageToSOC> data_10C = {
    {2.81505376344086, 0.02591267000715826},
    {3.161290322580645, 0.1358625626342162},
    {3.236559139784946, 0.2166070150322119},
    {3.52258064516129, 0.8694345025053686},
    {3.592831541218638, 0.9398711524695776},
    {3.773476702508961, 0.9914101646385111},
    {4.004301075268817, 1.0},
};

static const std::vector<VoltageToSOC> data_13C = {
    {2.81505376344086, 0.02763063707945598},
    {3.151254480286738, 0.18740157480314967},
    {3.306810035842294, 0.4605583392984968},
    {3.3770609318996416, 0.8659985683607732},
    {3.4974910394265235, 0.9450250536864709},
    {3.678136200716846, 0.9931281317108089},
    {3.9541218637992834, 0.9965640658554044},
};

static const std::vector<CurrentToVoltageToSOC> soc_data = {
    {1.0, data_1C}, {2.0, data_2C},   {3.0, data_3C},  {5.0, data_5C},
    {8.0, data_8C}, {10.0, data_10C}, {13.0, data_13C}};

/* Static class variable -----------------------------------------------------*/
const battery_data_t<bool> BatteryData::valid_cell = {{
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}},
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}},
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}},
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}},
}};

const battery_data_t<bool> BatteryData::valid_temperature = {{
    {{1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}},
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1}},
    {{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
    {{1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}},
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
    {{0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1}},
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}},
}};

/* Static function declaration -----------------------------------------------*/
static double state_of_charge(double voltage,
                              const std::vector<VoltageToSOC>& data_points);

/* Class function ------------------------------------------------------------*/
BatteryData::BatteryData() {
  memset(&voltage, 0, sizeof(voltage));
  memset(&temperature, 0, sizeof(temperature));
}

void BatteryData::update(BMS_Cell_Stats_t* frame) {
  int segment_index = frame->BMS_Segment_Index,
      cell_index = NUM_BATTERY_CELL_PER_FRAME * frame->BMS_Cell_Index;

  voltage[segment_index][cell_index] =
      valid_cell[segment_index][cell_index] * frame->BMS_Cell_Voltage_1_phys;
  voltage[segment_index][cell_index + 1] =
      valid_cell[segment_index][cell_index + 1] *
      frame->BMS_Cell_Voltage_2_phys;
  voltage[segment_index][cell_index + 2] =
      valid_cell[segment_index][cell_index + 2] *
      frame->BMS_Cell_Voltage_3_phys;

  temperature[segment_index][cell_index] =
      valid_temperature[segment_index][cell_index] *
      frame->BMS_Cell_Temperature_1_phys;
  temperature[segment_index][cell_index + 1] =
      valid_temperature[segment_index][cell_index + 1] *
      frame->BMS_Cell_Temperature_2_phys;
  temperature[segment_index][cell_index + 2] =
      valid_temperature[segment_index][cell_index + 2] *
      frame->BMS_Cell_Temperature_3_phys;
}

double BatteryData::average_voltage() {
  return std::accumulate(&voltage[0][0],
                         &voltage[NUM_BATTERY_SEGMENT - 1]
                                 [NUM_BATTERY_CELL_PER_SEGMENT - 1],
                         0) /
         NUM_VALID_CELL;
}

double BatteryData::average_temperature() {
  return std::accumulate(&voltage[0][0],
                         &voltage[NUM_BATTERY_SEGMENT - 1]
                                 [NUM_BATTERY_CELL_PER_SEGMENT - 1],
                         0) /
         NUM_VALID_TEMPERATURE;
}

double BatteryData::maximum_temperature() {
  return *std ::max_element(
      &temperature[0][0],
      &temperature[NUM_BATTERY_SEGMENT - 1][NUM_BATTERY_CELL_PER_SEGMENT - 1] +
          1);
}

/* Exported function ---------------------------------------------------------*/
double state_of_charge(double voltage, double _current) {
  double current = CURRENT_TO_C * _current;

  // find the two data points bounding the input value using binary search
  auto it =
      std::lower_bound(soc_data.begin(), soc_data.end(), voltage,
                       [](const CurrentToVoltageToSOC& a, double current) {
                         return a.current < current;
                       });

  std::vector<CurrentToVoltageToSOC>::const_iterator p1, p2;
  if (it == soc_data.begin()) {
    p1 = it;
    p2 = it + 1;
  } else {
    p1 = it - 1;
    p2 = it;
  }

  double soc1 = state_of_charge(voltage, p1->voltage_to_soc);
  double soc2 = state_of_charge(voltage, p2->voltage_to_soc);
  return soc1 + ((current - p1->current) / (p2->current - p1->current)) *
                    (soc2 - soc1);
}

/* Static function definition */
static double state_of_charge(double voltage,
                              const std::vector<VoltageToSOC>& data_points) {
  // find the two data points bounding the input value using binary search
  auto it = std::lower_bound(data_points.begin(), data_points.end(), voltage,
                             [](const VoltageToSOC& a, double voltage) {
                               return a.voltage < voltage;
                             });

  std::vector<VoltageToSOC>::const_iterator p1, p2;
  if (it == data_points.begin()) {
    p1 = it;
    p2 = it + 1;
  } else {
    p1 = it - 1;
    p2 = it;
  }

  return p1->soc + ((voltage - p1->voltage) / (p2->voltage - p1->voltage)) *
                       (p2->soc - p1->soc);
}
