using AdrGaspard.ChokefishSuite.MVVM;
using Microsoft.Win32;
using System.IO;
using System.Windows;
using System.Windows.Controls;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Views.Match
{
    /// <summary>
    /// Logique d'interaction pour RuleSelectionUC.xaml
    /// </summary>
    public partial class RuleSelectionUC : UserControl
    {
        public RuleSelectionUC()
        {
            InitializeComponent();
        }

        private void OnJsonSelectionRequested(object sender, RoutedEventArgs eventArgs)
        {
            if (DataContext is SchedulingRulesViewModel schedulingRulesVM)
            {
                OpenFileDialog dialog = new()
                {
                    Title = "Select a json file",
                    CheckFileExists = true,
                    CheckPathExists = true,
                    Multiselect = false,
                    ValidateNames = false,
                    Filter = "Json files (*.json)|*.json"
                };
                while (dialog.ShowDialog() is true)
                {
                    if (File.Exists(dialog.FileName))
                    {
                        schedulingRulesVM.SetPositionsFromJsonCommand.Execute(dialog.FileName.Replace('\\', '/'));
                        break;
                    }
                    else
                    {
                        dialog.Reset();
                    }
                }
            }
        }
    }
}
