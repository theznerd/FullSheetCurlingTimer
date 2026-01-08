#include "gatt/gatt_defs.hpp"
#include "host/ble_hs.h"
#include "esp_log.h"
#include <cstring>

#include "os/os_mbuf.h"

#include "status/fsct_status.hpp"

// External reference to main applicaiton queue
extern QueueHandle_t app_main_queue; // was previously called main_event_queue

namespace FSCT {

// TODO: Implement the logic to handle each characteristic access
//       Right now, these are just stubs.

// Handler for Command Characteristic (Write)
int gatt_svr_chr_access_command(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg) {
    if(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        if (ctxt->om->om_len < sizeof(CommandOpCode)) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        
        CommandPacket cmd;

        // Copy data from mbuf to struct (simplified)
        uint16_t len = ctxt->om->om_len;
        if(len > sizeof(CommandPacket)) len = sizeof(CommandPacket);
        ble_hs_mbuf_to_flat(ctxt->om, &cmd, len, NULL);

        ESP_LOGI("GATT", "Received Command OpCode: %d", (int)cmd.opcode);

        // Don't execute logic here - push it to the queue
        // This ensures that the BLE task isn't blocked and logic is centralized
        if(app_main_queue) {
            // TODO: Create an event struct appropriate for the app
            // xQueueSend(main_event_queue, &cmd, 0); 
        }

        return 0; // Success
    }
    return BLE_ATT_ERR_UNLIKELY;
}

// Handler for History (Read)
int gatt_svr_chr_access_history(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg) {
    // Retrieve last N times from your data store
    // int rc = os_mbuf_append(ctxt->om, &data, sizeof(data));
    return 0;
}

// Handler for State (Read + Notify)
int gatt_svr_chr_access_state(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg) {
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    const FSCT::BeamStatus beam = FSCT::fsct_status_get_beam();
    const FSCT::StatePacket payload = {
        .beam_broken = static_cast<uint8_t>(beam.beam_broken ? 1 : 0),
    };

    const int rc = os_mbuf_append(ctxt->om, &payload, sizeof(payload));
    return (rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

// Handler for Live Timing (Notify)
int gatt_svr_chr_access_live(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg) {
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    // This characteristic is primarily for notifications, but we also support READ
    // so clients can fetch the latest cached values without subscribing.
    if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    const FSCT::BeamStatus beam = FSCT::fsct_status_get_beam();
    const FSCT::LivePacket payload = {
        .beam_broken = static_cast<uint8_t>(beam.beam_broken ? 1 : 0),
        .beam_last_change_ms = beam.last_change_ms,
    };

    const int rc = os_mbuf_append(ctxt->om, &payload, sizeof(payload));
    return (rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

// Handler for Diagnostics (Read)
int gatt_svr_chr_access_diag(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg) {
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    const FSCT::BatteryStatus battery = FSCT::fsct_status_get_battery();
    const FSCT::DiagPacket payload = {
        .battery_mv = battery.battery_mv,
        .battery_percent = battery.battery_percent,
    };

    const int rc = os_mbuf_append(ctxt->om, &payload, sizeof(payload));
    return (rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

}
