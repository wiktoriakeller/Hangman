using HangmanClient.Stores;
using System.ComponentModel;

namespace HangmanClient.MVVM.Commands
{
    public class FinishGameCommand : CommandBase
    {
        private readonly NavigationStore _navigationStore;
        private readonly Game _game;

        public FinishGameCommand(NavigationStore navigationStore)
        {
            _navigationStore = navigationStore;
            _game = Game.Instance;
            _game.PropertyChanged += OnGameChanged;
        }

        public override void Execute(object? parameter)
        {
            _game.Reset();
            _navigationStore.CurrentViewModel = _navigationStore.mainMenuViewModel;
            _game.GameOver = false;
        }

        public override bool CanExecute(object? parameter)
        {
            return base.CanExecute(parameter) && _game.GameOver;
        }

        private void OnGameChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(_game.GameOver))
            {
                OnCanExecutedChange();
            }
        }
    }
}
