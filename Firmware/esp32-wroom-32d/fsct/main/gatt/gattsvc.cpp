#include "gatt/gatt_defs.hpp"
#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "gatt/gatt_handlers.hpp" // Forward declaration of callbacks

// Attribute Table
// Characteristics
static const struct ble_gatt_chr_def gatt_characteristics[] = {
    {
        .uuid = &FSCT::CHR_HISTORY_UUID.u,
        .access_cb = FSCT::gatt_svr_chr_access_history,
        .flags = BLE_GATT_CHR_F_READ,
    },
    {
        .uuid = &FSCT::CHR_STATE_UUID.u,
        .access_cb = FSCT::gatt_svr_chr_access_state,
        .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
    },
    {
        .uuid = &FSCT::CHR_LIVE_UUID.u,
        .access_cb = FSCT::gatt_svr_chr_access_live,
        .flags = BLE_GATT_CHR_F_NOTIFY,
    },
    {
        .uuid = &FSCT::CHR_COMMAND_UUID.u,
        .access_cb = FSCT::gatt_svr_chr_access_command,
        .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC,
    },
    {
        .uuid = &FSCT::CHR_DIAG_UUID.u,
        .access_cb = FSCT::gatt_svr_chr_access_diag,
        .flags = BLE_GATT_CHR_F_READ,
    },
    { 0 } // No more characteristics
};

// Service Definition
static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &FSCT::FSCT_SERVICE_UUID.u,
        .characteristics = gatt_characteristics,
    },
    { 0 } // No more services
};

int gatt_svc_init() {
    int rc;
    ble_svc_gap_init();
    ble_svc_gatt_init();
    rc = ble_gatts_count_cfg(gatt_svcs);
    if (rc != 0) return rc;
    rc = ble_gatts_add_svcs(gatt_svcs);
    return rc;
}