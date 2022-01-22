using HangmanClient.MVVM.Commands;
using HangmanClient.Network;
using System.Windows.Input;

namespace HangmanClient.MVVM.ViewModel
{
    public class GameViewModel : ViewModelBase
    {
        private readonly Server _server;
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
        public GameViewModel(Server server)
        {
            _server = server;
            _playerInput = "";
            SubmitLetter = new SubmitCommand(this, server);
        }

    }
}
