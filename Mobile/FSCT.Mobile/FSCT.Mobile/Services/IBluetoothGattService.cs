using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace FSCT.Mobile.Services
{
    public interface IBluetoothGattService : IAsyncDisposable
    {
        bool IsConnected { get; }

        event EventHandler<bool>? ConnectionChanged;

        /* Connection Handlers */
        Task ConnectAsync(string deviceId, CancellationToken cancellationToken = default);

        Task DisconnectAsync(CancellationToken cancellationToken = default);

        /* Attributes to read */
        // GetTimerStateAsync() // current timer state: timing, waiting
        // 
        // FSCTDiagnostics() // battery levels, firmware versions, signal strength, alignment status

        /* Commands back to device */
        /// <summary>
        /// Resets an active timer in the event that someone trips 
        /// a back line sensor inadvertently
        /// </summary>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        Task ResetAsync(CancellationToken cancellationToken = default);

        /// <summary>
        /// Deletes the last time stored in the FSCT broadcasting device.
        /// </summary>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        Task DeleteLastTimeAsync(CancellationToken cancellationToken = default);

        // POSSIBLE FUTURE FUNCTIONS
        // SetDirectionAsync() - however, believe we can handle this algorithmically 
    }
}
