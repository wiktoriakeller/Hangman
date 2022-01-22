using System.ComponentModel;

namespace HangmanClient.MVVM.Model
{
    public class Error : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;
        private static Error? instance;
        public static Error Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new Error();
                }
                return instance;
            }
        }
        private Error() { }
        private string? _message;
        public string? Message
        {
            get { return _message; }
            set
            {
                _message = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Message)));
            }
        }
    }
}
