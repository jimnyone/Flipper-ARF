#include "arf_cars_plugins.h"

#include <furi.h>
#include <storage/storage.h>
#include <loader/firmware_api/firmware_api.h>

#define TAG "ArfCarsPlugins"

#define ARF_CARS_PLUGIN_PATH_BUFFER_SIZE 128

// Preload + map a .fal, validate it is a plugin with the expected appid and API
// version, and return its descriptor. Frees the FlipperApplication on any error.
static const FlipperAppPluginDescriptor* arf_cars_load_plugin_fal(
    FlipperApplication** fal_app,
    const char* plugin_path,
    const char* application_id,
    uint32_t api_version) {
    const FlipperAppPluginDescriptor* app_descriptor = NULL;
    bool ok = false;

    do {
        FlipperApplicationPreloadStatus preload_res =
            flipper_application_preload(*fal_app, plugin_path);
        if(preload_res != FlipperApplicationPreloadStatusSuccess) {
            FURI_LOG_E(
                TAG,
                "Failed to preload plugin %s (%s)",
                plugin_path,
                preload_res == FlipperApplicationPreloadStatusNotEnoughMemory ?
                    "out of memory" :
                    "invalid or stale");
            break;
        }

        if(!flipper_application_is_plugin(*fal_app)) {
            FURI_LOG_E(TAG, "Plugin file is not a library: %s", plugin_path);
            break;
        }

        FlipperApplicationLoadStatus load_status = flipper_application_map_to_memory(*fal_app);
        if(load_status != FlipperApplicationLoadStatusSuccess) {
            FURI_LOG_E(TAG, "Failed to map plugin to memory: %s", plugin_path);
            break;
        }

        app_descriptor = flipper_application_plugin_get_descriptor(*fal_app);
        if(strcmp(app_descriptor->appid, application_id) != 0) {
            FURI_LOG_E(TAG, "Plugin appid mismatch (want %s)", application_id);
            break;
        }

        if(app_descriptor->ep_api_version != api_version) {
            FURI_LOG_E(TAG, "Plugin API version mismatch (want %lu)", (unsigned long)api_version);
            break;
        }

        ok = true;
    } while(false);

    if(ok) {
        return app_descriptor;
    }

    if(*fal_app) {
        flipper_application_free(*fal_app);
        *fal_app = NULL;
    }
    return NULL;
}

bool arf_cars_plugin_load(
    FlipperApplication** flipper_application_pointer,
    ArfCarsPlugin* plugin_pointer,
    const char* plugin_name) {
    furi_check(flipper_application_pointer);
    furi_check(plugin_pointer);

    // Already loaded? (defensive; the host unloads before loading a new one)
    if(plugin_pointer->plugin_pointer) {
        return true;
    }

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperApplication* fal_app = flipper_application_alloc(storage, firmware_api_interface);

    // The .fal files are embedded as app file-assets, extracted to the app's
    // assets dir at install time: apps_assets/<appid>/plugins/<name>.fal
    char buffer[ARF_CARS_PLUGIN_PATH_BUFFER_SIZE] = {0};
    snprintf(
        buffer,
        sizeof(buffer),
        "%s%s%s",
        APP_ASSETS_PATH("plugins/"),
        plugin_name,
        ARF_CARS_DOT_FAL);

    FURI_LOG_D(TAG, "Loading protocol plugin: %s", buffer);

    const FlipperAppPluginDescriptor* app_descriptor = arf_cars_load_plugin_fal(
        &fal_app,
        buffer,
        ARF_CARS_PROTOCOL_PLUGIN_APP_ID,
        ARF_CARS_PROTOCOL_PLUGIN_API_VERSION);

    bool ok = false;
    if(app_descriptor) {
        plugin_pointer->plugin_pointer = app_descriptor->entry_point;
        ok = true;
    } else if(fal_app) {
        flipper_application_free(fal_app);
        fal_app = NULL;
    }

    *flipper_application_pointer = fal_app;
    furi_record_close(RECORD_STORAGE);
    return ok;
}

void arf_cars_plugin_unload(
    FlipperApplication** flipper_application_pointer,
    ArfCarsPlugin* plugin_pointer) {
    furi_check(flipper_application_pointer);
    furi_check(plugin_pointer);

    if(*flipper_application_pointer) {
        // Unmaps the .fal from RAM (frees its decoders' code+data).
        flipper_application_free(*flipper_application_pointer);
        *flipper_application_pointer = NULL;
    }
    plugin_pointer->plugin_pointer = NULL;
}
