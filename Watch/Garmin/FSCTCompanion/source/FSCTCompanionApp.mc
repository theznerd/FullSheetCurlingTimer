import Toybox.Application;
import Toybox.Lang;
import Toybox.WatchUi;

class FSCTCompanionApp extends Application.AppBase {
    private var _bleDelegate as FSCTBleDelegate?;    

    function initialize() {
        AppBase.initialize();
    }

    // onStart() is called on application start up
    function onStart(state as Dictionary?) as Void {
        _bleDelegate = new FSCTBleDelegate(new FSCTBleProfileManager());
        BluetoothLowEnergy.setDelegate(_bleDelegate);
    }

    // onStop() is called when your application is exiting
    function onStop(state as Dictionary?) as Void {
        _bleDelegate = null;
    }

    // Return the initial view of your application here
    function getInitialView() as [Views] or [Views, InputDelegates] {
        return [ new FSCTCompanionView(), new FSCTCompanionDelegate(_bleDelegate) ];
    }

}

function getApp() as FSCTCompanionApp {
    return Application.getApp() as FSCTCompanionApp;
}