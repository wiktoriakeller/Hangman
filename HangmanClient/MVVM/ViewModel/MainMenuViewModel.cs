using HangmanClient.MVVM.Commands;
using HangmanClient.Stores;
using System;
using System.Windows.Input;

namespace HangmanClient.MVVM.ViewModel
{
    public class MainMenuViewModel : ViewModelBase
    {
        private readonly NavigationStore _navigationStore;

        public ICommand JoinCommand { get; }
        public ICommand CreateCommand { get; }

        public MainMenuViewModel(NavigationStore navigationStore,
            Func<JoinRoomViewModel> createJoinRoomViewModel,
            Func<CreateRoomViewModel> createCreateRoomViewModel)
        {
            _navigationStore = navigationStore;
            JoinCommand = new NavigationCommand(_navigationStore, createJoinRoomViewModel);
            CreateCommand = new NavigationCommand(_navigationStore, createCreateRoomViewModel);
        }
    }
}
