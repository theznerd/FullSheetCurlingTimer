import Toybox.Lang;
import Toybox.WatchUi;
import Toybox.BluetoothLowEnergy;
import FSCTHelperFunctions;

class FSCTConnectMenuDelegate extends WatchUi.Menu2InputDelegate {
    private var _menu as WatchUi.Menu2;
    private var _menuItemCount = 1;

    function initialize(menu as WatchUi.Menu2, bleDelegate as FSCTBleDelegate) {
        Menu2InputDelegate.initialize();
        _menu = menu;

        bleDelegate.notifyScanResult(self); // Register to receive scan results
    }

    function procScanResult(scanResult as BluetoothLowEnergy.ScanResult) as Void {
        System.println("Device found callback!");
        var deviceName = scanResult.getDeviceName();
        if (deviceName == null || deviceName == "") {
            deviceName = "Unnamed Device";
        }
        System.println("Found device: " + deviceName);
        var deviceId = FSCTHelperFunctions.stringReplace(deviceName, " ", ""); // remove spaces
        addMenuItem(deviceName, deviceId);

        // Temporarily stop scanning for demo purposes
        BluetoothLowEnergy.setScanState(BluetoothLowEnergy.SCAN_STATE_OFF);
    }

    function addMenuItem(label, id) as Void {
        _menu.addItem(new MenuItem(
            label,
            "Not connected...",
            id,
            {}
        ));
        _menuItemCount += 1;
    }

    function onSelect(menuItem as WatchUi.MenuItem) {
        var id = menuItem.getId();
        if (id == :scan || id == "scan") {
            // clear existing items including scan (for now - for testing purposes)
            while (_menuItemCount > 1) {
                _menu.deleteItem(1);
                _menuItemCount -= 1;
            }
            
            // Start Scanning
            System.println("Starting Scan...");
            BluetoothLowEnergy.setScanState(BluetoothLowEnergy.SCAN_STATE_SCANNING);
        }
    }
}