﻿using CommunityToolkit.Mvvm.ComponentModel;
using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class EngineSelectorViewModel : ObservableObject
    {
        private string _enginePath;
        private string _engineName;
        private bool _useWsl;

        public EngineSelectorViewModel()
        {
            _enginePath = "";
            _engineName = "";
            _useWsl = false;
        }

        public string EnginePath
        {
            get => _enginePath;
            set
            {
                if (value == "" || File.Exists(value))
                {
                    SetProperty(ref _enginePath, value);
                    EngineName = Path.GetFileName(value);
                    OnPropertyChanged(nameof(IsEnginePathValid));
                }
            }
        }

        public string EngineName
        {
            get => _engineName;
            set => SetProperty(ref _engineName, value);
        }

        public bool UseWsl
        {
            get => _useWsl;
            set => SetProperty(ref _useWsl, value);
        }

        public bool IsEnginePathValid => File.Exists(EnginePath);
    }
}