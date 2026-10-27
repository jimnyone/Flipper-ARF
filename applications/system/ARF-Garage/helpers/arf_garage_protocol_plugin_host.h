#pragma once
//
// [ARF_PORT 2026-10-28] Protocol-registry plugin host. Ported from ARF-Cars'
// arf_cars_protocol_plugin_host.{c,h}, renamed for ARF Garage. Garage is a single
// AM/OOK route, so this always loads the one am_def.fal registry.
//
// Purpose: keep only the garage/gate decoders in RAM, loaded on demand at RX
// time, so the receiver never instantiates all ~61 decoders (which caused OOM).
//
#include "subghz_txrx.h"

// Resolve the route for the current preset, (re)load its registry plugin if
// needed, bind it into the environment, and — if ensure_receiver_ready — make
// sure a SubGhzReceiver exists for that registry.
// Returns true on success. Safe to call repeatedly (idempotent when the route
// has not changed).
bool arf_garage_refresh_protocol_registry(SubGhzTxRx* txrx, bool ensure_receiver_ready);

// Unload the active registry plugin: reset the environment to an empty registry,
// free the receiver's registry binding, and unmap the .fal from RAM.
void arf_garage_unload_protocol_plugin(SubGhzTxRx* txrx);

// [ARF_PORT 2026-10-28] Reset the dynamic decoder state owned by the currently
// loaded plugin (e.g. FAAC SLH programming-mode globals). No-op if no plugin is
// loaded or the plugin exposes no reset_dynamic callback. Called by
// subghz_txrx_reset_dynamic_and_custom_btns().
void arf_garage_reset_dynamic_plugin_state(SubGhzTxRx* txrx);
