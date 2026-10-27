// [ARF_PORT 2026-10-26] AM-default protocol registry plugin (route AMDefault).
// Ported from ProtoPirate's protopirate_am_plugin.c and renamed. Holds only the
// AM automotive decoders; loaded on demand so only this subset is in RAM when
// receiving on an AM/OOK preset. raw + bin_raw are always present.
#include "../arf_cars_protocol_plugins.h"

#include "../raw.h"
#include "../bin_raw.h"
#include "../keeloq.h" // [ARF_PORT] generic KeeLoq decoder (AM + FM routes)
#include "../chrysler.h"
#include "../fiat_v0.h"
#include "../fiat_v1.h"
#include "../fiat_v2.h"
#include "../ford_v0.h"
#include "../ford_v3.h"
#include "../honda_v1.h"
#include "../kia_v1.h"
#include "../kia_v2.h"
#include "../mazda_v0.h"
#include "../psa.h"
#include "../renault_v0.h"
#include "../renault_v1.h"
#include "../subaru.h"
#include "../star_line.h"
// ARF extras (AM) not present in ProtoPirate:
#include "../gm_v0.h"
#include "../gm_v1.h"
#include "../toyota_v0.h"
#include "../toyota_v1.h"
#include "../opel_v1.h"
#include "../bmw.h"
#include "../sheriff_cfm.h"
#include "../suzuki.h"
#include "../mazda_v1.h"
#include "../mazda_v2.h"

#include <lib/flipper_application/flipper_application.h>

static const SubGhzProtocol* const arf_cars_registry_am_items[] = {
    &subghz_protocol_raw,
    &subghz_protocol_bin_raw,
    &subghz_protocol_keeloq, // [ARF_PORT] generic KeeLoq (also in fm_def)
    &subghz_protocol_chrysler,
    &fiat_protocol_v0,
    &fiat_v1_protocol,
    &fiat_v2_protocol,
    &subghz_protocol_ford_v0,
    &ford_protocol_v3,
    &honda_v1_protocol,
    &subghz_protocol_kia_v1,
    &subghz_protocol_kia_v2,
    &subghz_protocol_mazda_v0,
    &subghz_protocol_psa,
    &renault_v0_protocol,
    &renault_v1_protocol,
    &subghz_protocol_subaru,
    &subghz_protocol_star_line,
    // ARF extras (AM)
    &subghz_protocol_gm_v0,
    &subghz_protocol_gm_v1,
    &subghz_protocol_toyota_v0,
    &subghz_protocol_toyota_v1,
    &subghz_protocol_opel_v1,
    &subghz_protocol_bmw,
    &subghz_protocol_sheriff_cfm,
    &subghz_protocol_suzuki,
    &subghz_protocol_mazda_v1,
    &subghz_protocol_mazda_v2,
};

static const SubGhzProtocolRegistry arf_cars_registry_am = {
    .items = arf_cars_registry_am_items,
    .size = sizeof(arf_cars_registry_am_items) / sizeof(arf_cars_registry_am_items[0]),
};

static const ArfCarsProtocolPlugin arf_cars_am_plugin = {
    .plugin_name = "am_def",
    .kind = ArfCarsProtocolPluginKindRx,
    .route = SubGhzProtocolCatalogRouteAMDefault,
    .registry = &arf_cars_registry_am,
    .release = NULL,
};

static const FlipperAppPluginDescriptor arf_cars_am_plugin_descriptor = {
    .appid = ARF_CARS_PROTOCOL_PLUGIN_APP_ID,
    .ep_api_version = ARF_CARS_PROTOCOL_PLUGIN_API_VERSION,
    .entry_point = &arf_cars_am_plugin,
};

const FlipperAppPluginDescriptor* arf_cars_am_plugin_ep(void) {
    return &arf_cars_am_plugin_descriptor;
}
