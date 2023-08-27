using System.Diagnostics;
using System.Text;

Process engine = new()
{
    StartInfo = new()
    {
        FileName = "wsl",
        Arguments = @"/mnt/c/Users/Gaspard/Git\ projects/Chokefish/engine/build/Chokefish",
        UseShellExecute = false,
        RedirectStandardInput = true,
        RedirectStandardOutput = true,
        StandardOutputEncoding = Encoding.Unicode,
        RedirectStandardError = true,
        StandardErrorEncoding = Encoding.Unicode,
        CreateNoWindow = false,
    },
    EnableRaisingEvents = true
};
engine.Start();
engine.OutputDataReceived += OnEngineOutputReceived;
engine.ErrorDataReceived += OnEngineErrorReceived;
engine.BeginOutputReadLine();
engine.BeginErrorReadLine();

while (Console.ReadLine() is string content)
{
    engine.StandardInput.WriteLine(content);
    engine.StandardInput.Flush();
}
engine.WaitForExit();
engine.OutputDataReceived -= OnEngineOutputReceived;
engine.ErrorDataReceived -= OnEngineErrorReceived;
engine.StandardInput.Close();
engine.Close();

static void OnEngineOutputReceived(object sender, DataReceivedEventArgs eventArgs)
{
    if (eventArgs.Data is string content)
    {
        Console.WriteLine("stdout: " + content);
    }
    else
    {

    }
}

static void OnEngineErrorReceived(object sender, DataReceivedEventArgs eventArgs)
{
    if (eventArgs.Data is string content)
    {
        Console.WriteLine("stderr: " + content);
    }
    else
    {

    }
}

//using System.Diagnostics;
//using System.Text;

//using (Process process = new()
//{
//    StartInfo = new()
//    {
//        FileName = "wsl.exe",
//        RedirectStandardInput = true,
//        RedirectStandardOutput = true,
//        UseShellExecute = false,
//        CreateNoWindow = false,
//        StandardOutputEncoding = Encoding.Unicode,
//        StandardInputEncoding = Encoding.Unicode,
//    }
//})
//{
//    process.OutputDataReceived += OnOutputDataReceived;
//    process.Start();
//    process.BeginOutputReadLine();
//    StreamWriter inputWriter = process.StandardInput;
//    inputWriter.NewLine = "\n";
//    while (true)
//    {
//        if (Console.ReadLine() is string content)
//        {
//            if (content.ToLower() == "exit")
//            {
//                break;
//            }
//            await inputWriter.WriteLineAsync("bash -c \"" + content + "\"");
//            await inputWriter.FlushAsync();
//        }
//    }
//    process.WaitForExit();
//}

//static void OnOutputDataReceived(object sender, DataReceivedEventArgs e)
//{
//    if (e.Data is string content)
//    {
//        Console.WriteLine("WSL Output: " + content);
//    }
//}