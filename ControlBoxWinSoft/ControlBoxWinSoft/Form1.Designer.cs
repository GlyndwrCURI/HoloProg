namespace ControlBoxWinSoft
{
    partial class Form1
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
            this.textBoxComPortName = new System.Windows.Forms.TextBox();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.textBoxMemo1 = new System.Windows.Forms.TextBox();
            this.buttonOpen = new System.Windows.Forms.Button();
            this.buttonClose = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.buttonGetAll = new System.Windows.Forms.Button();
            this.buttonTriggerGet = new System.Windows.Forms.Button();
            this.buttonPixelDelayGet = new System.Windows.Forms.Button();
            this.buttonPixelWidthGet = new System.Windows.Forms.Button();
            this.buttonLineStartWidthGet = new System.Windows.Forms.Button();
            this.buttonLineStartDelayGet = new System.Windows.Forms.Button();
            this.buttonPixDivRatioGet = new System.Windows.Forms.Button();
            this.buttonPixFreqGet = new System.Windows.Forms.Button();
            this.buttonPixTrigSend = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.radioButtonLcosRealTrigger = new System.Windows.Forms.RadioButton();
            this.radioButtonInternalSimulator = new System.Windows.Forms.RadioButton();
            this.buttonPixelDelay = new System.Windows.Forms.Button();
            this.buttonPixelWidth = new System.Windows.Forms.Button();
            this.buttonLineStartWidth = new System.Windows.Forms.Button();
            this.buttonLineStartDelay = new System.Windows.Forms.Button();
            this.buttonPixelDivideRatio = new System.Windows.Forms.Button();
            this.buttonPixelFrequency = new System.Windows.Forms.Button();
            this.numericUpDownPixelWidth = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownPixelDelay = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownLineStartWidth = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownLineStartDelay = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownPixelDivideRatio = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownPixelFrequency = new System.Windows.Forms.NumericUpDown();
            this.buttonSendAll = new System.Windows.Forms.Button();
            this.buttonLineTrigger = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.buttonAutoTrigger = new System.Windows.Forms.Button();
            this.numericUpDownAutoTrigger = new System.Windows.Forms.NumericUpDown();
            this.textBoxCustomString = new System.Windows.Forms.TextBox();
            this.buttonSendCustomString = new System.Windows.Forms.Button();
            this.buttonTriggerSeries = new System.Windows.Forms.Button();
            this.buttonStartPrint = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.numericUpDownDelayMs = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownNumberOfLines = new System.Windows.Forms.NumericUpDown();
            this.Print = new System.Windows.Forms.GroupBox();
            this.textBoxPrintCommand = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.label9 = new System.Windows.Forms.Label();
            this.numericUpDownPrintColorOffset = new System.Windows.Forms.NumericUpDown();
            this.label8 = new System.Windows.Forms.Label();
            this.numericUpDownPrintShutterRedStoplLine = new System.Windows.Forms.NumericUpDown();
            this.label7 = new System.Windows.Forms.Label();
            this.numericUpDownPrintShutterRedStartLine = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.numericUpDownPrintShutterStopPixel = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.numericUpDownPrintShutterStartPixel = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.numericUpDownPrintTotalLines = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.numericUpDownPrintLineLength = new System.Windows.Forms.NumericUpDown();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label13 = new System.Windows.Forms.Label();
            this.numericUpDownLaserPulseWidth = new System.Windows.Forms.NumericUpDown();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.buttonSendLaserDelays = new System.Windows.Forms.Button();
            this.numericUpDownBlueLaserDelay = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownGreenLaserDelay = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownRedLaserDelay = new System.Windows.Forms.NumericUpDown();
            this.timer2 = new System.Windows.Forms.Timer(this.components);
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.label16 = new System.Windows.Forms.Label();
            this.buttonAutoGetStatus = new System.Windows.Forms.Button();
            this.numericUpDownGetStatusUpdate = new System.Windows.Forms.NumericUpDown();
            this.buttonGetStatus = new System.Windows.Forms.Button();
            this.label15 = new System.Windows.Forms.Label();
            this.numericUpDownStatusY = new System.Windows.Forms.NumericUpDown();
            this.label14 = new System.Windows.Forms.Label();
            this.numericUpDownStatusX = new System.Windows.Forms.NumericUpDown();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.textBoxCurrentConfig = new System.Windows.Forms.TextBox();
            this.buttonConfigAddSelected = new System.Windows.Forms.Button();
            this.buttonAddNewConfig = new System.Windows.Forms.Button();
            this.listBoxConfigNames = new System.Windows.Forms.ListBox();
            this.buttonSaveConfig = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPixelWidth)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPixelDelay)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownLineStartWidth)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownLineStartDelay)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPixelDivideRatio)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPixelFrequency)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownAutoTrigger)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownDelayMs)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownNumberOfLines)).BeginInit();
            this.Print.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintColorOffset)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintShutterRedStoplLine)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintShutterRedStartLine)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintShutterStopPixel)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintShutterStartPixel)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintTotalLines)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintLineLength)).BeginInit();
            this.groupBox4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownLaserPulseWidth)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownBlueLaserDelay)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownGreenLaserDelay)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownRedLaserDelay)).BeginInit();
            this.groupBox5.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownGetStatusUpdate)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownStatusY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownStatusX)).BeginInit();
            this.groupBox6.SuspendLayout();
            this.SuspendLayout();
            // 
            // textBoxComPortName
            // 
            this.textBoxComPortName.Location = new System.Drawing.Point(12, 12);
            this.textBoxComPortName.Name = "textBoxComPortName";
            this.textBoxComPortName.Size = new System.Drawing.Size(100, 20);
            this.textBoxComPortName.TabIndex = 0;
            this.textBoxComPortName.Text = "COM14";
            // 
            // serialPort1
            // 
            this.serialPort1.BaudRate = 230400;
            this.serialPort1.PortName = "COM12";
            this.serialPort1.ReadTimeout = 25;
            this.serialPort1.WriteTimeout = 25;
            // 
            // textBoxMemo1
            // 
            this.textBoxMemo1.Location = new System.Drawing.Point(656, 12);
            this.textBoxMemo1.Multiline = true;
            this.textBoxMemo1.Name = "textBoxMemo1";
            this.textBoxMemo1.Size = new System.Drawing.Size(326, 411);
            this.textBoxMemo1.TabIndex = 1;
            // 
            // buttonOpen
            // 
            this.buttonOpen.Location = new System.Drawing.Point(118, 12);
            this.buttonOpen.Name = "buttonOpen";
            this.buttonOpen.Size = new System.Drawing.Size(75, 23);
            this.buttonOpen.TabIndex = 2;
            this.buttonOpen.Text = "Open";
            this.buttonOpen.UseVisualStyleBackColor = true;
            this.buttonOpen.Click += new System.EventHandler(this.buttonOpen_Click);
            // 
            // buttonClose
            // 
            this.buttonClose.Enabled = false;
            this.buttonClose.Location = new System.Drawing.Point(118, 38);
            this.buttonClose.Name = "buttonClose";
            this.buttonClose.Size = new System.Drawing.Size(75, 23);
            this.buttonClose.TabIndex = 3;
            this.buttonClose.Text = "Close";
            this.buttonClose.UseVisualStyleBackColor = true;
            this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.buttonGetAll);
            this.groupBox1.Controls.Add(this.buttonTriggerGet);
            this.groupBox1.Controls.Add(this.buttonPixelDelayGet);
            this.groupBox1.Controls.Add(this.buttonPixelWidthGet);
            this.groupBox1.Controls.Add(this.buttonLineStartWidthGet);
            this.groupBox1.Controls.Add(this.buttonLineStartDelayGet);
            this.groupBox1.Controls.Add(this.buttonPixDivRatioGet);
            this.groupBox1.Controls.Add(this.buttonPixFreqGet);
            this.groupBox1.Controls.Add(this.buttonPixTrigSend);
            this.groupBox1.Controls.Add(this.groupBox3);
            this.groupBox1.Controls.Add(this.buttonPixelDelay);
            this.groupBox1.Controls.Add(this.buttonPixelWidth);
            this.groupBox1.Controls.Add(this.buttonLineStartWidth);
            this.groupBox1.Controls.Add(this.buttonLineStartDelay);
            this.groupBox1.Controls.Add(this.buttonPixelDivideRatio);
            this.groupBox1.Controls.Add(this.buttonPixelFrequency);
            this.groupBox1.Controls.Add(this.numericUpDownPixelWidth);
            this.groupBox1.Controls.Add(this.numericUpDownPixelDelay);
            this.groupBox1.Controls.Add(this.numericUpDownLineStartWidth);
            this.groupBox1.Controls.Add(this.numericUpDownLineStartDelay);
            this.groupBox1.Controls.Add(this.numericUpDownPixelDivideRatio);
            this.groupBox1.Controls.Add(this.numericUpDownPixelFrequency);
            this.groupBox1.Location = new System.Drawing.Point(12, 70);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(225, 282);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Pixel Trigger control";
            // 
            // buttonGetAll
            // 
            this.buttonGetAll.Enabled = false;
            this.buttonGetAll.Location = new System.Drawing.Point(172, 243);
            this.buttonGetAll.Name = "buttonGetAll";
            this.buttonGetAll.Size = new System.Drawing.Size(47, 23);
            this.buttonGetAll.TabIndex = 24;
            this.buttonGetAll.Text = "GetAll";
            this.buttonGetAll.UseVisualStyleBackColor = true;
            // 
            // buttonTriggerGet
            // 
            this.buttonTriggerGet.Enabled = false;
            this.buttonTriggerGet.Location = new System.Drawing.Point(185, 184);
            this.buttonTriggerGet.Name = "buttonTriggerGet";
            this.buttonTriggerGet.Size = new System.Drawing.Size(34, 23);
            this.buttonTriggerGet.TabIndex = 23;
            this.buttonTriggerGet.Text = "Get";
            this.buttonTriggerGet.UseVisualStyleBackColor = true;
            // 
            // buttonPixelDelayGet
            // 
            this.buttonPixelDelayGet.Enabled = false;
            this.buttonPixelDelayGet.Location = new System.Drawing.Point(185, 152);
            this.buttonPixelDelayGet.Name = "buttonPixelDelayGet";
            this.buttonPixelDelayGet.Size = new System.Drawing.Size(34, 23);
            this.buttonPixelDelayGet.TabIndex = 22;
            this.buttonPixelDelayGet.Text = "Get";
            this.buttonPixelDelayGet.UseVisualStyleBackColor = true;
            this.buttonPixelDelayGet.Click += new System.EventHandler(this.buttonPixelDelayGet_Click);
            // 
            // buttonPixelWidthGet
            // 
            this.buttonPixelWidthGet.Enabled = false;
            this.buttonPixelWidthGet.Location = new System.Drawing.Point(185, 126);
            this.buttonPixelWidthGet.Name = "buttonPixelWidthGet";
            this.buttonPixelWidthGet.Size = new System.Drawing.Size(34, 23);
            this.buttonPixelWidthGet.TabIndex = 21;
            this.buttonPixelWidthGet.Text = "Get";
            this.buttonPixelWidthGet.UseVisualStyleBackColor = true;
            this.buttonPixelWidthGet.Click += new System.EventHandler(this.buttonPixelWidthGet_Click);
            // 
            // buttonLineStartWidthGet
            // 
            this.buttonLineStartWidthGet.Enabled = false;
            this.buttonLineStartWidthGet.Location = new System.Drawing.Point(185, 97);
            this.buttonLineStartWidthGet.Name = "buttonLineStartWidthGet";
            this.buttonLineStartWidthGet.Size = new System.Drawing.Size(34, 23);
            this.buttonLineStartWidthGet.TabIndex = 20;
            this.buttonLineStartWidthGet.Text = "Get";
            this.buttonLineStartWidthGet.UseVisualStyleBackColor = true;
            this.buttonLineStartWidthGet.Click += new System.EventHandler(this.buttonLineStartWidthGet_Click);
            // 
            // buttonLineStartDelayGet
            // 
            this.buttonLineStartDelayGet.Enabled = false;
            this.buttonLineStartDelayGet.Location = new System.Drawing.Point(185, 71);
            this.buttonLineStartDelayGet.Name = "buttonLineStartDelayGet";
            this.buttonLineStartDelayGet.Size = new System.Drawing.Size(34, 23);
            this.buttonLineStartDelayGet.TabIndex = 19;
            this.buttonLineStartDelayGet.Text = "Get";
            this.buttonLineStartDelayGet.UseVisualStyleBackColor = true;
            this.buttonLineStartDelayGet.Click += new System.EventHandler(this.buttonLineStartDelayGet_Click);
            // 
            // buttonPixDivRatioGet
            // 
            this.buttonPixDivRatioGet.Enabled = false;
            this.buttonPixDivRatioGet.Location = new System.Drawing.Point(185, 45);
            this.buttonPixDivRatioGet.Name = "buttonPixDivRatioGet";
            this.buttonPixDivRatioGet.Size = new System.Drawing.Size(34, 23);
            this.buttonPixDivRatioGet.TabIndex = 18;
            this.buttonPixDivRatioGet.Text = "Get";
            this.buttonPixDivRatioGet.UseVisualStyleBackColor = true;
            this.buttonPixDivRatioGet.Click += new System.EventHandler(this.buttonPixDivRatioGet_Click);
            // 
            // buttonPixFreqGet
            // 
            this.buttonPixFreqGet.Enabled = false;
            this.buttonPixFreqGet.Location = new System.Drawing.Point(185, 19);
            this.buttonPixFreqGet.Name = "buttonPixFreqGet";
            this.buttonPixFreqGet.Size = new System.Drawing.Size(34, 23);
            this.buttonPixFreqGet.TabIndex = 17;
            this.buttonPixFreqGet.Text = "Get";
            this.buttonPixFreqGet.UseVisualStyleBackColor = true;
            this.buttonPixFreqGet.Click += new System.EventHandler(this.buttonPixFreqGet_Click);
            // 
            // buttonPixTrigSend
            // 
            this.buttonPixTrigSend.Enabled = false;
            this.buttonPixTrigSend.Location = new System.Drawing.Point(129, 184);
            this.buttonPixTrigSend.Name = "buttonPixTrigSend";
            this.buttonPixTrigSend.Size = new System.Drawing.Size(50, 23);
            this.buttonPixTrigSend.TabIndex = 16;
            this.buttonPixTrigSend.Text = "PixTrig";
            this.buttonPixTrigSend.UseVisualStyleBackColor = true;
            this.buttonPixTrigSend.Click += new System.EventHandler(this.buttonPixTrigSend_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.radioButtonLcosRealTrigger);
            this.groupBox3.Controls.Add(this.radioButtonInternalSimulator);
            this.groupBox3.Location = new System.Drawing.Point(6, 178);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(117, 55);
            this.groupBox3.TabIndex = 15;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "PixTrigger mux";
            // 
            // radioButtonLcosRealTrigger
            // 
            this.radioButtonLcosRealTrigger.AutoSize = true;
            this.radioButtonLcosRealTrigger.Location = new System.Drawing.Point(6, 35);
            this.radioButtonLcosRealTrigger.Name = "radioButtonLcosRealTrigger";
            this.radioButtonLcosRealTrigger.Size = new System.Drawing.Size(105, 17);
            this.radioButtonLcosRealTrigger.TabIndex = 1;
            this.radioButtonLcosRealTrigger.Text = "LCOS real trigger";
            this.radioButtonLcosRealTrigger.UseVisualStyleBackColor = true;
            this.radioButtonLcosRealTrigger.CheckedChanged += new System.EventHandler(this.radioButtonLcosRealTrigger_CheckedChanged);
            // 
            // radioButtonInternalSimulator
            // 
            this.radioButtonInternalSimulator.AutoSize = true;
            this.radioButtonInternalSimulator.Checked = true;
            this.radioButtonInternalSimulator.Location = new System.Drawing.Point(6, 19);
            this.radioButtonInternalSimulator.Name = "radioButtonInternalSimulator";
            this.radioButtonInternalSimulator.Size = new System.Drawing.Size(104, 17);
            this.radioButtonInternalSimulator.TabIndex = 0;
            this.radioButtonInternalSimulator.TabStop = true;
            this.radioButtonInternalSimulator.Text = "Internal simulator";
            this.radioButtonInternalSimulator.UseVisualStyleBackColor = true;
            this.radioButtonInternalSimulator.CheckedChanged += new System.EventHandler(this.radioButtonInternalSimulator_CheckedChanged);
            // 
            // buttonPixelDelay
            // 
            this.buttonPixelDelay.Enabled = false;
            this.buttonPixelDelay.Location = new System.Drawing.Point(91, 155);
            this.buttonPixelDelay.Name = "buttonPixelDelay";
            this.buttonPixelDelay.Size = new System.Drawing.Size(88, 23);
            this.buttonPixelDelay.TabIndex = 1;
            this.buttonPixelDelay.Text = "PixelDelay";
            this.buttonPixelDelay.UseVisualStyleBackColor = true;
            this.buttonPixelDelay.Click += new System.EventHandler(this.buttonPixelDelay_Click);
            // 
            // buttonPixelWidth
            // 
            this.buttonPixelWidth.Enabled = false;
            this.buttonPixelWidth.Location = new System.Drawing.Point(91, 126);
            this.buttonPixelWidth.Name = "buttonPixelWidth";
            this.buttonPixelWidth.Size = new System.Drawing.Size(88, 23);
            this.buttonPixelWidth.TabIndex = 1;
            this.buttonPixelWidth.Text = "PixelWidth";
            this.buttonPixelWidth.UseVisualStyleBackColor = true;
            this.buttonPixelWidth.Click += new System.EventHandler(this.buttonPixelWidth_Click);
            // 
            // buttonLineStartWidth
            // 
            this.buttonLineStartWidth.Enabled = false;
            this.buttonLineStartWidth.Location = new System.Drawing.Point(91, 97);
            this.buttonLineStartWidth.Name = "buttonLineStartWidth";
            this.buttonLineStartWidth.Size = new System.Drawing.Size(88, 23);
            this.buttonLineStartWidth.TabIndex = 1;
            this.buttonLineStartWidth.Text = "LineStartWidth";
            this.buttonLineStartWidth.UseVisualStyleBackColor = true;
            this.buttonLineStartWidth.Click += new System.EventHandler(this.buttonLineStartWidth_Click);
            // 
            // buttonLineStartDelay
            // 
            this.buttonLineStartDelay.Enabled = false;
            this.buttonLineStartDelay.Location = new System.Drawing.Point(91, 71);
            this.buttonLineStartDelay.Name = "buttonLineStartDelay";
            this.buttonLineStartDelay.Size = new System.Drawing.Size(88, 23);
            this.buttonLineStartDelay.TabIndex = 1;
            this.buttonLineStartDelay.Text = "LineStartDelay";
            this.buttonLineStartDelay.UseVisualStyleBackColor = true;
            this.buttonLineStartDelay.Click += new System.EventHandler(this.buttonLineStartDelay_Click);
            // 
            // buttonPixelDivideRatio
            // 
            this.buttonPixelDivideRatio.Enabled = false;
            this.buttonPixelDivideRatio.Location = new System.Drawing.Point(91, 45);
            this.buttonPixelDivideRatio.Name = "buttonPixelDivideRatio";
            this.buttonPixelDivideRatio.Size = new System.Drawing.Size(88, 23);
            this.buttonPixelDivideRatio.TabIndex = 1;
            this.buttonPixelDivideRatio.Text = "PixelDivideRatio";
            this.buttonPixelDivideRatio.UseVisualStyleBackColor = true;
            this.buttonPixelDivideRatio.Click += new System.EventHandler(this.buttonPixelDivideRatio_Click);
            // 
            // buttonPixelFrequency
            // 
            this.buttonPixelFrequency.Enabled = false;
            this.buttonPixelFrequency.Location = new System.Drawing.Point(91, 19);
            this.buttonPixelFrequency.Name = "buttonPixelFrequency";
            this.buttonPixelFrequency.Size = new System.Drawing.Size(88, 23);
            this.buttonPixelFrequency.TabIndex = 1;
            this.buttonPixelFrequency.Text = "PixelFrequency";
            this.buttonPixelFrequency.UseVisualStyleBackColor = true;
            this.buttonPixelFrequency.Click += new System.EventHandler(this.buttonPixelFrequency_Click);
            // 
            // numericUpDownPixelWidth
            // 
            this.numericUpDownPixelWidth.Location = new System.Drawing.Point(6, 126);
            this.numericUpDownPixelWidth.Maximum = new decimal(new int[] {
            10000000,
            0,
            0,
            0});
            this.numericUpDownPixelWidth.Name = "numericUpDownPixelWidth";
            this.numericUpDownPixelWidth.Size = new System.Drawing.Size(79, 20);
            this.numericUpDownPixelWidth.TabIndex = 0;
            this.numericUpDownPixelWidth.ThousandsSeparator = true;
            this.numericUpDownPixelWidth.Value = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numericUpDownPixelWidth.ValueChanged += new System.EventHandler(this.numericUpDownPixelWidth_ValueChanged);
            // 
            // numericUpDownPixelDelay
            // 
            this.numericUpDownPixelDelay.Location = new System.Drawing.Point(6, 155);
            this.numericUpDownPixelDelay.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numericUpDownPixelDelay.Name = "numericUpDownPixelDelay";
            this.numericUpDownPixelDelay.Size = new System.Drawing.Size(79, 20);
            this.numericUpDownPixelDelay.TabIndex = 0;
            this.numericUpDownPixelDelay.ThousandsSeparator = true;
            this.numericUpDownPixelDelay.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numericUpDownPixelDelay.ValueChanged += new System.EventHandler(this.numericUpDownPixelDelay_ValueChanged);
            // 
            // numericUpDownLineStartWidth
            // 
            this.numericUpDownLineStartWidth.Location = new System.Drawing.Point(6, 97);
            this.numericUpDownLineStartWidth.Maximum = new decimal(new int[] {
            10000000,
            0,
            0,
            0});
            this.numericUpDownLineStartWidth.Name = "numericUpDownLineStartWidth";
            this.numericUpDownLineStartWidth.Size = new System.Drawing.Size(79, 20);
            this.numericUpDownLineStartWidth.TabIndex = 0;
            this.numericUpDownLineStartWidth.ThousandsSeparator = true;
            this.numericUpDownLineStartWidth.Value = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numericUpDownLineStartWidth.ValueChanged += new System.EventHandler(this.numericUpDownLineStartWidth_ValueChanged);
            // 
            // numericUpDownLineStartDelay
            // 
            this.numericUpDownLineStartDelay.Location = new System.Drawing.Point(6, 71);
            this.numericUpDownLineStartDelay.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numericUpDownLineStartDelay.Name = "numericUpDownLineStartDelay";
            this.numericUpDownLineStartDelay.Size = new System.Drawing.Size(79, 20);
            this.numericUpDownLineStartDelay.TabIndex = 0;
            this.numericUpDownLineStartDelay.ThousandsSeparator = true;
            this.numericUpDownLineStartDelay.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numericUpDownLineStartDelay.ValueChanged += new System.EventHandler(this.numericUpDownLineStartDelay_ValueChanged);
            // 
            // numericUpDownPixelDivideRatio
            // 
            this.numericUpDownPixelDivideRatio.Location = new System.Drawing.Point(6, 45);
            this.numericUpDownPixelDivideRatio.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numericUpDownPixelDivideRatio.Name = "numericUpDownPixelDivideRatio";
            this.numericUpDownPixelDivideRatio.Size = new System.Drawing.Size(79, 20);
            this.numericUpDownPixelDivideRatio.TabIndex = 0;
            this.numericUpDownPixelDivideRatio.ThousandsSeparator = true;
            this.numericUpDownPixelDivideRatio.Value = new decimal(new int[] {
            63,
            0,
            0,
            0});
            this.numericUpDownPixelDivideRatio.ValueChanged += new System.EventHandler(this.numericUpDownPixelDivideRatio_ValueChanged);
            // 
            // numericUpDownPixelFrequency
            // 
            this.numericUpDownPixelFrequency.Location = new System.Drawing.Point(6, 19);
            this.numericUpDownPixelFrequency.Maximum = new decimal(new int[] {
            500000,
            0,
            0,
            0});
            this.numericUpDownPixelFrequency.Name = "numericUpDownPixelFrequency";
            this.numericUpDownPixelFrequency.Size = new System.Drawing.Size(79, 20);
            this.numericUpDownPixelFrequency.TabIndex = 0;
            this.numericUpDownPixelFrequency.ThousandsSeparator = true;
            this.numericUpDownPixelFrequency.Value = new decimal(new int[] {
            83333,
            0,
            0,
            0});
            this.numericUpDownPixelFrequency.ValueChanged += new System.EventHandler(this.numericUpDownPixelFrequency_ValueChanged);
            // 
            // buttonSendAll
            // 
            this.buttonSendAll.Enabled = false;
            this.buttonSendAll.Location = new System.Drawing.Point(103, 313);
            this.buttonSendAll.Name = "buttonSendAll";
            this.buttonSendAll.Size = new System.Drawing.Size(75, 23);
            this.buttonSendAll.TabIndex = 2;
            this.buttonSendAll.Text = "SendAll";
            this.buttonSendAll.UseVisualStyleBackColor = true;
            this.buttonSendAll.Click += new System.EventHandler(this.buttonSendAll_Click);
            // 
            // buttonLineTrigger
            // 
            this.buttonLineTrigger.Enabled = false;
            this.buttonLineTrigger.Location = new System.Drawing.Point(580, 272);
            this.buttonLineTrigger.Name = "buttonLineTrigger";
            this.buttonLineTrigger.Size = new System.Drawing.Size(75, 24);
            this.buttonLineTrigger.TabIndex = 2;
            this.buttonLineTrigger.Text = "LineTrigger";
            this.buttonLineTrigger.UseVisualStyleBackColor = true;
            this.buttonLineTrigger.Click += new System.EventHandler(this.buttonLineTrigger_Click);
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // buttonAutoTrigger
            // 
            this.buttonAutoTrigger.Enabled = false;
            this.buttonAutoTrigger.Location = new System.Drawing.Point(580, 302);
            this.buttonAutoTrigger.Name = "buttonAutoTrigger";
            this.buttonAutoTrigger.Size = new System.Drawing.Size(75, 23);
            this.buttonAutoTrigger.TabIndex = 5;
            this.buttonAutoTrigger.Text = "AutoTrigger";
            this.buttonAutoTrigger.UseVisualStyleBackColor = true;
            this.buttonAutoTrigger.Click += new System.EventHandler(this.buttonAutoTrigger_Click);
            // 
            // numericUpDownAutoTrigger
            // 
            this.numericUpDownAutoTrigger.Location = new System.Drawing.Point(519, 302);
            this.numericUpDownAutoTrigger.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.numericUpDownAutoTrigger.Name = "numericUpDownAutoTrigger";
            this.numericUpDownAutoTrigger.Size = new System.Drawing.Size(55, 20);
            this.numericUpDownAutoTrigger.TabIndex = 6;
            this.numericUpDownAutoTrigger.Value = new decimal(new int[] {
            400,
            0,
            0,
            0});
            this.numericUpDownAutoTrigger.ValueChanged += new System.EventHandler(this.numericUpDownAutoTrigger_ValueChanged);
            // 
            // textBoxCustomString
            // 
            this.textBoxCustomString.Location = new System.Drawing.Point(12, 389);
            this.textBoxCustomString.Name = "textBoxCustomString";
            this.textBoxCustomString.Size = new System.Drawing.Size(225, 20);
            this.textBoxCustomString.TabIndex = 7;
            this.textBoxCustomString.Text = "@a0s-1 50 10 10 40 1 10 1";
            // 
            // buttonSendCustomString
            // 
            this.buttonSendCustomString.Enabled = false;
            this.buttonSendCustomString.Location = new System.Drawing.Point(580, 331);
            this.buttonSendCustomString.Name = "buttonSendCustomString";
            this.buttonSendCustomString.Size = new System.Drawing.Size(75, 23);
            this.buttonSendCustomString.TabIndex = 8;
            this.buttonSendCustomString.Text = "SendCustomString";
            this.buttonSendCustomString.UseVisualStyleBackColor = true;
            this.buttonSendCustomString.Click += new System.EventHandler(this.buttonSendCustomString_Click);
            // 
            // buttonTriggerSeries
            // 
            this.buttonTriggerSeries.Enabled = false;
            this.buttonTriggerSeries.Location = new System.Drawing.Point(172, 25);
            this.buttonTriggerSeries.Name = "buttonTriggerSeries";
            this.buttonTriggerSeries.Size = new System.Drawing.Size(75, 23);
            this.buttonTriggerSeries.TabIndex = 9;
            this.buttonTriggerSeries.Text = "TriggerSeries";
            this.buttonTriggerSeries.UseVisualStyleBackColor = true;
            this.buttonTriggerSeries.Click += new System.EventHandler(this.buttonTriggerSeries_Click);
            // 
            // buttonStartPrint
            // 
            this.buttonStartPrint.Enabled = false;
            this.buttonStartPrint.Location = new System.Drawing.Point(172, 54);
            this.buttonStartPrint.Name = "buttonStartPrint";
            this.buttonStartPrint.Size = new System.Drawing.Size(75, 37);
            this.buttonStartPrint.TabIndex = 12;
            this.buttonStartPrint.Text = "StartPrint";
            this.buttonStartPrint.UseVisualStyleBackColor = true;
            this.buttonStartPrint.Click += new System.EventHandler(this.buttonStartPrint_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.buttonStartPrint);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.buttonTriggerSeries);
            this.groupBox2.Controls.Add(this.numericUpDownDelayMs);
            this.groupBox2.Controls.Add(this.numericUpDownNumberOfLines);
            this.groupBox2.Location = new System.Drawing.Point(12, 415);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(262, 106);
            this.groupBox2.TabIndex = 13;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "groupBox2";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(30, 53);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(56, 13);
            this.label2.TabIndex = 14;
            this.label2.Text = "Delay [ms]";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 27);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(80, 13);
            this.label1.TabIndex = 15;
            this.label1.Text = "Number of lines";
            // 
            // numericUpDownDelayMs
            // 
            this.numericUpDownDelayMs.Location = new System.Drawing.Point(92, 51);
            this.numericUpDownDelayMs.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numericUpDownDelayMs.Name = "numericUpDownDelayMs";
            this.numericUpDownDelayMs.Size = new System.Drawing.Size(74, 20);
            this.numericUpDownDelayMs.TabIndex = 12;
            this.numericUpDownDelayMs.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // numericUpDownNumberOfLines
            // 
            this.numericUpDownNumberOfLines.Location = new System.Drawing.Point(92, 25);
            this.numericUpDownNumberOfLines.Name = "numericUpDownNumberOfLines";
            this.numericUpDownNumberOfLines.Size = new System.Drawing.Size(74, 20);
            this.numericUpDownNumberOfLines.TabIndex = 13;
            this.numericUpDownNumberOfLines.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // Print
            // 
            this.Print.Controls.Add(this.textBoxPrintCommand);
            this.Print.Controls.Add(this.button1);
            this.Print.Controls.Add(this.label9);
            this.Print.Controls.Add(this.numericUpDownPrintColorOffset);
            this.Print.Controls.Add(this.label8);
            this.Print.Controls.Add(this.numericUpDownPrintShutterRedStoplLine);
            this.Print.Controls.Add(this.label7);
            this.Print.Controls.Add(this.numericUpDownPrintShutterRedStartLine);
            this.Print.Controls.Add(this.label6);
            this.Print.Controls.Add(this.numericUpDownPrintShutterStopPixel);
            this.Print.Controls.Add(this.label5);
            this.Print.Controls.Add(this.numericUpDownPrintShutterStartPixel);
            this.Print.Controls.Add(this.label4);
            this.Print.Controls.Add(this.numericUpDownPrintTotalLines);
            this.Print.Controls.Add(this.label3);
            this.Print.Controls.Add(this.numericUpDownPrintLineLength);
            this.Print.Location = new System.Drawing.Point(450, 12);
            this.Print.Name = "Print";
            this.Print.Size = new System.Drawing.Size(200, 255);
            this.Print.TabIndex = 14;
            this.Print.TabStop = false;
            this.Print.Text = "Print";
            // 
            // textBoxPrintCommand
            // 
            this.textBoxPrintCommand.Location = new System.Drawing.Point(6, 229);
            this.textBoxPrintCommand.Name = "textBoxPrintCommand";
            this.textBoxPrintCommand.Size = new System.Drawing.Size(188, 20);
            this.textBoxPrintCommand.TabIndex = 15;
            // 
            // button1
            // 
            this.button1.Enabled = false;
            this.button1.Location = new System.Drawing.Point(119, 193);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 14;
            this.button1.Text = "Send print";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(69, 182);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(63, 13);
            this.label9.TabIndex = 13;
            this.label9.Text = "Color offsett";
            // 
            // numericUpDownPrintColorOffset
            // 
            this.numericUpDownPrintColorOffset.Location = new System.Drawing.Point(6, 178);
            this.numericUpDownPrintColorOffset.Name = "numericUpDownPrintColorOffset";
            this.numericUpDownPrintColorOffset.Size = new System.Drawing.Size(57, 20);
            this.numericUpDownPrintColorOffset.TabIndex = 12;
            this.numericUpDownPrintColorOffset.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(69, 156);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(101, 13);
            this.label8.TabIndex = 11;
            this.label8.Text = "Shutter red stop line";
            // 
            // numericUpDownPrintShutterRedStoplLine
            // 
            this.numericUpDownPrintShutterRedStoplLine.Location = new System.Drawing.Point(6, 152);
            this.numericUpDownPrintShutterRedStoplLine.Name = "numericUpDownPrintShutterRedStoplLine";
            this.numericUpDownPrintShutterRedStoplLine.Size = new System.Drawing.Size(57, 20);
            this.numericUpDownPrintShutterRedStoplLine.TabIndex = 10;
            this.numericUpDownPrintShutterRedStoplLine.Value = new decimal(new int[] {
            8,
            0,
            0,
            0});
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(69, 130);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(101, 13);
            this.label7.TabIndex = 9;
            this.label7.Text = "Shutter red start line";
            // 
            // numericUpDownPrintShutterRedStartLine
            // 
            this.numericUpDownPrintShutterRedStartLine.Location = new System.Drawing.Point(6, 126);
            this.numericUpDownPrintShutterRedStartLine.Name = "numericUpDownPrintShutterRedStartLine";
            this.numericUpDownPrintShutterRedStartLine.Size = new System.Drawing.Size(57, 20);
            this.numericUpDownPrintShutterRedStartLine.TabIndex = 8;
            this.numericUpDownPrintShutterRedStartLine.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(69, 104);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(88, 13);
            this.label6.TabIndex = 7;
            this.label6.Text = "Shutter stop pixel";
            // 
            // numericUpDownPrintShutterStopPixel
            // 
            this.numericUpDownPrintShutterStopPixel.Location = new System.Drawing.Point(6, 100);
            this.numericUpDownPrintShutterStopPixel.Maximum = new decimal(new int[] {
            1200,
            0,
            0,
            0});
            this.numericUpDownPrintShutterStopPixel.Name = "numericUpDownPrintShutterStopPixel";
            this.numericUpDownPrintShutterStopPixel.Size = new System.Drawing.Size(57, 20);
            this.numericUpDownPrintShutterStopPixel.TabIndex = 6;
            this.numericUpDownPrintShutterStopPixel.Value = new decimal(new int[] {
            40,
            0,
            0,
            0});
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(69, 78);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(88, 13);
            this.label5.TabIndex = 5;
            this.label5.Text = "Shutter start pixel";
            // 
            // numericUpDownPrintShutterStartPixel
            // 
            this.numericUpDownPrintShutterStartPixel.Location = new System.Drawing.Point(6, 74);
            this.numericUpDownPrintShutterStartPixel.Maximum = new decimal(new int[] {
            1200,
            0,
            0,
            0});
            this.numericUpDownPrintShutterStartPixel.Name = "numericUpDownPrintShutterStartPixel";
            this.numericUpDownPrintShutterStartPixel.Size = new System.Drawing.Size(57, 20);
            this.numericUpDownPrintShutterStartPixel.TabIndex = 4;
            this.numericUpDownPrintShutterStartPixel.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(69, 52);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(55, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "Total lines";
            // 
            // numericUpDownPrintTotalLines
            // 
            this.numericUpDownPrintTotalLines.Location = new System.Drawing.Point(6, 48);
            this.numericUpDownPrintTotalLines.Maximum = new decimal(new int[] {
            1200,
            0,
            0,
            0});
            this.numericUpDownPrintTotalLines.Name = "numericUpDownPrintTotalLines";
            this.numericUpDownPrintTotalLines.Size = new System.Drawing.Size(57, 20);
            this.numericUpDownPrintTotalLines.TabIndex = 2;
            this.numericUpDownPrintTotalLines.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(69, 26);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(59, 13);
            this.label3.TabIndex = 1;
            this.label3.Text = "Line length";
            // 
            // numericUpDownPrintLineLength
            // 
            this.numericUpDownPrintLineLength.Location = new System.Drawing.Point(6, 22);
            this.numericUpDownPrintLineLength.Maximum = new decimal(new int[] {
            1200,
            0,
            0,
            0});
            this.numericUpDownPrintLineLength.Name = "numericUpDownPrintLineLength";
            this.numericUpDownPrintLineLength.Size = new System.Drawing.Size(57, 20);
            this.numericUpDownPrintLineLength.TabIndex = 0;
            this.numericUpDownPrintLineLength.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.label13);
            this.groupBox4.Controls.Add(this.numericUpDownLaserPulseWidth);
            this.groupBox4.Controls.Add(this.label10);
            this.groupBox4.Controls.Add(this.label11);
            this.groupBox4.Controls.Add(this.label12);
            this.groupBox4.Controls.Add(this.buttonSendLaserDelays);
            this.groupBox4.Controls.Add(this.numericUpDownBlueLaserDelay);
            this.groupBox4.Controls.Add(this.numericUpDownGreenLaserDelay);
            this.groupBox4.Controls.Add(this.numericUpDownRedLaserDelay);
            this.groupBox4.Location = new System.Drawing.Point(250, 70);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(194, 252);
            this.groupBox4.TabIndex = 15;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Laser Control";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(82, 101);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(89, 13);
            this.label13.TabIndex = 10;
            this.label13.Text = "Laser pulse width";
            // 
            // numericUpDownLaserPulseWidth
            // 
            this.numericUpDownLaserPulseWidth.Location = new System.Drawing.Point(6, 96);
            this.numericUpDownLaserPulseWidth.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numericUpDownLaserPulseWidth.Name = "numericUpDownLaserPulseWidth";
            this.numericUpDownLaserPulseWidth.Size = new System.Drawing.Size(70, 20);
            this.numericUpDownLaserPulseWidth.TabIndex = 9;
            this.numericUpDownLaserPulseWidth.Value = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(82, 76);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(56, 13);
            this.label10.TabIndex = 8;
            this.label10.Text = "Blue delay";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(82, 50);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(64, 13);
            this.label11.TabIndex = 7;
            this.label11.Text = "Green delay";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(82, 24);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(55, 13);
            this.label12.TabIndex = 6;
            this.label12.Text = "Red delay";
            // 
            // buttonSendLaserDelays
            // 
            this.buttonSendLaserDelays.Enabled = false;
            this.buttonSendLaserDelays.Location = new System.Drawing.Point(1, 135);
            this.buttonSendLaserDelays.Name = "buttonSendLaserDelays";
            this.buttonSendLaserDelays.Size = new System.Drawing.Size(75, 23);
            this.buttonSendLaserDelays.TabIndex = 3;
            this.buttonSendLaserDelays.Text = "Send delays";
            this.buttonSendLaserDelays.UseVisualStyleBackColor = true;
            this.buttonSendLaserDelays.Click += new System.EventHandler(this.buttonSendLaserDelays_Click);
            // 
            // numericUpDownBlueLaserDelay
            // 
            this.numericUpDownBlueLaserDelay.Location = new System.Drawing.Point(6, 71);
            this.numericUpDownBlueLaserDelay.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numericUpDownBlueLaserDelay.Name = "numericUpDownBlueLaserDelay";
            this.numericUpDownBlueLaserDelay.Size = new System.Drawing.Size(70, 20);
            this.numericUpDownBlueLaserDelay.TabIndex = 2;
            this.numericUpDownBlueLaserDelay.Value = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            // 
            // numericUpDownGreenLaserDelay
            // 
            this.numericUpDownGreenLaserDelay.Location = new System.Drawing.Point(6, 45);
            this.numericUpDownGreenLaserDelay.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numericUpDownGreenLaserDelay.Name = "numericUpDownGreenLaserDelay";
            this.numericUpDownGreenLaserDelay.Size = new System.Drawing.Size(70, 20);
            this.numericUpDownGreenLaserDelay.TabIndex = 1;
            this.numericUpDownGreenLaserDelay.Value = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            // 
            // numericUpDownRedLaserDelay
            // 
            this.numericUpDownRedLaserDelay.Location = new System.Drawing.Point(6, 19);
            this.numericUpDownRedLaserDelay.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numericUpDownRedLaserDelay.Name = "numericUpDownRedLaserDelay";
            this.numericUpDownRedLaserDelay.Size = new System.Drawing.Size(70, 20);
            this.numericUpDownRedLaserDelay.TabIndex = 0;
            this.numericUpDownRedLaserDelay.Value = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            // 
            // timer2
            // 
            this.timer2.Tick += new System.EventHandler(this.timer2_Tick);
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.label16);
            this.groupBox5.Controls.Add(this.buttonAutoGetStatus);
            this.groupBox5.Controls.Add(this.numericUpDownGetStatusUpdate);
            this.groupBox5.Controls.Add(this.buttonGetStatus);
            this.groupBox5.Controls.Add(this.label15);
            this.groupBox5.Controls.Add(this.numericUpDownStatusY);
            this.groupBox5.Controls.Add(this.label14);
            this.groupBox5.Controls.Add(this.numericUpDownStatusX);
            this.groupBox5.Location = new System.Drawing.Point(280, 331);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(199, 190);
            this.groupBox5.TabIndex = 17;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Status";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(87, 109);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(20, 13);
            this.label16.TabIndex = 20;
            this.label16.Text = "ms";
            // 
            // buttonAutoGetStatus
            // 
            this.buttonAutoGetStatus.Enabled = false;
            this.buttonAutoGetStatus.Location = new System.Drawing.Point(7, 127);
            this.buttonAutoGetStatus.Name = "buttonAutoGetStatus";
            this.buttonAutoGetStatus.Size = new System.Drawing.Size(74, 23);
            this.buttonAutoGetStatus.TabIndex = 19;
            this.buttonAutoGetStatus.Text = "AutoGet";
            this.buttonAutoGetStatus.UseVisualStyleBackColor = true;
            this.buttonAutoGetStatus.Click += new System.EventHandler(this.buttonAutoGetStatus_Click);
            // 
            // numericUpDownGetStatusUpdate
            // 
            this.numericUpDownGetStatusUpdate.Location = new System.Drawing.Point(7, 104);
            this.numericUpDownGetStatusUpdate.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numericUpDownGetStatusUpdate.Minimum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numericUpDownGetStatusUpdate.Name = "numericUpDownGetStatusUpdate";
            this.numericUpDownGetStatusUpdate.Size = new System.Drawing.Size(74, 20);
            this.numericUpDownGetStatusUpdate.TabIndex = 18;
            this.numericUpDownGetStatusUpdate.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numericUpDownGetStatusUpdate.ValueChanged += new System.EventHandler(this.numericUpDown3_ValueChanged);
            // 
            // buttonGetStatus
            // 
            this.buttonGetStatus.Enabled = false;
            this.buttonGetStatus.Location = new System.Drawing.Point(6, 69);
            this.buttonGetStatus.Name = "buttonGetStatus";
            this.buttonGetStatus.Size = new System.Drawing.Size(75, 23);
            this.buttonGetStatus.TabIndex = 17;
            this.buttonGetStatus.Text = "GetStatus";
            this.buttonGetStatus.UseVisualStyleBackColor = true;
            this.buttonGetStatus.Click += new System.EventHandler(this.buttonGetStatus_Click);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(82, 47);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(14, 13);
            this.label15.TabIndex = 3;
            this.label15.Text = "Y";
            // 
            // numericUpDownStatusY
            // 
            this.numericUpDownStatusY.Location = new System.Drawing.Point(6, 45);
            this.numericUpDownStatusY.Maximum = new decimal(new int[] {
            2000,
            0,
            0,
            0});
            this.numericUpDownStatusY.Name = "numericUpDownStatusY";
            this.numericUpDownStatusY.Size = new System.Drawing.Size(70, 20);
            this.numericUpDownStatusY.TabIndex = 2;
            this.numericUpDownStatusY.ValueChanged += new System.EventHandler(this.numericUpDownStatusY_ValueChanged);
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(82, 21);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(14, 13);
            this.label14.TabIndex = 1;
            this.label14.Text = "X";
            // 
            // numericUpDownStatusX
            // 
            this.numericUpDownStatusX.Location = new System.Drawing.Point(6, 19);
            this.numericUpDownStatusX.Maximum = new decimal(new int[] {
            2000,
            0,
            0,
            0});
            this.numericUpDownStatusX.Name = "numericUpDownStatusX";
            this.numericUpDownStatusX.Size = new System.Drawing.Size(70, 20);
            this.numericUpDownStatusX.TabIndex = 0;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.textBoxCurrentConfig);
            this.groupBox6.Controls.Add(this.buttonConfigAddSelected);
            this.groupBox6.Controls.Add(this.buttonAddNewConfig);
            this.groupBox6.Controls.Add(this.listBoxConfigNames);
            this.groupBox6.Controls.Add(this.buttonSaveConfig);
            this.groupBox6.Location = new System.Drawing.Point(656, 429);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(326, 162);
            this.groupBox6.TabIndex = 19;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Configuration";
            // 
            // textBoxCurrentConfig
            // 
            this.textBoxCurrentConfig.Location = new System.Drawing.Point(6, 14);
            this.textBoxCurrentConfig.Name = "textBoxCurrentConfig";
            this.textBoxCurrentConfig.Size = new System.Drawing.Size(120, 20);
            this.textBoxCurrentConfig.TabIndex = 23;
            // 
            // buttonConfigAddSelected
            // 
            this.buttonConfigAddSelected.Location = new System.Drawing.Point(132, 43);
            this.buttonConfigAddSelected.Name = "buttonConfigAddSelected";
            this.buttonConfigAddSelected.Size = new System.Drawing.Size(96, 23);
            this.buttonConfigAddSelected.TabIndex = 22;
            this.buttonConfigAddSelected.Text = "LoadSelected";
            this.buttonConfigAddSelected.UseVisualStyleBackColor = true;
            this.buttonConfigAddSelected.Click += new System.EventHandler(this.button3_Click);
            // 
            // buttonAddNewConfig
            // 
            this.buttonAddNewConfig.Location = new System.Drawing.Point(132, 14);
            this.buttonAddNewConfig.Name = "buttonAddNewConfig";
            this.buttonAddNewConfig.Size = new System.Drawing.Size(96, 23);
            this.buttonAddNewConfig.TabIndex = 21;
            this.buttonAddNewConfig.Text = "AddNewConfig";
            this.buttonAddNewConfig.UseVisualStyleBackColor = true;
            this.buttonAddNewConfig.Click += new System.EventHandler(this.buttonAddNewConfig_Click);
            // 
            // listBoxConfigNames
            // 
            this.listBoxConfigNames.FormattingEnabled = true;
            this.listBoxConfigNames.Location = new System.Drawing.Point(6, 37);
            this.listBoxConfigNames.Name = "listBoxConfigNames";
            this.listBoxConfigNames.Size = new System.Drawing.Size(120, 95);
            this.listBoxConfigNames.TabIndex = 20;
            // 
            // buttonSaveConfig
            // 
            this.buttonSaveConfig.Location = new System.Drawing.Point(132, 72);
            this.buttonSaveConfig.Name = "buttonSaveConfig";
            this.buttonSaveConfig.Size = new System.Drawing.Size(96, 23);
            this.buttonSaveConfig.TabIndex = 19;
            this.buttonSaveConfig.Text = "SaveToSelected";
            this.buttonSaveConfig.UseVisualStyleBackColor = true;
            this.buttonSaveConfig.Click += new System.EventHandler(this.buttonSaveConfig_Click_1);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(994, 592);
            this.Controls.Add(this.groupBox6);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.Print);
            this.Controls.Add(this.buttonSendAll);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.buttonSendCustomString);
            this.Controls.Add(this.textBoxCustomString);
            this.Controls.Add(this.numericUpDownAutoTrigger);
            this.Controls.Add(this.buttonAutoTrigger);
            this.Controls.Add(this.buttonLineTrigger);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.buttonClose);
            this.Controls.Add(this.buttonOpen);
            this.Controls.Add(this.textBoxMemo1);
            this.Controls.Add(this.textBoxComPortName);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.groupBox1.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPixelWidth)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPixelDelay)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownLineStartWidth)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownLineStartDelay)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPixelDivideRatio)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPixelFrequency)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownAutoTrigger)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownDelayMs)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownNumberOfLines)).EndInit();
            this.Print.ResumeLayout(false);
            this.Print.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintColorOffset)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintShutterRedStoplLine)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintShutterRedStartLine)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintShutterStopPixel)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintShutterStartPixel)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintTotalLines)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPrintLineLength)).EndInit();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownLaserPulseWidth)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownBlueLaserDelay)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownGreenLaserDelay)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownRedLaserDelay)).EndInit();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownGetStatusUpdate)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownStatusY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownStatusX)).EndInit();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBoxComPortName;
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.TextBox textBoxMemo1;
        private System.Windows.Forms.Button buttonOpen;
        private System.Windows.Forms.Button buttonClose;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button buttonLineStartWidth;
        private System.Windows.Forms.Button buttonLineStartDelay;
        private System.Windows.Forms.Button buttonPixelDivideRatio;
        private System.Windows.Forms.Button buttonPixelFrequency;
        private System.Windows.Forms.NumericUpDown numericUpDownLineStartWidth;
        private System.Windows.Forms.NumericUpDown numericUpDownLineStartDelay;
        private System.Windows.Forms.NumericUpDown numericUpDownPixelDivideRatio;
        private System.Windows.Forms.NumericUpDown numericUpDownPixelFrequency;
        private System.Windows.Forms.Button buttonLineTrigger;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button buttonAutoTrigger;
        private System.Windows.Forms.NumericUpDown numericUpDownAutoTrigger;
        private System.Windows.Forms.Button buttonPixelDelay;
        private System.Windows.Forms.NumericUpDown numericUpDownPixelDelay;
        private System.Windows.Forms.Button buttonPixelWidth;
        private System.Windows.Forms.NumericUpDown numericUpDownPixelWidth;
        private System.Windows.Forms.TextBox textBoxCustomString;
        private System.Windows.Forms.Button buttonSendCustomString;
        private System.Windows.Forms.Button buttonTriggerSeries;
        private System.Windows.Forms.Button buttonStartPrint;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown numericUpDownDelayMs;
        private System.Windows.Forms.NumericUpDown numericUpDownNumberOfLines;
        private System.Windows.Forms.Button buttonSendAll;
        private System.Windows.Forms.GroupBox Print;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.NumericUpDown numericUpDownPrintColorOffset;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.NumericUpDown numericUpDownPrintShutterRedStoplLine;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown numericUpDownPrintShutterRedStartLine;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.NumericUpDown numericUpDownPrintShutterStopPixel;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown numericUpDownPrintShutterStartPixel;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.NumericUpDown numericUpDownPrintTotalLines;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown numericUpDownPrintLineLength;
        private System.Windows.Forms.TextBox textBoxPrintCommand;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.RadioButton radioButtonLcosRealTrigger;
        private System.Windows.Forms.RadioButton radioButtonInternalSimulator;
        private System.Windows.Forms.Button buttonPixTrigSend;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.NumericUpDown numericUpDownBlueLaserDelay;
        private System.Windows.Forms.NumericUpDown numericUpDownGreenLaserDelay;
        private System.Windows.Forms.NumericUpDown numericUpDownRedLaserDelay;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Button buttonSendLaserDelays;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.NumericUpDown numericUpDownLaserPulseWidth;
        private System.Windows.Forms.Button buttonGetAll;
        private System.Windows.Forms.Button buttonTriggerGet;
        private System.Windows.Forms.Button buttonPixelDelayGet;
        private System.Windows.Forms.Button buttonPixelWidthGet;
        private System.Windows.Forms.Button buttonLineStartWidthGet;
        private System.Windows.Forms.Button buttonLineStartDelayGet;
        private System.Windows.Forms.Button buttonPixDivRatioGet;
        private System.Windows.Forms.Button buttonPixFreqGet;
        private System.Windows.Forms.Timer timer2;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Button buttonAutoGetStatus;
        private System.Windows.Forms.NumericUpDown numericUpDownGetStatusUpdate;
        private System.Windows.Forms.Button buttonGetStatus;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.NumericUpDown numericUpDownStatusY;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.NumericUpDown numericUpDownStatusX;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Button buttonSaveConfig;
        private System.Windows.Forms.Button buttonConfigAddSelected;
        private System.Windows.Forms.Button buttonAddNewConfig;
        private System.Windows.Forms.ListBox listBoxConfigNames;
        private System.Windows.Forms.TextBox textBoxCurrentConfig;
    }
}

