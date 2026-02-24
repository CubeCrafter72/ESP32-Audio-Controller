using System.Diagnostics;
using System.Drawing;
using System.IO.Ports;
using System.Windows.Forms;
using Windows.Foundation;
using NAudio.CoreAudioApi;
using Windows.Media.Control;

NotifyIcon? trayIcon = null;

AppDomain.CurrentDomain.UnhandledException += (sender, e) => LogException(e.ExceptionObject as Exception);
Application.ThreadException += (sender, e) => LogException(e.Exception);
Application.SetUnhandledExceptionMode(UnhandledExceptionMode.CatchException);

SerialPort serialPort;
System.Timers.Timer keepAliveTimer;

var deviceEnumerator = new MMDeviceEnumerator();
var playbackDevice = deviceEnumerator.GetDefaultAudioEndpoint(DataFlow.Render, Role.Multimedia);
var captureDevice = deviceEnumerator.GetDefaultAudioEndpoint(DataFlow.Capture, Role.Multimedia);

TypedEventHandler<GlobalSystemMediaTransportControlsSession, MediaPropertiesChangedEventArgs> propsChangedHandler;
TypedEventHandler<GlobalSystemMediaTransportControlsSession, TimelinePropertiesChangedEventArgs> timelineChangedHandler;
TypedEventHandler<GlobalSystemMediaTransportControlsSession, PlaybackInfoChangedEventArgs> playbackInfoChangedHandler;

GlobalSystemMediaTransportControlsSession? currentSession = null;

while (true)
{
    try 
    {
        serialPort = new SerialPort("COM3", 115200);
        serialPort.DataReceived += DataReceivedHandler;
        serialPort.Encoding = System.Text.Encoding.UTF8;
        serialPort.Open();
                    
        Debug.WriteLine("COM3 opened successfully.");
        break;
    }
    catch (Exception ex)
    {
        Debug.WriteLine("Failed to open COM3: " + ex.Message);
        Thread.Sleep(5000);
    }
}

propsChangedHandler = (sender, args) => SendSessionInfo(currentSession);
timelineChangedHandler = (sender, args) => SendSessionInfo(currentSession);
playbackInfoChangedHandler = (sender, args) => SendSessionInfo(currentSession);

keepAliveTimer = new System.Timers.Timer(5000);
keepAliveTimer.Elapsed += (sender, eventArgs) => SendPing();  
keepAliveTimer.Start();

SendPing();
RegisterMediaSessionHandler();

trayIcon = new NotifyIcon
{
    Icon = new Icon("slider.ico"),
    Text = "Audio Controller",
    Visible = true
};

var contextMenu = new ContextMenuStrip();

contextMenu.Items.Add("Open Crash Log", null, (s, e) => {
    var logFilePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "crash.log");
    if (File.Exists(logFilePath))
        Process.Start(new ProcessStartInfo(logFilePath) { UseShellExecute = true });
    else
        MessageBox.Show("No crash log found.", "Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
});

contextMenu.Items.Add("Reboot ESP32", null, (s, e) => {
    serialPort.WriteLine("reboot");
    Debug.WriteLine("Rebooting ESP32...");
});

contextMenu.Items.Add(new ToolStripSeparator());

contextMenu.Items.Add("Exit", null, (s, e) => {
    trayIcon.Visible = false;
    Application.Exit();
});

trayIcon.ContextMenuStrip = contextMenu;

Application.Run();
            
keepAliveTimer.Stop();

if (serialPort.IsOpen)
    serialPort.Close();


async void RegisterMediaSessionHandler()
{
    var manager = await GlobalSystemMediaTransportControlsSessionManager.RequestAsync();
    var currentSession = manager.GetCurrentSession();
    
    if (currentSession != null)
        SubscribeToMediaSession(currentSession);
    else
        Debug.WriteLine("No active media session found.");
    
    manager.CurrentSessionChanged += (s, e) =>
    {
        var newSession = manager.GetCurrentSession();

        UnregisterListeners();
        
        if (newSession == null)
        {
            serialPort.WriteLine("track-stop");
            Debug.WriteLine("track-stop");
            return;
        }
        
        SubscribeToMediaSession(newSession);
    };
}

void UnregisterListeners()
{
    if (currentSession == null) return;
    
    currentSession.MediaPropertiesChanged -= propsChangedHandler;
    currentSession.PlaybackInfoChanged -= playbackInfoChangedHandler;
    currentSession.TimelinePropertiesChanged -= timelineChangedHandler;
}

void SubscribeToMediaSession(GlobalSystemMediaTransportControlsSession session)
{
    if (currentSession == session)
        return;

    currentSession = session;
    
    currentSession.MediaPropertiesChanged += propsChangedHandler;
    currentSession.PlaybackInfoChanged += playbackInfoChangedHandler;
    currentSession.TimelinePropertiesChanged += timelineChangedHandler;
    
    SendSessionInfo(currentSession);
}

async void SendSessionInfo(GlobalSystemMediaTransportControlsSession? session)
{
    if (session == null) return;
    
    try
    {
        var props = await session.TryGetMediaPropertiesAsync();
        if (props == null) return;
        
        var playbackInfo = session.GetPlaybackInfo();
        if (playbackInfo == null) return;
        
        var timeline = session.GetTimelineProperties();
        if (timeline == null) return;

        if (string.IsNullOrEmpty(props.Title))
            return;

        var artist = string.IsNullOrEmpty(props.Artist) ? "null" : props.Artist;
        var isPlaying = playbackInfo.PlaybackStatus == GlobalSystemMediaTransportControlsSessionPlaybackStatus.Playing;

        serialPort.WriteLine($"track|{props.Title}|{artist}|{isPlaying}|{(int) timeline.Position.TotalSeconds}|{(int) timeline.EndTime.TotalSeconds}");
        Debug.WriteLine($"track|{props.Title}|{artist}|{isPlaying}|{(int) timeline.Position.TotalSeconds}|{(int) timeline.EndTime.TotalSeconds}");
    }
    catch (Exception ex)
    {
        Debug.WriteLine($"Error in SendSessionInfo: {ex.Message}");
    }
}

async void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
{
    var sp = (SerialPort) sender;
    var data = sp.ReadLine().Trim();

    var parts = data.Split('|');
    var command = parts[0];

    switch (command)
    {
        case "master":
            if (parts.Length > 1 && int.TryParse(parts[1], out var masterVolume))
            {
                masterVolume = SetDeviceVolume(playbackDevice, masterVolume);
                Debug.WriteLine($"Master volume set to {masterVolume}%");
            }
            else
                Debug.WriteLine("Invalid volume value received: " + data);
            break;
        case "mic":
            if (parts.Length > 1 && int.TryParse(parts[1], out var micVolume))
            {
                micVolume = SetDeviceVolume(captureDevice, micVolume);
                Debug.WriteLine($"Microphone volume set to {micVolume}%");
            }
            else
                Debug.WriteLine("Invalid microphone volume value received: " + data);
            break;
        case "left":
            if (parts.Length > 1 && int.TryParse(parts[1], out var leftKnobVolume))
            {
                SetProcessVolume(["chrome", "brave"], leftKnobVolume);
                Debug.WriteLine($"Browser volume set to {leftKnobVolume}%");
            }
            else
                Debug.WriteLine("Invalid left knob volume value received: " + data);
            break;
        case "right":
            if (parts.Length > 1 && int.TryParse(parts[1], out var rightKnobVolume))
            {
                SetProcessVolume(["spotify"], rightKnobVolume);
                Debug.WriteLine($"Spotify volume set to {rightKnobVolume}%");
            }
            else
                Debug.WriteLine("Invalid right knob volume value received: " + data);
            break;
        case "play":
            if (currentSession != null) await currentSession.TryTogglePlayPauseAsync();
            break;
        case "next":
            if (currentSession != null) await currentSession.TrySkipNextAsync();
            break;
        case "previous":
            if (currentSession != null) await currentSession.TrySkipPreviousAsync();
            break;
        default:
            Debug.WriteLine("Unknown command: " + data);
            break;
    }
}

void SendPing()
{
    if (serialPort.IsOpen)
        serialPort.WriteLine("ping");
}

int SetDeviceVolume(MMDevice device, int volume)
{
    volume = Math.Clamp(volume, 0, 100);
    device.AudioEndpointVolume.MasterVolumeLevelScalar = volume / 100.0f;
    
    return volume;
}

void SetProcessVolume(string[] processNames, int volume)
{
    var floatVolume = Math.Clamp(volume, 0, 100) / 100.0f;
    var sessions = playbackDevice.AudioSessionManager.Sessions;
    
    for (var i = 0; i < sessions.Count; i++)
    {
        using var session = sessions[i];
        try 
        {
            if (session.IsSystemSoundsSession) continue;
                
            var processId = (int) session.GetProcessID;
            using var p = Process.GetProcessById(processId);
            
            if (processNames.Contains(p.ProcessName, StringComparer.OrdinalIgnoreCase))
            {
                session.SimpleAudioVolume.Volume = floatVolume;
            }
        }
        catch (ArgumentException) {}
    }
}

void LogException(Exception? ex)
{
    if (ex == null) return;
    try
    {
        var logFilePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "crash.log");
        var logContent = $"[{DateTime.Now}] Crash Report:\n{ex}\n--------------------------------------------------\n";
        File.AppendAllText(logFilePath, logContent);

        trayIcon?.ShowBalloonTip(3000, "Audio Controller Crashed", "The application has crashed. Check crash.log.", ToolTipIcon.Error);
    }
    catch
    {
        // Ignore logging errors
    }
}