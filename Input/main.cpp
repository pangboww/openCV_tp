#include <QApplication>
#include <QThread>
#include <libgen.h>		// for basename
#include <iostream>		// for cout

#include "QcvVideoCapture.h"
#include "CaptureFactory.h"
#include "QcvSimpleDFT.h"
#include "mainwindow.h"

/**
 * Usage function shown just before launching QApp
 * @param name the name of the program (argv[0])
 */
void usage(char * name);

/**
 * Test program OpenCV2 + QT4
 * @param argc argument count
 * @param argv argument values
 * @return QTApp return value
 * @par usage : <Progname> [--device | -d] <#> | [--file | -f ] <filename>
 * [--mirror | -m] [--size | -s] <width>x<height>
 * 	- device : [--device | -d] <device #> (0, 1, ...) Opens capture device #
 * 	- filename : [--file | -f ] <filename> Opens a video file or URL (including rtsp)
 * 	- mirror : mirrors image horizontally before display
 *	- size : [--size | -s] <width>x<height> resize capture to fit desired <width>
 *	and <height>
 */
int main(int argc, char *argv[])
{
	// ------------------------------------------------------------------------
	// Instanciate QApplication to receive special QT args
	// ------------------------------------------------------------------------
	QApplication app(argc, argv);

	// Gets arguments after QT specials removed
	QStringList argList = QCoreApplication::arguments();

	int threadNumber = 3;
	// parse arguments for --threads tag
	for (QListIterator<QString> it(argList); it.hasNext(); )
	{
		QString currentArg(it.next());

		if (currentArg == "-t" || currentArg =="--threads")
		{
			// Next argument should be thread number integer
			if (it.hasNext())
			{
				QString threadString(it.next());
				bool convertOk;
				threadNumber = threadString.toInt(&convertOk,10);
				if (!convertOk || threadNumber < 1 || threadNumber > 3)
				{
					qWarning("Warning: Invalid thread number %d",threadNumber);
					threadNumber = 3;
				}
			}
			else
			{
				qWarning("Warning: thread tag found with no following thread number");
			}

		}
	}
	// ------------------------------------------------------------------------
	// Create Capture factory using program arguments and
	// open Video Capture
	// ------------------------------------------------------------------------
	CaptureFactory factory(argList);
	factory.setSkippable(true);

	// Helper thread for capture
	QThread * capThread = NULL;
	if (threadNumber > 1)
	{
		capThread = new QThread();
	}

	// Capture
	QcvVideoCapture * capture = factory.getCaptureInstance(capThread);

	// ------------------------------------------------------------------------
	// Create Fourier Processor
	// ------------------------------------------------------------------------
	// Helper thread for processor
	QThread * procThread = NULL;
	if (threadNumber > 2)
	{
		procThread = new QThread();
	}
	else
	{
		if (threadNumber > 1)
		{
			procThread = capThread;
		}
	}

	// Processsor
	QcvSimpleDFT * processor = NULL;
	if (procThread == NULL)
	{
		processor = new QcvSimpleDFT(capture->getImage());
	}
	else
	{
		if (procThread != capThread)
		{
			processor = new QcvSimpleDFT(capture->getImage(),
										 capture->getMutex(),
										 procThread);
		}
		else // procThread == capThread
		{
			processor = new QcvSimpleDFT(capture->getImage(),
										 NULL,
										 procThread);
		}
	}

	// ------------------------------------------------------------------------
	// Connects capture to processor
	// ------------------------------------------------------------------------
	// Connects capture update to QHistandLUT update
	QObject::connect(capture, SIGNAL(updated()),
					 processor, SLOT(update()));

	// connect capture changed image to QHistandLUT set input
	QObject::connect(capture, SIGNAL(imageChanged(Mat*)),
					 processor, SLOT(setSourceImage(Mat*)));
	// ------------------------------------------------------------------------
	// Now that Capture & Histogram are on then
	// add our MainWindow as toplevel
	// and launches app
	// ------------------------------------------------------------------------
	MainWindow w(capture, processor);
	w.show();

	usage(argv[0]);

	int retVal = app.exec();

	// ------------------------------------------------------------------------
	// Cleanup & return
	// ------------------------------------------------------------------------
	delete processor;
	delete capture;
	bool sameThread = capThread == procThread;

	if (capThread != NULL)
	{
		delete capThread;
	}

	if (procThread != NULL && !sameThread)
	{
		delete procThread;
	}

	return retVal;
}

/*
 * Usage function shown just before launching QApp
 * @param name the name of the program (argv[0])
 */
void usage(char * name)
{
	cout << "usage  : " << basename(name) << " "
		 << "[-d | --device] <device number> "
		 << "[-v | --video] <video file> "
		 << "[-s | --size] <width>x<height> "
		 << "[-m | --mirror]"
		 << "[-g | --gray]"
		 << endl;
}
