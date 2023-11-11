using System;

namespace L3gion
{ 
    public class Main
    {
        public float FloatVar { get; set; }
        public Main()
        {
            Console.WriteLine("Main constructor!");
        }
        public void printMessage()
        {
            Console.WriteLine("Hello world from C#");
        }

        public void printInt(int value)
        {
            Console.WriteLine($"C# says: {value}");
        }

        public void printCustomMsg(string msg)
        {
            Console.WriteLine($"C# says: {msg}");
        }
    }
}
