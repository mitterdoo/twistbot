using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BejTwistBot
{
	public partial class MainWindow : Form
	{
		private Grid grid;
		private Bitmap ActualImage;
		private Bitmap ActualBonus;
		public Bitmap Image
		{
			get
			{
				return ActualImage;
			}
			set
			{
				ActualImage = value;
				pictureBox1.Refresh();
			}
		}
		public Bitmap BonusImage
		{
			get
			{
				return ActualBonus;
			}
			set
			{
				ActualBonus = value;
				pictureBox2.Refresh();
			}
		}

		public MainWindow()
		{
			InitializeComponent();
			pictureBox1.Paint += new PaintEventHandler(PictureBox1_Paint);
			pictureBox2.Paint += new PaintEventHandler(PictureBox2_Paint);
		}

		private void MainWindow_Load(object sender, EventArgs e)
		{
			grid = new Grid(this);
		}
		private void PictureBox1_Paint(object sender, PaintEventArgs e)
		{
			if (ActualImage != null)
			{
				e.Graphics.DrawImage(ActualImage, 0, 0);
			}
		}
		private void PictureBox2_Paint(object sender, PaintEventArgs e)
		{
			if (BonusImage != null)
			{
				e.Graphics.DrawImage(BonusImage, 0, 0);
			}
		}
		private void ClickTimer_Tick(object sender, EventArgs e)
		{
			grid.Update();
		}

		private void pictureBox1_Click(object sender, EventArgs e)
		{

		}

		private void button1_Click(object sender, EventArgs e)
		{
			grid.ReplayState = 0;
		}

		private void label1_Click(object sender, EventArgs e)
		{

		}

		private void button1_Click_1(object sender, EventArgs e)
		{
			grid.ReplayState = 0;
		}
	}
}
