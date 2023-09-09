using AdrGaspard.ChokefishSuite.MVVM.Models;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Collections.Immutable;
using System.Text.Json;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class SchedulingRulesViewModel : ObservableObject
    {
        private static readonly ImmutableList<Position> EmptyPositionsList = Enumerable.Empty<Position>().ToImmutableList();
        private static readonly JsonSerializerOptions JsonOptions = new(JsonSerializerDefaults.General) { PropertyNameCaseInsensitive = true };

        private readonly object _lock;
        private bool _parsing;
        private IReadOnlyList<Position> _positions;
        private string _positionsPath;
        private bool _keepGoingWhenConclusive;
        private bool _randomizeStartPositionsOrder;
        private ulong _thinkTimeInMs;
        private uint _minimumGamesCount;
        private uint _maximumGamesCount;
        private double _eloDifference;
        private double _falsePositiveRisk;
        private double _falseNegativeRisk;

        public SchedulingRulesViewModel()
        {
            _lock = new();
            _parsing = false;
            _positions = EmptyPositionsList;
            _positionsPath = "";
            _thinkTimeInMs = 100;
            _minimumGamesCount = 2;
            _maximumGamesCount = 1000;
            _eloDifference = 10;
            _falsePositiveRisk = 0.05;
            _falseNegativeRisk = 0.05;
            SetPositionsFromJsonCommand = new RelayCommand<string?>(SetPositionsFromJson);
        }

        public bool Parsing
        {
            get => _parsing;
            private set => SetProperty(ref _parsing, value);
        }

        public IReadOnlyList<Position> Positions
        {
            get => _positions;
            private set => SetProperty(ref _positions, value);
        }

        public string PositionsPath
        {
            get => _positionsPath;
            private set => SetProperty(ref _positionsPath, value);
        }

        public bool KeepGoingWhenConclusive
        {
            get => _keepGoingWhenConclusive;
            set => SetProperty(ref _keepGoingWhenConclusive, value);
        }

        public bool RandomizeStartPositionsOrder
        {
            get => _randomizeStartPositionsOrder;
            set => SetProperty(ref _randomizeStartPositionsOrder, value);
        }

        public ulong ThinkTimeInMs
        {
            get => _thinkTimeInMs;
            set
            {
                if (value > 0)
                {
                    SetProperty(ref _thinkTimeInMs, value);
                }
            }
        }

        public uint MinimumGamesCount
        {
            get => _minimumGamesCount;
            set => SetProperty(ref _minimumGamesCount, value);
        }

        public uint MaximumGamesCount
        {
            get => _maximumGamesCount;
            set => SetProperty(ref _maximumGamesCount, value);
        }

        public double EloDifference
        {
            get => _eloDifference;
            set => SetProperty(ref _eloDifference, value);
        }

        public double FalsePositiveRisk
        {
            get => _falsePositiveRisk;
            set => SetProperty(ref _falsePositiveRisk, value);
        }

        public double FalseNegativeRisk
        {
            get => _falseNegativeRisk;
            set => SetProperty(ref _falseNegativeRisk, value);
        }

        public ICommand SetPositionsFromJsonCommand { get; private init; }

        private void SetPositionsFromJson(string? filePath)
        {
            Task.Run(() =>
            {
                lock (_lock)
                {
                    Parsing = true;
                    if (!string.IsNullOrWhiteSpace(filePath) && File.Exists(filePath))
                    {
                        PositionsPath = filePath;
                        try
                        {
                            string json = File.ReadAllText(filePath);
                            Position[]? positions = JsonSerializer.Deserialize<Position[]>(json, JsonOptions);
                            if (positions is not null && positions.All(position => position.Name is not null && position.Value is not null))
                            {
                                Positions = positions.ToImmutableList();
                            }
                        }
                        catch
                        {
                            Positions = EmptyPositionsList;
                        }
                    }
                    else
                    {
                        PositionsPath = "";
                    }
                    Parsing = false;
                }
            });
        }
    }
}
