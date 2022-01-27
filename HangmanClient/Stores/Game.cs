using HangmanClient.MVVM.Model;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;

namespace HangmanClient.Stores
{
    public class Game : INotifyPropertyChanged
    {
        public IDictionary<string, int> PlayerIndexes;
        public bool GameStarted;
        private bool gameOver;
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
        public string SecretWord
        {
            get => secretWord;
            set
            {
                secretWord = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(SecretWord)));
            }
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
        public Player MainPlayer { get; set; }
        public bool GameOver { get => gameOver; set 
            { 
                gameOver = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(GameOver)));
            } 
        }
        private static object m_lock = new object();
        private Game()
        {
            secretWord = "Waiting for players";
            GameOver = false;
            GameStarted = false;
            _players = new ObservableCollection<Player>();
            PlayerIndexes = new Dictionary<string, int>();
        }
        public void AddPlayer(Player player)
        {
            PlayerIndexes.Add(player.Username, Players.Count);
            Players.Add(player);
        }

        public void RemovePlayer(string username)
        {
            int index;
            if (PlayerIndexes.TryGetValue(username, out index))
            {
                Players.RemoveAt(index);
                PlayerIndexes.Remove(username);
            }
        }

        public void Reset()
        {
            Players.Clear();
            PlayerIndexes.Clear();
            SecretWord = string.Empty;
        }
    }
}
