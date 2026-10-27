#pragma once
//
// [ARF_PORT 2026-10-28] Minimal .fal plugin loader for the protocol-registry
// plugin. Ported from ARF-Cars' arf_cars_plugins.{c,h} and renamed to
// arf_garage_*. Loads the single am_def.fal that holds the garage/gate registry.
//
#include "../protocols/arf_garage_protocol_plugins.h"

#include <lib/flipper_application/flipper_application.h>

// ".fal" suffix, kept once.
#define ARF_GARAGE_DOT_FAL ".fal"

// Union so the loader can store the opaque entry-point pointer and callers can
// read it back as the typed protocol-plugin descriptor.
typedef union {
    const ArfGarageProtocolPlugin* protocol_plugin;
    const void* plugin_pointer; // DO NOT USE directly; only for load/unload.
} ArfGaragePlugin;

// Load the registry .fal named `plugin_name` (e.g. "am_def"). On success,
// *flipper_application_pointer holds the mapped FlipperApplication and
// plugin_pointer->protocol_plugin points at the descriptor. Returns false on any
// failure (and leaves everything NULL/freed).
bool arf_garage_plugin_load(
    FlipperApplication** flipper_application_pointer,
    ArfGaragePlugin* plugin_pointer,
    const char* plugin_name);

// Unmap the .fal from RAM and clear the pointers.
void arf_garage_plugin_unload(
    FlipperApplication** flipper_application_pointer,
    ArfGaragePlugin* plugin_pointer);
