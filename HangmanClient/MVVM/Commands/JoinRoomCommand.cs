using HangmanClient.MVVM.ViewModel;
using HangmanClient.Network;
using System.ComponentModel;

namespace HangmanClient.MVVM.Commands
{
    public class JoinRoomCommand : CommandBase
    {
        private readonly Server _server;
        private readonly JoinRoomViewModel _viewModel;
        private readonly NavigationCommand _navigationCommand;
        public JoinRoomCommand(Server server, JoinRoomViewModel viewModel)
        {
            _server = server;
            _viewModel = viewModel;
            _viewModel.PropertyChanged += OnViewModelChanged;
            _navigationCommand = _viewModel.NavigateGameCommand;
        }


        public override bool CanExecute(object? parameter)
        {
            if (_viewModel.RoomId != null)
            {
                return !string.IsNullOrEmpty(_viewModel.Username) &&
                    _viewModel.RoomId >= 0 &&
                    base.CanExecute(parameter);
            }
            return false;
        }

        public override void Execute(object? parameter)
        {
            if (_server.JoinRoom(_viewModel.RoomId.GetValueOrDefault(), _viewModel.Username))
            {
                _navigationCommand.Execute(parameter);
            }
        }
        private void OnViewModelChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(JoinRoomViewModel.Username) ||
                e.PropertyName == nameof(JoinRoomViewModel.RoomId))
            {
                OnCanExecutedChange();
            }
        }
    }
}
