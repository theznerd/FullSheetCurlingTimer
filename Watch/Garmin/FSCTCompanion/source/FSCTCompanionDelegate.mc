import Toybox.Lang;
import Toybox.WatchUi;

class FSCTCompanionDelegate extends WatchUi.BehaviorDelegate {
    private var _bleDelegate as FSCTBleDelegate?;

    function initialize(bleDelegate as FSCTBleDelegate) {
        BehaviorDelegate.initialize();
        _bleDelegate = bleDelegate;
    }

    function onMenu() as Boolean {
        var menu = new Rez.Menus.fsct_connect();
        WatchUi.pushView(menu, new FSCTConnectMenuDelegate(menu, _bleDelegate), WatchUi.SLIDE_IMMEDIATE);
        return true;
    }
}