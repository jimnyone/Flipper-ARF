#pragma once
//
// [ARF_PORT 2026-10-26] Minimal .fal plugin loader for the protocol-registry
// plugins. Ported from ProtoPirate's protopirate_plugins.{c,h} but trimmed to
// ONLY the registry-loading path (ProtoPirate also loads config/about/emulate/
// bruteforce plugins — ARF Cars does not need those; its BF/emulate come from the
// native Sub-GHz scenes it copied). Renamed to arf_cars_*.
//
#include "../protocols/arf_cars_protocol_plugins.h"

#include <lib/flipper_application/flipper_application.h>

// ".fal" suffix, kept once.
#define ARF_CARS_DOT_FAL ".fal"

// Union so the loader can store the opaque entry-point pointer and callers can
// read it back as the typed protocol-plugin descriptor.
typedef union {
    const ArfCarsProtocolPlugin* protocol_plugin;
    const void* plugin_pointer; // DO NOT USE directly; only for load/unload.
} ArfCarsPlugin;

// Load the registry .fal named `plugin_name` (e.g. "am_def"). On success,
// *flipper_application_pointer holds the mapped FlipperApplication and
// plugin_pointer->protocol_plugin points at the descriptor. Returns false on any
// failure (and leaves everything NULL/freed).
bool arf_cars_plugin_load(
    FlipperApplication** flipper_application_pointer,
    ArfCarsPlugin* plugin_pointer,
    const char* plugin_name);

// Unmap the .fal from RAM and clear the pointers.
void arf_cars_plugin_unload(
    FlipperApplication** flipper_application_pointer,
    ArfCarsPlugin* plugin_pointer);
