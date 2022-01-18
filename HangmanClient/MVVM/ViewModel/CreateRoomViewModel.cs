using HangmanClient.MVVM.Commands;
using HangmanClient.Network;
using HangmanClient.Stores;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace HangmanClient.MVVM.ViewModel
{
    public class CreateRoomViewModel : ViewModelBase
    {
        
        private readonly Server _server;
        private string _username;
        public string Username
        {
            get { return _username; }
            set
            {
                _username = value;
                OnPropertyChanged(nameof(Username));
            }
        }
        public ICommand CreateRoomCommand { get; }
        public ICommand BackCommand { get; }
        public NavigationCommand NavigateGameCommand { get; }

        public CreateRoomViewModel(Server server,
                                   NavigationStore navigationStore,
                                   Func<MainMenuViewModel> createMainMenuViewModel,
                                   Func<GameViewModel> createGameViewModel)
        {
            _username = string.Empty;
            _server = server;
            BackCommand = new NavigationCommand(navigationStore, createMainMenuViewModel);
            NavigateGameCommand = new NavigationCommand(navigationStore, createGameViewModel);
            CreateRoomCommand = new CreateRoomCommand(this, server);
        }

        
    }
}
