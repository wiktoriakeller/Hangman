using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HangmanClient.Network
{
    public class PacketWriter
    {
        MemoryStream _memoryStream;
        public PacketWriter()
        {
            _memoryStream = new MemoryStream();
        }

        public void WriteCode(byte code)
        {
            _memoryStream.WriteByte(code);
        }

        public void WriteString(string text)
        {
            var len = text.Length;
            _memoryStream.Write(BitConverter.GetBytes(len));
            _memoryStream.Write(Encoding.ASCII.GetBytes(text));
        }

        public byte[] GetBytes()
        {
            return _memoryStream.ToArray();
        }
    }
}
