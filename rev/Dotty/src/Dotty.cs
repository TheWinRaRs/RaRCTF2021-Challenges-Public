using System;
using System.Linq;
using System.Collections.Generic;


namespace Dotty
{
    class Program
    {
        private static Dictionary<char, string> mapper = new Dictionary<char,string>()
        {
          {' ', "/"},
          {'A', ".-"},
          {'B', "-..."},
          {'C', "-.-."},
          {'D', "-.."},
          {'E', "."},
          {'F', "..-."},
          {'G', "--."},
          {'H', "...."},
          {'I', ".."},
          {'J', ".---"},
          {'K', "-.-"},
          {'L', ".-.."},
          {'M', "--"},
          {'N', "-."},
          {'O', "---"},
          {'P', ".--."},
          {'Q', "--.-"},
          {'R', ".-."},
          {'S', "..."},
          {'T', "-"},
          {'U', "..-"},
          {'V', "...-"},
          {'W', ".--"},
          {'X', "-..-"},
          {'Y', "-.--"},
          {'Z', "--.."},
          {'1', ".----"},
          {'2', "..---"},
          {'3', "...--"},
          {'4', "....-"},
          {'5', "....."},
          {'6', "-...."},
          {'7', "--..."},
          {'8', "---.."},
          {'9', "----."},
          {'0', "-----"},
        };

        static string Dotter(string phrase) {
            return String.Join("|", from char c in phrase select mapper[char.ToUpper(c)]);
        }

        static void Main(string[] args)
        {
            Console.Write("Please enter your secret to encode: ");
            string phrase = Console.ReadLine();
            string transform = Dotter(phrase);
            if (transform == Check.check) {
                Console.WriteLine("That's the right secret!");
            } else {
                Console.WriteLine(transform);
            }

        }
    }
}
