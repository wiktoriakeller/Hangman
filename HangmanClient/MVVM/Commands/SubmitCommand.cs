using HangmanClient.MVVM.ViewModel;
using HangmanClient.Network;
using HangmanClient.Stores;
using System.ComponentModel;

namespace HangmanClient.MVVM.Commands
{
    public class SubmitCommand : CommandBase
    {
        private readonly Game _game;
        private readonly GameViewModel _gameViewModel;
        private readonly Server _server;

        public SubmitCommand(GameViewModel gameViewModel, Server server)
        {
            _gameViewModel = gameViewModel;
            _gameViewModel.PropertyChanged += OnViewModelChanged;
            _server = server;
            _game = Game.Instance;
        }

        public override void Execute(object? parameter)
        {
            _server.SubmitLetter(_gameViewModel.PlayerInput);
            _gameViewModel.PlayerInput = "";
        }

        public override bool CanExecute(object? parameter)
        {
            if (_gameViewModel.PlayerInput.Length == 1)
            {
                return base.CanExecute(parameter) && _game.GameStarted;
            }
            return false;
        }

        private void OnViewModelChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(GameViewModel.PlayerInput))
            {
                OnCanExecutedChange();
            }
        }
    }
}
