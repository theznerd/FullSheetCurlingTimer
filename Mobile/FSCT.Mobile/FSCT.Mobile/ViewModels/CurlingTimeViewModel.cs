using CommunityToolkit.Mvvm.ComponentModel;
using FSCT.Mobile.Models;

namespace FSCT.Mobile.ViewModels
{
    public partial class CurlingTimeViewModel : ViewModelBase
    {
        public CurlingTimeViewModel()
        {
            // Create an empty CurlingTime instance
        }

        public CurlingTimeViewModel(CurlingTime curlingTime)
        {
            BackLineToHogLineMilliseconds = curlingTime.BackLineToHogLineMilliseconds;
            HogLineToHogLineMilliseconds = curlingTime.HogLineToHogLineMilliseconds;
            HogLineStoneSpeed = curlingTime.HogLineStoneSpeed;
            HogLineStoneSpeedAccuracy = curlingTime.HogLineStoneSpeedAccuracy;
        }

        // The observable properties use source generators to create
        // the backing fields and notify changes.
        [ObservableProperty]
        private int? _backLineToHogLineMilliseconds;

        [ObservableProperty]
        private int? _hogLineToHogLineMilliseconds;

        [ObservableProperty]
        private double? _hogLineStoneSpeed;

        [ObservableProperty]
        private double? _hogLineStoneSpeedAccuracy;

        // Convert the properties to user-friendly strings for the view.
        public string BackLineToHogLineMillisecondsString
        {
            get
            {
                if (BackLineToHogLineMilliseconds.HasValue)
                {
                    return (BackLineToHogLineMilliseconds.Value / 1000.0).ToString("00.00");
                }
                else
                {
                    return "--.--";
                }
            }
        }

        public string HogLineToHogLineMillisecondsString
        {
            get
            {
                if (HogLineToHogLineMilliseconds.HasValue)
                {
                    return (HogLineToHogLineMilliseconds.Value / 1000.0).ToString("00.00");
                }
                else
                {
                    return "--.--";
                }
            }
        }

        public string HogLineStoneSpeedString
        {
            get
            {
                if (HogLineStoneSpeed.HasValue)
                {
                    return HogLineStoneSpeed.Value.ToString("0.00") + "m/s";
                }
                else
                {
                    return "-.--m/s";
                }
            }
        }

        public string HogLineStoneSpeedAccuracyString
        {
            get
            {
                if (HogLineStoneSpeedAccuracy.HasValue)
                {
                    if (HogLineStoneSpeedAccuracy.Value >= -0.005 && HogLineStoneSpeedAccuracy.Value <= -0.005)
                    {
                        return "00%"; // We'll never have an exactly 0% accuracy in the data, so we'll consider anything beyond ±0.5% as no positive or negative throw
                    }
                    if (HogLineStoneSpeedAccuracy.Value < 0)
                    {
                        return "-" + (HogLineStoneSpeedAccuracy.Value * 100).ToString("00") + "%";
                    }
                    return "+" + (HogLineStoneSpeedAccuracy.Value * 100).ToString("00") + "%";
                }
                else
                {
                    return "±--%";
                }
            }
        }

        public CurlingTime GetCurlingTime()
        {
            return new CurlingTime
            {
                BackLineToHogLineMilliseconds = BackLineToHogLineMilliseconds,
                HogLineToHogLineMilliseconds = HogLineToHogLineMilliseconds,
                HogLineStoneSpeed = HogLineStoneSpeed,
                HogLineStoneSpeedAccuracy = HogLineStoneSpeedAccuracy
            };
        }
    }
}
