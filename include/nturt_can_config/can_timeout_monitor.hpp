/**
 * @file can_timeout_monitor.hpp
 * @author QuantumSpawner jet22854111@gmail.com
 * @brief Utilities for checking can reception timeout for coderdbc.
 */

#ifndef CAN_TIMEOUT_MONITOR
#define CAN_TIMEOUT_MONITOR

// glibc include
#include <stdint.h>

// nturt include
#include "nturt_can_config.h"

/* macro ---------------------------------------------------------------------*/
/* can frame index for rx receive timeout error ------------------------------*/
#define NUM_FRAME_FRONT 5
#define NUM_FRAME_REAR 3
#define NUM_FRAME_BMS 2
#define NUM_FRAME_INVERTER 5
#define NUM_FRAME_IMU 3
#define NUM_FRAME                                                          \
  (NUM_FRAME_FRONT + NUM_FRAME_REAR + NUM_FRAME_BMS + NUM_FRAME_INVERTER + \
   NUM_FRAME_IMU)

// front box frame
#define FRAME_FRONT_BASE 0UL
#define FRAME_FRONT(X) (1UL << (FRAME_FRONT_BASE + X))
#define FRAME_FRONT_MASK ((1UL << (FRAME_FRONT_BASE + NUM_FRAME_FRONT)) - 1UL)

#define VCU_Status_INDEX FRAME_FRONT(0)
#define INV_Command_Message_INDEX FRAME_FRONT(1)
#define FRONT_SENSOR_1_INDEX FRAME_FRONT(2)
#define FRONT_SENSOR_2_INDEX FRAME_FRONT(3)
#define FRONT_SENSOR_3_INDEX FRAME_FRONT(4)

// rear box frame
#define FRAME_REAR_BASE (FRAME_FRONT_BASE + NUM_FRAME_FRONT)
#define FRAME_REAR(X) (1UL << (FRAME_REAR_BASE + X))
#define FRAME_REAR_MASK                                  \
  (((1UL << (FRAME_REAR_BASE + NUM_FRAME_REAR)) - 1UL) - \
   ((1UL << FRAME_REAR_BASE) - 1UL))

#define REAR_SENSOR_Status_INDEX FRAME_REAR(0)
#define REAR_SENSOR_1_INDEX FRAME_REAR(1)
#define REAR_SENSOR_2_INDEX FRAME_REAR(2)

// bms frame
#define FRAME_BMS_BASE (FRAME_REAR_BASE + NUM_FRAME_REAR)
#define FRAME_BMS(X) (1UL << (FRAME_BMS_BASE + X))
#define FRAME_BMS_MASK                                 \
  (((1UL << (FRAME_BMS_BASE + NUM_FRAME_BMS)) - 1UL) - \
   ((1UL << FRAME_BMS_BASE) - 1UL))

#define BMS_Status_INDEX FRAME_BMS(0)
#define BMS_Cell_Stats_INDEX FRAME_BMS(1)

// inverter frame
#define FRAME_INVERTER_BASE (FRAME_BMS_BASE + NUM_FRAME_BMS)
#define FRAME_INVERTER(X) (1UL << (FRAME_INVERTER_BASE + X))
#define FRAME_INVERTER_MASK                                      \
  (((1UL << (FRAME_INVERTER_BASE + NUM_FRAME_INVERTER)) - 1UL) - \
   ((1UL << FRAME_INVERTER_BASE) - 1UL))

#define INV_Fast_Info_INDEX FRAME_INVERTER(0)
#define INV_Fault_Codes_INDEX FRAME_INVERTER(1)
#define INV_Internal_States_INDEX FRAME_INVERTER(2)
#define INV_Temperature_Set_2_INDEX FRAME_INVERTER(3)
#define INV_Temperature_Set_3_INDEX FRAME_INVERTER(4)

// imu frame
#define FRAME_IMU_BASE (FRAME_INVERTER_BASE + NUM_FRAME_INVERTER)
#define FRAME_IMU(X) (1UL << (FRAME_IMU_BASE + X))
#define FRAME_IMU_MASK                                 \
  (((1UL << (FRAME_IMU_BASE + NUM_FRAME_IMU)) - 1UL) - \
   ((1UL << FRAME_IMU_BASE) - 1UL))

#define IMU_Acceleration_INDEX FRAME_IMU(0)
#define IMU_Angular_Velocity_INDEX FRAME_IMU(1)
#define IMU_Quaternion_INDEX FRAME_IMU(2)

namespace can_timeout_monior {

/* Exported variable ---------------------------------------------------------*/
/// @brief CAN frame reception error, where each bit represent timeout frame.
extern uint32_t can_rx_error;

/* Exported function ---------------------------------------------------------*/
/// @brief Callback function called when receiving a new frame, clears the bit
/// on can_rx_error if the corresponding bit is set.
void fmon_mono(FrameMonitor_t* mon, uint32_t msgid);

/// @brief Coderdbc callback function called when reception timeout, sets the
/// bit on can_rx_error of the corresponding frame.
void tout_mono(FrameMonitor_t* mon, uint32_t msgid, uint32_t lastcyc);

}  // namespace can_timeout_monior

#endif  // CAN_TIMEOUT_MONITOR
