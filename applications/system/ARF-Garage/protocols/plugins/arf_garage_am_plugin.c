// [ARF_PORT 2026-10-28] AM-default protocol registry plugin (route AMDefault).
// Ported from ARF-Cars' arf_cars_am_plugin.c and renamed. Garage is a single
// AM/OOK route, so this is the ONE and only registry plugin: it holds the
// ~21 most-common garage/gate decoders. Loaded on demand so only this subset is
// in RAM when receiving. raw + bin_raw are always present.
#include "../arf_garage_protocol_plugins.h"

#include "../raw.h"
#include "../bin_raw.h"
#include "../gate_tx.h"
#include "../keeloq.h"
#include "../nice_flo.h"
#include "../nice_flor_s.h"
#include "../came.h"
#include "../came_twee.h"
#include "../came_atomo.h"
#include "../faac_slh.h"
#include "../hormann.h"
#include "../somfy_telis.h"
#include "../somfy_keytis.h"
#include "../princeton.h"
#include "../linear.h"
#include "../secplus_v1.h"
#include "../secplus_v2.h"
#include "../megacode.h"
#include "../holtek.h"
#include "../holtek_ht12x.h"
#include "../smc5326.h"
#include "../alutech_at_4n.h"
#include "../jarolift.h"

#include <lib/flipper_application/flipper_application.h>

static const SubGhzProtocol* const arf_garage_registry_am_items[] = {
    &subghz_protocol_raw,
    &subghz_protocol_bin_raw,
    &subghz_protocol_gate_tx,
    &subghz_protocol_keeloq,
    &subghz_protocol_nice_flo,
    &subghz_protocol_nice_flor_s,
    &subghz_protocol_came,
    &subghz_protocol_came_twee,
    &subghz_protocol_came_atomo,
    &subghz_protocol_faac_slh,
    &subghz_protocol_hormann,
    &subghz_protocol_somfy_telis,
    &subghz_protocol_somfy_keytis,
    &subghz_protocol_princeton,
    &subghz_protocol_linear,
    &subghz_protocol_secplus_v1,
    &subghz_protocol_secplus_v2,
    &subghz_protocol_megacode,
    &subghz_protocol_holtek,
    &subghz_protocol_holtek_th12x,
    &subghz_protocol_smc5326,
    &subghz_protocol_alutech_at_4n,
    &subghz_protocol_jarolift,
};

static const SubGhzProtocolRegistry arf_garage_registry_am = {
    .items = arf_garage_registry_am_items,
    .size = sizeof(arf_garage_registry_am_items) / sizeof(arf_garage_registry_am_items[0]),
};

// [ARF_PORT 2026-10-28] Reset the dynamic decoder state that lives in this .fal.
// Currently only FAAC SLH keeps programming-mode globals that must be cleared when
// the main app switches between signals/buttons (see faac_slh_reset_prog_mode()).
static void arf_garage_am_plugin_reset_dynamic(void) {
    faac_slh_reset_prog_mode();
}

static const ArfGarageProtocolPlugin arf_garage_am_plugin = {
    .plugin_name = "am_def",
    .kind = ArfGarageProtocolPluginKindRx,
    .route = SubGhzProtocolCatalogRouteAMDefault,
    .registry = &arf_garage_registry_am,
    .release = NULL,
    .reset_dynamic = arf_garage_am_plugin_reset_dynamic,
};

static const FlipperAppPluginDescriptor arf_garage_am_plugin_descriptor = {
    .appid = ARF_GARAGE_PROTOCOL_PLUGIN_APP_ID,
    .ep_api_version = ARF_GARAGE_PROTOCOL_PLUGIN_API_VERSION,
    .entry_point = &arf_garage_am_plugin,
};

const FlipperAppPluginDescriptor* arf_garage_am_plugin_ep(void) {
    return &arf_garage_am_plugin_descriptor;
}
