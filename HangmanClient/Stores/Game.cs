using HangmanClient.MVVM.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HangmanClient.Stores
{
    public class Game
    {
        private static readonly Game _instance = new Game();
        public ObservableCollection<Player> Players = new();
        public int RoomId { get; set; }

        private Game()
        {
        }
        public static Game GetInstance()
        {
            return _instance;
        }
    }
}
