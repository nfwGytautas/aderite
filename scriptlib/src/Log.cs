using System.Runtime.CompilerServices;

namespace Aderite
{

    /// <summary>
    /// Utility class for providing logging to the engine console and editor
    /// </summary>
    public class Log
	{
		/// <summary>
		/// Logs a trace message to the editor console
		/// </summary>
		/// <param name="message">Message to write</param>
		public static void Trace(string message) 
		{
			__Trace(message);
		}

		/// <summary>
		/// Logs a warning message to the editor console
		/// </summary>
		/// <param name="message">Message to write</param>
		public static void Warn(string message)
        {
			__Warn(message);
        }

		/// <summary>
		/// Logs an error message to the editor console
		/// </summary>
		/// <param name="message">Message to write</param>
		public static void Error(string message)
        {
			__Error(message);
        }

		[MethodImpl(MethodImplOptions.InternalCall)]
		private extern static void __Trace(string message);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private extern static void __Warn(string message);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private extern static void __Error(string message);
	}

}
