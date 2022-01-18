using System.IO;

namespace HangmanClient.MVVM.Model
{
    public class Player
    {
        public string Username { get; set; }
        public int HangmanState { get; set; }
        public string HangmanImage { 
            get 
            {
                return Path.GetFullPath(@"Images\" + HangmanState.ToString() + ".bmp"); 
            } 
        }

        public Player(string username, int hangmanState)
        {
            Username = username;
            HangmanState = hangmanState;
        }
    }
}
