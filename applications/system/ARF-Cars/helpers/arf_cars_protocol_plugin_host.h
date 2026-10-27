#pragma once
//
// [ARF_PORT 2026-10-26] Protocol-registry plugin host. Ported from ProtoPirate's
// protopirate_protocol_plugin_host.{c,h}, adapted to ARF Cars (which drives the
// radio through SubGhzTxRx directly instead of a ProtoPirateApp wrapper).
//
// Purpose: keep only the decoders for the ACTIVE route in RAM. The active route
// is derived from the current preset (AM/FM from CC1101 MDMCFG2) + frequency band
// (VAG) + preset name (F4/Honda1) via subghz_protocol_catalog_get_route().
// When the route changes, the old .fal (and its receiver) are torn down and only
// the new one is loaded, so the receiver never instantiates all ~41 decoders.
//
#include "subghz_txrx.h"

// Resolve the route for the current preset, (re)load its registry plugin if
// needed, bind it into the environment, and — if ensure_receiver_ready — make
// sure a SubGhzReceiver exists for that registry.
// Returns true on success. Safe to call repeatedly (idempotent when the route
// has not changed).
bool arf_cars_refresh_protocol_registry(SubGhzTxRx* txrx, bool ensure_receiver_ready);

// Unload the active registry plugin: reset the environment to an empty registry,
// free the receiver's registry binding, and unmap the .fal from RAM.
void arf_cars_unload_protocol_plugin(SubGhzTxRx* txrx);
