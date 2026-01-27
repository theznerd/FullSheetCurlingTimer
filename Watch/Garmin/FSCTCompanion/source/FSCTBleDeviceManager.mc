import Toybox.BluetoothLowEnergy;
import Toybox.Lang;
import Toybox.System;

class FSCTBleDeviceManager {
    private var _device as Device?;
    private var _fsctService as Service?;
    private var _fsctHistory as Characteristic?;
    private var _fsctState as Characteristic?;
    private var _fsctLive as Characteristic?;
    private var _fsctCommand as Characteristic?;
    private var _fsctDiag as Characteristic?;

    private var _profileManager;

    public function initialize(bleDelegate as FSCTBleDelegate, profileManager as FSCTBleProfileManager) {
        // Initialization code here
        _device = null;

        

        _profileManager = profileManager;
    }
}