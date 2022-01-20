using HangmanClient.MVVM.Model;
using System;
using System.Collections.ObjectModel;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace HangmanClient.Network
{
    public enum OperationCodes : byte
    {
        JoinRoom = 0,
        CreateRoom= 1,
        RoomFull = 2,
    }

    public class Server
    {
        public int RoomId { get; set; }
        private readonly TcpClient _tcpClient;
        private readonly EventWaitHandle _waitHandle = new AutoResetEvent(false);

        public string ErrorMessage { get; private set; }

        public Server()
        {
            RoomId = -1;
            _tcpClient = new TcpClient();
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

            bool timeout = _waitHandle.WaitOne();
            _waitHandle.Reset();
            if (timeout && RoomId >= 0)
                return true;
            return false;
        }

        public bool CreateRoom(string username)
        {
            var packet = PacketWriter.GetPacket((byte)OperationCodes.CreateRoom,
                                                username);
            _tcpClient.Client.Send(packet);

            bool timeout = _waitHandle.WaitOne();
            _waitHandle.Reset();
            if (timeout && RoomId >= 0)
                return true;
            return false;
        }

        public ObservableCollection<Player> GetConnectedPlayers()
        {

            return new ObservableCollection<Player>()
            {
                new Player("Player1", 0),
                new Player("Player2", 1),
                new Player("Player3", 2),
                new Player("Player4", 3),
                new Player("Player5", 4),
                new Player("Player6", 5),
            };
        }

        private void ReadMessages()
        {
            Task.Run(async () =>
            {
                PacketReader packetReader = new PacketReader(_tcpClient.GetStream());
                while (true)
                {
                    var message = await packetReader.GetMessageAsync();
                    var code = (OperationCodes)message[0];
                    switch (code)
                    {
                        case OperationCodes.CreateRoom:
                            HandleCreateResponse(message);
                            break;
                        case OperationCodes.RoomFull:
                            HandleRoomFull();
                            break;
                        default:
                            break;
                    }
                }
            });
        }

        private void HandleRoomFull()
        {
            ErrorMessage = "This room is full!";
            _waitHandle.Set();
        }

        private void HandleCreateResponse(string message)
        {
            var splited = message.Split(" ");
            var roomId = int.Parse(splited[1]);
            if (roomId >= 0)
            {
                RoomId = roomId;
            }
            _waitHandle.Set();
        }
    }
}
