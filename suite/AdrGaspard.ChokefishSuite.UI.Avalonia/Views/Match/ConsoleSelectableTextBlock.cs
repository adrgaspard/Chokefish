using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Views.Match
{
    public class ConsoleSelectableTextBlock : SelectableTextBlock
    {
        protected override void OnPointerPressed(PointerPressedEventArgs e)
        {
            string? text = Inlines is { Count: > 0 } ? Inlines.Text : Text;
            if (text is null || !e.GetCurrentPoint(this).Properties.IsLeftButtonPressed)
            {
                return;
            }
            Thickness padding = this.Padding;
            Point point = e.GetPosition(this) - new Point(padding.Left, padding.Top);
            int index = TextLayout.HitTestPoint(point).TextPosition;
            switch (e.ClickCount)
            {
                case 1:
                    if (e.KeyModifiers.HasFlag(KeyModifiers.Shift))
                    {
                        int oldIndex = SelectionStart;
                        SetCurrentValue(SelectionStartProperty, Math.Min(oldIndex, index));
                        SetCurrentValue(SelectionEndProperty, Math.Max(oldIndex, index));
                    }
                    else
                    {
                        SetCurrentValue(SelectionStartProperty, index);
                        SetCurrentValue(SelectionEndProperty, index);
                    }
                    break;
                case 2:
                    int lineStart = text.LastIndexOf('\n', Math.Max(index - 1, 0)) + 1;
                    int lineEnd = text.IndexOf('\n', index);
                    SetCurrentValue(SelectionStartProperty, lineStart);
                    SetCurrentValue(SelectionEndProperty, lineEnd == -1 ? text.Length : lineEnd);
                    break;
                default:
                    SelectAll();
                    break;
            }
            e.Pointer.Capture(this);
            e.Handled = true;
        }
    }
}
