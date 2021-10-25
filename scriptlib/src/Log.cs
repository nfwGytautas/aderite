using System;

namespace Aderite {

	/// <summary>
	/// Utility class for providing logging to the engine console and editor
	/// </summary>
	public class Log
	{
		public static void Info(string message) 
		{
			Console.WriteLine("Info message: " + message);
		}
	}

}
