// [ARF_PORT 2026-10-26] FM-F4 protocol registry plugin (route FMF4).
// Selected on FM presets whose name contains "F4". Ported from ProtoPirate's
// protopirate_fm_f4_plugin.c and renamed.
#include "../arf_cars_protocol_plugins.h"

#include "../raw.h"
#include "../bin_raw.h"
#include "../ford_v1.h"
#include "../ford_v2.h"
#include "../ford_v3.h"
#include "../honda_v2.h"

#include <lib/flipper_application/flipper_application.h>

static const SubGhzProtocol* const arf_cars_registry_fm_f4_items[] = {
    &subghz_protocol_raw,
    &subghz_protocol_bin_raw,
    &ford_protocol_v1,
    &ford_protocol_v2,
    &ford_protocol_v3,
    &honda_v2_protocol,
};

static const SubGhzProtocolRegistry arf_cars_registry_fm_f4 = {
    .items = arf_cars_registry_fm_f4_items,
    .size = sizeof(arf_cars_registry_fm_f4_items) / sizeof(arf_cars_registry_fm_f4_items[0]),
};

static const ArfCarsProtocolPlugin arf_cars_fm_f4_plugin = {
    .plugin_name = "fm_f4",
    .kind = ArfCarsProtocolPluginKindRx,
    .route = SubGhzProtocolCatalogRouteFMF4,
    .registry = &arf_cars_registry_fm_f4,
    .release = NULL,
};

static const FlipperAppPluginDescriptor arf_cars_fm_f4_plugin_descriptor = {
    .appid = ARF_CARS_PROTOCOL_PLUGIN_APP_ID,
    .ep_api_version = ARF_CARS_PROTOCOL_PLUGIN_API_VERSION,
    .entry_point = &arf_cars_fm_f4_plugin,
};

const FlipperAppPluginDescriptor* arf_cars_fm_f4_plugin_ep(void) {
    return &arf_cars_fm_f4_plugin_descriptor;
}
