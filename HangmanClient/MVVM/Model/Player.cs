using System.ComponentModel;
using System.IO;
using System.Runtime.CompilerServices;

namespace HangmanClient.MVVM.Model
{
    public class Player : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;
        private string username;
        private int state;

        public string Username {
            get { return username; }
            set
            {
                username = value;
                OnPropertyChanged();
            }
        }
        public int HangmanState {
            get { return state; }
            set
            {
                state = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(HangmanImage));
            }
        }
        public string HangmanImage { 
            get 
            {
                return Path.GetFullPath(@"Images\" + state.ToString() + ".bmp"); 
            } 
        }

        public Player(string username, int hangmanState)
        {
            Username = username;
            HangmanState = hangmanState;
        }

        protected void OnPropertyChanged([CallerMemberName] string? name = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }
    }
}
