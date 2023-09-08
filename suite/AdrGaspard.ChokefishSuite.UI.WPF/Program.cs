using System;

namespace AdrGaspard.ChokefishSuite.UI.WPF
{
    public static class Program
    {
        [STAThread]
        public static void Main()
        {
            App application = new();
            application.InitializeComponent();
            application.Run();
        }
    }
}
