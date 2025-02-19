﻿using System.IO;
using System.Text;

namespace HangmanClient.Network
{
    public static class PacketWriter
    {
        public static byte[] GetPacket(byte code, string message)
        {
            MemoryStream memoryStream = new MemoryStream();
            memoryStream.WriteByte(code);
            memoryStream.WriteByte((byte)' ');
            memoryStream.Write(Encoding.ASCII.GetBytes(message + "\n"));
            return memoryStream.ToArray();
        }
    }
}
