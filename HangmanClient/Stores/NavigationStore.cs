using HangmanClient.MVVM.ViewModel;
using System;

namespace HangmanClient.Stores
{
    public class NavigationStore
    {
        private ViewModelBase _currentViewModel;
        public MainMenuViewModel? mainMenuViewModel;
        public ViewModelBase CurrentViewModel
        {
            get { return _currentViewModel; }
            set
            {
                _currentViewModel = value;
                OnCurrentViewModelChanged();
            }
        }

        private void OnCurrentViewModelChanged()
        {
            CurrentViewModelChanged?.Invoke();
        }

        public event Action CurrentViewModelChanged;
    }
}
