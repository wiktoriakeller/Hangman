using System.Diagnostics;
using System.IO;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace HangmanClient.Network
{
    public class PacketReader : BinaryReader
    {
        private StreamReader streamReader;
        public PacketReader(NetworkStream input) : base(input)
        {
            streamReader = new StreamReader(input);
        }

        public async Task<string> GetMessageAsync()
        {
            var msg = await streamReader.ReadLineAsync();
            if (msg != null)
            {
                Debug.WriteLine(msg);
                return msg;
            }
            return "";
        }
    }
}
