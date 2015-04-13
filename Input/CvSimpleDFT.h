/*
 * CvSimpleDFT.h
 *
 *  Created on: 21 févr. 2012
 *      Author: davidroussel
 */

#ifndef CVDFT_H_
#define CVDFT_H_

#include <vector>
using namespace std;

#include <opencv/cv.h>
using namespace cv;

#include "CvProcessor.h"

/**
 * Class to compute DFT on input image
 */
class CvSimpleDFT : virtual public CvProcessor
{
	public:
		/**
		 * Minimum log scale factor.
		 * Default value is 5.
		 */
		static const double minLogScaleFactor;

		/**
		 * Maximum log scale factor.
		 * Default value is 20 or 30.
		 */
		static const double maxLogScaleFactor;

	protected:
		/**
		 * Minimum of source image rows & cols for cropping source
		 */
		int minSize;

		/**
		 * Maximum of source image rows & cols for cropping source
		 */
		int maxSize;

		/**
		 * Border size to crop on source image
		 */
		int borderSize;

		/**
		 * DFT optimal size
		 */
		int optimalDFTSize;

		/**
		 * Optimal Fourier size
		 */
		Size dftSize;

		/**
		 * Input frame cropped to square size for FFT: CV_8UC<nbChannels>
		 */
		Mat inFrameSquare;

		/**
		 * Input frame cropped color channels: CV_8UC1 x <nbChannels>
		 */
		vector<Mat> channels;

		/**
		 * Input frame square channels converted to doubles: CV_64FC1 x <nbChannels>
		 */
		vector<Mat> channelsDouble;

		/**
		 * Input frame square channels complex channels:
		 * CV_64FC1 x 2 x <nbChannels>
		 */
		vector<vector<Mat> > channelsDoubleComplexComponents;

		/**
		 * Input frame square complex image: CV_64FC2 x <nbChannels>
		 */
		vector<Mat> channelsComplexImages;

		/**
		 * Complex spectrum images: CV_64FC2 x <nbChannels>
		 */
		vector<Mat> channelsComplexSpectrums;

		/**
		 * Complex spectrum channels: CV_64FC1 x 2 x <nbChannels>
		 */
		vector<vector<Mat> > channelsComplexSpectrumComponents;

		/**
		 * Spectrum magnitude: CV_64FC1 x <nbChannels>
		 */
		vector<Mat> channelsSpectrumMagnitude;

		/**
		 * LogScale factor to apply on log magnitude to show spectrum.
		 */
		double logScaleFactor;

		/**
		 * log spectrum magnitude: CV_64FC1 x <nbChannels>
		 */
		vector<Mat> channelsSpectrumLogMagnitude;

		/**
		 * [Log] spectrum magnitude channels converted for display:
		 * CV_8UC1 x <nbChannels>
		 */
		vector<Mat> channelsSpectrumLogMagnitudeDisplay;

		/**
		 * [Log] spectrum magnitude image converted for display:
		 * CV_8UC<nbChannels>
		 */
		Mat spectrumMagnitudeImage;

	public:
		/**
		 * DFT processor constructor
		 * @param sourceImage the source image
		 * @pre source image is not NULL
		 */
		CvSimpleDFT(Mat * sourceImage);

		/**
		 * DFT Processor destructor
		 */
		virtual ~CvSimpleDFT();

		/**
		 * DFT Update.
		 * Steps in update
		 * 	- crop source image to a square according to optima FFT size
		 * 	- split in frame square into color channels
		 *	- converts these color channels to double
		 *	- apply frequency shift on double channels to
		 *		- produce the shifted real component of source channels
		 *		- produce later a spectrum with low frequencies at image center
		 *	- merge real/image channels into complex image per channel
		 *	- compute dft on each channel
		 *	- split channels complex spectrum in to real/imag components
		 *	- compute channels spectrum magnitude from real/imag components
		 *	- log scale channels spectrum magnitude
		 *	- converts channels log magnitude for display
		 */
		virtual void update();

		// --------------------------------------------------------------------
		// Options settings and gettings
		// --------------------------------------------------------------------

		/**
		 * Optimal dft size for current source image
		 * @return the current optimal dft size
		 */
		int getOptimalDftSize() const;

		/**
		 * Get current log scale factor
		 * @return the current log scale factor
		 */
		double getLogScaleFactor() const;

		/**
		 * Setting the log scale factor
		 * @param logScaleFactor the new log scale factor
		 */
		virtual void setLogScaleFactor(double logScaleFactor);

	protected:

		// --------------------------------------------------------------------
		// Setup and cleanup attributes
		// --------------------------------------------------------------------

		/**
		 * Setup internal attributes according to source image
		 * @param sourceImage a new source image
		 * @param fullSetup full setup is needed when source image is changed
		 */
		void setup(Mat * sourceImage, bool fullSetup = true);

		/**
		 * Clean up internal atrtibutes before changing source image or
		 * cleaning up class before destruction
		 */
		void cleanup();

		// --------------------------------------------------------------------
		// Utility methods
		// --------------------------------------------------------------------
		/**
		 * Modify image to obtain reverse frequencies on the Fourier transform
		 * (low frequencies at the center of the image and high frequencies on
		 * the border), or modify image obtained from reverse Fourier transform
		 * with reversed frequencies.
		 * @param imgIn source image
		 * @param imgOut destination image
		 * @par Algorithm:
		 * This is based on the following property of the Z transform :
		 * \f[
		 * 	TZ\left\{a^{k} x_{k}\right\} = X\left(\frac{z}{a}\right)
		 * \f]
		 * if \f$y_{k} = (-1)^{k} x_{k}\f$ then \f$Y(z) = X(-z)\f$
		 * which can be explained in Fourier space by replacing
		 * \f$z\f$ by \f$e^{j 2 \pi F}\f$:
		 * \f[
		 * Y\left[e^{j 2 \pi F}\right] = X\left[-e^{j2\pi F}\right] =
		 * X\left[e^{j\pi}e^{j2\pi F}\right] =
		 * X\left[e^{j2\pi\left(F + \frac{1}{2}\right)}\right]
		 * \f]
		 * hence
		 * \f[
		 * Y(F) = X\left(F + \frac{1}{2}\right)
		 * \f]
		 * or
		 * \f[
		 * Y(f) = X\left(f + \frac{f_{e}}{2}\right)
		 * \f]
		 * where \f$f_{e}\f$ is the sampling frequency, which means the
		 * resulting Fourier transform will present an \f$\frac{f_{e}}{2}\f$
		 * frequency offset. And since the sampling frequency lies in the middle
		 * of the spectrum in the DFT. Low frequencies will appear centered
		 * around the middle of the spectrum.
		 *
		 * In 2D the algorithm is the following:
		 * \f[
		 * imgOut(i,j) = (-1)^{i+j} \cdot imgIn(i,j)
		 * \f]
		 * \f$f_{e}\f$ is at the center of the spectrum image in 2D, which
		 * means, low frequencies will be located at the center of the image.
		 */
		template <typename T>
		void frequencyShift(Mat & imgIn, Mat & imgOut);

		/**
		 * Log scale T valued image
		 * @param imgIn input image
		 * @param imgOut output image
		 * @param scaleFactor such as
		 * \f$ imgOut = scaleFactor \times \log(1 + imgIn)\f$
		 */
		template <typename T>
		void logScaleImg(const Mat & imgIn, Mat & imgOut, const T scaleFactor);
};

#endif /* CVDFT_H_ */
