using System.Diagnostics;

namespace AdrGaspard.ChokefishSuite.Core.Utils
{
    public class IOTransmitter : IDisposable
    {
        private readonly Process _process;
        private readonly object _lock;
        private bool _started;
        private bool _exited;

        public IOTransmitter(string pathToExecutable, string arguments = "")
        {
            _process = new()
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = pathToExecutable,
                    Arguments = arguments,
                    UseShellExecute = false,
                    RedirectStandardInput = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true
                }
            };
            _process.OutputDataReceived += OnOutputDataReceived;
            _process.ErrorDataReceived += OnErrorDataReceived;
            _process.Exited += OnExited;
            _started = false;
            _exited = false;
            _lock = new();
        }

        public event EventHandler<string>? OutputDataReceived;

        public event EventHandler<string>? ErrorDataReceived;

        public event EventHandler? Exited;

        public void Start()
        {
            lock (_lock)
            {
                if (!_started)
                {
                    _started = true;
                    _ = _process.Start();
                    _process.BeginOutputReadLine();
                    _process.BeginErrorReadLine();
                }
            }
        }

        public void SendInputData(string data)
        {
            lock (_lock)
            {
                if (_started && !_exited && !string.IsNullOrEmpty(data))
                {
                    _process.StandardInput.WriteLine(data);
                    _process.StandardInput.Flush();
                }
            }
        }

        public void Dispose()
        {
            lock (_lock)
            {
                _process.CancelOutputRead();
                _process.CancelErrorRead();
                _process.Dispose();
            }
        }

        private void OnErrorDataReceived(object sender, DataReceivedEventArgs eventArgs)
        {
            if (!string.IsNullOrEmpty(eventArgs.Data))
            {
                OutputDataReceived?.Invoke(this, eventArgs.Data);
            }
        }

        private void OnOutputDataReceived(object sender, DataReceivedEventArgs eventArgs)
        {
            if (!string.IsNullOrEmpty(eventArgs.Data))
            {
                ErrorDataReceived?.Invoke(this, eventArgs.Data);
            }
        }

        private void OnExited(object? sender, EventArgs eventArgs)
        {
            lock (_lock)
            {
                _exited = true;
            }
            Exited?.Invoke(this, eventArgs);
        }
    }
}