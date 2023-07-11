#include "nturt_can_config/can_timeout_monitor.hpp"

// glibc include
#include <stdint.h>

// stl include
#include <unordered_map>

// nturt include
#include "nturt_can_config.h"

/// @brief Map to convert can id to frame name.
static const std::unordered_map<uint32_t, uint32_t> can_id_to_frame_index = {
    {VCU_Status_CANID, VCU_Status_INDEX},
    {INV_Command_Message_CANID, INV_Command_Message_INDEX},
    {FRONT_SENSOR_1_CANID, FRONT_SENSOR_1_INDEX},
    {FRONT_SENSOR_2_CANID, FRONT_SENSOR_2_INDEX},
    {FRONT_SENSOR_3_CANID, FRONT_SENSOR_3_INDEX},

    {REAR_SENSOR_Status_CANID, REAR_SENSOR_Status_INDEX},
    {REAR_SENSOR_1_CANID, REAR_SENSOR_1_INDEX},
    {REAR_SENSOR_2_CANID, REAR_SENSOR_2_INDEX},

    {BMS_Status_CANID, BMS_Status_INDEX},
    {BMS_Cell_Stats_CANID, BMS_Cell_Stats_INDEX},

    {INV_Fast_Info_CANID, INV_Fast_Info_INDEX},
    {INV_Fault_Codes_CANID, INV_Fault_Codes_INDEX},
    {INV_Internal_States_CANID, INV_Internal_States_INDEX},
    {INV_Temperature_Set_2_CANID, INV_Temperature_Set_2_INDEX},
    {INV_Temperature_Set_3_CANID, INV_Temperature_Set_3_INDEX},

    {IMU_Acceleration_CANID, IMU_Acceleration_INDEX},
    {IMU_Angular_Velocity_CANID, IMU_Angular_Velocity_INDEX},
    {IMU_Quaternion_CANID, IMU_Quaternion_INDEX},
};

namespace can_timeout_monior {

uint32_t can_rx_error = 0;

void fmon_mono(FrameMonitor_t* mon, uint32_t msgid) {
  if (mon->cycle_error) {
    mon->cycle_error = false;

    if (can_id_to_frame_index.find(msgid) != can_id_to_frame_index.end()) {
      can_rx_error &= ~can_id_to_frame_index.at(msgid);
    }
  }
}

void tout_mono(FrameMonitor_t* mon, uint32_t msgid, uint32_t lastcyc) {
  if (!mon->cycle_error) {
    mon->cycle_error = true;

    if (can_id_to_frame_index.find(msgid) != can_id_to_frame_index.end()) {
      can_rx_error |= can_id_to_frame_index.at(msgid);
    }
  }
}

}  // namespace can_timeout_monior
