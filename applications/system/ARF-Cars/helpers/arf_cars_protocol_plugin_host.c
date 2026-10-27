#include "arf_cars_protocol_plugin_host.h"
#include "subghz_txrx_i.h"
#include "../protocols/protocol_items.h"
#include "../protocols/arf_cars_protocol_plugins.h"

#include <furi.h>
#include <string.h>

#define TAG "ArfCarsProtoPlugin"

// Map a route to the .fal plugin file name (without extension). Must match the
// appids declared in application.fam.
static const char* arf_cars_get_registry_plugin_name(SubGhzProtocolCatalogRoute route) {
    switch(route) {
    case SubGhzProtocolCatalogRouteAMVag:
        return "am_vag";
    case SubGhzProtocolCatalogRouteFMDefault:
        return "fm_def";
    case SubGhzProtocolCatalogRouteFMF4:
        return "fm_f4";
    case SubGhzProtocolCatalogRouteFMHonda1:
        return "fm_honda1";
    case SubGhzProtocolCatalogRouteAMDefault:
    default:
        return "am_def";
    }
}

static const SubGhzProtocolRegistry arf_cars_empty_protocol_registry = {
    .items = NULL,
    .size = 0,
};

// Tear down the receiver (destroys the instantiated decoders) before the .fal
// that owns them is unmapped. Keeps the worker alive but detaches the receiver.
static void arf_cars_rx_teardown_for_registry_switch(SubGhzTxRx* txrx) {
    if(txrx->worker && subghz_worker_is_running(txrx->worker)) {
        subghz_worker_stop(txrx->worker);
    }
    if(txrx->receiver) {
        // Detach from worker and drop the RX callback before freeing.
        subghz_worker_set_context(txrx->worker, NULL);
        subghz_receiver_set_rx_callback(txrx->receiver, NULL, NULL);
        subghz_receiver_free(txrx->receiver);
        txrx->receiver = NULL;
    }
}

void arf_cars_unload_protocol_plugin(SubGhzTxRx* txrx) {
    furi_check(txrx);

    // Point the environment at an empty registry so it never references memory
    // inside the .fal we are about to unmap.
    if(txrx->environment) {
        subghz_environment_set_protocol_registry(
            txrx->environment, &arf_cars_empty_protocol_registry);
    }
    txrx->protocol_registry = NULL;

    if(txrx->running_plugin.protocol_plugin && txrx->running_plugin.protocol_plugin->release) {
        txrx->running_plugin.protocol_plugin->release();
    }

    arf_cars_plugin_unload(
        &txrx->protocol_plugin_flipper_application, &txrx->running_plugin);
    txrx->protocol_registry_route_valid = false;
}

// Load (or reuse) the registry .fal for `route` and return its small registry.
static bool arf_cars_ensure_protocol_registry_plugin(
    SubGhzTxRx* txrx,
    SubGhzProtocolCatalogRoute route,
    const SubGhzProtocolRegistry** registry) {
    *registry = NULL;

    if(!txrx->environment) {
        FURI_LOG_E(TAG, "No environment");
        return false;
    }

    // Fast path: the right plugin is already loaded.
    if(txrx->running_plugin.protocol_plugin &&
       txrx->running_plugin.protocol_plugin->kind == ArfCarsProtocolPluginKindRx &&
       txrx->running_plugin.protocol_plugin->registry && txrx->protocol_registry_route_valid &&
       txrx->protocol_registry_route == route) {
        *registry = txrx->running_plugin.protocol_plugin->registry;
        return true;
    }

    // Different route: unload whatever is currently loaded.
    if(txrx->running_plugin.protocol_plugin) {
        arf_cars_unload_protocol_plugin(txrx);
    }

    const char* plugin_name = arf_cars_get_registry_plugin_name(route);
    if(!arf_cars_plugin_load(
           &txrx->protocol_plugin_flipper_application, &txrx->running_plugin, plugin_name)) {
        FURI_LOG_E(TAG, "Failed to load RX protocol plugin %s", plugin_name);
        return false;
    }

    if(txrx->running_plugin.protocol_plugin->kind != ArfCarsProtocolPluginKindRx) {
        FURI_LOG_E(TAG, "Plugin %s is not an RX plugin", plugin_name);
        arf_cars_unload_protocol_plugin(txrx);
        return false;
    }

    if(txrx->running_plugin.protocol_plugin->route != route) {
        FURI_LOG_E(
            TAG,
            "Plugin route mismatch (expected %d got %d)",
            (int)route,
            (int)txrx->running_plugin.protocol_plugin->route);
        arf_cars_unload_protocol_plugin(txrx);
        return false;
    }

    txrx->protocol_registry_route = route;
    txrx->protocol_registry_route_valid = true;
    *registry = txrx->running_plugin.protocol_plugin->registry;
    return true;
}

bool arf_cars_refresh_protocol_registry(SubGhzTxRx* txrx, bool ensure_receiver_ready) {
    furi_check(txrx);
    if(!txrx->environment || !txrx->preset) {
        return true;
    }

    const char* preset_name = furi_string_get_cstr(txrx->preset->name);
    SubGhzProtocolCatalogRoute route = subghz_protocol_catalog_get_route(
        preset_name,
        txrx->preset->frequency,
        txrx->preset->data,
        txrx->preset->data_size,
        NULL);

    bool route_changed = !txrx->running_plugin.protocol_plugin ||
                         (txrx->running_plugin.protocol_plugin->kind !=
                          ArfCarsProtocolPluginKindRx) ||
                         !txrx->protocol_registry_route_valid ||
                         (txrx->protocol_registry_route != route);

    if(route_changed) {
        arf_cars_rx_teardown_for_registry_switch(txrx);
        arf_cars_unload_protocol_plugin(txrx);
    } else if(ensure_receiver_ready && !txrx->receiver) {
        arf_cars_rx_teardown_for_registry_switch(txrx);
        arf_cars_unload_protocol_plugin(txrx);
    }

    const SubGhzProtocolRegistry* registry = NULL;
    if(!arf_cars_ensure_protocol_registry_plugin(txrx, route, &registry) || !registry) {
        FURI_LOG_E(TAG, "Failed to resolve registry plugin for route %d", (int)route);
        return false;
    }

    const bool registry_already_bound = (txrx->protocol_registry == registry);
    if(!registry_already_bound) {
        FURI_LOG_I(
            TAG, "Using route %d registry (%zu protocols)", (int)route, registry->size);
        subghz_environment_set_protocol_registry(txrx->environment, registry);
        txrx->protocol_registry = registry;
    }

    if(!ensure_receiver_ready) {
        return true;
    }

    if(txrx->receiver) {
        return true;
    }

    txrx->receiver = subghz_receiver_alloc_init(txrx->environment);
    if(!txrx->receiver) {
        FURI_LOG_E(TAG, "Failed to allocate receiver for route %d", (int)route);
        return false;
    }

    subghz_receiver_set_filter(txrx->receiver, SubGhzProtocolFlag_Decodable);

    // Re-attach the receiver to the worker and restore the saved RX callback so
    // the scene keeps receiving after the receiver was recreated for a new route.
    subghz_worker_set_context(txrx->worker, txrx->receiver);
    if(txrx->rx_callback) {
        subghz_receiver_set_rx_callback(
            txrx->receiver, txrx->rx_callback, txrx->rx_callback_context);
    }
    return true;
}
