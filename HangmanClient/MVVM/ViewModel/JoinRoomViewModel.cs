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
    public class JoinRoomViewModel : ViewModelBase
    {
        private int _roomId;
        public int RoomId
        {
            get { return _roomId; }
            set 
            { 
                _roomId = value;
                OnPropertyChanged(nameof(RoomId));
            }
        }

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

        public ICommand JoinCommand { get; }
        public ICommand BackCommand { get; }
        public NavigationCommand NavigateGameCommand { get; }

        public JoinRoomViewModel(Server server,
            NavigationStore navigationStore,
            Func<MainMenuViewModel> createMainMenuViewModel,
            Func<GameViewModel> createGameViewModel)
        {
            _username = "";
            NavigateGameCommand = new NavigationCommand(navigationStore, createGameViewModel);
            BackCommand = new NavigationCommand(navigationStore, createMainMenuViewModel);
            JoinCommand = new JoinRoomCommand(server, this);
        }

    }
}
