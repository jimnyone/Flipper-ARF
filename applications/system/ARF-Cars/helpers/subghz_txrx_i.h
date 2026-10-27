#pragma once

#include "subghz_txrx.h"

// [ARF_PORT 2026-10-26] Dynamic protocol-registry plugin state (see
// helpers/arf_cars_protocol_plugin_host.*). Only the registry for the active
// route lives in RAM at a time.
#include "arf_cars_plugins.h"
#include <lib/flipper_application/flipper_application.h>

struct SubGhzTxRx {
    SubGhzWorker* worker;

    SubGhzEnvironment* environment;
    SubGhzReceiver* receiver;
    SubGhzTransmitter* transmitter;
    SubGhzProtocolDecoderBase* decoder_result;
    FlipperFormat* fff_data;

    SubGhzRadioPreset* preset;
    SubGhzSetting* setting;

    // [ARF_PORT] Active protocol-registry plugin (the loaded .fal), the route it
    // provides, and the registry currently bound into the environment.
    FlipperApplication* protocol_plugin_flipper_application;
    ArfCarsPlugin running_plugin;
    SubGhzProtocolCatalogRoute protocol_registry_route;
    const SubGhzProtocolRegistry* protocol_registry;
    bool protocol_registry_route_valid; // false until a route has been loaded once

    // [ARF_PORT] Saved RX callback/context, so it can be re-applied whenever the
    // receiver is recreated for a new route.
    SubGhzReceiverCallback rx_callback;
    void* rx_callback_context;

    uint8_t hopper_timeout;
    uint8_t hopper_idx_frequency;
    bool is_database_loaded;
    SubGhzHopperState hopper_state;

    uint8_t preset_hopper_timeout;
    size_t preset_hopper_idx;
    SubGhzPresetHopperState preset_hopper_state;

    SubGhzTxRxState txrx_state;
    SubGhzSpeakerState speaker_state;
    const SubGhzDevice* radio_device;
    SubGhzRadioDeviceType radio_device_type;

    SubGhzTxRxNeedSaveCallback need_save_callback;
    void* need_save_context;

    bool debug_pin_state;
};
