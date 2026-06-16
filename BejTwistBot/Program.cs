using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace BejTwistBot
{
	class Program
	{

		[DllImport( "user32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall )]
		public static extern void mouse_event(uint dwFlags, uint dx, uint dy, uint cButtons, uint dwExtraInfo);

		private const int MOUSEEVENTF_LEFTDOWN = 0x02;
		private const int MOUSEEVENTF_LEFTUP = 0x04;
		private const int MOUSEEVENTF_RIGHTDOWN = 0x08;
		private const int MOUSEEVENTF_RIGHTUP = 0x10;
		static public void click(uint x, uint y)
		{
			Cursor.Position = new System.Drawing.Point( (int)x, (int)y);
			Thread.Sleep(33);
			mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, x, y, 0, 0);
		}

		[STAThread]
		static void Main(string[] args)
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);
			MainWindow window = new MainWindow();
			Application.Run(window);

		}
	}
}
