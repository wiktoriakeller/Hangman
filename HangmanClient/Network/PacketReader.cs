using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace HangmanClient.Network
{
    public class PacketReader : BinaryReader
    {
        private NetworkStream _networkStream;
        private StreamReader streamReader;
        public PacketReader(NetworkStream input) : base(input)
        {
            _networkStream = input;
            streamReader = new StreamReader(_networkStream);
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
