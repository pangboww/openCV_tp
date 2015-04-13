#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QObject>
#include <QFileDialog>
#include <QDebug>
#include <assert.h>

#include "QcvMatWidgetImage.h"
#include "QcvMatWidgetLabel.h"
#include "QcvMatWidgetGL.h"

/*
 * MainWindow constructor.
 * @param capture the capture QObject to capture frames from devices
 * or video files
 * @param processor Fourier transform and filter processor
 * @param parent parent widget
 */
MainWindow::MainWindow(QcvVideoCapture * capture,
					   QcvSimpleDFT * processor,
					   QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	capture(capture),
	processor(processor),
	preferredWidth(341),
	preferredHeight(256)
{
	ui->setupUi(this);
	ui->scrollAreaSource->setBackgroundRole(QPalette::Mid);
	ui->scrollAreaSpectrum->setBackgroundRole(QPalette::Mid);

	// ------------------------------------------------------------------------
	// Assertions
	// ------------------------------------------------------------------------
	assert(capture != NULL);

	assert(processor != NULL);

	// ------------------------------------------------------------------------
	// Special widgets initialisation
	// ------------------------------------------------------------------------
	// Replace QcvMatWidget instances with QcvMatWidgetImage instances
	// sets image widget sources for the first time
	// connects processor->update to image Widgets->updated
	// connects processor->image changed to image widgets->setSourceImage
	setRenderingMode(RENDER_IMAGE);

	ui->labelFFTSizeValue->setText(QString::number(processor->getOptimalDftSize()));

	// ------------------------------------------------------------------------
	// rest of Signal/Slot connections
	// ------------------------------------------------------------------------
	// processor->sendText --> labelFFTSizeValue->setText when source image
	// changes, fft size might also change

	connect(processor, SIGNAL(sendText(QString)),
			ui->labelFFTSizeValue, SLOT(setText(QString)));

	// Capture, processor and this messages to status bar
	connect(capture, SIGNAL(messageChanged(QString,int)),
			ui->statusBar, SLOT(showMessage(QString,int)));

	connect(processor, SIGNAL(sendMessage(QString,int)),
			ui->statusBar, SLOT(showMessage(QString,int)));

	connect(this, SIGNAL(sendMessage(QString,int)),
			ui->statusBar, SLOT(showMessage(QString,int)));

	// When Processor source image changes, some attributes are reinitialised
	// So we have to set them up again according to current UI values
	connect(processor, SIGNAL(imageChanged()),
			this, SLOT(setupProcessorFromUI()));

	// Connects UI requests to capture
	connect(this, SIGNAL(sizeChanged(const QSize &)),
			capture, SLOT(setSize(const QSize &)));
	connect(this, SIGNAL(openDevice(int,uint,uint)),
			capture, SLOT(open(int,uint,uint)));
	connect(this, SIGNAL(openFile(QString,uint,uint)),
			capture, SLOT(open(QString,uint,uint)));
	connect(this, SIGNAL(flipVideo(bool)), capture, SLOT(setFlipVideo(bool)));
	connect(this, SIGNAL(grayImage(bool)), capture, SLOT(setGray(bool)));

	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// UI setup according to capture options
	// ------------------------------------------------------------------------
	// Sets size radioButton states
	if (capture->isResized())
	{
		/*
		 * Initial Size radio buttons configuration
		 */
		ui->radioButtonOrigSize->setChecked(false);
		ui->radioButtonCustomSize->setChecked(true);
		/*
		 * Initial Size menu items configuration
		 */
		ui->actionOriginalSize->setChecked(false);
		ui->actionConstrainedSize->setChecked(true);

		QSize size = capture->getSize();
		qDebug("Capture->size is %dx%d", size.width(), size.height());
		preferredWidth = size.width();
		preferredHeight = size.height();
	}
	else
	{
		/*
		 * Initial Size radio buttons configuration
		 */
		ui->radioButtonCustomSize->setChecked(false);
		ui->radioButtonOrigSize->setChecked(true);

		/*
		 * Initial Size menu items configuration
		 */
		ui->actionConstrainedSize->setChecked(false);
		ui->actionOriginalSize->setChecked(true);
	}

	// Sets spinboxes preferred size
	ui->spinBoxWidth->setValue(preferredWidth);
	ui->spinBoxHeight->setValue(preferredHeight);

	// Sets flipCheckbox and menu item states
	bool flipped = capture->isFlipVideo();
	ui->actionFlip->setChecked(flipped);
	ui->checkBoxFlip->setChecked(flipped);

	// Sets grayCheckbox and menu item states
	bool gray = capture->isGray();
	ui->actionGray->setChecked(gray);
	ui->checkBoxGray->setChecked(gray);

	// ------------------------------------------------------------------------
	// UI setup according to DFTProcessor options
	// ------------------------------------------------------------------------
	// Setting up log scale spinbox value and boundaries
	ui->spinBoxMag->setValue((int)processor->getLogScaleFactor());
	ui->spinBoxMag->setMinimum((int)processor->minLogScaleFactor);
	ui->spinBoxMag->setMaximum((int)processor->maxLogScaleFactor);
}

/*
 * MainWindow destructor
 */
MainWindow::~MainWindow()
{
	delete ui;
}

/*
 * Menu action when Sources->camera 0 is selected
 * Sets capture to open device 0. If device is not available
 * menu item is set to inactive.
 */
void MainWindow::on_actionCamera_0_triggered()
{
	int width = 0;
	int height = 0;

	if (ui->radioButtonCustomSize->isChecked())
	{
		width = preferredWidth;
		height = preferredHeight;
	}

	qDebug("Opening device 0 ...");
//	if (!capture->open(0, width, height))
//	{
//		qWarning("Unable to open device 0");
//		// disable menu item if camera 0 does not exist
//		ui->actionCamera_0->setDisabled(true);
//	}

	emit openDevice(0, width, height);
}

/*
 * Menu action when Sources->camera 1 is selected
 * Sets capture to open device 0. If device is not available
 * menu item is set to inactive
 */
void MainWindow::on_actionCamera_1_triggered()
{
	int width = 0;
	int height = 0;

	if (ui->radioButtonCustomSize->isChecked())
	{
		width = preferredWidth;
		height = preferredHeight;
	}

	qDebug("Opening device 1 ...");
//	if (!capture->open(1, width, height))
//	{
//		qWarning("Unable to open device 1");
//		// disable menu item if camera 1 does not exist
//		ui->actionCamera_1->setDisabled(true);
//	}

	emit openDevice(1, width, height);
}

/*
 * Menu action when Sources->file is selected.
 * Opens file dialog and tries to open selected file (is not empty),
 * then sets capture to open the selected file
 */
void MainWindow::on_actionFile_triggered()
{
	int width = 0;
	int height = 0;

	if (ui->radioButtonCustomSize->isChecked())
	{
		width = preferredWidth;
		height = preferredHeight;
	}

	QString fileName =
	QFileDialog::getOpenFileName(this,
								 tr("Open Video"),
								 "./",
								 tr("Video Files (*.avi *.mkv *.mp4 *.m4v)"),
								 NULL,
								 QFileDialog::ReadOnly);

	qDebug("Opening file %s ...", fileName.toStdString().c_str());

	if (fileName.length() > 0)
	{
//		if (!capture->open(fileName, width, height))
//		{
//			qWarning("Unable to open device file : %s",
//					 fileName.toStdString().c_str());
//		}

//		setupProcessorFromUI(); // Should already be called by imageChanged signal

		emit openFile(fileName, width, height);
	}
	else
	{
		qWarning("empty file name");
	}
}

/*
 * Menu action to qui application
 */
void MainWindow::on_actionQuit_triggered()
{
	this->close();
}

/*
 * Menu action when flip image is selected.
 * Sets capture to change flip status which leads to reverse
 * image horizontally
 */
void MainWindow::on_actionFlip_triggered()
{
	// capture->setFlipVideo(!capture->isFlipVideo());
	emit flipVideo(!capture->isFlipVideo());
	/*
	 * There is no need to update ui->checkBoxFlip since it is connected
	 * to ui->actionFlip through signals/slots
	 */
}

/*
 * Menu action when gray image is selected.
 * Sets capture to change gray status which leads convert captured image
 * to gray or not
 */
void MainWindow::on_actionGray_triggered()
{
	bool isGray = !capture->isGray();

	// capture->setGray(isGray);
	emit grayImage(isGray);
}

/*
 * Menu action when original image size is selected.
 * Sets capture not to resize image
 */
void MainWindow::on_actionOriginalSize_triggered()
{

	ui->actionConstrainedSize->setChecked(false);

	// capture->setSize(0, 0);
	emit sizeChanged(QSize(0, 0));
}

/*
 * Menu action when constrained image size is selected.
 * Sets capture resize to preferred width and height
 */
void MainWindow::on_actionConstrainedSize_triggered()
{
	ui->actionOriginalSize->setChecked(false);

	// capture->setSize(preferredWidth, preferredHeight);
	emit sizeChanged(QSize(preferredWidth, preferredHeight));
}

/*
 * Changes widgetImage nature according to desired rendering mode.
 * Possible values for mode are:
 * 	- IMAGE: widgetImage is assigned to a QcvMatWidgetImage instance
 * 	- PIXMAP: widgetImage is assigned to a QcvMatWidgetLabel instance
 * 	- GL: widgetImage is assigned to a QcvMatWidgetGL instance
 * @param mode
 */
void MainWindow::setRenderingMode(const RenderMode mode)
{
	// Disconnect signals from slots first
	disconnect(processor, SIGNAL(updated()),
			   ui->sourceImage, SLOT(update()));
	disconnect(processor, SIGNAL(updated()),
			   ui->spectrumImage, SLOT(update()));

	disconnect(processor, SIGNAL(squareImageChanged(Mat*)),
			   ui->sourceImage, SLOT(setSourceImage(Mat*)));
	disconnect(processor, SIGNAL(spectrumImageChanged(Mat*)),
			   ui->spectrumImage, SLOT(setSourceImage(Mat*)));

	// remove widgets in scroll areas
	QWidget * wSource = ui->scrollAreaSource->takeWidget();
	QWidget * wSpectrum = ui->scrollAreaSpectrum->takeWidget();

	if ((wSource == ui->sourceImage) &&
		(wSpectrum == ui->spectrumImage))
	{
		// delete removed widgets
		delete ui->sourceImage;
		delete ui->spectrumImage;

		// create new widget
		Mat * sourceMat = processor->getImagePtr("square");
		Mat * spectrumMat = processor->getImagePtr("spectrum");

		switch (mode)
		{
			case RENDER_PIXMAP:
				ui->sourceImage = new QcvMatWidgetLabel(sourceMat);
				ui->spectrumImage = new QcvMatWidgetLabel(spectrumMat);
				break;
			case RENDER_GL:
				ui->sourceImage = new QcvMatWidgetGL(sourceMat);
				ui->spectrumImage = new QcvMatWidgetGL(spectrumMat);
				break;
			case RENDER_IMAGE:
			default:
				ui->sourceImage = new QcvMatWidgetImage(sourceMat);
				ui->spectrumImage = new QcvMatWidgetImage(spectrumMat);
				break;
		}

		if ((ui->sourceImage != NULL) &&
			(ui->spectrumImage != NULL))
		{
			// Name the new images widgets with same name as in UI files
			 ui->sourceImage->setObjectName(QString::fromUtf8("sourceImage"));
			 ui->spectrumImage->setObjectName(QString::fromUtf8("spectrumImage"));

			// add to scroll areas
			ui->scrollAreaSource->setWidget(ui->sourceImage);
			ui->scrollAreaSpectrum->setWidget(ui->spectrumImage);

			// Reconnect signals to slots
			connect(processor, SIGNAL(updated()),
					ui->sourceImage, SLOT(update()));
			connect(processor, SIGNAL(updated()),
					ui->spectrumImage, SLOT(update()));

			connect(processor, SIGNAL(squareImageChanged(Mat*)),
					ui->sourceImage, SLOT(setSourceImage(Mat*)));
			connect(processor, SIGNAL(spectrumImageChanged(Mat*)),
					ui->spectrumImage, SLOT(setSourceImage(Mat*)));

			// Sends message to status bar and sets menu checks
			message.clear();
			message.append(tr("Render more set to "));
			switch (mode)
			{
				case RENDER_IMAGE:
					ui->actionRenderPixmap->setChecked(false);
					ui->actionRenderOpenGL->setChecked(false);
					message.append(tr("QImage"));
					break;
				case RENDER_PIXMAP:
					ui->actionRenderImage->setChecked(false);
					ui->actionRenderOpenGL->setChecked(false);
					message.append(tr("QPixmap in QLabel"));
					break;
				case RENDER_GL:
					ui->actionRenderImage->setChecked(false);
					ui->actionRenderPixmap->setChecked(false);
					message.append(tr("QGLWidget"));
					break;
				default:
				break;
			}
			emit sendMessage(message, 5000);
		}
		else
		{
			qDebug("MainWindow::on_actionRenderXXX some new widget is null");
		}
	}
	else
	{
		qDebug("MainWindow::on_actionRenderXXX removed widget is not in ui->");
	}
}

/*
 * Re setup processor from UI settings when source changes
 */
void MainWindow::setupProcessorFromUI()
{
	processor->setLogScaleFactor((double)ui->spinBoxMag->value());
}

/*
 * Menu action to replace current image rendering widget by a
 * QcvMatWidgetImage instance.
 */
void MainWindow::on_actionRenderImage_triggered()
{
	qDebug("Setting image rendering to: images");
	setRenderingMode(RENDER_IMAGE);
}

/*
 * Menu action to replace current image rendering widget by a
 * QcvMatWidgetLabel with pixmap instance.
 */
void MainWindow::on_actionRenderPixmap_triggered()
{
	qDebug("Setting image rendering to: pixmaps");
	setRenderingMode(RENDER_PIXMAP);
}

/*
 * Menu action to replace current image rendering widget by a
 * QcvMatWidgetGL instance.
 */
void MainWindow::on_actionRenderOpenGL_triggered()
{
	qDebug("Setting image rendering to: opengl");
	setRenderingMode(RENDER_GL);
}

/*
 * Original size radioButton action.
 * Sets capture resize to off
 */
void MainWindow::on_radioButtonOrigSize_clicked()
{
	ui->actionConstrainedSize->setChecked(false);
	// capture->setSize(0, 0);
	emit sizeChanged(QSize(0, 0));
}

/*
 * Custom size radioButton action.
 * Sets capture resize to preferred width and height
 */
void MainWindow::on_radioButtonCustomSize_clicked()
{
	ui->actionOriginalSize->setChecked(false);
	// capture->setSize(preferredWidth, preferredHeight);
	emit sizeChanged(QSize(preferredWidth, preferredHeight));
}

/*
 * Width spinbox value change.
 * Changes the preferred width and if custom size is selected apply
 * this custom width
 * @param value the desired width
 */
void MainWindow::on_spinBoxWidth_valueChanged(int value)
{
	preferredWidth = value;
	if (ui->radioButtonCustomSize->isChecked())
	{
		// capture->setSize(preferredWidth, preferredHeight);
		emit sizeChanged(QSize(preferredWidth, preferredHeight));
	}
}

/*
 * Height spinbox value change.
 * Changes the preferred height and if custom size is selected apply
 * this custom height
 * @param value the desired height
 */
void MainWindow::on_spinBoxHeight_valueChanged(int value)
{
	preferredHeight = value;
	if (ui->radioButtonCustomSize->isChecked())
	{
		// capture->setSize(preferredWidth, preferredHeight);
		emit sizeChanged(QSize(preferredWidth, preferredHeight));
	}
}

/*
 * Flip capture image horizontally.
 * changes capture flip status
 */
void MainWindow::on_checkBoxFlip_clicked()
{
	/*
	 * There is no need to update ui->actionFlip since it is connected
	 * to ui->checkBoxFlip through signals/slots
	 */
	// capture->setFlipVideo(ui->checkBoxFlip->isChecked());
	emit flipVideo(ui->checkBoxFlip->isChecked());
}

/*
 * convert capture image to gray level.
 * changes cpature gray conversion status
 */
void MainWindow::on_checkBoxGray_clicked()
{
	bool isGray = ui->checkBoxGray->isChecked();
	// capture->setGray(isGray);
	emit grayImage(isGray);
}

/*
 * Changes logscale factor for spectrum
 * @param value the new logscale factor
 */
void MainWindow::on_spinBoxMag_valueChanged(int value)
{
	processor->setLogScaleFactor((double)value);

	double realScale = processor->getLogScaleFactor();

	ui->spinBoxMag->setValue((int)realScale);
}
