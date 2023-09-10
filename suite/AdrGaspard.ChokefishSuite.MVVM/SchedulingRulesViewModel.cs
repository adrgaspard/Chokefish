using AdrGaspard.ChokefishSuite.MVVM.Models;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Collections.Immutable;
using System.Text.Json;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class SchedulingRulesViewModel : ObservableObject, IValidatable
    {
        private static readonly ImmutableList<Position> EmptyPositionsList = Enumerable.Empty<Position>().ToImmutableList();
        private static readonly JsonSerializerOptions JsonOptions = new(JsonSerializerDefaults.General) { PropertyNameCaseInsensitive = true };

        private readonly object _lock;
        private bool _isValid;
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
            _isValid = false;
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

        public bool IsValid
        {
            get => _isValid;
            private set => SetProperty(ref _isValid, value);
        }

        public bool Parsing
        {
            get => _parsing;
            private set
            {
                SetProperty(ref _parsing, value);
                RefreshIsValid();
            }
        }

        public IReadOnlyList<Position> Positions
        {
            get => _positions;
            private set
            {
                SetProperty(ref _positions, value);
                RefreshIsValid();
            }
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
                SetProperty(ref _thinkTimeInMs, value);
                RefreshIsValid();
            }
        }

        public uint MinimumGamesCount
        {
            get => _minimumGamesCount;
            set
            {
                SetProperty(ref _minimumGamesCount, value);
                RefreshIsValid();
            }
        }

        public uint MaximumGamesCount
        {
            get => _maximumGamesCount;
            set
            {
                SetProperty(ref _maximumGamesCount, value);
                RefreshIsValid();
            }
        }

        public double EloDifference
        {
            get => _eloDifference;
            set
            {
                SetProperty(ref _eloDifference, value);
                RefreshIsValid();
            }
        }

        public double FalsePositiveRisk
        {
            get => _falsePositiveRisk;
            set
            {
                SetProperty(ref _falsePositiveRisk, value);
                RefreshIsValid();
            }
        }

        public double FalseNegativeRisk
        {
            get => _falseNegativeRisk;
            set
            {
                SetProperty(ref _falseNegativeRisk, value);
                RefreshIsValid();
            }
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

        private void RefreshIsValid()
        {
            IsValid = !Parsing && Positions.Count > 0 && ThinkTimeInMs > 0 && MaximumGamesCount >= MinimumGamesCount && MaximumGamesCount > 0 && EloDifference > 0 && FalsePositiveRisk >= 0 && FalsePositiveRisk <= 1 && FalseNegativeRisk >= 0 && FalseNegativeRisk <= 1;
        }
    }
}
