#pragma once
/*
 * ARF-Garage — garage door / gate / remote protocol catalog.
 *
 * This app carries its OWN copies of the protocol sources (under protocols/)
 * because individual `subghz_protocol_*` symbols are NOT exported to FAPs via
 * targets/f7/api_symbols.csv — only the generic lib/subghz blocks API is.
 *
 * To add a protocol:
 *   1. Copy <name>.c/.h from lib/subghz/protocols/ into this folder.
 *   2. Fix its includes: "../blocks/X.h"             -> <lib/subghz/blocks/X.h>
 *                        "../blocks/custom_btn_i.h"  -> "custom_btn_i.h"
 *                        "../subghz_keystore_i.h"    -> "subghz_keystore_i.h"
 *                        "base.h"                    -> <lib/subghz/protocols/base.h>
 *   3. Add its #include below and an entry in protocol_items.c.
 *   4. Add the .c to `sources` in application.fam.
 */

// [ARF_PORT 2026-10-28] The decoder headers are NOT included here anymore. The
// decoders used to be compiled into the main binary and instantiated all at once
// by the receiver, which caused OOM when opening the app. They now live in a
// single AM route plugin (plugins/arf_garage_am_plugin.c -> am_def.fal) that is
// loaded on demand. The main binary keeps only a name-only table (below) used by
// the Protocol Filter scene, plus raw/bin_raw/custom_btn which the UI needs.
// NO <lib/subghz/registry.h> or decoder headers here — a name-only table pulls in
// no decoder code.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// [ARF_PORT 2026-10-28] Name-only catalog for the Protocol Filter ("Protocol
// List") scene. These are the garage/gate protocols shipped in the am_def.fal
// plugin. Strings only — NO decoder references — so the main binary never pulls
// in (and never instantiates) any decoder. Keep this in sync with the registry
// declared in protocols/plugins/arf_garage_am_plugin.c.
extern const char* const arf_garage_protocol_names[];
extern const size_t arf_garage_protocol_names_count;

// [ARF_PORT 2026-10-28] Garage is (practically) all AM/OOK, so there is a single
// route. The enum is kept (one value) so the shared plugin host code can be
// ported verbatim from ARF-Cars.
typedef enum {
    SubGhzProtocolCatalogRouteAMDefault = 0,
} SubGhzProtocolCatalogRoute;

typedef enum {
    SubGhzProtocolCatalogRoutePolicyAMDefault = 0,
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

