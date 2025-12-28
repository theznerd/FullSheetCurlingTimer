using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace FSCT.Mobile.Services
{
    public interface IBluetoothScanner
    {
        IAsyncEnumerable<BluetoothDeviceInfo> ScanAsync(CancellationToken cancellationToken = default);
        Task<bool> EnsurePermissionsAsync(CancellationToken cancellationToken = default);
    }

    public sealed record BluetoothDeviceInfo(string DeviceId, string Name, int Rssi);
}
