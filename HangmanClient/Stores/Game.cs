using HangmanClient.MVVM.Model;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Threading.Tasks;

namespace HangmanClient.Stores
{
    public class Game
    {
        private Game()
        {
            _players = new ObservableCollection<Player>();
        }
        private static volatile Game instance;
        public static Game Instance
        {
            get
            {
                if (instance == null)
                {
                    lock (m_lock)
                    {
                        if (instance == null)
                        {
                            instance = new Game();
                        }
                    }
                }
                return instance;
            }
        }

        private static object m_lock = new object();

        private ObservableCollection<Player> _players;

        public ObservableCollection<Player> Players
        {
            get
            {
                return _players;
            }
        }
        public int RoomId { get; set; }

    }
}
