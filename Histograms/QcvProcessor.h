/*
 * QcvProcessor.h
 *
 *  Created on: 19 févr. 2012
 *	  Author: davidroussel
 */

#ifndef QCVPROCESSOR_H_
#define QCVPROCESSOR_H_

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QMutex>
#include <QThread>
#include "CvProcessor.h"

/**
 * Qt flavored class to process a source image with OpenCV 2+
 */
class QcvProcessor : public QObject, public virtual CvProcessor
{
	Q_OBJECT

	protected:

		/**
		 * Default timeout to show messages
		 */
		static int defaultTimeOut;

		/**
		 * Number format used to format numbers into QStrings
		 */
		static char numberFormat[10];

		/**
		 * The regular expression used to validate new number formats
		 * @see #setNumberFormat
		 */
		static QRegExp numberRegExp;

		/**
		 * The Source image mutex in order to avoid concurrent access to
		 * the source image (typically the source image may be modified
		 */
		QMutex * sourceLock;

		/**
		 * the thread in which this processor should run
		 */
		QThread * updateThread;

		/**
		 * Message to send when something changes
		 */
		QString message;

		/**
		 * String used to store formatted process time value
		 */
		QString processTimeString;

	public:

		/**
		 * QcvProcessor constructor
		 * @param image the source image
		 * @param imageLock the mutex for concurrent access to the source image.
		 * In order to avoid concurrent access to the same image
		 * @param updateThread the thread in which this processor should run
		 * @param parent parent QObject
		 */
		QcvProcessor(Mat * image,
					 QMutex * imageLock = NULL,
					 QThread * updateThread = NULL,
					 QObject * parent = NULL);

		/**
		 * QcvProcessor destructor
		 */
		virtual ~QcvProcessor();

		/**
		 * Sets new number format
		 * @param format the new number format
		 * @pre format string should look like "%8.1f" or at least not be longer
		 * than 10 chars since format is a 10 chars array.
		 * @post id format string is valid and shorter than 10 chars
		 * it has been applied as the new format string.
		 */
		static void setNumberFormat(const char * format);

	public slots:
		/**
		 * Update computed images slot and sends updated signal
		 */
		virtual void update();

		/**
		 * Changes source image slot.
		 * Attributes needs to be cleaned up then set up again
		 * @param image the new source Image
		 * @throw CvProcessorException#NULL_IMAGE when new source image is NULL
		 * @post Various signals are emitted:
		 * 	- imageChanged(sourceImage)
		 * 	- imageCchanged()
		 * 	- if image size changed then imageSizeChanged() is emitted
		 * 	- if image color space changed then imageColorsChanged() is emitted
		 */
		virtual void setSourceImage(Mat * image) throw (CvProcessorException);

		/**
		 * Sets Time per feature processing time unit slot.
		 * @param value the time per feature value (true or false)
		 */
		virtual void setTimePerFeature(const bool value);

	signals:
		/**
		 * Signal emitted when update is complete
		 */
		void updated();

		/**
		 * Signal emitted when processor has finished.
		 * Used to tell helper threads to quit
		 */
		void finished();

		/**
		 * Signal emitted when source image is reallocated
		 */
		void imageChanged();

		/**
		 * Signal emitted when source image is reallocated
		 * @param image the new source image pointer or none if just
		 * image changed notification is required
		 */
		void imageChanged(Mat * image);

		/**
		 * Signal emitted when source image colors changes from color to gray
		 * or from gray to color
		 */
		void imageColorsChanged();

		/**
		 * Signal emitted when source image size changes
		 */
		void imageSizeChanged();

		/**
		 * Signal emited when processing time has channged
		 * @param value the new value of the processing time
		 */
		void processTimeUpdated(const QString & formattedValue);

		/**
		 * Signal to set text somewhere
		 * @param message the message
		 */
		void sendText(const QString & message);

		/**
		 * Signal to send update message when something changes
		 * @param message the message
		 * @param timeout number of ms the message should be displayed
		 */
		void sendMessage(const QString & message, int timeout = defaultTimeOut);

};

#endif /* QCVPROCESSOR_H_ */
