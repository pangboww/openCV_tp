/*
 * QcvMatWidgetGL.cpp
 *
 *  Created on: 28 févr. 2011
 *	  Author: davidroussel
 */
#include <QDebug>

#include "QcvMatWidgetGL.h"

/*
 * OpenCV QT Widget default constructor
 * @param parent parent widget
 */
QcvMatWidgetGL::QcvMatWidgetGL(QWidget *parent,
							   MouseSense mouseSense) :
	QcvMatWidget(parent, mouseSense),
	gl(NULL)
//	glCount(0)
{
}

/*
 * OpenCV QT Widget constructor
 * @param parent parent widget
 */
QcvMatWidgetGL::QcvMatWidgetGL(Mat * sourceImage,
							   QWidget *parent,
							   MouseSense mouseSense) :
	QcvMatWidget(sourceImage, parent, mouseSense),
	gl(NULL)
//	glCount(0)
{
	setSourceImage(sourceImage);
}

/*
 * OpenCV Widget destructor.
 */
QcvMatWidgetGL::~QcvMatWidgetGL()
{
	if (gl != NULL)
	{
		layout->removeWidget(gl);
		delete gl;
	}
}

/*
 * Sets new source image
 * @param sourceImage the new source image
 */
void QcvMatWidgetGL::setSourceImage(Mat *sourceImage)
{
	QcvMatWidget::setSourceImage(sourceImage);

	if (gl != NULL)
	{
		layout->removeWidget(gl);
		delete gl;
	}

	convertImage();

	gl = new QGLImageRender(displayImage, this);

	layout->addWidget(gl, 0, Qt::AlignCenter);
}

/*
 * paint event reimplemented to draw content
 * @param event the paint event
 */
void QcvMatWidgetGL::paintEvent(QPaintEvent * event)
{
	QcvMatWidget::paintEvent(event);
//	qDebug() << "Paint event # " << glCount++;
	gl->update();
}
