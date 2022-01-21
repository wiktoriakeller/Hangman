using HangmanClient.MVVM.Model;
using HangmanClient.Network;
using HangmanClient.Stores;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HangmanClient.MVVM.ViewModel
{
    public class GameViewModel : ViewModelBase
    {
        private readonly Server _server;
        public GameViewModel(Server server)
        {
            _server = server;
        }
    }
}
