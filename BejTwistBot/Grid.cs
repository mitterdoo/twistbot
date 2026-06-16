using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using System.Windows.Forms;

namespace BejTwistBot
{
	class HSV
	{
		public double H;
		public double S;
		public double V;
		public HSV(double h =0, double s =1, double v =1)
		{
			H = h;
			S = s;
			V = v;
		}
		public HSV( Color color )
		{
			double max = Math.Max(color.R, Math.Max(color.G, color.B));
			double min = Math.Min(color.R, Math.Min(color.G, color.B));

			H = color.GetHue();
			S = (max == 0) ? 0 : 1 - (1 * min / max);
			V = max / 255;
		}

		public Color ToColor()
		{
			double hue = H;
			double saturation = S;
			double value = V;
			int hi = Convert.ToInt32(Math.Floor(hue / 60)) % 6;
			double f = hue / 60 - Math.Floor(hue / 60);

			value = value * 255;
			int v = Convert.ToInt32(value);
			int p = Convert.ToInt32(value * (1 - saturation));
			int q = Convert.ToInt32(value * (1 - f * saturation));
			int t = Convert.ToInt32(value * (1 - (1 - f) * saturation));

			if (hi == 0)
				return Color.FromArgb(255, v, t, p);
			else if (hi == 1)
				return Color.FromArgb(255, q, v, p);
			else if (hi == 2)
				return Color.FromArgb(255, p, v, t);
			else if (hi == 3)
				return Color.FromArgb(255, p, q, v);
			else if (hi == 4)
				return Color.FromArgb(255, t, p, v);
			else
				return Color.FromArgb(255, v, p, q);
		}
		public override string ToString()
		{
			return H.ToString() + ", " + S.ToString() + ", " + V.ToString();
		}
	}
	static class Extensions
	{
		public static Color GemColor(this Bitmap bmp, int x, int y, int w, int h)
		{
			int avgR = 0;
			int avgG = 0;
			int avgB = 0;
			int avgCount = 0;


			for (int ox = x; ox < x + w; ox++)
			{
				for (int oy = y; oy < y + h; oy++)
				{
					Color pixel = bmp.GetPixel(ox, oy);
					if (pixel.R == pixel.G && pixel.G == pixel.B && pixel.R <= 40)
						continue;
					avgR += pixel.R;
					avgG += pixel.G;
					avgB += pixel.B;
					avgCount++;
				}
			}
			if (avgCount == 0)
				avgCount = 1;
			Color gemColor = Color.FromArgb(avgR / avgCount, avgG / avgCount, avgB / avgCount);
			for (int ox = x; ox < x + w; ox++)
			{
				for (int oy = y; oy < y + h; oy++)
				{
					bmp.SetPixel(ox, oy, gemColor);

				}
			}
			return gemColor;
		}
		public static void DrawRect( this Bitmap bmp, int x, int y, int w, int h, Color color )
		{
			for (int ox = x; ox < x + w; ox++)
			{
				for (int oy = y; oy < y + h; oy++)
				{
					if (ox < 0 || oy < 0 || ox >= bmp.Width || oy >= bmp.Height)
						continue;
					bmp.SetPixel(ox, oy, color);

				}
			}
		}
	}
	class vec2
	{
		public float x, y;
		public vec2( float X, float Y )
		{
			x = X;
			y = Y;
		}
		public static vec2 operator +(vec2 a, vec2 b)
		{
			return new vec2(a.x + b.x, a.y + b.y);
		}
		public static vec2 operator *(int a, vec2 b)
		{
			return new vec2(a * b.x, a * b.y);
		}
		public static vec2 operator *(vec2 a, int b)
		{
			return new vec2(b * a.x, b * a.y);
		}
		public override string ToString()
		{
			return x.ToString() + ", " + y.ToString();
		}
	}
	class Grid
	{
		[DllImport("user32.dll", CharSet = CharSet.Auto)]
		public static extern IntPtr FindWindow(string strClassName, string strWindowName);

		private MainWindow Window;
		public int ReplayState = 0;
		static public int GridSize = 8;
		static int GemSize = 64;
		static int AvgSize = 24;
		static int GridX = 356;
		static int GridY = 70;
		struct Match
		{
			public vec2 Pos;
			public int Length;
			public Cell.Gem ColorA;
			public Cell.Gem ColorB;
		}

		public Cell[,] Contents = new Cell[GridSize, GridSize];

		public Grid(MainWindow window)
		{
			Window = window;
			Update();

		}
		public vec2 GetGridPos()
		{

			vec2 Window = GetWindowPos();
			Window.x += GridX;
			Window.y += GridY;

			return Window;
		}
		public vec2 GetWindowPos()
		{

			User32.Rect rect = new User32.Rect();
			User32.GetWindowRect(FindWindow("MainWindow", "Bejeweled Twist  1.0"), ref rect);

			return new vec2(rect.Left, rect.Top);
		}
		public vec2 GetCellPos( float x, float y )
		{

			vec2 offset = GetGridPos();
			return offset + new vec2(x, y) * GemSize;

		}

		public void ClickCell( float x, float y )
		{
			vec2 pos = GetCellPos(x, y);
			Program.click((uint)pos.x, (uint)pos.y);
		}

		public void ClickReplayButton()
		{
			ClickCell(4, 8.25f);
		}
		public bool IsBonus()
		{
			Bitmap bmp = CaptureSpeedBonus();
			int r = 0;
			int g = 0;
			int b = 0;
			int count = 0;
			for( int x = 0; x < bmp.Width; x++ )
			{
				for( int y = 0; y < bmp.Height; y++ )
				{
					count++;
					Color pixel = bmp.GetPixel(x, y);
					r += pixel.R;
					g += pixel.G;
					b += pixel.B;
				}
			}
			HSV avg = new HSV(Color.FromArgb(r / count, g / count, b / count));
			Console.WriteLine(avg);
			return false;
		}

		public void Update()
		{

			bool NoChanges = true;
			bool CanReplay = false;
			Bitmap bmp = CaptureApplication();
			Bitmap bonus = CaptureMegaBonus();
			Bitmap replay = CaptureReplay();
			GC.Collect();
			GC.WaitForPendingFinalizers();

			if( replay != null )
			{
				int total_r = 0;
				int total_g = 0;
				int total_b = 0;
				int total = 0;
				for (int x = 0; x < replay.Width; x++)
				{
					for (int y = 0; y < replay.Height; y++)
					{
						total++;
						Color col = replay.GetPixel(x, y);
						total_r += col.R;
						total_g += col.G;
						total_b += col.B;
					}
				}
				HSV hsv =  new HSV( Color.FromArgb(total_r / total, total_g / total, total_b / total) );
				if( hsv.H >= 295 && hsv.H <= 320 )
				{
					CanReplay = true;
				}

			}
			bool foobar = IsBonus();

			if (bmp != null)
			{
				Cell.Gem[,] OldTypes = new Cell.Gem[8, 8];
				for( int x = 0; x < 8; x++ )
				{
					for( int y = 0; y < 8; y++ )
					{
						OldTypes[x, y] = GetCell(x, y) != null ? GetCell(x, y).Type : Cell.Gem.None;
					}
				}
				ClearGrid();
				if (Window.Image != null)
				{
					Window.Image.Dispose();
				}
				for (int x = 0; x < GridSize; x++)
				{
					for (int y = 0; y < GridSize; y++)
					{

						int X = x * GemSize + GemSize / 2 - AvgSize / 2;
						int Y = y * GemSize + GemSize / 2 - AvgSize / 2;
						int Size = AvgSize;

						Color gemColor = bmp.GemColor(X, Y, Size, Size);

						Cell.Gem gem = GetGemFromColor(gemColor);


						if( OldTypes[ x, y ] != gem )
						{
							NoChanges = false;
						}
						SetCell(x, y, gem);
						switch (gem)
						{
							case Cell.Gem.Red:
								bmp.DrawRect(X, Y, Size, Size, Color.FromArgb(255, 0, 0));
								break;

							case Cell.Gem.Orange:
								bmp.DrawRect(X, Y, Size, Size, Color.FromArgb(255, 128, 0));
								break;

							case Cell.Gem.Yellow:
								bmp.DrawRect(X, Y, Size, Size, Color.FromArgb(255, 255, 0));
								break;

							case Cell.Gem.Green:
								bmp.DrawRect(X, Y, Size, Size, Color.FromArgb(0, 255, 0));
								break;

							case Cell.Gem.Blue:
								bmp.DrawRect(X, Y, Size, Size, Color.FromArgb(0, 0, 255));
								break;

							case Cell.Gem.Purple:
								bmp.DrawRect(X, Y, Size, Size, Color.FromArgb(255, 45, 255));
								break;

							case Cell.Gem.White:
								bmp.DrawRect(X, Y, Size, Size, Color.FromArgb(255, 255, 255));
								break;

							default:
								bmp.DrawRect(X, Y, Size, Size, Color.FromArgb(0, 0, 0));
								break;
						}

					}
				}
				if( NoChanges )
				{
					bmp.DrawRect(0, 0, 16, 16, Color.FromArgb(0, 255, 0));
				}
				else
				{
					bmp.DrawRect(0, 0, 16, 16, Color.FromArgb(255, 0, 0));
				}

			}
			Cell.Gem Bonus_A = Cell.Gem.None;
			Cell.Gem Bonus_B = Cell.Gem.None;
			Cell.Gem Bonus_C = Cell.Gem.None;
			Cell.Gem Bonus_D = Cell.Gem.None;
			int Current = 0;
			if( bonus != null )
			{
				if( Window.BonusImage != null )
				{
					Window.BonusImage.Dispose();
				}

				Bonus_A = GetGemFromHSV( new HSV(bonus.GetPixel(20, 20)) );
				Bonus_B = GetGemFromHSV(new HSV(bonus.GetPixel(46, 20)));
				Bonus_C = GetGemFromHSV(new HSV(bonus.GetPixel(72, 20)));
				Bonus_D = GetGemFromHSV(new HSV(bonus.GetPixel(99, 20)));

				Color Cur_A = bonus.GetPixel(20, 4);
				Color Cur_B = bonus.GetPixel(46, 4);
				Color Cur_C = bonus.GetPixel(72, 4);
				Color Cur_D = bonus.GetPixel(99, 4);
				if (Cur_A.R > 230 && Cur_A.G > 230 && Cur_A.B > 180)
					Current = 1;
				else if (Cur_B.R > 230 && Cur_B.G > 230 && Cur_B.B > 180)
					Current = 2;
				else if (Cur_C.R > 230 && Cur_C.G > 230 && Cur_C.B > 180)
					Current = 3;
				else if (Cur_D.R > 230 && Cur_D.G > 230 && Cur_D.B > 180)
					Current = 4;

				Window.BonusImage = bonus;
			}
			Cell.Gem CurrentBonus = Cell.Gem.None;
			if (Current == 1)
				CurrentBonus = Bonus_A;
			else if (Current == 2)
				CurrentBonus = Bonus_B;
			else if (Current == 3)
				CurrentBonus = Bonus_C;
			else if (Current == 4)
				CurrentBonus = Bonus_D;

			if( Control.IsKeyLocked( Keys.CapsLock ) )// && NoChanges )
			{
				/*

					0: Idle
					1: Replaying
					2: Replay button visible (but don't click)


				*/

				if (CanReplay && ReplayState == 0)
				{
					ClickReplayButton();
					ReplayState = 1;
				}
				else if (CanReplay && ReplayState == 1)
				{
					ReplayState = 2;
                }
				else if(!CanReplay && ReplayState == 2 )
				{
					ReplayState = 0;
				}
				Console.WriteLine(ReplayState);
				if( ReplayState == 0 || ReplayState == 2 )
				{
					List<Match> Moves = new List<Match>();
					for (int x = 1; x < 8; x++)
					{
						for (int y = 1; y < 8; y++)
						{
							Cell a = GetCell(x - 1, y - 1);
							Cell b = GetCell(x, y - 1);
							Cell c = GetCell(x, y);
							Cell d = GetCell(x - 1, y);

							// rotate clockwise and see what we get
							SetCell(x - 1, y - 1, d.Type);
							SetCell(x, y - 1, a.Type);
							SetCell(x, y, b.Type);
							SetCell(x - 1, y, c.Type);

							Cell newA = GetCell(x - 1, y - 1);
							Cell newB = GetCell(x, y - 1);
							Cell newC = GetCell(x, y);
							Cell newD = GetCell(x - 1, y);
							if (newA.Matches() ||
								newB.Matches() ||
								newC.Matches() ||
								newD.Matches())
							{
								Match match = new Match();
								match.Pos = new vec2(x, y);
								int Length = 0;
								if (newA.Length() > Length) Length = newA.Length();
								if (newB.Length() > Length) Length = newB.Length();
								if (newC.Length() > Length) Length = newC.Length();
								if (newD.Length() > Length) Length = newD.Length();

								if (newA.Matches())
								{
									match.ColorA = newA.Type;
								}
								if (newB.Matches())
								{
									if (match.ColorA == Cell.Gem.None)
										match.ColorA = newB.Type;
									else if (match.ColorA != newB.Type)
										match.ColorB = newB.Type;
								}
								if (newC.Matches())
								{
									if (match.ColorA == Cell.Gem.None)
										match.ColorA = newC.Type;
									else if (match.ColorA != newC.Type)
										match.ColorB = newC.Type;
								}
								if (newD.Matches())
								{
									if (match.ColorA == Cell.Gem.None)
										match.ColorA = newD.Type;
									else if (match.ColorA != newC.Type)
										match.ColorB = newD.Type;
								}
								if (match.ColorA != Cell.Gem.None && match.ColorB != Cell.Gem.None)
									Length += 1;
								match.Length = Length;

								Moves.Insert(Moves.Count, match);
							}

							SetCell(x - 1, y - 1, a.Type);
							SetCell(x, y - 1, b.Type);
							SetCell(x, y, c.Type);
							SetCell(x - 1, y, d.Type);

						}
					}

					Random rnd = new Random();
					if (Moves.Count > 0)
					{

						List<Match> ThreeMoves = new List<Match>();
						int Highest = 0;
						int HighestInd = -1;
						Match Picked = Moves[0];
						for (int i = 0; i < Moves.Count; i++)
						{
							Match match = Moves[i];
							if (CurrentBonus != Cell.Gem.None && (CurrentBonus == match.ColorA || CurrentBonus == match.ColorB))
							{
								Console.WriteLine("Current Bonus: " + Bonus_A + ", " + Bonus_B + ", " + Bonus_C + ", " + Bonus_D + " (on gem " + Current + ")");
								Console.WriteLine("Making bonus move (" + CurrentBonus + ")");
								if (Current == 4)
									Console.WriteLine("> BONUS COMPLETE!");
								Picked = match;
								Highest = 0;
								ClickCell(match.Pos.x, match.Pos.y);
								break;
							}
							if (match.Length == 3)
							{
								ThreeMoves.Insert(ThreeMoves.Count, match);
							}
							if (i == 0 || match.Length > Highest)
							{
								Highest = match.Length;
								HighestInd = i;
							}
						}
						if (Highest == 3)
						{
							Picked = ThreeMoves[rnd.Next(ThreeMoves.Count)];
						}
						int X = ((int)Picked.Pos.x - 1) * GemSize;
						int Y = ((int)Picked.Pos.y - 1) * GemSize;
						vec2 rectPos = new vec2(X, Y);
						int Thick = 8;
						bmp.DrawRect((int)rectPos.x, (int)rectPos.y, GemSize * 2, Thick, Color.FromArgb(0, 255, 0));
						bmp.DrawRect((int)rectPos.x + GemSize * 2 - Thick, (int)rectPos.y, Thick, GemSize * 2, Color.FromArgb(0, 255, 0));
						bmp.DrawRect((int)rectPos.x, (int)rectPos.y + GemSize * 2 - Thick, GemSize * 2, Thick, Color.FromArgb(0, 255, 0));
						bmp.DrawRect((int)rectPos.x, (int)rectPos.y, Thick, GemSize * 2, Color.FromArgb(0, 255, 0));
						ClickCell(Picked.Pos.x, Picked.Pos.y);
					}
					else
					{
						Console.WriteLine("!!! PANIC !!!");
						Console.WriteLine("No possible moves to keep combo going! Twisting random gems...");
						ClickCell(rnd.Next(1, 8), rnd.Next(1, 8));
					}

				}


			}
			if (bmp != null)
				Window.Image = bmp;

		}

		


		public void ClearGrid()
		{
			for( int x = 0; x < GridSize; x++ )
			{
				for( int y = 0; y < GridSize; y++ )
				{
					SetCell(x, y, Cell.Gem.None);
				}
			}
		}

		public void SetCell( int x, int y, Cell.Gem gem )
		{
			Contents[x, y] = new Cell(this, x, y, gem);
		}
		public Cell GetCell( int x, int y )
		{
			return Contents[x, y];
		}


		private Cell.Gem GetGemFromColor( Color color )
		{
			HSV hsv = new HSV(color);
			if (hsv.S < .10 && hsv.V > 0.80)
				return Cell.Gem.White;
			if (SimilarHue( hsv, 0 ) && hsv.S < .36)
				return Cell.Gem.None;
			if (SimilarHue(hsv, 350) || SimilarHue(hsv, 8))
				return Cell.Gem.Red;
			if (SimilarHue(hsv, 32) || SimilarHue( hsv, 41 ))
				return Cell.Gem.Orange;
			if (SimilarHue(hsv, 58))
				return Cell.Gem.Yellow;
			if (SimilarHue(hsv, 132) || SimilarHue( hsv, 118 ))
				return Cell.Gem.Green;
			if (SimilarHue(hsv, 211) || SimilarHue( hsv, 197 ))
				return Cell.Gem.Blue;
			if (SimilarHue(hsv, 300))
				return Cell.Gem.Purple;

			return Cell.Gem.None ;
		}
		private Cell.Gem GetGemFromHSV( HSV hsv )
		{
			Color col = hsv.ToColor();
			if (SimilarHue(hsv, 17))
				return Cell.Gem.Red;
			if (SimilarHue(hsv, 31))
				return Cell.Gem.Orange;
			if (SimilarHue(hsv, 50) && hsv.S > 0.6)
				return Cell.Gem.Yellow;
			if (SimilarHue(hsv, 100))
				return Cell.Gem.Green;
			if (SimilarHue(hsv, 200))
				return Cell.Gem.Blue;
			if (SimilarHue(hsv,318 ))
				return Cell.Gem.Purple;
			if (SimilarHue(hsv, 42))
				return Cell.Gem.White;

			return Cell.Gem.None;
		}

		int Tolerance = 64;

		private bool Similar(Color A, Color B)
		{

			int R_Dist = Math.Abs(A.R - B.R);
			int G_Dist = Math.Abs(A.G - B.G);
			int B_Dist = Math.Abs(A.B - B.B);

			return R_Dist < Tolerance && G_Dist < Tolerance && B_Dist < Tolerance;

		}
		private bool SimilarHue(HSV A, int H)
		{
			double H_Dist = Math.Abs(A.H - H);

			return H_Dist < 8;

		}

		public Bitmap CaptureApplication()
		{

		
			//var proc = Process.GetProcessesByName(procName)[0];
			//Console.WriteLine(proc.ProcessName);
			int ImageSize = GemSize * GridSize;

			var bmp = new Bitmap(ImageSize, ImageSize, PixelFormat.Format32bppArgb);
			Graphics graphics = Graphics.FromImage(bmp);
			vec2 GridPos = GetGridPos();
			graphics.CopyFromScreen((int)GridPos.x, (int)GridPos.y, 0, 0, new Size(ImageSize, ImageSize), CopyPixelOperation.SourceCopy);

			return bmp;
		}
		public Bitmap CaptureMegaBonus()
		{
			Bitmap bmp = new Bitmap(116, 42, PixelFormat.Format32bppArgb);
			Graphics graphics = Graphics.FromImage(bmp);
			vec2 Pos = GetWindowPos() + new vec2(115, 486);
			graphics.CopyFromScreen((int)Pos.x, (int)Pos.y, 0, 0, new Size(116, 42), CopyPixelOperation.SourceCopy);
			return bmp;
		}
		

		public Bitmap CaptureReplay()
		{
			Bitmap bmp = new Bitmap(114, 32, PixelFormat.Format32bppArgb);
			Graphics graphics = Graphics.FromImage(bmp);
			vec2 Pos = GetWindowPos() + new vec2(560, 589);
			graphics.CopyFromScreen((int)Pos.x, (int)Pos.y, 0, 0, new Size(114, 32), CopyPixelOperation.SourceCopy);
			return bmp;
		}

		public Bitmap CaptureSpeedBonus()
		{
			Bitmap bmp = new Bitmap(114, 32, PixelFormat.Format32bppArgb);
			Graphics graphics = Graphics.FromImage(bmp);
			vec2 Pos = GetWindowPos() + new vec2(560, 32);
			graphics.CopyFromScreen((int)Pos.x, (int)Pos.y, 0, 0, new Size(114, 32), CopyPixelOperation.SourceCopy);

			return bmp;
		}

		private class User32
		{
			public struct Rect
			{
				public int Left;
				public int Top;
				public int Right;
				public int Bottom;
			}

			[DllImport("user32.dll")]
			public static extern IntPtr GetWindowRect(IntPtr hWnd, ref Rect rect);
		}
		

	}
}
