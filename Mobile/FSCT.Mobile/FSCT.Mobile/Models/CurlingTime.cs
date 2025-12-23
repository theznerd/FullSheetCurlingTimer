using CommunityToolkit.Mvvm.ComponentModel;

namespace FSCT.Mobile.Models
{
    /// <summary>
    /// Represents timing data for a throw in curling. Timing data
    /// is provided by the FSCT device after each throw. Values are
    /// initially null and populated when data is received.
    /// </summary>
    public class CurlingTime
    {
        public int? BackLineToHogLineMilliseconds { get; set; }
        public int? HogLineToHogLineMilliseconds { get; set; }
        public double? HogLineStoneSpeed { get; set; }
        public double? HogLineStoneSpeedAccuracy { get; set; }
    }
}
