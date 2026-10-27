// [ARF_PORT 2026-10-26] AM-VAG protocol registry plugin (route AMVag).
// Selected when receiving on AM within the VAG frequency band (434.19-434.45 MHz).
// Ported from ProtoPirate's protopirate_am_vag_plugin.c and renamed.
#include "../arf_cars_protocol_plugins.h"

#include "../raw.h"
#include "../bin_raw.h"
#include "../vag.h"

#include <lib/flipper_application/flipper_application.h>

static const SubGhzProtocol* const arf_cars_registry_am_vag_items[] = {
    &subghz_protocol_raw,
    &subghz_protocol_bin_raw,
    &subghz_protocol_vag,
};

static const SubGhzProtocolRegistry arf_cars_registry_am_vag = {
    .items = arf_cars_registry_am_vag_items,
    .size = sizeof(arf_cars_registry_am_vag_items) / sizeof(arf_cars_registry_am_vag_items[0]),
};

static const ArfCarsProtocolPlugin arf_cars_am_vag_plugin = {
    .plugin_name = "am_vag",
    .kind = ArfCarsProtocolPluginKindRx,
    .route = SubGhzProtocolCatalogRouteAMVag,
    .registry = &arf_cars_registry_am_vag,
    .release = NULL,
};

static const FlipperAppPluginDescriptor arf_cars_am_vag_plugin_descriptor = {
    .appid = ARF_CARS_PROTOCOL_PLUGIN_APP_ID,
    .ep_api_version = ARF_CARS_PROTOCOL_PLUGIN_API_VERSION,
    .entry_point = &arf_cars_am_vag_plugin,
};

const FlipperAppPluginDescriptor* arf_cars_am_vag_plugin_ep(void) {
    return &arf_cars_am_vag_plugin_descriptor;
}
