/*
 * QcvColorSpaces.cpp
 *
 *  Created on: 25 févr. 2012
 *      Author: davidroussel
 */

#include <QDebug>
#include "QcvColorSpaces.h"

/*
 * QcvColorSpaces constructor
 * @param inFrame the input frame from capture
 * @param imageLock the mutex on source image
 * @param updateThread the thread in which this processor runs
 * @param parent object
 */
QcvColorSpaces::QcvColorSpaces(Mat * inFrame,
							   QMutex * imageLock,
							   QThread * updateThread,
							   QObject * parent) :
	CvProcessor(inFrame),
	QcvProcessor(inFrame, imageLock, updateThread, parent),
	CvColorSpaces(inFrame)
{
}

/*
 * QcvColorSpaces destructor
 */
QcvColorSpaces::~QcvColorSpaces()
{
}

/*
 * Update computed images and sends displayImageChanged signal if
 * required
 */
void QcvColorSpaces::update()
{
	if (sourceLock != NULL)
	{
		sourceLock->lock();
		// qDebug() << "QcvColorSpaces::update : lock";
	}

	CvColorSpaces::update();

	if (sourceLock != NULL)
	{
		// qDebug() << "QcvColorSpaces::update : unlock";
		sourceLock->unlock();
	}

	if (displayImageChanged)
	{
		emit imageChanged(&displayImage);
	}

	QcvProcessor::update(); // emits updated signal
}

/*
 * Select image to set in displayImage and sends notification message
 * @param select the index to select display image
 */
void QcvColorSpaces::setDisplayImageIndex(const Display index)
{
	CvColorSpaces::setDisplayImageIndex(index);

	message.clear();
	message.append(tr("Display Image set to: "));
	switch (index)
	{
		case INPUT:
			message.append(tr("Input"));
			break;
		case GRAY:
			message.append(tr("Gray level"));
			break;
		case RED:
			message.append(tr("Red component of RGB space"));
			break;
		case GREEN:
			message.append(tr("Green component of RGB space"));
			break;
		case BLUE:
			message.append(tr("Blue component of RGB space"));
			break;
		case MAX_BGR:
			message.append(tr("Maximum of RGB components"));
			break;
		case XYZ_X:
			message.append(tr("X component of XYZ space"));
			break;
		case XYZ_Y:
			message.append(tr("Y component of XYZ space"));
			break;
		case XYZ_Z:
			message.append(tr("Z component of XYZ space"));
			break;
		case HUE:
			message.append(tr("Hue component of HSV space"));
			break;
		case SATURATION:
			message.append(tr("Saturation component of HSV space"));
			break;
		case VALUE:
			message.append(tr("Value component of HSV space"));
			break;
		case Y:
			message.append(tr("Y component of YCbCr space"));
			break;
		case Cr:
			message.append(tr("Cr component of YCbCr space"));
			break;
		case Cb:
			message.append(tr("Cb component of YCbCr space"));
			break;
		case NbSelected:
		default:
			message.append(tr("Unknown"));
			break;
	}

	emit sendMessage(message, defaultTimeOut);
}

/*
 * Sets the color display status of selected component and sends
 * notification message
 * @param c the selected component:
 * @param value the value to set on the selected component
 */
void QcvColorSpaces::setColorChannel(const ShowColor c, const bool value)
{
	CvColorSpaces::setColorChannel(c, value);

	message.clear();
	message.append(tr("Setting "));
	switch (c)
	{
		case BINDEX:
			message.append(tr("blue"));
			break;
		case GINDEX:
			message.append(tr("green"));
			break;
		case RINDEX:
			message.append(tr("red"));
			break;
		case HINDEX:
			message.append(tr("hue"));
			break;
		case CbINDEX:
			message.append(tr("Cb"));
			break;
		case CrINDEX:
			message.append(tr("Cr"));
			break;
		case NbShows:
		default:
			message.append(tr("unknown"));
			break;
	}
	message.append(tr("component show as colored to: "));
	if (value)
	{
		message.append(tr("on"));
	}
	else
	{
		message.append(tr("off"));
	}

	emit sendMessage(message, defaultTimeOut);

}

/*
 * Select hue display mode and sends notification message
 * @param mode the mode so select
 */
void QcvColorSpaces::setHueDisplayMode(const HueDisplay mode)
{
	CvColorSpaces::setHueDisplayMode(mode);

	message.clear();
	message.append(tr("Setting hue color display as: "));
	switch (mode)
	{
		case HUECOLOR:
			message.append(tr("hue only"));
			break;
		case HUESATURATE:
			message.append(tr("hue x saturation"));
			break;
		case HUEVALUE:
			message.append(tr("hue x value"));
			break;
		case HUEGRAY:
			message.append(tr("hue as gray"));
			break;
		case NBHUES:
		default:
			message.append(tr("unknown"));
			break;
	}

	emit sendMessage(message, defaultTimeOut);
}
