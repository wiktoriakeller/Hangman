using HangmanClient.MVVM.Model;
using HangmanClient.Stores;
using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace HangmanClient.Network
{
    public enum OperationCodes : byte
    {
        SendNewRoomId = 1,
        JoinRoom = 2,
        NotUniqueName = 3,
        InvalidRoom = 4,
        GameAlreadyStarted = 5,
        SendPlayerName = 6,
        SendAllPlayerNames = 7,
        RoomIsFull = 8,
        SendWord = 9,
        CheckLetter = 11,
        IncorrectLetter = 12,
        CorrectLetter = 13,
        SendHangmanWithName = 14,
        PlayerLeft = 15,
        EndGame = 16

    }

    public class Server
    {
        private readonly TcpClient _tcpClient;
        private readonly EventWaitHandle _waitHandle = new AutoResetEvent(false);
        private readonly Game _game;
        private bool _operationFailed;
        public string ErrorMessage { get; private set; }

        public Server()
        {
            ErrorMessage = "";
            _game = Game.Instance;
            _tcpClient = new TcpClient();
        }

        private bool WaitForResponse()
        {
            _operationFailed = false;
            bool timeout = _waitHandle.WaitOne();
            _waitHandle.Reset();
            if (timeout && !_operationFailed)
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
                    System.Windows.MessageBox.Show("Server time out");
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
                            HandleRoomFull();
                            break;
                        case OperationCodes.NotUniqueName:
                            HandleNotUniqueName();
                            break;
                        case OperationCodes.SendWord:
                            HandleNewWord(split);
                            break;
                        case OperationCodes.IncorrectLetter:
                            HandleIncorrectLetter(split);
                            break;
                        case OperationCodes.CorrectLetter:
                            HandleNewWord(split);
                            break;
                        case OperationCodes.SendHangmanWithName:
                            HandleHangmanUpdate(split);
                            break;
                        default:
                            break;
                    }
                }
            });
        }

        private void HandleHangmanUpdate(string[] split)
        {
            _game.Players[_game.PlayerIndexes[split[1]]].HangmanState = int.Parse(split[2]);
        }

        private void HandleIncorrectLetter(string[] split)
        {
            _game.MainPlayer.HangmanState = int.Parse(split[1]);
        }

        private void HandleNewWord(string[] split)
        {
            _game.SecretWord = split[1];
        }

        private void HandleNotUniqueName()
        {
            _operationFailed = true;
            ErrorMessage = "This username is already in use";
            _waitHandle.Set();
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
                    _game.AddPlayer(player);
                }
            }
            _waitHandle.Set();
        }

        private void HandleRoomFull()
        {
            _operationFailed = true;
            ErrorMessage = "This room is full!";
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
