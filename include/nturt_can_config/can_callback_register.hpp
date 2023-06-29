/**
 * @file can_callback_register.hpp
 * @author QuantumSpawner jet22854111@gmail.com
 * @brief Utilities for registering callback functions for coderdbc.
 */

#ifndef CAN_CALLBACK_REGISTER_HPP
#define CAN_CALLBACK_REGISTER_HPP

// glibc include
#include <stdint.h>

// stl include
#include <functional>

// nturt include
#include "nturt_can_config.h"

/* typedef -------------------------------------------------------------------*/
/// @brief Typedef for get_tick callback function.
typedef std::function<uint32_t()> get_tick_t;

/// @brief Typedef for send_can_message callback function.
typedef std::function<int(uint32_t, uint8_t, uint8_t*, uint8_t)>
    send_can_message_t;

/// @brief Typedef for fmon_mono callback function.
typedef std::function<void(FrameMonitor_t*, uint32_t)> fmon_mono_t;

/// @brief Typedef for tout_mono callback function.
typedef std::function<void(FrameMonitor_t*, uint32_t, uint32_t)> tout_mono_t;

class CanCallbackRegieter {
 public:
  CanCallbackRegieter(const CanCallbackRegieter&) = delete;

  CanCallbackRegieter& operator=(const CanCallbackRegieter&) = delete;

  /**
   * @brief Register callback function for get_tick.
   *
   * @param get_tick Callback function for get_tick.
   */
  static void register_callback(get_tick_t get_tick);

  /**
   * @brief Register callback function for send_can_message.
   *
   * @param send_can_message Callback function for send_can_message.
   */
  static void register_callback(send_can_message_t send_can_message);

  /**
   * @brief Register callback function for fmon_mono.
   *
   * @param fmon_mono Callback function for fmon_mono.
   */
  static void register_callback(fmon_mono_t fmon_mono);

  /**
   * @brief Register callback function for tout_mono.
   *
   * @param tout_mono Callback function for tout_mono.
   */
  static void register_callback(tout_mono_t tout_mono);

  /// @brief  Reset all callback functions.
  static void reset();

  static uint32_t get_tick();

  static int send_can_message(uint32_t msgid, uint8_t ide, uint8_t* d,
                              uint8_t len);

  static void fmon_mono(FrameMonitor_t* mon, uint32_t msgid);

  static void tout_mono(FrameMonitor_t* mon, uint32_t msgid, uint32_t lastcyc);

 private:
  CanCallbackRegieter();

  static CanCallbackRegieter& get();

  void reset_impl();

  static CanCallbackRegieter instance;

  get_tick_t get_tick_;

  send_can_message_t send_can_message_;

  fmon_mono_t fmon_mono_;

  tout_mono_t tout_mono_;
};

#endif  // CAN_CALLBACK_REGISTER_HPP
