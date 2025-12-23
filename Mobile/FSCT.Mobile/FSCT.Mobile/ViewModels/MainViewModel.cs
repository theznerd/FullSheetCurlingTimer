using CommunityToolkit.Mvvm.ComponentModel;
using System.Collections.ObjectModel;

namespace FSCT.Mobile.ViewModels;

public partial class MainViewModel : ViewModelBase
{
    [ObservableProperty]
    private string _greeting = "Welcome to Avalonia!";

    [ObservableProperty]
    private bool _connected = false;

    /// <summary>
    /// Gets a collection of <see cref="CurlingTimeViewModel"/> which allows adding and removing items
    /// </summary>
    public ObservableCollection<CurlingTimeViewModel> CurlingTimes { get; } = new();

    [ObservableProperty]
    private CurlingTimeViewModel _currentCurlingTime = new();
}
