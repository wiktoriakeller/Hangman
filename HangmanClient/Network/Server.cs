using HangmanClient.MVVM.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace HangmanClient.Network
{
    public enum OperationCodes : byte
    {
        JoinRoomRequest = 0,
        JoinRoomResponse = 1,
        CreateRoomRequest = 2,
        CreateRoomResponse = 3,

    }

    public class Server
    {
        private readonly TcpClient _tcpClient;
        public int RoomId { get; set; }

        public Server()
        {
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

        private void ReadMessages()
        {
            Task.Run(async () =>
            {
                PacketReader packetReader = new PacketReader(_tcpClient.GetStream());
                while (true)
                {
                    var message = await packetReader.GetMessageAsync();
                    Debug.WriteLine(message);
                }
            });
        }

        public bool JoinRoom(int roomId, Player player)
        {
            var packet = PacketWriter.GetPacket((byte)OperationCodes.JoinRoomRequest,
                                                roomId.ToString());
            _tcpClient.Client.Send(packet);

            return true;    //TODO add check if the player was added to the room
        }

        public bool CreateRoom(string username)
        {
            var packet = PacketWriter.GetPacket((byte)OperationCodes.CreateRoomRequest,
                                                username);
            _tcpClient.Client.Send(packet);
            return true;    //TODO add check if the player was added to the room
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
    }
}
