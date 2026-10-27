#include "protocol_items.h" // IWYU pragma: keep

#include <furi.h>
#include <string.h>

#define TAG "ARFGarageCatalog"

#define SUBGHZ_PROTOCOL_CATALOG_CC1101_REG_MDMCFG2 0x12U
#define SUBGHZ_PROTOCOL_CATALOG_CC1101_MOD_FORMAT_MASK 0x70U
#define SUBGHZ_PROTOCOL_CATALOG_CC1101_MOD_FORMAT_2FSK 0x00U
#define SUBGHZ_PROTOCOL_CATALOG_CC1101_MOD_FORMAT_GFSK 0x10U
#define SUBGHZ_PROTOCOL_CATALOG_CC1101_MOD_FORMAT_ASK_OOK 0x30U
#define SUBGHZ_PROTOCOL_CATALOG_CC1101_MOD_FORMAT_4FSK 0x40U
#define SUBGHZ_PROTOCOL_CATALOG_CC1101_MOD_FORMAT_MSK 0x70U
#define SUBGHZ_PROTOCOL_CATALOG_VAG_FREQUENCY_MIN 434190000UL
#define SUBGHZ_PROTOCOL_CATALOG_VAG_FREQUENCY_MAX 434450000UL

#define SUBGHZ_PROTOCOL_CATALOG_COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

#define SUBGHZ_PROTOCOL_CATALOG_TX_KEY(key) key

/*
 * [ARF_PORT 2026-10-28] Protocol display names for the Protocol Filter UI.
 * Name-only (no decoder references), so this table pulls NO decoder code into
 * the main binary. The real decoders live in the single am_def.fal plugin
 * (plugins/arf_garage_am_plugin.c). These must match the `.name` of every
 * protocol registered in that plugin (EXCLUDING raw/bin_raw, mirroring ARF-Cars).
 */
const char* const arf_garage_protocol_names[] = {
    "GateTX",
    "KeeLoq",
    "Nice FLO",
    "Nice FloR-S",
    "CAME",
    "CAME TWEE",
    "CAME Atomo",
    "Faac SLH",
    "Hormann HSM",
    "Somfy Telis",
    "Somfy Keytis",
    "Princeton",
    "Linear",
    "Security+ 1.0",
    "Security+ 2.0",
    "MegaCode",
    "Holtek",
    "Holtek_HT12X",
    "SMC5326",
    "Alutech AT-4N",
    "Jarolift",
};

const size_t arf_garage_protocol_names_count =
    sizeof(arf_garage_protocol_names) / sizeof(arf_garage_protocol_names[0]);

typedef struct {
    const char* alias;
    const char* canonical_name;
} SubGhzProtocolCatalogAlias;

// [ARF_PORT 2026-10-28] Garage is a single AM/OOK route, so the catalog carries
// no automotive routing entries. It is intentionally empty: every garage protocol
// resolves to the one AM route, and TX never needs preset-remapping (see
// subghz_txrx_tx_start — catalog_find returns NULL for garage protocols, so the
// remapping block is a no-op). The functions below still compile and are callable.
static const SubGhzProtocolCatalogEntry subghz_protocol_catalog[] = {
    {NULL, SubGhzProtocolCatalogRoutePolicyAMDefault, NULL},
};

static const SubGhzProtocolCatalogAlias subghz_protocol_catalog_aliases[] = {
    {NULL, NULL},
};

static bool catalog_string_equal(const char* a, const char* b) {
    return a && b && strcmp(a, b) == 0;
}

static const SubGhzProtocolCatalogEntry*
    catalog_find_canonical(const char* canonical_name) {
    if(!canonical_name || canonical_name[0] == '\0') {
        return NULL;
    }
    for(size_t i = 0; i < SUBGHZ_PROTOCOL_CATALOG_COUNT_OF(subghz_protocol_catalog); i++) {
        if(catalog_string_equal(
               canonical_name, subghz_protocol_catalog[i].canonical_name)) {
            return &subghz_protocol_catalog[i];
        }
    }
    return NULL;
}

static const char* catalog_alias_to_canonical(const char* protocol_name) {
    if(!protocol_name || protocol_name[0] == '\0') {
        return NULL;
    }
    for(size_t i = 0; i < SUBGHZ_PROTOCOL_CATALOG_COUNT_OF(subghz_protocol_catalog_aliases); i++) {
        if(catalog_string_equal(
               protocol_name, subghz_protocol_catalog_aliases[i].alias)) {
            return subghz_protocol_catalog_aliases[i].canonical_name;
        }
    }
    return NULL;
}

const SubGhzProtocolCatalogEntry*
    subghz_protocol_catalog_find(const char* protocol_name) {
    if(!protocol_name || protocol_name[0] == '\0') {
        return NULL;
    }
    const SubGhzProtocolCatalogEntry* entry =
        catalog_find_canonical(protocol_name);
    if(entry) {
        return entry;
    }

    const char* canonical_name = catalog_alias_to_canonical(protocol_name);
    if(canonical_name) {
        return catalog_find_canonical(canonical_name);
    }
    return NULL;
}

const char* subghz_protocol_catalog_canonical_name(const char* protocol_name) {
    const SubGhzProtocolCatalogEntry* entry = subghz_protocol_catalog_find(protocol_name);
    return entry ? entry->canonical_name : protocol_name;
}

bool subghz_protocol_catalog_can_tx(const char* protocol_name) {
    return subghz_protocol_catalog_tx_key(protocol_name) != NULL;
}

const char* subghz_protocol_catalog_tx_key(const char* protocol_name) {
    const SubGhzProtocolCatalogEntry* entry = subghz_protocol_catalog_find(protocol_name);
    return entry ? entry->tx_key : NULL;
}

const char*
    subghz_protocol_catalog_display_name(const char* protocol_name, uint32_t protocol_type) {
    if(!protocol_name) {
        return NULL;
    }

    if(catalog_string_equal(protocol_name, "Suzuki") ||
       catalog_string_equal(protocol_name, "Suzuki V0")) {
        return "Suzuki V0";
    }
    if(catalog_string_equal(protocol_name, "Honda V0")) {
        return "Honda V0";
    }

    const char* canonical_name = subghz_protocol_catalog_canonical_name(protocol_name);
    if(catalog_string_equal(canonical_name, "Kia V0")) {
        if(protocol_type == 2U) {
            return "Suzuki V0";
        }
        if(protocol_type == 3U) {
            return "Honda V0";
        }
    }

    return canonical_name;
}

// [ARF_PORT 2026-10-28] Single-route: always AM default. The arguments are kept
// so the shared plugin-host + TX code can be ported verbatim from ARF-Cars.
SubGhzProtocolCatalogRoute subghz_protocol_catalog_get_route(
    const char* preset_name,
    uint32_t frequency,
    const uint8_t* preset_data,
    size_t preset_data_size,
    const char* protocol_name) {
    UNUSED(preset_name);
    UNUSED(frequency);
    UNUSED(preset_data);
    UNUSED(preset_data_size);
    UNUSED(protocol_name);
    return SubGhzProtocolCatalogRouteAMDefault;
}

const char* subghz_protocol_catalog_get_route_name(SubGhzProtocolCatalogRoute route) {
    switch(route) {
    case SubGhzProtocolCatalogRouteAMDefault:
    default:
        return "AM_DEFAULT";
    }
}

const char* subghz_protocol_catalog_route_to_preset_name(SubGhzProtocolCatalogRoute route) {
    switch(route) {
    case SubGhzProtocolCatalogRouteAMDefault:
    default:
        return "AM650";
    }
}
