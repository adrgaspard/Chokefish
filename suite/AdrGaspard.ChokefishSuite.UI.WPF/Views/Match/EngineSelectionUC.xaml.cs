using AdrGaspard.ChokefishSuite.MVVM;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Views.Match
{
    /// <summary>
    /// Logique d'interaction pour EngineSelectionUC.xaml
    /// </summary>
    public partial class EngineSelectionUC : UserControl
    {
        public EngineSelectionUC()
        {
            InitializeComponent();
        }

        private void OnExecutableSelectionRequested(object sender, RoutedEventArgs eventArgs)
        {
            if (DataContext is EngineSelectorViewModel engineSelectorVM)
            {
                OpenFileDialog dialog = new()
                {
                    Title = "Select an executable",
                    CheckFileExists = true,
                    CheckPathExists = true,
                    Multiselect = false,
                    ValidateNames = false
                };
                while (dialog.ShowDialog() is true)
                {
                    if (File.Exists(dialog.FileName))
                    {
                        engineSelectorVM.EnginePath = dialog.FileName.Replace('\\', '/');
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
