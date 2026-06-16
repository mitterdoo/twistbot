namespace BejTwistBot
{
	partial class MainWindow
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainWindow));
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.ClickTimer = new System.Windows.Forms.Timer(this.components);
			this.pictureBox2 = new System.Windows.Forms.PictureBox();
			this.label1 = new System.Windows.Forms.Label();
			this.button1 = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
			this.SuspendLayout();
			// 
			// pictureBox1
			// 
			this.pictureBox1.Location = new System.Drawing.Point(12, 12);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(512, 512);
			this.pictureBox1.TabIndex = 0;
			this.pictureBox1.TabStop = false;
			this.pictureBox1.Click += new System.EventHandler(this.pictureBox1_Click);
			// 
			// ClickTimer
			// 
			this.ClickTimer.Enabled = true;
			this.ClickTimer.Interval = 400;
			this.ClickTimer.Tick += new System.EventHandler(this.ClickTimer_Tick);
			// 
			// pictureBox2
			// 
			this.pictureBox2.Location = new System.Drawing.Point(12, 530);
			this.pictureBox2.Name = "pictureBox2";
			this.pictureBox2.Size = new System.Drawing.Size(116, 42);
			this.pictureBox2.TabIndex = 1;
			this.pictureBox2.TabStop = false;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(134, 530);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(350, 41);
			this.label1.TabIndex = 2;
			this.label1.Text = resources.GetString("label1.Text");
			this.label1.Click += new System.EventHandler(this.label1_Click);
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(490, 530);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(33, 42);
			this.button1.TabIndex = 3;
			this.button1.Text = "FIX";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler(this.button1_Click_1);
			// 
			// MainWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(536, 581);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.pictureBox2);
			this.Controls.Add(this.pictureBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.Name = "MainWindow";
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.Text = "TwistBot";
			this.Load += new System.EventHandler(this.MainWindow_Load);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.PictureBox pictureBox1;
		private System.Windows.Forms.Timer ClickTimer;
		private System.Windows.Forms.PictureBox pictureBox2;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button button1;
	}
}