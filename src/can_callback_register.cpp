#include "nturt_can_config/can_callback_register.hpp"

// glibc include
#include <stdint.h>

// stl include
#include <chrono>
#include <iostream>

// nturt include
#include "nturt_can_config.h"
#include "nturt_can_config/can_timeout_monitor.hpp"

/* callback function prototype -----------------------------------------------*/
extern "C" {
uint32_t __get__tick__();
int __send_can_message__(uint32_t msgid, uint8_t ide, uint8_t* d, uint8_t len);
void _FMon_MONO_nturt_can_config(FrameMonitor_t* mon, uint32_t msgid);
void _TOut_MONO_nturt_can_config(FrameMonitor_t* mon, uint32_t msgid,
                                 uint32_t lastcyc);
}

CanCallbackRegieter::CanCallbackRegieter() { get().reset_impl(); }

void CanCallbackRegieter::register_callback(get_tick_t get_tick) {
  get().get_tick_ = get_tick;
}

void CanCallbackRegieter::register_callback(
    send_can_message_t send_can_message) {
  get().send_can_message_ = send_can_message;
}

void CanCallbackRegieter::register_callback(fmon_mono_t fmon_mono) {
  get().fmon_mono_ = fmon_mono;
}

void CanCallbackRegieter::register_callback(tout_mono_t tout_mono) {
  get().tout_mono_ = tout_mono;
}

void CanCallbackRegieter::reset() { get().reset_impl(); }

uint32_t CanCallbackRegieter::get_tick() { return get().get_tick_(); }

int CanCallbackRegieter::send_can_message(uint32_t msgid, uint8_t ide,
                                          uint8_t* d, uint8_t len) {
  return get().send_can_message_(msgid, ide, d, len);
}

void CanCallbackRegieter::fmon_mono(FrameMonitor_t* mon, uint32_t msgid) {
  get().fmon_mono_(mon, msgid);
}

void CanCallbackRegieter::tout_mono(FrameMonitor_t* mon, uint32_t msgid,
                                    uint32_t lastcyc) {
  get().tout_mono_(mon, msgid, lastcyc);
}

CanCallbackRegieter& CanCallbackRegieter::get() { return instance; }

void CanCallbackRegieter::reset_impl() {
  get().get_tick_ = []() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
  };

  get().send_can_message_ = [](uint32_t, uint8_t, uint8_t*, uint8_t) {
    return 1;
  };

  get().fmon_mono_ = can_timeout_monior::fmon_mono;

  get().tout_mono_ = can_timeout_monior::tout_mono;
}

CanCallbackRegieter CanCallbackRegieter::instance;

/* callback function ---------------------------------------------------------*/
// coderdbc callback function for getting current time in ms
uint32_t __get__tick__() { return CanCallbackRegieter::get_tick(); }

// coderdbc callback function for sending can message
int __send_can_message__(uint32_t msgid, uint8_t ide, uint8_t* d, uint8_t len) {
  return CanCallbackRegieter::send_can_message(msgid, ide, d, len);
}

// coderdbc callback function called when receiving a new frame
void _FMon_MONO_nturt_can_config(FrameMonitor_t* mon, uint32_t msgid) {
  CanCallbackRegieter::fmon_mono(mon, msgid);
}

// coderdbc callback function called when reception timeout
void _TOut_MONO_nturt_can_config(FrameMonitor_t* mon, uint32_t msgid,
                                 uint32_t lastcyc) {
  CanCallbackRegieter::tout_mono(mon, msgid, lastcyc);
}
