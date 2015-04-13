/*
 * QcvSimpleDFT.cpp
 *
 *  Created on: 22 févr. 2012
 *      Author: davidroussel
 */

#include "QcvSimpleDFT.h"


/*
 * QcvSimpleDFT constructor
 * @param image the source image
 * @param imageLock the mutex on source image
 * @param updateThread the thread in which this processor runs
 * @param parent parent QObject
 */
QcvSimpleDFT::QcvSimpleDFT(Mat * image,
						   QMutex * imageLock,
						   QThread * updateThread,
						   QObject * parent) :
	CvProcessor(image), // <-- virtual base class constructor first
	QcvProcessor(image, imageLock, updateThread, parent),
	CvSimpleDFT(image)
{
}

/*
 * QcvSimpleDFT destructor
 */
QcvSimpleDFT::~QcvSimpleDFT()
{
	message.clear();
}

/*
 * Update computed images slot and sends updated signal
 * required
 */
void QcvSimpleDFT::update()
{
	if (sourceLock != NULL)
	{
		sourceLock->lock();
		// qDebug() << "QcvSimpleDFT::update : lock";
	}

	/*
	 * Update DFT images
	 */
	CvSimpleDFT::update();

	if (sourceLock != NULL)
	{
		// qDebug() << "QcvSimpleDFT::update : unlock";
		sourceLock->unlock();
	}

	/*
	 * emit updated signal
	 */
	QcvProcessor::update();
}

/*
 * Changes source image slot.
 * Attributes needs to be cleaned up then set up again
 * @param image the new source Image
 */
void QcvSimpleDFT::setSourceImage(Mat *image)
	throw (CvProcessorException)
{
	Size previousDftSize(dftSize);

	QcvProcessor::setSourceImage(image);

	emit squareImageChanged(&inFrameSquare);

	emit spectrumImageChanged(&spectrumMagnitudeImage);

	if ((previousDftSize.width != dftSize.width) ||
		(previousDftSize.height != dftSize.height))
	{
		emit imageSizeChanged();
		emit sendText(QString::number(optimalDFTSize));
	}

	// Force update
	update();
}
