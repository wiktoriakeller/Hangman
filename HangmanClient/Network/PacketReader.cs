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
