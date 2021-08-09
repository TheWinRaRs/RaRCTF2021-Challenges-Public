# Dotty
## Reversing

# Solution
We are given an exe, 'Dotty.exe'. `file Dotty.exe` reveals that it is a DotNET assembly. Using DNSpy to decode it, we can see:

```cs
namespace Dotty
{
	internal class Check
	{
		public static string check = "-|....|.|/|..-.|.-..|.-|--.|/|..|...|/|---|.---|--.-|-..-|.|-.--|...--|..-|--|--..|.....|.--|..|--|.-..|.|.-..|.....|....-|-|.-|.....|-.-|--...|---|.-|--..|-|--.|..---|..---|--...|--.|-...|--..|..-.|-....|-.|.-..|--.-|.--.|.|--...|-|-....|.--.|--..|--...|.-..|.....|-|--.|-.-.|-.|-..|-...|--|--|...--|-..|.-|-.|.-..|.....|/|-...|.-|...|.|...--|..---";
	}
}
```
in `Check.cs`, and in `Dotty.cs`:
```cs
namespace Dotty
{
	// Token: 0x02000002 RID: 2
	internal class Program
	{
		// Token: 0x06000002 RID: 2 RVA: 0x00002058 File Offset: 0x00000258
		private static string Dotter(string phrase)
		{
			return string.Join("|", from char c in phrase
			select Program.mapper[char.ToUpper(c)]);
		}

		// Token: 0x06000003 RID: 3 RVA: 0x0000208C File Offset: 0x0000028C
		private static void Main(string[] args)
		{
			Console.Write("Please enter your secret to encode: ");
			string phrase = Console.ReadLine();
			string text = Program.Dotter(phrase);
			if (text == Check.check)
			{
				Console.WriteLine("That's the right secret!");
			}
			else
			{
				Console.WriteLine(text);
			}
		}

		// Token: 0x04000001 RID: 1
		private static Dictionary<char, string> mapper = new Dictionary<char, string>
		{
			{
				' ',
				"/"
			},
			{
				'A',
				".-"
			},
            [ .. snip .. ]
```
If we run it, we can see that our input is translated to morse code. Before it's printed out howeever, it's checked against `Check.check`, a constant string. If we read this, we get the string

`-|....|.|/|..-.|.-..|.-|--.|/|..|...|/|---|.---|--.-|-..-|.|-.--|...--|..-|--|--..|.....|.--|..|--|.-..|.|.-..|.....|....-|-|.-|.....|-.-|--...|---|.-|--..|-|--.|..---|..---|--...|--.|-...|--..|..-.|-....|-.|.-..|--.-|.--.|.|--...|-|-....|.--.|--..|--...|.-..|.....|-|--.|-.-.|-.|-..|-...|--|--|...--|-..|.-|-.|.-..|.....|/|-...|.-|...|.|...--|..---`

We can then plug this into a morse code decoder (using `|` as the letter and `/` as the word delimiter) and get: `THE FLAG IS OJQXEY3UMZ5WIMLEL54TA5K7OAZTG227GBZF6NLQPE7T6PZ7L5TGCNDBMM3DANL5 BASE32`. Decoding this string from Base32, we get: `rarctf{d1d_y0u_p33k_0r_5py????_fa4ac605}`
