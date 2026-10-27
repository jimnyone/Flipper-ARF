#pragma once
/*
 * ARF-Cars — automotive protocol catalog.
 *
 * This app carries its OWN copies of the protocol sources (under protocols/)
 * because individual `subghz_protocol_*` symbols are NOT exported to FAPs via
 * targets/f7/api_symbols.csv — only the generic lib/subghz blocks API is.
 *
 * To add a protocol:
 *   1. Copy <name>.c/.h from lib/subghz/protocols/ into this folder.
 *   2. Fix its includes: "../blocks/X.h"        -> <lib/subghz/blocks/X.h>
 *                        "../blocks/custom_btn_i.h" -> "custom_btn_i.h"
 *                        "../subghz_keystore_i.h"   -> "subghz_keystore_i.h"
 *                        "../types.h"           -> <lib/subghz/types.h>
 *   3. Add its #include below and an entry in protocol_items.c.
 *   4. Add the .c to `sources` in application.fam.
 */

// [ARF_PORT 2026-10-26] Dynamic protocol-registry plugins.
// The main app no longer owns a big SubGhzProtocolRegistry with pointers to all
// ~41 decoders (that forced every decoder into the binary AND, when passed to
// subghz_receiver_alloc_init, instantiated all of them at once -> out of memory).
// Instead, each ROUTE (AM / AM-VAG / FM / FM-F4 / FM-Honda1) is a separate .fal
// plugin holding a small registry; the host loads only the active one at RX time
// (see helpers/arf_cars_protocol_plugin_host.*). So NO decoder .h is included
// here, and NO arf_cars_protocol_registry is exported anymore.
//
// The Protocol Filter scene still needs to list every protocol name; it uses the
// name-only table below (arf_cars_protocol_names[]), which carries no decoder
// references and therefore pulls nothing into the main binary.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// [ARF_PORT] Full list of protocol display names, for the Protocol Filter UI.
// Keep in sync with the protocols registered across the .fal plugins.
extern const char* const arf_cars_protocol_names[];
extern const size_t arf_cars_protocol_names_count;

typedef enum {
    SubGhzProtocolCatalogRouteAMDefault = 0,
    SubGhzProtocolCatalogRouteAMVag,
    SubGhzProtocolCatalogRouteFMDefault,
    SubGhzProtocolCatalogRouteFMF4,
    SubGhzProtocolCatalogRouteFMHonda1,
} SubGhzProtocolCatalogRoute;

typedef enum {
    SubGhzProtocolCatalogRoutePolicyAMDefault = 0,
    SubGhzProtocolCatalogRoutePolicyAMVag,
    SubGhzProtocolCatalogRoutePolicyFMDefault,
    SubGhzProtocolCatalogRoutePolicyFMF4,
    SubGhzProtocolCatalogRoutePolicyFMHonda1,
    SubGhzProtocolCatalogRoutePolicyByModulation,
} SubGhzProtocolCatalogRoutePolicy;

typedef struct {
    const char* canonical_name;
    SubGhzProtocolCatalogRoutePolicy route_policy;
    const char* tx_key;
} SubGhzProtocolCatalogEntry;

const SubGhzProtocolCatalogEntry*
    subghz_protocol_catalog_find(const char* protocol_name);
const char* subghz_protocol_catalog_canonical_name(const char* protocol_name);
bool subghz_protocol_catalog_can_tx(const char* protocol_name);
const char* subghz_protocol_catalog_tx_key(const char* protocol_name);
const char* subghz_protocol_catalog_display_name(const char* protocol_name, uint32_t protocol_type);
SubGhzProtocolCatalogRoute subghz_protocol_catalog_get_route(
    const char* preset_name,
    uint32_t frequency,
    const uint8_t* preset_data,
    size_t preset_data_size,
    const char* protocol_name);
const char* subghz_protocol_catalog_get_route_name(SubGhzProtocolCatalogRoute route);
const char* subghz_protocol_catalog_route_to_preset_name(SubGhzProtocolCatalogRoute route);

