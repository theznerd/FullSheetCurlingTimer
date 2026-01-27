import Toybox.BluetoothLowEnergy;

class FSCTBleProfileManager {
    // Service UUID: 4375726c-696e-6720-5469-6d6572000000 ("Curling Timer" in ASCII)
    public const FSCT_SERVICE_UUID =     BluetoothLowEnergy.longToUuid(0x4375726c696e6720l, 0x54696d6572000000l);

    // Characteristic UUIDs
    public const FSCT_CHR_HISTORY_UUID = BluetoothLowEnergy.longToUuid(0x4375726c696e6720l, 0x54696d6572000001l);
    public const FSCT_CHR_STATE_UUID =   BluetoothLowEnergy.longToUuid(0x4375726c696e6720l, 0x54696d6572000002l);
    public const FSCT_CHR_LIVE_UUID =    BluetoothLowEnergy.longToUuid(0x4375726c696e6720l, 0x54696d6572000003l);
    public const FSCT_CHR_COMMAND_UUID = BluetoothLowEnergy.longToUuid(0x4375726c696e6720l, 0x54696d6572000004l);
    public const FSCT_CHR_DIAG_UUID =    BluetoothLowEnergy.longToUuid(0x4375726c696e6720l, 0x54696d6572000005l);

    private const _fsctProfileDef = {
        :uuid => FSCT_SERVICE_UUID,
        :characteristics => [
            {:uuid => FSCT_CHR_HISTORY_UUID},
            {:uuid => FSCT_CHR_STATE_UUID},
            {:uuid => FSCT_CHR_LIVE_UUID},
            {:uuid => FSCT_CHR_COMMAND_UUID}, 
            {:uuid => FSCT_CHR_DIAG_UUID}
        ]
    };

    // Register the bluetooth profile
    public function registerProfiles() as Void {
        BluetoothLowEnergy.registerProfile(_fsctProfileDef);
    }
}