/***************************************************************************
 *   Copyright (C) 2003 by Matthias H. Hennig                              *
 *   hennig@cn.stir.ac.uk                                                  *
 *   Copyright (C) 2005-2020 by Bernd Porr                                 *
 *   mail@berndporr.me.uk                                                  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QDebug> 

#include "attys-ecg.h"
#include "radial_gradient.h"
//#include "wigglywidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QTextStream>
#include <QComboBox>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QGraphicsView>
#include <QLineEdit>


#include "dialog.h"

#include "AttysComm.h"
#include "AttysScan.h"





#define NOTCH_BW 2.5

MainWindow::MainWindow(QWidget *parent) :
	QWidget(parent) {

#ifdef __APPLE__
	char styleSheet[] = "";
	char styleSheetCombo[] = "";
	char styleSheetGroupBox[] = "";
	char styleSheetButton[] = "";
#else
	setStyleSheet("background-color:rgb(179,160,225);color: white;");
  //  setStyleSheet("background-image:url(sky.jpg); background-position: center;color: white;"); 
//	setStyleSheet("background-image:url(sky.jpg); background-position: center;color: white;");
    //setStyleSheet("background-color: rgba(0,0,0,0);color: white;"); //child widgets
	setAutoFillBackground( true );
//	this->setObjectName("MainWindow");
//	this->setStyleSheet("#MainWindow{background-image:url(sky.jpg); background-position: center;color: black;}");
//	this->setAutoFillBackground( true );
	char styleSheet[] = "padding:0px;margin:0px;border:0px;";
	char styleSheetCombo[] = "padding:0px;margin:0px;border:0px;margin-right:2px;font: 16px";
	char styleSheetGroupBox[] = "padding:1px;margin:0px;border:0px";
	char styleSheetButton[] = "background-color: grey; border: none; outline: none; border-width: 0px; font: 16px; padding: 5px; color: white;";
#endif

	attysScan.getAttysComm(0)->setAdc_samplingrate_index(AttysComm::ADC_RATE_250HZ);
	sampling_rate = attysScan.getAttysComm(0)->getSamplingRateInHz();

	rr_det1 = new ECG_rr_det();
	rr_det2 = new ECG_rr_det();
	bPMCallback1 = new BPMCallback(this,rr_det1);
	bPMCallback2 = new BPMCallback(this,rr_det2);
	rr_det1->setRrListener(bPMCallback1);
	rr_det2->setRrListener(bPMCallback2);

	attysCallback = new AttysCallback(this);
	attysScan.getAttysComm(0)->registerCallback(attysCallback);

	// set the PGA to max gain
	attysScan.getAttysComm(0)->setAdc0_gain_index(AttysComm::ADC_GAIN_6);

	// connect both channels so that we can use just 3 electrodes
	attysScan.getAttysComm(0)->setAdc0_mux_index(AttysComm::ADC_MUX_ECG_EINTHOVEN);
	attysScan.getAttysComm(0)->setAdc1_mux_index(AttysComm::ADC_MUX_ECG_EINTHOVEN);

	attysScan.getAttysComm(0)->setAccel_full_scale_index(AttysComm::ACCEL_16G);
	minAcc = -16 * attysScan.getAttysComm(0)->oneG;
	maxAcc = -minAcc;

	// 50Hz or 60Hz mains notch filter
	// we set it to 50Hz initially
	setNotch(50);

	// highpass
	iirhp1.setup(sampling_rate, HP_CUTOFF);
	iirhp2.setup(sampling_rate, HP_CUTOFF);

	// highpass filters for the adaptive filter
	for(int i=0;i<3;i++) {
		iirAcc[i].setup(sampling_rate, HP_CUTOFF);
	}
		
	// lms
	lms1 = new Fir1*[3];
	lms2 = new Fir1*[3];
	for(int i=0;i<3;i++) {
		lms1[i] = new Fir1(LMS_COEFF);
		lms1[i]->setLearningRate(LEARNING_RATE);
		lms2[i] = new Fir1(LMS_COEFF);
		lms2[i]->setLearningRate(LEARNING_RATE);
	}

	QHBoxLayout *mainLayout = new QHBoxLayout(this);



	QVBoxLayout *controlLayout = new QVBoxLayout;
	controlLayout->setAlignment(Qt::AlignTop);

	mainLayout->addLayout(controlLayout);



	QVBoxLayout *plotLayout = new QVBoxLayout;

	mainLayout->addLayout(plotLayout);


	mainLayout->setStretchFactor(controlLayout, 1);
	mainLayout->setStretchFactor(plotLayout, 10);

	double maxTime = 5;
	double minRange = -2;
	double maxRange = 2;
	const char* xlabel = "t/s";
	QDesktopWidget *mydesktop = QApplication::desktop();
	int h = mydesktop->height() / 5;
	int w = mydesktop->width();
/*	
	dataPlotI = new DataPlot(maxTime,
		sampling_rate,
		minRange,
		maxRange,
		"Einthoven I",
		xlabel,
		"I/mV",
		this);
	dataPlotI->setMaximumSize(w, h);
	dataPlotI->setStyleSheet(styleSheet);
	plotLayout->addWidget(dataPlotI);
	dataPlotI->show();

	dataPlotII = new DataPlot(maxTime,
		sampling_rate,
		minRange,
		maxRange,
		"Einthoven II",
		xlabel,
		"II/mV",
		this);
	dataPlotII->setMaximumSize(w, h);
	dataPlotII->setStyleSheet(styleSheet);
	plotLayout->addWidget(dataPlotII);
	dataPlotII->show();
*/
	dataPlotIII = new DataPlot(maxTime,
		sampling_rate,
		minRange,
		maxRange,
		"Einthoven III",
		xlabel,
		"III/mV",
		this);
	dataPlotIII->setMaximumSize(w, h);
	dataPlotIII->setStyleSheet(styleSheet);
	plotLayout->addWidget(dataPlotIII);
	dataPlotIII->show();

	dataPlotBPM = new DataPlot(120,
		1,
		0,
		200,
		"Heartrate",
		"RR number",
		"RR/bpm",
		this);
	dataPlotBPM->setMaximumSize(w, h);
	dataPlotBPM->setStyleSheet(styleSheet);
	plotLayout->addWidget(dataPlotBPM);
	dataPlotBPM->show();

	dataPlotHRV = new DataPlot(120,
		1,
		0,
		100,
		"Heart Rate Variability",
		"RR number",
		"HRV RMS (s)",
		this);
	dataPlotHRV->setMaximumSize(w, h);
	dataPlotHRV->setStyleSheet(styleSheet);
	plotLayout->addWidget(dataPlotHRV);
	dataPlotHRV->show();	



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	QVBoxLayout *circleLayout = new QVBoxLayout;
	circleLayout->setAlignment(Qt::AlignRight);

	circle = new RadialGradient();
	circleLayout->addWidget(circle);


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	QVBoxLayout *manifestLayout = new QVBoxLayout;
	manifestLayout->setAlignment(Qt::AlignRight);

	manifest = new Dialog();
	manifestLayout->addWidget(manifest);

/*
      WigglyWidget *manifest = new WigglyWidget;
      QLineEdit *lineEdit = new QLineEdit;

      QVBoxLayout *layout = new QVBoxLayout(this);
      layout->addWidget(manifest);
      layout->addWidget(lineEdit);

      connect(lineEdit, &QLineEdit::textChanged, manifest, &WigglyWidget::setText);
      lineEdit->setText(tr("Hello world!"));

      setWindowTitle(tr("Manifest"));
      resize(360, 145);
*/

	//manifest = new Dialog();
	//manifestLayout->addWidget(manifest);



	/*---- Buttons ----*/

	QGroupBox   *ECGfunGroup = new QGroupBox(this);
	ECGfunGroup->setStyleSheet(styleSheetGroupBox);
	QVBoxLayout *ecgFunLayout = new QVBoxLayout;
	ecgFunLayout->setAlignment(Qt::AlignTop);
	ECGfunGroup->setLayout(ecgFunLayout);
	ECGfunGroup->setAlignment(Qt::AlignJustify);
	ECGfunGroup->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	controlLayout->addWidget(ECGfunGroup);

	notchFreq = new QComboBox(ECGfunGroup);
	notchFreq->setStyleSheet(styleSheetCombo);
	notchFreq->addItem(tr("50Hz bandstop"));
	notchFreq->addItem(tr("60Hz bandstop"));
	ecgFunLayout->addWidget(notchFreq);
	connect(notchFreq, SIGNAL(currentIndexChanged(int)), SLOT(slotSelectNotchFreq(int)));

	yRange = new QComboBox(ECGfunGroup);
	yRange->setStyleSheet(styleSheetCombo);
	yRange->addItem(tr("1mV"),1.0);
	yRange->addItem(tr("1.5mV"),1.5);
	yRange->addItem(tr("2mV"),2.0);
	ecgFunLayout->addWidget(yRange);
	connect(yRange, SIGNAL(currentIndexChanged(int)), SLOT(slotSelectYrange(int)));
	yRange->setCurrentIndex(2);

	lmsCheckBox = new QCheckBox( "artefact removal" );
	ecgFunLayout->addWidget(lmsCheckBox);

	QGroupBox   *ECGfileGroup = new QGroupBox(this);
	ECGfileGroup->setStyleSheet(styleSheetGroupBox);
	QVBoxLayout *ecgfileLayout = new QVBoxLayout;
	ecgfileLayout->setAlignment(Qt::AlignTop);
	ECGfileGroup->setLayout(ecgfileLayout);
	ECGfileGroup->setAlignment(Qt::AlignJustify);
	ECGfileGroup->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	controlLayout->addWidget(ECGfileGroup);

	saveECG = new QPushButton(ECGfileGroup);
	saveECG->setStyleSheet(styleSheetButton);
	saveECG->setText("Filename");
	ecgfileLayout->addWidget(saveECG);
	connect(saveECG, SIGNAL(clicked()), SLOT(slotSaveECG()));

	recordECG = new QPushButton(ECGfileGroup);
	recordECG->setStyleSheet(styleSheetButton);
	recordECG->setText("Record on/off");
	recordECG->setCheckable(true);
	recordECG->setEnabled(false);
	ecgfileLayout->addWidget(recordECG);
	connect(recordECG, SIGNAL(clicked()), SLOT(slotRecordECG()));
	statusLabel = new QLabel(ECGfileGroup);
	ecgfileLayout->addWidget(statusLabel);

//start breathe button ################################################################################

	QGroupBox   *ECGstartGroup = new QGroupBox(this);
	ECGstartGroup->setStyleSheet(styleSheetGroupBox);
	QVBoxLayout *ecgstartLayout = new QVBoxLayout;
	ecgstartLayout->setAlignment(Qt::AlignTop);
	ECGstartGroup->setLayout(ecgstartLayout);
	ECGstartGroup->setAlignment(Qt::AlignJustify);
	ECGstartGroup->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	controlLayout->addWidget(ECGstartGroup);

	START = new QPushButton(ECGstartGroup);
	START->setStyleSheet(styleSheetButton);
	START->setText("Breathe");
	ecgstartLayout->addWidget(START);
	connect(START, SIGNAL(clicked()), SLOT(slotSTART()));
	controlLayout->addStretch(0);

//start manifest button ################################################################################

	QGroupBox   *ECGstartmanifestGroup = new QGroupBox(this);
	ECGstartmanifestGroup->setStyleSheet(styleSheetGroupBox);
	QVBoxLayout *ecgstartmanifestLayout = new QVBoxLayout;
	ecgstartmanifestLayout->setAlignment(Qt::AlignTop);
	ECGstartmanifestGroup->setLayout(ecgstartmanifestLayout);
	ECGstartmanifestGroup->setAlignment(Qt::AlignJustify);
	ECGstartmanifestGroup->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	controlLayout->addWidget(ECGstartmanifestGroup);

	STARTmanifest = new QPushButton(ECGstartmanifestGroup);
	STARTmanifest->setStyleSheet(styleSheetButton);
	STARTmanifest->setText("Affirmations");
	ecgstartmanifestLayout->addWidget(STARTmanifest);
	connect(STARTmanifest, SIGNAL(clicked()), SLOT(slotSTARTmanifest()));
	controlLayout->addStretch(0);


// BPM ############################################################################################

	QGroupBox   *ECGbpmGroup = new QGroupBox(this);
	ECGbpmGroup->setStyleSheet(styleSheetGroupBox);
	QVBoxLayout *ecgbpmLayout = new QVBoxLayout;
	ecgbpmLayout->setAlignment(Qt::AlignTop);
	ECGbpmGroup->setLayout(ecgbpmLayout);
	ECGbpmGroup->setAlignment(Qt::AlignJustify);
	ECGbpmGroup->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	controlLayout->addWidget(ECGbpmGroup);

	bpmLabel = new QLabel(ECGbpmGroup);
	ecgbpmLayout->addWidget(bpmLabel);
	bpmLabel->setFont(QFont("Courier", 16));
	bpmLabel->setText("--- BPM");

	clearBPM = new QPushButton(ECGbpmGroup);
	clearBPM->setStyleSheet(styleSheetButton);
	clearBPM->setText("clear BPM plot");
	ecgbpmLayout->addWidget(clearBPM);
	connect(clearBPM, SIGNAL(clicked()), SLOT(slotClearBPM()));

//HRV ###########################################################################################
	QGroupBox   *ECGhrvGroup = new QGroupBox(this); 
	ECGhrvGroup->setStyleSheet(styleSheetGroupBox);
	QVBoxLayout *ecghrvLayout = new QVBoxLayout;
	ecghrvLayout->setAlignment(Qt::AlignTop);
	ECGhrvGroup->setLayout(ecghrvLayout);
	ECGhrvGroup->setAlignment(Qt::AlignJustify);
	ECGhrvGroup->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	controlLayout->addWidget(ECGhrvGroup);

	hrvLabel = new QLabel(ECGhrvGroup);
	ecghrvLayout->addWidget(hrvLabel);
	hrvLabel->setFont(QFont("Courier", 16));
	hrvLabel->setText("--- HRV RMS (ms)");

	clearHRV = new QPushButton(ECGhrvGroup);
	clearHRV->setStyleSheet(styleSheetButton);
	clearHRV->setText("clear HRV plot");
	ecghrvLayout->addWidget(clearHRV);
	connect(clearHRV, SIGNAL(clicked()), SLOT(slotClearHRV()));

	// Generate timer event every 50ms
	startTimer(50);

	attysScan.getAttysComm(0)->start();


//#######################################################################
    //Dialog manifest;
}

MainWindow::~MainWindow()
{
	attysScan.getAttysComm(0)->unregisterCallback();
	attysScan.getAttysComm(0)->quit();
	if (ecgFile) {
		fclose(ecgFile);
	}
}

void MainWindow::slotSaveECG()
{
	if (recordingOn) return;
	QString filters(tr("tab separated values (*.tsv)"));
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(filters);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
	
	if (dialog.exec()) {
		fileName = dialog.selectedFiles()[0];
		if (!fileName.isNull()) {
			QString extension = dialog.selectedNameFilter();
			extension = extension.mid(extension.indexOf("."), 4);
			if (fileName.indexOf(extension) == -1) {
				fileName = fileName + extension;
			}
			ecgFile = fopen(fileName.toLocal8Bit().constData(), "wt");
			if (ecgFile) {
				recordECG->setEnabled(true);
				start_time = 0;
			}
			else {
				QMessageBox msgbox;
				msgbox.setText("Could not save to "+ fileName);
				msgbox.exec();
				fileName = "";
			}
		}
	}
}

void MainWindow::setNotch(double f) {
	iirnotch1.setup(sampling_rate, f, NOTCH_BW);
	iirnotch2.setup(sampling_rate, f, NOTCH_BW);
}

void MainWindow::slotSelectNotchFreq(int f) {
	switch (f) {
	case 0:
		setNotch(50);
		break;
	case 1:
		setNotch(60);
		break;
	}
}

void MainWindow::slotSelectYrange(int r) {
	double y = 1.0;
	switch (r) {
	case 0:
		y = 1.0;
		break;
	case 1:
		y = 1.5;
		break;
	case 2:
		y = 2.0;
		break;
	}
//	dataPlotI->setYScale(-y, y);
//	dataPlotII->setYScale(-y, y);
	dataPlotIII->setYScale(-y, y);
}

void MainWindow::slotSTART()
{   
  //declare here maybe??
  circle->resize(500, 500);  
  circle->setWindowTitle("ATTYS ECG breathe");
  dataPlotIII->hide();
  circle->show();
  //std::system("./windows/radialgradient/radialgradient");

    
}

void MainWindow::slotSTARTmanifest()
{   
	dataPlotIII->hide();
	manifest->show();
}


void MainWindow::slotClearBPM()
{
	dataPlotBPM->reset();
}

void MainWindow::slotClearHRV()
{
	dataPlotHRV->reset();
}


void MainWindow::slotRecordECG()
{
	if (recordingOn && (!(recordECG->isChecked()))) {
		recordECG->setEnabled(false);
	}
	recordingOn = recordECG->isChecked();
	sampleNumber = 0;
	start_time = 0;
	if (recordingOn) {
		setWindowTitle(QString("attys-ecg: recording ") + fileName);
	}
	else {
		setWindowTitle(QString("attys-ecg"));
		saveFileMutex.lock();
		if (ecgFile) {
			fclose(ecgFile);
			ecgFile = NULL;
		}
		saveFileMutex.unlock();
	}
}

void MainWindow::timerEvent(QTimerEvent *) {
//	dataPlotI->replot();
//	dataPlotII->replot();
	dataPlotIII->replot();
	dataPlotBPM->replot();
	dataPlotHRV->replot();
//	dataPlotAccX->replot();
//	dataPlotAccY->replot();
//	dataPlotAccZ->replot();
	if (recordingOn) {
		QString tRecString = QString::number(current_secs);
		statusLabel->setText("Rec: t=" + tRecString+" sec");
	}
	else {
		if (attysScan.getAttysComm(0)->getIsCharging()) {
			statusLabel->setText("! Disconnect the charger before !\n! attaching any electrodes !");
		} else {
			statusLabel->setText("");
		}
	}
}


void MainWindow::hasData(double t, float *sample)
{
	double y1 = sample[AttysComm::INDEX_Analogue_channel_1];
	double y2 = sample[AttysComm::INDEX_Analogue_channel_2];

	accX = sample[AttysComm::INDEX_Acceleration_X];
	accY = sample[AttysComm::INDEX_Acceleration_Y];
	accZ = sample[AttysComm::INDEX_Acceleration_Z];

	// removing 50Hz notch
	y1 = iirnotch1.filter(y1);
	y2 = iirnotch2.filter(y2);

	// highpass filtering of the data
	y1 = iirhp1.filter(y1);
	y2 = iirhp2.filter(y2);

	// do we do adaptive filtering
	if (lmsCheckBox->isChecked()) {
		// fixme: FIR / delay for y1 / y2
		
		// adaptive filtering
		double corr1 = 0;
		double corr2 = 0;
		for(int i=0;i<3;i++) {
			double acc = sample[AttysComm::INDEX_Acceleration_X+i];
			acc = iirAcc[i].filter(acc);
			// fprintf(stderr,"%f\n",acc);
			corr1 += lms1[i]->filter(acc);
			corr2 += lms2[i]->filter(acc);
		}

		//printf("%e %e %e %e\n",y1,y2,corr1,corr2);

		II = y1 - corr1;
		III = y2 - corr2;

		for(int i=0;i<3;i++) {
			if (fabs(II)>FAULT_THRES) {
				lms1[i]->reset();
				lms1[i]->zeroCoeff();
			} else {
				lms1[i]->lms_update(II);
			}
			if (fabs(III)>FAULT_THRES) {
				lms2[i]->reset();
				lms2[i]->zeroCoeff();
			} else {
				lms2[i]->lms_update(III);
			}
		}
	} else {
		// passthrough
		II = y1;
		III = y2;
	}		

	I = II - III;

	rr_det1->detect(I);
	rr_det2->detect(II);
	
	aVR = III / 2 - II;
	aVL = II / 2 - III;
	aVF = II / 2 + III / 2;

	// plot the data
	const double scaling = 1000;
//	dataPlotI->setNewData(I*scaling);
//	dataPlotII->setNewData(II*scaling);
	dataPlotIII->setNewData(III*scaling);

//	dataPlotAccX->setNewData(accX);
//	dataPlotAccY->setNewData(accY);
//	dataPlotAccZ->setNewData(accZ);

	// Are we recording?
	saveFileMutex.lock();
	if (ecgFile && (recordECG->isChecked()))
	{
		char s = '\t';
		if (0 == start_time) {
			start_time = t;
		}
		current_secs = (int)round(t - start_time);
		fprintf(ecgFile, "%e%c", t - start_time, s);
	//	fprintf(ecgFile, "%e%c", I, s);
	//	fprintf(ecgFile, "%e%c", II, s);
		fprintf(ecgFile, "%e%c", III, s);
	//	fprintf(ecgFile, "%e%c", aVR, s);
	//	fprintf(ecgFile, "%e%c", aVL, s);
	//	fprintf(ecgFile, "%e%c", aVF, s);
		fprintf(ecgFile, "%f%c", bpm_1, s);
		bpm_1 = 0;
	//	fprintf(ecgFile, "%e%c", accX, s);
	//	fprintf(ecgFile, "%e%c", accY, s);
	//	fprintf(ecgFile, "%e\n", accZ);
	}
	saveFileMutex.unlock();
    
	sampleNumber++;
}

void MainWindow::hasRpeak(ECG_rr_det* det,
			   long,
			   float t,
			   float newBpm,
			   double,
			   double) {

	if (rr_det1-> getAmplitude() > rr_det2->getAmplitude()) {
		// channel one stronger
		//	printf("rr_det_+2= %f\n",newBpm);
		rr_det_channel = 1;
		if (det == rr_det1) {
			float bpm_1 = newBpm;
		}
	} else {
		// channel two stronger
		rr_det_channel = 2;
		if (det == rr_det2) {
		//	printf("rr_det_2= %f\n",newBpm);
			float bpm_1 = newBpm;
		}
	}
	float bpm_1 = newBpm;

	//printf("t:%f  t_prev:%f\n",t,t_prev);

	if (t_prev>0){

		//RMS hrv from difference between interval t and last t
		RRdiff = t-t_prev;
		
		accumulator = accumulator + (RRdiff*RRdiff); //add interval difference^2 to sum
		hrv = sqrt((accumulator/n));
		//printf("\nRRDiff: %f  acc:%f  n:%d  hrv:%f", RRdiff, accumulator, n, hrv);
		//printf("\n hrv:%f\n",hrv);
		n++;
	}
	t_prev = t;

	//plot and display

	int   ms = static_cast<int>(hrv*1000);
//	printf("ms:%d\n",ms);

	dataPlotBPM->setNewData(bpm_1);
//	printf("original bpm_1: %f\n",bpm_1);
	dataPlotHRV->setNewData(ms);
//	printf("originaldataplothrv1: %f\n",bpm_1);
//	printf("originaldataplothrv2: %f\n",bpm_1);
//	printf("originaldataplothrv3: %f\n",bpm_1);
//	printf("originaldataplothrv4: %f\n",bpm_1);
	char tmp[16];
	char tmp1[32];
	float tempBpm=bpm_1;
//	printf("temp: %f  Bpm_1 : %f\n",tempBpm,bpm_1);
//	QDebug() //wtf happens here??
//	printf("justb4converted: %f\n",tempBpm);

	int   tempBPMMM = static_cast<int>(tempBpm);


//	bpm_1 = (int)bpm_1;
//	printf("converted: %d\n",tempBPMMM);

	sprintf(tmp,"%03d/%d BPM",tempBPMMM,rr_det_channel);
	sprintf(tmp1,"HRV: %d ms ",ms); 

	circle->getRadius(tempBPMMM);



	if (ecgFile && (recordECG->isChecked())) {
		fflush(ecgFile);
	}

	bpmLabel->setText(tmp);
	hrvLabel->setText(tmp1);

}
