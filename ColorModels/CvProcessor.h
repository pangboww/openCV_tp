/*
 * CvProcessor.h
 *
 *  Created on: 21 févr. 2012
 *	  Author: davidroussel
 */

#ifndef CVPROCESSOR_H_
#define CVPROCESSOR_H_

#include <string>
#include <map>
#include <ctime>	// for clock
using namespace std;

#include <opencv2/core/core.hpp>	// for Mat
using namespace cv;

#include "CvProcessorException.h"

/**
 * Class to process a source image with OpenCV 2+
 */
class CvProcessor
{
	public:

		/**
		 * Verbose level for error / warnings / notification messages
		 */
		typedef enum
		{
			VERBOSE_NONE = 0,	//!< no messages are displayed
			VERBOSE_ERRORS,	  //!< only error messages are displayed
			VERBOSE_WARNINGS,	//!< error & warning messages are displayed
			VERBOSE_NOTIFICATIONS, //!< error, warning and notifications messages are displayed
			VERBOSE_ACTIVITY, //!< all previouses + log messages
			NBVERBOSELEVEL
		} VerboseLevel;


		/**
		 * Index of channels in OpenCV BGR or Gray images
		 */
		typedef enum
		{
			BLUE = 0,//!< Blue component is first in BGR images
			GRAY = 0,//!< Gray component is first in gray images
			GREEN,   //!< Green component is second in BGR images
			RED,	  //!< Red component is last in BGR images
			NBCHANNELS
		} Channels;

	protected:
		/**
		 * The source image: CV_8UC<nbChannels>
		 */
		Mat * sourceImage;

		/**
		 * Source image number of channels (generally 1 or 3)
		 */
		int nbChannels;

		/**
		 * Source image size (cols, rows)
		 */
		Size size;

		/**
		 * The source image type (generally CV_8UC<nbChannels>)
		 */
		int type;

		/**
		 * Map to store aditionnal images pointers by name
		 */
		map<string, Mat*> images;

		/**
		 * The verbose level for printed messages
		 */
		VerboseLevel verboseLevel;

		/**
		 * Process time in ticks (~1e6 ticks/second)
		 * @see clock_t for details on ticks
		 */
		clock_t processTime;

		/**
		 * Indicates if processing time is absolute or measured in ticks/feature
		 * processed by this processor.
		 * A feature can be any kind of things the processor has to detect or
		 * create while processing an image.
		 */
		bool timePerFeature;

	public:
		/**
		 * OpenCV image processor constructor
		 * @param sourceImage the source image
		 * @param verbose level for printed messages
		 * @pre source image is not NULL
		 */
		CvProcessor(Mat * sourceImage,
					const VerboseLevel level = VERBOSE_NONE);

		/**
		 * OpenCV image Processor destructor
		 */
		virtual ~CvProcessor();

		/**
		 * OpenCV image Processor abstract Update
		 * @note this method should be implemented in sub classes
		 */
		virtual void update() = 0;

		// --------------------------------------------------------------------
		// Images accessors
		// --------------------------------------------------------------------
		/**
		 * Changes source image
		 * @param sourceImage the new source image
		 * @throw CvProcessorException#NULL_IMAGE when new source image is NULL
		 * @note this method should NOT be directly reimplemented in sub classes
		 * unless it is transformed into a QT slot
		 */
		virtual void setSourceImage(Mat * sourceImage)
			throw (CvProcessorException);

		/**
		 * Adds a named image to additionnal images
		 * @param name the name of the image
		 * @param image the image reference
		 * @return true if image has been added to additionnal images map, false
		 * if image key (the name) already exists in the addtitionnal images map.
		 */
		bool addImage(const char * name, Mat * image);

		/**
		 * Adds a named image to additionnal images
		 * @param name the name of the image
		 * @param image the image reference
		 * @return true if image has been added to additionnal images map, false
		 * if image key (the name) already exists in the addtitionnal images map.
		 */
		bool addImage(const string & name, Mat * image);

//		/*
//		 * Update named image in additionnal images.
//		 * @param name the name of the image
//		 * @param image the image reference
//		 * @post the image located at key name is updated.
//		 */
//		virtual void updateImage(const char * name, const Mat & image);
//
//		/*
//		 * Update named image in additionnal images.
//		 * @param name the name of the image
//		 * @param image the image reference
//		 * @post the image located at key name is updated.
//		 */
//		virtual void updateImage(const string & name, const Mat & image);

		/**
		 * Get image by name
		 * @param name the name of the image we're looking for
		 * @return the image registered by this name in the additionnal images
		 * map
		 * @throw CvProcessorException#INVALID_NAME is used name is not already
		 * registerd in the images
		 */
		const Mat & getImage(const char * name) const
			throw (CvProcessorException);

		/**
		 * Get image by name
		 * @param name the name of the image we're looking for
		 * @return the image registered by this name in the additionnal images
		 * map
		 * @throw CvProcessorException#INVALID_NAME is used name is not already
		 * registerd in the images
		 */
		const Mat & getImage(const string & name) const
			throw (CvProcessorException);

		/**
		 * Get image pointer by name
		 * @param name the name of the image we're looking for
		 * @return the image pointer registered by this name in the additionnal
		 * images map
		 * @throw CvProcessorException#INVALID_NAME is used name is not already
		 * registerd in the images
		 */
		Mat * getImagePtr(const char * name)
			throw (CvProcessorException);

		/**
		 * Get image pointer by name
		 * @param name the name of the image we're looking for
		 * @return the image registered by this name in the additionnal images
		 * map
		 * @throw CvProcessorException#INVALID_NAME is used name is not already
		 * registerd in the images
		 */
		Mat * getImagePtr(const string & name)
			throw (CvProcessorException);
		// --------------------------------------------------------------------
		// Options settings and gettings
		// --------------------------------------------------------------------
		/**
		 * Number of channels in source image
		 * @return the number of channels of source image
		 */
		int getNbChannels() const;

		/**
		 * Type of the source image
		 * @return the openCV type of the source image
		 */
		int getType() const;

		/**
		 * Get the current verbose level
		 * @return the current verbose level
		 */
		VerboseLevel getVerboseLevel() const;

		/**
		 * Set new verbose level
		 * @param level the new verobse level
		 */
		virtual void setVerboseLevel(const VerboseLevel level);

		/**
		 * Return processor processing time of step index [default implementation
		 * returning only processTime, should be reimplemented in subclasses]
		 * @param index index of the step which processing time is required,
		 * 0 indicates all steps, and values above 0 indicates step #. If
		 * required index is bigger than number of steps than all steps value
		 * should be returned.
		 * @return the processing time of step index.
		 * @note should be reimplemented in subclasses in order to define
		 * time/feature behaviour
		 */
		virtual double getProcessTime(const size_t index = 0) const;

		/**
		 * Indicates if processing time is per feature processed in the current
		 * image or absolute
		 * @return
		 */
		bool isTimePerFeature() const;

		/**
		 * Sets Time per feature processing time unit
		 * @param value the time per feature value (true or false)
		 */
		virtual void setTimePerFeature(const bool value);

	protected:
		// --------------------------------------------------------------------
		// Setup and cleanup attributes
		// --------------------------------------------------------------------
		/**
		 * Setup internal attributes according to source image
		 * @param sourceImage a new source image
		 * @param fullSetup full setup is needed when source image is changed
		 * @pre sourceimage is not NULL
		 * @note this method should be reimplemented in sub classes
		 */
		virtual void setup(Mat * sourceImage, const bool fullSetup = true);

		/**
		 * Clean up internal attributes before changing source image or
		 * cleaning up class before destruction
		 * @note this method should be reimplemented in sub classes
		 */
		virtual void cleanup();
};

#endif /* CVPROCESSOR_H_ */
