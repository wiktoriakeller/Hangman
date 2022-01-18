using HangmanClient.MVVM.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace HangmanClient.Network
{
    public class Server
    {
        readonly TcpClient _tcpClient;
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
                }
                catch (Exception)
                {
                    System.Windows.MessageBox.Show("Server time out");
                }
            }
        }

        public bool JoinRoom(int roomId, Player player)
        {
            System.Windows.MessageBox.Show("Joining room " + roomId.ToString());
            return true;
        }

        public bool CreateRoom(string player)
        {
            System.Windows.MessageBox.Show("Creating a room");
            return true;
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
