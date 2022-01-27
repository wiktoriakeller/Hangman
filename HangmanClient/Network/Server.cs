using HangmanClient.MVVM.Model;
using HangmanClient.Stores;
using System;
using System.Configuration;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace HangmanClient.Network
{
    public enum OperationCodes : byte
    {
        SendNewRoomId = 48,
        JoinRoom = 49,
        NotUniqueName = 50,
        InvalidRoom = 51,
        GameAlreadyStarted = 52,
        SendPlayerName = 53,
        SendAllPlayerNames = 54,
        RoomIsFull = 55,
        SendWord = 56,
        CheckLetter = 57,
        IncorrectLetter = 58,
        CorrectLetter = 59,
        SendHangmanWithName = 60,
        EndGame = 61,
        StartGame = 62,
        StartedWaiting = 63,
        TimerStopped = 64,
        RoomCreationFailed = 65,
        NotInARoom = 66,
        PlayerLeft = 67,
        Draw = 68
    }

    public class Server
    {
        private readonly TcpClient _tcpClient;
        private readonly EventWaitHandle _waitHandle = new AutoResetEvent(false);
        private readonly Game _game;
        private readonly Error _error;
        private bool _operationFailed;
        private int _timeout;

        public Server()
        {
            _game = Game.Instance;
            _error = Error.Instance;
            _tcpClient = new TcpClient();

            var timeout = ConfigurationManager.AppSettings["Tiemout"];
            if (!int.TryParse(timeout, out _timeout))
            {
                _timeout = 25000;
            }
        }

        private bool WaitForResponse()
        {
            _operationFailed = false;
            bool completedInTime = _waitHandle.WaitOne(_timeout);
            _waitHandle.Reset();
            if(completedInTime == false)
            {
                MessageBox.Show("Connection timeout");
                Application.Current.Shutdown();
            }

            if (completedInTime && !_operationFailed)
                return true;
            return false;
        }

        public void Connect(string hostName, int port)
        {
            if (_tcpClient.Connected == false)
            {
                try
                {
                    _tcpClient.Connect(hostName, port);
                    ReadMessages();
                }
                catch (Exception)
                {
                    MessageBox.Show("Connection timeout");
                    Application.Current.Shutdown();
                }
            }
        }

        public bool JoinRoom(int roomId, string username)
        {
            var packet = PacketWriter.GetPacket((byte)OperationCodes.JoinRoom,
                                                roomId.ToString() + " " + username);
            _tcpClient.Client.Send(packet);
            if (WaitForResponse())
            {
                _game.MainPlayer = new Player(username, 0);
                _game.RoomId = roomId;
                return true;
            }
            return false;
        }

        public bool CreateRoom(string username)
        {
            var packet = PacketWriter.GetPacket((byte)OperationCodes.SendNewRoomId,
                                                username);
            _tcpClient.Client.Send(packet);
            if (WaitForResponse())
            {
                _game.MainPlayer = new Player(username, 0);
                return true;
            }
            return false;
        }

        public void SubmitLetter(string letter)
        {
            letter = letter.ToLower();
            var packet = PacketWriter.GetPacket((byte)OperationCodes.CheckLetter, letter);
            _tcpClient.Client.Send(packet);
        }
    
        private void ReadMessages()
        {
            Task.Run(async () =>
            {
                PacketReader packetReader = new PacketReader(_tcpClient.GetStream());
                while (true)
                {
                    var message = await packetReader.GetMessageAsync();
                    if (message == null)
                    {
                        continue;
                    }

                    var split = message.Split(' ');
                    var code = (OperationCodes)message[0];
                    switch (code)
                    {
                        case OperationCodes.SendNewRoomId:
                            HandleCreateResponse(split);
                            break;
                        case OperationCodes.JoinRoom:
                            HandleJoinResponse(split);
                            break;
                        case OperationCodes.SendPlayerName:
                            HandleNewPlayer(split);
                            break;
                        case OperationCodes.RoomIsFull:
                            HandleError("This room is full!");
                            break;
                        case OperationCodes.NotUniqueName:
                            HandleError("This username is already in use");
                            break;
                        case OperationCodes.GameAlreadyStarted:
                            HandleError("This game has already started");
                            break;
                        case OperationCodes.InvalidRoom:
                            HandleError("This room number is invalid");
                            break;
                        case OperationCodes.RoomCreationFailed:
                            HandleError("Room can not be created");
                            break;
                        case OperationCodes.IncorrectLetter:
                            HandleIncorrectLetter(split);
                            break;
                        case OperationCodes.CorrectLetter:
                            HandleNewWord(split[1]);
                            break;
                        case OperationCodes.SendHangmanWithName:
                            HandleHangmanUpdate(split);
                            break;
                        case OperationCodes.StartGame:
                            HandleStartGame(split);
                            break;
                        case OperationCodes.StartedWaiting:
                            HandleNewWord("Game will start soon...");
                            break;
                        case OperationCodes.EndGame:
                            HandleEndGame(split);
                            break;
                        case OperationCodes.PlayerLeft:
                            HandlePlayerLeft(split);
                            break;
                        case OperationCodes.TimerStopped:
                            HandleNewWord("Waiting for players");
                            break;
                        case OperationCodes.Draw:
                            HandleDraw(split);
                            break;
                        default:
                            break;
                    }
                }
            });
        }

        private void HandleDraw(string[] split)
        {
            Application.Current.Dispatcher.BeginInvoke(new Action(() => _game.GameStarted = false));
            Application.Current.Dispatcher.BeginInvoke(new Action(() => _game.GameOver = true));
            _game.SecretWord = $"Game Over, it is a draw!\nSecret word was: {split[1]}";
        }

        private void HandlePlayerLeft(string[] split)
        {
            var playerName = split[1];
            Application.Current.Dispatcher.BeginInvoke(new Action(() =>
            _game.RemovePlayer(playerName)));

        }

        private void HandleEndGame(string[] split)
        {
            Application.Current.Dispatcher.BeginInvoke(new Action(() => _game.GameStarted = false));
            Application.Current.Dispatcher.BeginInvoke(new Action(() => _game.GameOver = true));
            _game.SecretWord = $"Game Over, {split[1]} is the winner!\nSecret word was: {split[2]}";
        }

        private void HandleStartGame(string[] split)
        {
            _game.GameStarted = true;
            _game.SecretWord = split[1];
        }

        private void HandleHangmanUpdate(string[] split)
        {
            Application.Current.Dispatcher.BeginInvoke(new Action(() =>
            _game.Players[_game.PlayerIndexes[split[1]]].HangmanState = int.Parse(split[2])));
        }

        private void HandleIncorrectLetter(string[] split)
        {
            Application.Current.Dispatcher.BeginInvoke(new Action(() =>
           _game.MainPlayer.HangmanState = int.Parse(split[1])));
            if (int.Parse(split[1]) == 6)
            {
                _game.GameStarted = false;
                _game.SecretWord = "You lost :(";
            }
        }

        private void HandleNewWord(string message)
        {
            _game.SecretWord = message;
        }

        private void HandleNewPlayer(string[] split)
        {
            Player player = new Player(split[1], 0);
            Application.Current.Dispatcher.BeginInvoke(new Action(() => _game.AddPlayer(player)));
        }

        private void HandleJoinResponse(string[] split)
        {
            for (int i = 1; i < split.Length; i++)
            {
                if (split[i] != "")
                {
                    Player player = new Player(split[i], 0);
                    Application.Current.Dispatcher.BeginInvoke(new Action(() => _game.AddPlayer(player)));
                }
            }
            _waitHandle.Set();
        }

        private void HandleError(string message)
        {
            _operationFailed = true;
            _error.Message = message;
            _waitHandle.Set();
        }

        private void HandleCreateResponse(string[] split)
        {
            var roomId = int.Parse(split[1]);
            _game.RoomId = roomId;
            _waitHandle.Set();
        }
    }
}
