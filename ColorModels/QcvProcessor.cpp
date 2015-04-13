/*
 * QCvProcessor.cpp
 *
 *  Created on: 19 févr. 2012
 *	  Author: davidroussel
 */

#include <QRegExpValidator>
#include <QDebug>
#include <cstring>		// for strcpy
#include "QcvProcessor.h"

/*
 * Default timeout to show messages
 */
int QcvProcessor::defaultTimeOut = 5000;

/*
 * Number format used to format numbers into QStrings
 */
char QcvProcessor::numberFormat[10] = {"%8.1f ms"};

/*
 * The regular expression used to validate new number formats
 * @see #setNumberFormat
 */
QRegExp QcvProcessor::numberRegExp("%[+- 0#]*[0-9]*([.][0-9]+)?[efEF]");

/*
 * QcvProcessor constructor
 * @param image the source image
 * @param imageLock the mutex for concurrent access to the source image
 * In order to avoid concurrent access to the same image
 * @param updateThread the thread in which this processor should run
 * @param parent parent QObject
 */
QcvProcessor::QcvProcessor(Mat * image,
						   QMutex * imageLock,
						   QThread * updateThread,
						   QObject * parent) :
	CvProcessor(image),	// <-- virtual base class constructor first
	QObject(parent),
	sourceLock(imageLock),
	updateThread(updateThread),
	message(),
	processTimeString()
{
	if (updateThread != NULL)
	{
		this->moveToThread(updateThread);

		connect(this, SIGNAL(finished()), updateThread, SLOT(quit()),
				Qt::DirectConnection);

		updateThread->start();
	}
}

/*
 * QcvProcessor destructor
 */
QcvProcessor::~QcvProcessor()
{
	// Lock might be already destroyed in source object so don't try to unlock

	message.clear();
	processTimeString.clear();

	emit finished();

	if (updateThread != NULL)
	{
		// Wait until update thread has received the "finished" signal through
		// "quit" slot
		updateThread->wait();
	}
}

/*
 * Sets new number format
 * @param format the new number format
 */
void QcvProcessor::setNumberFormat(const char * format)
{
	/*
	 * The format string should validate the following regex
	 * %[+- 0#]*[0-9]*([.][0-9]+)?[efEF]
	 */
	QRegExpValidator validator(numberRegExp, NULL);

	QString qFormat(format);
	int pos = 0;
	if ((validator.validate(qFormat,pos) == QValidator::Acceptable) &&
		(strlen(format) <= 10))
	{
		strcpy(numberFormat, format);
	}
	else
	{
		qWarning("QcvProcessor::setNumberFormat(%s) : invalid format", format);
	}
}


/*
 * Update computed images slot and sends updated signal
 * required
 */
void QcvProcessor::update()
{
	/*
	 * Important note : CvProcessor::update() should NOT be called here
	 * since it should be called in QcvXXXprocessor subclasses such that
	 * QcvXXXProcessor::update method should contain :
	 *	- call to CvXXXProcessor::update() (not QCvXXXProcessor)
	 *	- emit signals from QcvXXXProcessor
	 *	- call to QcvProcessor::update() (this method)
	 */
	emit updated();
	processTimeString.sprintf(numberFormat, getProcessTime(0) / 1000.0);
	emit processTimeUpdated(processTimeString);
}

/*
 * Changes source image slot.
 * Attributes needs to be cleaned up then set up again
 * @param image the new source Image
 * @post Various signals are emitted:
 * 	- imageChanged(sourceImage)
 * 	- imageCchanged()
 * 	- if image size changed then imageSizeChanged() is emitted
 * 	- if image color space changed then imageColorsChanged() is emitted
 */
void QcvProcessor::setSourceImage(Mat *image)
	throw (CvProcessorException)
{
	if (verboseLevel >= VERBOSE_NOTIFICATIONS)
	{
		clog << "QcvProcessor::setSourceImage(" << (ulong) image << ")" << endl;
	}

	Size previousSize(sourceImage->size());
	int previousNbChannels(nbChannels);

	if (sourceLock != NULL)
	{
		sourceLock->lock();
		// qDebug() << "QcvProcessor::setSourceImage: lock";
	}

	CvProcessor::setSourceImage(image);

	if (sourceLock != NULL)
	{
		// qDebug() << "QcvProcessor::setSourceImage: unlock";
		sourceLock->unlock();
	}

	emit imageChanged(sourceImage);

	emit imageChanged();

	if ((previousSize.width != image->cols) ||
		(previousSize.height != image->rows))
	{
		emit imageSizeChanged();
	}

	if (previousNbChannels != nbChannels)
	{
		emit imageColorsChanged();
	}

	// Force update
	update();
}

/*
 * Sets Time per feature processing time unit slot
 * @param value the time per feature value (true or false)
 */
void QcvProcessor::setTimePerFeature(const bool value)
{
	CvProcessor::setTimePerFeature(value);
}
