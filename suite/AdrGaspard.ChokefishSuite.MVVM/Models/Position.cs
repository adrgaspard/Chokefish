using System.Text.Json.Serialization;

namespace AdrGaspard.ChokefishSuite.MVVM.Models
{
    public class Position
    {
        public string Name { get; private init; }
        public string Value { get; private init; }

        public Position(string name, string value)
        {
            Name = name;
            Value = value;
        }
    }
}
