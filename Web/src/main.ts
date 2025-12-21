import "./style.css";

type Ui = {
  connectBtn: HTMLButtonElement;
  disconnectBtn: HTMLButtonElement;
  btSupport: HTMLElement;
  state: HTMLElement;
  device: HTMLElement;
  error: HTMLElement;
};

const ui: Ui = {
  connectBtn: mustGet("connectBtn"),
  disconnectBtn: mustGet("disconnectBtn"),
  btSupport: mustGetEl("btSupport"),
  state: mustGetEl("state"),
  device: mustGetEl("device"),
  error: mustGetEl("error"),
};

let activeDevice: BluetoothDevice | null = null;

renderSupport();
wireEvents();

function wireEvents() {
  ui.connectBtn.addEventListener("click", async () => {
    clearError();

    if (!isBluetoothSupported()) {
      setState("Web Bluetooth not supported in this browser");
      setError("navigator.bluetooth is not available");
      return;
    }

    try {
      setState("Requesting device…");

      // Minimal chooser — update filters once we know the device name/service UUID.
      const device = await navigator.bluetooth.requestDevice({
        acceptAllDevices: true,
        optionalServices: [],
      });

      activeDevice = device;
      ui.device.textContent = device.name || "(unnamed device)";

      device.addEventListener("gattserverdisconnected", () => {
        setState("Disconnected");
        ui.disconnectBtn.disabled = true;
        ui.connectBtn.disabled = false;
      });

      setState("Connecting…");

      // This establishes a GATT connection. We are not discovering services yet.
      await device.gatt?.connect();

      setState("Connected");
      ui.disconnectBtn.disabled = false;
      ui.connectBtn.disabled = true;
    } catch (e) {
      setState("Idle");
      setError(toMessage(e));
    }
  });

  ui.disconnectBtn.addEventListener("click", () => {
    clearError();

    if (!activeDevice) {
      return;
    }

    try {
      activeDevice.gatt?.disconnect();
    } catch (e) {
      setError(toMessage(e));
    } finally {
      setState("Disconnected");
      ui.disconnectBtn.disabled = true;
      ui.connectBtn.disabled = false;
    }
  });
}

function renderSupport() {
  ui.btSupport.textContent = isBluetoothSupported() ? "Supported" : "Not supported";
}

function isBluetoothSupported(): boolean {
  return typeof navigator !== "undefined" && "bluetooth" in navigator;
}

function setState(value: string) {
  ui.state.textContent = value;
}

function setError(value: string) {
  ui.error.textContent = value;
}

function clearError() {
  ui.error.textContent = "None";
}

function mustGet(id: string): HTMLButtonElement {
  const el = document.getElementById(id);
  if (!el) throw new Error(`Missing element #${id}`);
  if (!(el instanceof HTMLButtonElement)) throw new Error(`#${id} is not a button`);
  return el;
}

function mustGetEl(id: string): HTMLElement {
  const el = document.getElementById(id);
  if (!el) throw new Error(`Missing element #${id}`);
  return el;
}

function toMessage(e: unknown): string {
  if (e instanceof Error) return e.message;
  if (typeof e === "string") return e;
  try {
    return JSON.stringify(e);
  } catch {
    return String(e);
  }
}
