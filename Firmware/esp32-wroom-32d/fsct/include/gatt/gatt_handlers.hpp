#pragma once
#include <cstdint>
#include "host/ble_hs.h"

namespace FSCT {

// Handler for reading the history of timing events
int gatt_svr_chr_access_history(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

// Handler for reading/notifying the current state
int gatt_svr_chr_access_state(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg);

// Handler for live timing notifications (subscription management)
int gatt_svr_chr_access_live(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt, void *arg);

// Handler for writing commands (reset, delete last, etc)
int gatt_svr_chr_access_command(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

// Handler for reading diagnostics
int gatt_svr_chr_access_diag(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt, void *arg);

} // namespace FSCT