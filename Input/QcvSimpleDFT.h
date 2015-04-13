/*
 * QcvSimpleDFT.h
 *
 *  Created on: 22 févr. 2012
 *      Author: davidroussel
 */

#ifndef QCVDFT_H_
#define QCVDFT_H_

#include "QcvProcessor.h"
#include "CvSimpleDFT.h"

/**
 * Qt flavored Simple Fourier transform
 */
class QcvSimpleDFT: public QcvProcessor, public CvSimpleDFT
{
	Q_OBJECT

	public:

		/**
		 * QcvSimpleDFT constructor
		 * @param image the source image
		 * @param imageLock the mutex on source image
		 * @param updateThread the thread in which this processor runs
		 * @param parent parent QObject
		 */
		QcvSimpleDFT(Mat * image,
					 QMutex * imageLock = NULL,
					 QThread * updateThread = NULL,
					 QObject * parent = NULL);

		/**
		 * QcvSimpleDFT destructor
		 */
		virtual ~QcvSimpleDFT();

		// --------------------------------------------------------------------
		// Options settings with message notification
		// --------------------------------------------------------------------

	public slots:
		/**
		 * Update computed images slot and sends updated signal
		 * required
		 */
		void update();

		/**
		 * Changes source image slot.
		 * Attributes needs to be cleaned up then set up again
		 * @param image the new source Image
		 */
		void setSourceImage(Mat * image)
			throw (CvProcessorException);

	signals:

//		/**
//		 * Signal sent when source image changes to adjust max filter sizes
//		 */
//		void dftSizeChanged();

		/**
		 * Signal sent when input dftSize square image has been reallocated
		 * @param image the new in square image
		 */
		void squareImageChanged(Mat * image);

		/**
		 * Signal sent when spectrum image has been reallocated
		 * @param image the new spectrum image
		 */
		void spectrumImageChanged(Mat * image);

		/**
		 * Signal sent when inverse image has been reallocated
		 * @param image the new inverse image
		 */
		void inverseImageChanged(Mat * image);
};

#endif /* QCVDFT_H_ */
