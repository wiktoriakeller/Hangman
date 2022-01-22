using HangmanClient.MVVM.ViewModel;
using HangmanClient.Network;
using HangmanClient.Stores;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace HangmanClient
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private Server _server;
        private readonly NavigationStore _navigationStore;

        public App()
        {
            _navigationStore = new NavigationStore();
            _server = new Server();
            string? hostName = ConfigurationManager.AppSettings["ServerIP"];
            if (string.IsNullOrEmpty(hostName))
            {
                hostName = "localhost";
            }
            var port = ConfigurationManager.AppSettings["Port"];
            int portNumber;
            if (!int.TryParse(port, out portNumber))
            {
                portNumber = 12345;
            }

            _server.Connect(hostName, portNumber);
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            _navigationStore.CurrentViewModel = new MainMenuViewModel(_navigationStore, CreateJoinRoomViewModel, CreateCreateRoomViewModel);

            MainWindow = new MainWindow()
            {
                DataContext = new MainViewModel(_server, _navigationStore)
            };
            MainWindow.Show();
            base.OnStartup(e);
        }

        private GameViewModel CreateGameViewModel()
        {
            return new GameViewModel(_server);
        }

        private CreateRoomViewModel CreateCreateRoomViewModel()
        {
            return new CreateRoomViewModel(_server, _navigationStore, CreateMainMenuViewModel, CreateGameViewModel);
        }

        private JoinRoomViewModel CreateJoinRoomViewModel()
        {
            return new JoinRoomViewModel(_server, _navigationStore, CreateMainMenuViewModel, CreateGameViewModel);
        }

        private MainMenuViewModel CreateMainMenuViewModel()
        {   if (_navigationStore.mainMenuViewModel == null)
            {
                return new MainMenuViewModel(_navigationStore, CreateJoinRoomViewModel, CreateCreateRoomViewModel);
            }
            return _navigationStore.mainMenuViewModel;
        }
    }
}
