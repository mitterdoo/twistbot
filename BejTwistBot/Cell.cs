using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BejTwistBot
{
	class Cell
	{
		public enum Gem
		{
			None,
			Red,
			Orange,
			Yellow,
			Green,
			Blue,
			Purple,
			White
		}
		public Gem Type;
		Grid Parent;
		int x, y;

		public Cell(Grid grid, int X, int Y, Gem gem)
		{
			Parent = grid;
			x = X;
			y = Y;
			Type = gem;

		}

		public vec2 GetPos()
		{
			return new vec2(x, y);
		}

		public Cell Get(int X, int Y)
		{
			if (this == null)
			{
				return null;
			}
			X = x + X;
			Y = y + Y;
			if (X < 0 || X > 7 || Y < 0 || Y > 7)
				return null;
			return Parent.GetCell(X, Y);

		}

		public int Length()
		{
			//Console.WriteLine("Checking (" + x.ToString() + ", " + y.ToString() + ")");
			int Count_H = 1; // 1 to count self
			int Count_V = 1;
			for( int ox = -1; ox <= 1; ox++ )
			{
				for( int oy = -1; oy <= 1; oy++ )
				{
					if (ox == 0 && oy == 0 || ox != 0 && oy != 0)
						continue;
					
					for( int i = 1; i < 3; i++ )
					{
						Cell next = Get(ox * i, oy * i);
						if( next == null || !next.Is( this ) )
						{
							break;
						}
						if (ox != 0)
							Count_H++;
						else
							Count_V++;
					}
				}
			}
			int Max = Math.Max(Count_H, Count_V);
			return Max;

		}
		public bool Matches()
		{
			int length = Length();
			return length >= 3;
		}

		public Cell Left(int Dist=1)
		{
			if( this == null )
			{
				return null;
			}
			return Get(-Dist, 0);
		}
		public Cell Right(int Dist=1)
		{
			if (this == null)
			{
				return null;
			}
			return Get(Dist, 0);
		}
		public Cell Up(int Dist=1)
		{
			if (this == null)
			{
				return null;
			}
			return Get(0, -Dist);
		}
		public Cell Down(int Dist=1)
		{
			if (this == null)
			{
				return null;
			}
			return Get(0, Dist);
		}

		public bool Is( Cell b )
		{
			if (this == null || b == null)
			{
				return false;
			}
			if (Type == Gem.None || b.Type == Gem.None)
				return false;
			return Type == b.Type;
		}

	}
}
