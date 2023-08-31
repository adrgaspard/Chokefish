using CommunityToolkit.Mvvm.ComponentModel;
using System.Windows.Media;

namespace AdrGaspard.ChokefishSuite.UI.WPF.ViewModels
{
    public class MainViewModel : ObservableObject
    {
        public Color AppColor => Colors.Gold;

        public string AppName => "ChokefishSuite";
    }
}
