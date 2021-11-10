using System.Runtime.CompilerServices;

namespace Aderite
{
	/// <summary>
	/// Enum taken from aderite runtime, it contains codes for various keyboard keys
	/// </summary>
	public enum Key
	{
		// The unknown key
		UNKNOWN		= 1,

		// Printable keys
		SPACE		= 32,
		APOSTROPHE	= 39,  // '
		COMMA		= 44,  // ,
		MINUS		= 45,  // -
		PERIOD		= 46,  // . 
		SLASH		= 47,  // /
		NUM_0		= 48,
		NUM_1		= 49,
		NUM_2		= 50,
		NUM_3		= 51,
		NUM_4		= 52,
		NUM_5		= 53,
		NUM_6		= 54,
		NUM_7		= 55,
		NUM_8		= 56,
		NUM_9		= 57,
		SEMICOLON	= 59,  // ; 
		EQUAL		= 61,  // = 

		// Letters
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		// Special
		LEFT_BRACKET	= 91,  // [
		BACKSLASH		= 92,  // /*\*/
		RIGHT_BRACKET	= 93,  // ]
		GRAVE_ACCENT	= 96,  // `
		WORLD_1			= 161, // non-US #1 
		WORLD_2			= 162, // non-US #2 

		// Function keys
		ESCAPE			= 256,
		ENTER			= 257,
		TAB				= 258,
		BACKSPACE		= 259,
		INSERT			= 260,
		DEL				= 261, // DELETE conflicts with winnt.h file
		RIGHT			= 262,
		LEFT			= 263,
		DOWN			= 264,
		UP				= 265,
		PAGE_UP			= 266,
		PAGE_DOWN		= 267,
		HOME			= 268,
		END				= 269,
		CAPS_LOCK		= 280,
		SCROLL_LOCK		= 281,
		NUM_LOCK		= 282,
		PRINT_SCREEN	= 283,
		PAUSE			= 284,

		// F keys
		F1		= 290,
		F2		= 291,
		F3		= 292,
		F4		= 293,
		F5		= 294,
		F6		= 295,
		F7		= 296,
		F8		= 297,
		F9		= 298,
		F10		= 299,
		F11		= 300,
		F12		= 301,
		F13		= 302,
		F14		= 303,
		F15		= 304,
		F16		= 305,
		F17		= 306,
		F18		= 307,
		F19		= 308,
		F20		= 309,
		F21		= 310,
		F22		= 311,
		F23		= 312,
		F24		= 313,
		F25		= 314,

		// Keypad numbers
		KP_0	= 320,
		KP_1	= 321,
		KP_2	= 322,
		KP_3	= 323,
		KP_4	= 324,
		KP_5	= 325,
		KP_6	= 326,
		KP_7	= 327,
		KP_8	= 328,
		KP_9	= 329,

		// Keypad actions
		KP_DECIMAL	= 330,
		KP_DIVIDE	= 331,
		KP_MULTIPLY = 332,
		KP_SUBTRACT = 333,
		KP_ADD		= 334,
		KP_ENTER	= 335,
		KP_EQUAL	= 336,
	
		// Modifier keys
		LEFT_SHIFT		= 340,
		LEFT_CONTROL	= 341,
		LEFT_ALT		= 342,
		LEFT_SUPER		= 343,
		RIGHT_SHIFT		= 344,
		RIGHT_CONTROL	= 345,
		RIGHT_ALT		= 346,
		RIGHT_SUPER		= 347,
		MENU			= 348,

		// Number of keys
		COUNT = MENU,
	};

	/// <summary>
	/// Enum taken from aderite runtime, it contains various keys for mouse buttons
	/// </summary>
	public enum MouseKey
    {
        B1,
        B2,
        B3,
        B4,
        B5,
        B6,
        B7,
        B8,

        COUNT = B8,

        // Helper
        LEFT = B1,
        RIGHT = B2,
        MIDDLE = B3,
    }

    /// <summary>
    /// Input manager of aderite engine
    /// </summary>
    public class Input
    {
		/// <summary>
		/// Returns true if the specified key is currently down
		/// </summary>
		/// <param name="key">Key code</param>
		/// <returns>True if key is down, false otherwise</returns>
        public static bool IsKeyDown(Key key)
        {
			return __IsKeyDown(key);
        }

		/// <summary>
		/// Returns true if the specified key is currently down
		/// </summary>
		/// <param name="key">Key code</param>
		/// <returns>True if key is down, false otherwise</returns>
		public static bool IsMouseButtonDown(MouseKey key)
        {
			return __IsMouseButtonDown(key);
        }

		/// <summary>
		/// Returns the current mouse position on the window
		/// </summary>
		/// <returns>Current mouse position on the window</returns>
        public static Vector2 GetMousePosition()
        {
			return __GetMousePosition();
		}

		/// <summary>
		/// Returns the delta of the mouse since last frame
		/// </summary>
		/// <returns>Delta of the mouse since last frame</returns>
        public static Vector2 GetMouseDelta()
        {
			return __GetMouseDelta();
        }

		/// <summary>
		/// Returns the scrollwheel delta since last frame
		/// </summary>
		/// <returns></returns>
        public static double GetScrollDelta()
        {
			return __GetScrollDelta();
        }

		[MethodImpl(MethodImplOptions.InternalCall)]
		private extern static bool __IsKeyDown(Key key);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private extern static bool __IsMouseButtonDown(MouseKey key);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private extern static Vector2 __GetMousePosition();

		[MethodImpl(MethodImplOptions.InternalCall)]
		private extern static Vector2 __GetMouseDelta();

		[MethodImpl(MethodImplOptions.InternalCall)]
		private extern static double __GetScrollDelta();
    }
}
