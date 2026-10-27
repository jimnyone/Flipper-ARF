#pragma once
//
// [ARF_PORT 2026-10-26] Dynamic protocol-registry plugin descriptor.
// Ported from ProtoPirate's protopirate_protocol_plugins.h and renamed for ARF
// Cars. Each route (AM / AM-VAG / FM / FM-F4 / FM-Honda1) is compiled into a
// separate .fal plugin that holds a SMALL protocol registry. At runtime only the
// registry for the active route is loaded into RAM, so the receiver instantiates
// only that subset of decoders instead of all ~41 at once (which caused OOM).
//
#include <lib/subghz/types.h>
#include "protocol_items.h"

#define ARF_CARS_PROTOCOL_PLUGIN_APP_ID "arf_cars_protocols"
#define ARF_CARS_PROTOCOL_PLUGIN_API_VERSION ((uint32_t)sizeof(ArfCarsProtocolPlugin))

typedef enum {
    ArfCarsProtocolPluginKindRx = 0,
} ArfCarsProtocolPluginKind;

typedef struct {
    const char* plugin_name;
    ArfCarsProtocolPluginKind kind;
    // The route this plugin provides; must match the route requested by the host.
    SubGhzProtocolCatalogRoute route;
    // The small registry (only the decoders for this route).
    const SubGhzProtocolRegistry* registry;
    void (*release)(void);
} ArfCarsProtocolPlugin;
