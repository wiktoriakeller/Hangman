using HangmanClient.MVVM.Model;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Threading.Tasks;

namespace HangmanClient.Stores
{
    public class Game : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;
        private ObservableCollection<Player> _players;
        public ObservableCollection<Player> Players
        {
            get
            {
                return _players;
            }
        }
        private int roomId;
        public int RoomId
        {
            get => roomId; set
            {
                roomId = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(RoomId)));
            }
        }
        private string secretWord;
        public string SecretWord { get => secretWord; set
            {
                secretWord = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(SecretWord)));

            }
        }
        private Game()
        {
            _players = new ObservableCollection<Player>();
        }
        private static volatile Game? instance;
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
    }
}
