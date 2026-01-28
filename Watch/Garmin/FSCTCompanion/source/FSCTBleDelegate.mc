import Toybox.BluetoothLowEnergy;
import Toybox.Lang;

class FSCTBleDelegate extends BluetoothLowEnergy.BleDelegate {
    private var _profileManager as FSCTBleProfileManager;
    private var _onScanResult as WeakReference?;

    function initialize(profileManager as FSCTBleProfileManager) {
        BleDelegate.initialize();
        _profileManager = profileManager;
    }

    // Start BLE scanning
    public function start() as Void {
        BluetoothLowEnergy.setScanState(BluetoothLowEnergy.SCAN_STATE_SCANNING);
    }

    // Stop BLE Scanning
    public function stop() as Void {
        BluetoothLowEnergy.setScanState(BluetoothLowEnergy.SCAN_STATE_OFF);
    }

    // Handle new scan results being received
    public function onScanResults(scanResults as Iterator) as Void {
        System.println("Scan results received in delegate");
        for (var result = scanResults.next(); result != null; result = scanResults.next()) {
            if(result instanceof ScanResult) {
                if(contains(result.getServiceUuids(), _profileManager.FSCT_SERVICE_UUID)) {
                    System.println("Broadcasting scan result from delegate");
                    System.println("Device Name: " + result.getDeviceName());
                    System.println("Device Raw Data:" + result.getRawData().toString());
                    // System.println("Service UUIDs: " + result.getServiceUuids().toString());
                    broadcastScanResult(result);
                }
            }
        }
    }

    // Who to notify when a scan result is found
    public function notifyScanResult(scanDelegate as FSCTConnectMenuDelegate) as Void {
        _onScanResult = scanDelegate.weak();
    }

    // Broadcast a new scan result
    public function broadcastScanResult(scanResult as ScanResult) as Void {
        if (_onScanResult != null) {
            if(_onScanResult.stillAlive()) {
                (_onScanResult.get() as FSCTConnectMenuDelegate).procScanResult(scanResult);
            }
        }
    }

    // there is no built-in "in" operator for Iterator, so we make our own
    private function contains(iter as Iterator, obj as Uuid) as Boolean {
        for (var uuid = iter.next(); uuid != null; uuid = iter.next()) {
            if (uuid.equals(obj)) {
                return true;
            }
        }
        return false;
    }
}