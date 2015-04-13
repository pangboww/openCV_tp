/*
 * QGLImageRender.cpp
 *
 *  Created on: 28 févr. 2011
 *	  Author: davidroussel
 */
#include <QDebug>
#ifdef __APPLE__
    #include <gl.h>
    #include <glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif
#include "QGLImageRender.h"

QGLImageRender::QGLImageRender(const Mat & image, QWidget *parent) :
	QGLWidget(parent),
	image(image)
//	fCount(0)
{
	if (!doubleBuffer())
	{
		qWarning("QGLImageRender::QGLImageRender caution : no double buffer");
	}
	if (this->image.data == NULL)
	{
		qWarning("QGLImageRender::QGLImageRender caution : image data is null");
	}
}

QGLImageRender::~QGLImageRender()
{
	image.release();
}

void QGLImageRender::initializeGL()
{
	qDebug("GL init ...");
	glClearColor(0.0, 0.0, 0.0, 0.0);
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void QGLImageRender::paintGL()
{
//	qDebug("GL drawing pixels ...");

	glClear(GL_COLOR_BUFFER_BIT);

	if (image.data != NULL)
	{
		glDrawPixels(image.cols, image.rows, GL_RGB,
					 GL_UNSIGNED_BYTE, image.data);
		// In any circumstance you should NOT use glFlush or swapBuffers() here
	}
	else
	{
		qWarning("Nothing to draw");
	}

}

void QGLImageRender::resizeGL(int width, int height)
{
//	qDebug("GL resizeGL ...");
//	glViewport(0, 0, width, height);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluOrtho2D(0.0, 0.0, (GLdouble)width, (GLdouble)height);

	qDebug("GL Resize (%d, %d)",width, height);

//	GLfloat zoom, xZoom, yZoom;
//
//	xZoom = (GLfloat)width/(GLfloat)image.cols;
//	yZoom = (GLfloat)height/(GLfloat)image.rows;
//
//	if (xZoom < yZoom)
//	{
//		zoom = xZoom;
//	}
//	else
//	{
//		zoom = yZoom;
//	}

	glViewport(0, 0, (GLsizei) width, (GLsizei) height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (image.data != NULL)
	{
//		gluOrtho2D(0, (GLdouble) image.cols, 0, (GLdouble) image.rows);
		glOrtho(0, (GLdouble) image.cols, 0, (GLdouble) image.rows, 1.0, -1.0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* apply the right translate so the image drawing starts top left */
	if (image.data != NULL)
	{
		/*
		 * For some reason we should not start drawing exactly at the limit
		 * of the drawing plane so we start drawing at image.rows - something
		 * which could be very tiny
		 */
        glRasterPos2i(0,image.rows);
	}
	else
	{
		qWarning("QGLImageRender::resizeGL(...) : image.data is NULL");
	}

	/* apply the right zoom factor so image are displayed top 2 bottom */
    glPixelZoom(1.0, -1.0);
}


QSize QGLImageRender::sizeHint () const
{
	return minimumSizeHint();
}

QSize QGLImageRender::minimumSizeHint() const
{
	if (image.data != NULL)
	{
		return QSize(image.cols, image.rows);
	}
	else
	{
		qWarning("QGLImageRender::minimumSizeHint : probably invalid sizeHint");
		return QSize(320,240);
	}
}

QSizePolicy	QGLImageRender::sizePolicy () const
{
	return QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
