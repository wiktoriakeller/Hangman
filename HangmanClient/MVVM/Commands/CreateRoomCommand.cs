﻿using HangmanClient.MVVM.ViewModel;
using HangmanClient.Network;
using System.ComponentModel;

namespace HangmanClient.MVVM.Commands
{
    public class CreateRoomCommand : CommandBase
    {
        private readonly Server _server;
        private readonly CreateRoomViewModel _viewModel;
        private readonly NavigationCommand _navigationCommand;

        public CreateRoomCommand(CreateRoomViewModel viewModel, Server server)
        {
            _server = server;
            _viewModel = viewModel;
            _viewModel.PropertyChanged += OnViewModelChanged;
            _navigationCommand = _viewModel.NavigateGameCommand;
        }

        public override void Execute(object? parameter)
        {
            if (_server.CreateRoom(_viewModel.Username))
            {
                _navigationCommand.Execute(parameter);
            }
        }

        public override bool CanExecute(object? parameter)
        {
            return !string.IsNullOrEmpty(_viewModel.Username) && base.CanExecute(parameter);
        }

        private void OnViewModelChanged(object? sender, PropertyChangedEventArgs e)
        {
            OnCanExecutedChange();
        }

    }
}
