// [ARF_PORT 2026-10-26] FM-Honda1 protocol registry plugin (route FMHonda1).
// Selected on FM presets whose name contains "Honda1"/"Honda 1". Ported from
// ProtoPirate's protopirate_fm_honda1_plugin.c and renamed.
#include "../arf_cars_protocol_plugins.h"

#include "../raw.h"
#include "../bin_raw.h"
#include "../honda_static.h"

#include <lib/flipper_application/flipper_application.h>

static const SubGhzProtocol* const arf_cars_registry_fm_honda1_items[] = {
    &subghz_protocol_raw,
    &subghz_protocol_bin_raw,
    &honda_static_protocol,
};

static const SubGhzProtocolRegistry arf_cars_registry_fm_honda1 = {
    .items = arf_cars_registry_fm_honda1_items,
    .size =
        sizeof(arf_cars_registry_fm_honda1_items) / sizeof(arf_cars_registry_fm_honda1_items[0]),
};

static const ArfCarsProtocolPlugin arf_cars_fm_honda1_plugin = {
    .plugin_name = "fm_honda1",
    .kind = ArfCarsProtocolPluginKindRx,
    .route = SubGhzProtocolCatalogRouteFMHonda1,
    .registry = &arf_cars_registry_fm_honda1,
    .release = NULL,
};

static const FlipperAppPluginDescriptor arf_cars_fm_honda1_plugin_descriptor = {
    .appid = ARF_CARS_PROTOCOL_PLUGIN_APP_ID,
    .ep_api_version = ARF_CARS_PROTOCOL_PLUGIN_API_VERSION,
    .entry_point = &arf_cars_fm_honda1_plugin,
};

const FlipperAppPluginDescriptor* arf_cars_fm_honda1_plugin_ep(void) {
    return &arf_cars_fm_honda1_plugin_descriptor;
}
