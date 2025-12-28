#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "esp_err.h"
#include "gpio/gpiostartup.hpp"
#include "gatt/gatt_defs.hpp"
#include <string>

// Forward declare the service initialization function
int gatt_svc_init();

static void ble_app_advertise(void);

static void ble_app_on_sync(void) {
    int rc; 
    uint8_t addr_type;

    // 1. Determine address
    rc = ble_hs_id_infer_auto(0, &addr_type);
    if (rc != 0) {
        ESP_LOGE("GATT", "Error determining address type: %d", rc);
        return;
    }
    
    // 2. Start advertising
    ble_app_advertise();
}

static void ble_app_advertise(void) {
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    struct ble_hs_adv_fields rsp_fields; // For Scan Response
    int rc;

    // 1. Advertising Data (Flags + UUID)
    memset(&fields, 0, sizeof fields);
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    
    // Advertise the curling service UUID so apps can find us
    fields.uuids128 = (ble_uuid128_t*)&FSCT::FSCT_SERVICE_UUID;
    fields.num_uuids128 = 1;
    fields.uuids128_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if(rc != 0) {
        ESP_LOGE("GATT", "Error setting adv fields: %d", rc);
        return;
    }

    // 2. Scan Response Data (Name)
    memset(&rsp_fields, 0, sizeof rsp_fields);
    
    // set the device name to "FSCT Timer - n"
    std::string name = "FSCT Timer - " + std::to_string(fsct_gpio_group);
    rsp_fields.name = (uint8_t*)name.c_str();
    rsp_fields.name_len = name.length();
    rsp_fields.name_is_complete = 1;

    rc = ble_gap_adv_rsp_set_fields(&rsp_fields);
    if(rc != 0) {
        ESP_LOGE("GATT", "Error setting scan rsp fields: %d", rc);
        return;
    }

    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(0, NULL, BLE_HS_FOREVER, &adv_params, NULL, NULL);
    if(rc != 0) {
        ESP_LOGE("GATT", "Error enabling advertisement: %d", rc);
    }
}

static void host_task(void *param) {
    nimble_port_run(); // This function will not return
    nimble_port_freertos_deinit();
}

void initialize_nimble() {
    // Determine if we need to initialize GATT based on the GPIO settings.
    // If the position is 0 (near back-line), then we enable GATT as this
    // device is the main timer unit. Other positions are for remote units
    // that do not broadcast via GATT.
    if(fsct_gpio_position != 0) {
        ESP_LOGI("GATT", "GATT disabled for position %d", fsct_gpio_position);
        return;
    }
 
    esp_err_t ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE("GATT", "NimBLE init failed");
        return;
    }

    // Initialize Services
    int rc = gatt_svc_init();
    if(rc != 0) {
        ESP_LOGE("GATT", "GATT Service init failed: %d", rc);
        return;
    }

    ble_hs_cfg.sync_cb = ble_app_on_sync;
    // ble_hs_cfg.store_status_cb = ble_store_util_status_rr; // If using bonding

    nimble_port_freertos_init(host_task);
}