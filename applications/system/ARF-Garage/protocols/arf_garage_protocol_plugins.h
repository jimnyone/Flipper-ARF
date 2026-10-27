#pragma once
//
// [ARF_PORT 2026-10-28] Dynamic protocol-registry plugin descriptor.
// Ported from ARF-Cars' arf_cars_protocol_plugins.h and renamed for ARF Garage.
// Garage is a single AM/OOK route, so there is only ONE .fal plugin (am_def)
// holding the small garage/gate protocol registry. At runtime only that registry
// is loaded into RAM, so the receiver instantiates only that subset of decoders
// instead of all ~61 at once (which caused OOM).
//
#include <lib/subghz/types.h>
#include "protocol_items.h"

#define ARF_GARAGE_PROTOCOL_PLUGIN_APP_ID "arf_garage_protocols"
#define ARF_GARAGE_PROTOCOL_PLUGIN_API_VERSION ((uint32_t)sizeof(ArfGarageProtocolPlugin))

typedef enum {
    ArfGarageProtocolPluginKindRx = 0,
} ArfGarageProtocolPluginKind;

typedef struct {
    const char* plugin_name;
    ArfGarageProtocolPluginKind kind;
    // The route this plugin provides; must match the route requested by the host.
    SubGhzProtocolCatalogRoute route;
    // The small registry (only the decoders for this route).
    const SubGhzProtocolRegistry* registry;
    void (*release)(void);
    // [ARF_PORT 2026-10-28] Reset the per-decoder dynamic state that lives inside
    // this plugin (e.g. FAAC SLH programming-mode globals). Called by the main app
    // from subghz_txrx_reset_dynamic_and_custom_btns() when switching between
    // signals/buttons, because those decoder statics are no longer linked into the
    // main binary. May be NULL if the plugin has no such state.
    void (*reset_dynamic)(void);
} ArfGarageProtocolPlugin;
