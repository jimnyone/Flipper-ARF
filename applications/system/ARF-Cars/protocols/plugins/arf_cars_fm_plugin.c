// [ARF_PORT 2026-10-26] FM-default protocol registry plugin (route FMDefault).
// Ported from ProtoPirate's protopirate_fm_plugin.c and renamed. Holds the FM
// (2-FSK) automotive decoders. raw + bin_raw are always present.
#include "../arf_cars_protocol_plugins.h"

#include "../raw.h"
#include "../bin_raw.h"
#include "../keeloq.h" // [ARF_PORT] generic KeeLoq decoder (AM + FM routes)
#include "../scher_khan.h"
#include "../kia_v0.h"
#include "../kia_v3_v4.h"
#include "../kia_v5.h"
#include "../kia_v6.h"
#include "../kia_v7.h"
#include "../mazda_v0.h"
#include "../psa.h"
#include "../renault_v1.h"
// ARF extras (FM) not present in ProtoPirate:
#include "../mercedes_v0.h"
#include "../mercedes_v1.h"
#include "../mitsubishi_v0.h"

#include <lib/flipper_application/flipper_application.h>

static const SubGhzProtocol* const arf_cars_registry_fm_items[] = {
    &subghz_protocol_raw,
    &subghz_protocol_bin_raw,
    &subghz_protocol_keeloq, // [ARF_PORT] generic KeeLoq (also in am_def)
    &subghz_protocol_scher_khan,
    &subghz_protocol_kia_v0,
    &subghz_protocol_kia_v3_v4,
    &subghz_protocol_kia_v5,
    &subghz_protocol_kia_v6,
    &subghz_protocol_kia_v7,
    &subghz_protocol_mazda_v0,
    &subghz_protocol_psa,
    &renault_v1_protocol,
    // ARF extras (FM)
    &subghz_protocol_mercedes_v0,
    &subghz_protocol_mercedes_v1,
    &subghz_protocol_mitsubishi_v0,
};

static const SubGhzProtocolRegistry arf_cars_registry_fm = {
    .items = arf_cars_registry_fm_items,
    .size = sizeof(arf_cars_registry_fm_items) / sizeof(arf_cars_registry_fm_items[0]),
};

static const ArfCarsProtocolPlugin arf_cars_fm_plugin = {
    .plugin_name = "fm_def",
    .kind = ArfCarsProtocolPluginKindRx,
    .route = SubGhzProtocolCatalogRouteFMDefault,
    .registry = &arf_cars_registry_fm,
    .release = NULL,
};

static const FlipperAppPluginDescriptor arf_cars_fm_plugin_descriptor = {
    .appid = ARF_CARS_PROTOCOL_PLUGIN_APP_ID,
    .ep_api_version = ARF_CARS_PROTOCOL_PLUGIN_API_VERSION,
    .entry_point = &arf_cars_fm_plugin,
};

const FlipperAppPluginDescriptor* arf_cars_fm_plugin_ep(void) {
    return &arf_cars_fm_plugin_descriptor;
}
