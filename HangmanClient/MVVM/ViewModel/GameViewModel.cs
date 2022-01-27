using HangmanClient.MVVM.Commands;
using HangmanClient.Network;
using HangmanClient.Stores;
using System.Windows.Input;

namespace HangmanClient.MVVM.ViewModel
{
    public class GameViewModel : ViewModelBase
    {
        private string _playerInput;
        public string PlayerInput
        {
            get { return _playerInput; }
            set
            {
                _playerInput = value;
                OnPropertyChanged(nameof(PlayerInput));
            }
        }
        public ICommand SubmitLetter { get; }
        public ICommand BackCommand { get; }
        public GameViewModel(Server server, NavigationStore navigationStore)
        {
            _playerInput = "";
            SubmitLetter = new SubmitCommand(this, server);
            BackCommand = new FinishGameCommand(navigationStore);
        }
    }
}
