using Avalonia.Media;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Models
{
    public static class Brushes
    {
        private static readonly Color DarkColor = Color.FromRgb(0xB8, 0x8C, 0x78);
        private static readonly Color LightColor = Color.FromRgb(0xF1, 0xD9, 0xC8);
        private static readonly Color HighlightedColor = Color.FromArgb(0xB0, 0x12, 0xA0, 0xDA);

        public static readonly IBrush DarkColorBrush = new SolidColorBrush(DarkColor);
        public static readonly IBrush LightColorBrush = new SolidColorBrush(LightColor);
        public static readonly IBrush HighlightedDarkColorBrush = new SolidColorBrush(Merge(DarkColor, HighlightedColor));
        public static readonly IBrush HighlightedLightColorBrush = new SolidColorBrush(Merge(LightColor, HighlightedColor));

        private static Color Merge(params Color[] colors)
        {
            if (colors.Length == 0)
            {
                return Colors.Transparent;
            }
            Color result = colors[0];
            for (int i = 1; i < colors.Length; i++)
            {
                float add = (colors[i].A + 1f) / 256;
                result = Color.FromArgb(
                    0xFF,
                    ClampByte(result.R * (1 - add) + colors[i].R * add),
                    ClampByte(result.G * (1 - add) + colors[i].G * add),
                    ClampByte(result.B * (1 - add) + colors[i].B * add));
            }
            return result;
        }

        private static byte ClampByte(float value)
        {
            return (byte)Math.Clamp(value, 0, 255);
        }
    }
}
