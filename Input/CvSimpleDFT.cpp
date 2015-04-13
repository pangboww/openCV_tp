/*
 * CvSimpleDFT.cpp
 *
 *  Created on: 21 févr. 2012
 *      Author: davidroussel
 */

#include <limits>
#include <cmath>

//#include <iostream>
//using namespace std;

#include "CvSimpleDFT.h"

/*
 * Minimum log scale factor.
 * Default value is 5.
 */
const double CvSimpleDFT::minLogScaleFactor = 5.0;

/*
 * Maximum log scale factor.
 * Default value is 20.
 */
const double CvSimpleDFT::maxLogScaleFactor = 30.0;

/*
 * DFT processor constructor
 * @param sourceImage the source image
 */
CvSimpleDFT::CvSimpleDFT(Mat * sourceImage) :
	CvProcessor(sourceImage),
	minSize(MIN(sourceImage->rows, sourceImage->cols)),
	maxSize(MAX(sourceImage->rows, sourceImage->cols)),
	borderSize((maxSize-minSize)/2),
	optimalDFTSize(getOptimalDFTSize(minSize)),
	dftSize(optimalDFTSize, optimalDFTSize),
	inFrameSquare(dftSize, type),
	logScaleFactor(10.0),
	spectrumMagnitudeImage(dftSize, type)
{
	setup(sourceImage, false);

	addImage("square", &inFrameSquare);
	addImage("spectrum", &spectrumMagnitudeImage);
}

/*
 * DFT Processor destructor
 */
CvSimpleDFT::~CvSimpleDFT()
{
	cleanup();
}

/*
 * Setup internal attributes according to source image
 * @param sourceImage a new source image
 * @param fullSetup full setup is needed when source image is changed
 */
void CvSimpleDFT::setup(Mat *sourceImage, bool fullSetup)
{
	// Full setup starting point (already performed in constructor)
	if (fullSetup)
	{
		CvProcessor::setup(sourceImage, fullSetup);
		minSize = MIN(sourceImage->rows, sourceImage->cols);
		maxSize = MAX(sourceImage->rows, sourceImage->cols);
		borderSize = (maxSize-minSize)/2;
		optimalDFTSize = getOptimalDFTSize(minSize);
		dftSize.height = optimalDFTSize;
		dftSize.width = optimalDFTSize;
		inFrameSquare = Mat(dftSize, type);
//		logScaleFactor = 10.0;
		spectrumMagnitudeImage = Mat(dftSize, type);
	}

	// Partial setup starting point
	for (int i=0; i < nbChannels; i++)
	{
		channels.push_back(Mat(dftSize, CV_8UC1));
		channelsDouble.push_back(Mat(dftSize, CV_64FC1));
		channelsDoubleComplexComponents.push_back(vector<Mat>());
		channelsComplexImages.push_back(Mat(dftSize, CV_64FC2));
		channelsComplexSpectrums.push_back(Mat(dftSize, CV_64FC2));
		channelsComplexSpectrumComponents.push_back(vector<Mat>());
		channelsSpectrumMagnitude.push_back(Mat(dftSize, CV_64FC1));
		channelsSpectrumLogMagnitude.push_back(Mat(dftSize, CV_64FC1));
		channelsSpectrumLogMagnitudeDisplay.push_back(Mat(dftSize, CV_8UC1));

		// complex channels
		for (int j=0; j < 2; j++)
		{
			channelsDoubleComplexComponents[i].push_back(Mat(dftSize, CV_64FC1));
			channelsComplexSpectrumComponents[i].push_back(Mat(dftSize, CV_64FC1));
		}

		// fill complex channels of channelsDoubleComplexComponents with 0
		channelsDoubleComplexComponents[i][1] = Scalar(0.0);
	}
}

void CvSimpleDFT::cleanup()
{
	for (int i=0; i < nbChannels; i++)
	{
		// complex channels
		for (int j=0; j < 2; j++)
		{
			channelsComplexSpectrumComponents[i][j].release();
			channelsDoubleComplexComponents[i][j].release();
		}

		channelsSpectrumLogMagnitudeDisplay[i].release();
		channelsSpectrumLogMagnitude[i].release();
		channelsSpectrumMagnitude[i].release();
		channelsComplexSpectrumComponents[i].clear();
		channelsComplexSpectrums[i].release();
		channelsComplexImages[i].release();
		channelsDoubleComplexComponents[i].clear();
		channelsDouble[i].release();
		channels[i].release();
	}

	channelsSpectrumLogMagnitudeDisplay.clear();
	channelsSpectrumLogMagnitude.clear();
	channelsSpectrumMagnitude.clear();
	channelsComplexSpectrumComponents.clear();
	channelsComplexSpectrums.clear();
	channelsComplexImages.clear();
	channelsDoubleComplexComponents.clear();
	channelsDouble.clear();
	channels.clear();

	spectrumMagnitudeImage.release();
	inFrameSquare.release();

	// super cleanup
	CvProcessor::cleanup();
}

/*
 * Update
 */
void CvSimpleDFT::update()
{
//	clog << "CvSimpleDFT::update()" << endl;

	/*
	 * Crop source image to center square and resize it to nearest
	 * DFT optimal size
	 * *sourceImage -> inFrameSquare
	 */
	if (sourceImage->cols > sourceImage->rows)
	{
		// wider than high : resize a colRange(borderSize, borderSize + minSize)
		// of sourceImage to dftSize in inFrameSquare
		resize(sourceImage->colRange(borderSize, borderSize + minSize),
			   inFrameSquare,
			   dftSize,
			   0,
			   0,
			   INTER_AREA);
	}
	else
	{
		// higher than wide : resize a rowRange(borderSize, borderSize + minSize)
		// of sourceImage to dftSize in inFrameSquare
		resize(sourceImage->rowRange(borderSize, borderSize + minSize),
			   inFrameSquare,
			   dftSize,
			   0,
			   0,
			   INTER_AREA);
	}

	/*
	 * Split input frame square to individual channels
	 * inFrameSquare -> channels
	 */
	// TODO à compléter ...

	// Process each component (1 for gray images, 3 for color images)
	for (int i=0; i < nbChannels; i++)
	{
		/*
		 * Fourier transform processing
		 * 	- Convert uchar center square image to CV_64F real component
		 * 	- perform frequency shift on real image to obtain low frequencies
		 * 		in the middle of the DFT image rather than in the corners
		 * 	- merge real & imag component to complexImage before DFT
		 * 		imag component could be filled with 0
		 * 	- compute DFT
		 * 	- split DFT channels
		 * 	- compute DFT magnitude from DFT channels
		 * 	- logScale magnitude with factor (5 to 20)
		 * 	- convertScaleAbs logMagnitude to CV_8UC1 to display image
		 *
		 */

		// convert component to double
		// channels[] -> channelsDouble
		// TODO à compléter ...

		// Frequency shift channelsDouble to real complex component with
		// frequencyShift<double>(...)
		// Frequency shift allow to prepare spatial image components to
		// produce frequency image later with low frequencies in the center
		// of frequency image
		// channelsDouble[] -> channelsDoubleComplexComponents[][0]
		// TODO à compléter ...
		// channelsDoubleComplexComponents[i][1] is already filled with 0 in
		// setup method so frequency shift is not necessary on imaginary part

		// Merge Real and Imaginary into a complex component image
		// channelsDoubleComplexComponents[] -> channelsComplexImages[]
		// TODO à compléter ...

		// Perform Fourier transform (dft) on Complex component image
		// channelsComplexImages[] -> channelsComplexSpectrums[] with
		// DFT_COMPLEX_OUTPUT
		// TODO à compléter ...

		// Split component Complex spectrum to real/imag channels
		// channelsComplexSpectrums[] -> channelsComplexSpectrumComponents[]
		// TODO à compléter ...

		// Compute component spectrum magnitude
		// channelsComplexSpectrumComponents[][0 & 1] -> channelsSpectrumMagnitude[]
		// TODO à compléter ...

		// Log scale magnitude with logScaleImg<double>(...) and logScaleFactor
		// channelsSpectrumMagnitude[] -> channelsSpectrumLogMagnitude[]
		// TODO à compléter ...

		// Convert Log scale channels Spectrum to display channels
		// channelsSpectrumLogMagnitude[] -> channelsSpectrumLogMagnitudeDisplay[]
		// TODO à compléter ...
	}

	// Merge channels spectrum Log magnitude to color spectrum image
	// channelsSpectrumLogMagnitudeDisplay -> spectrumMagnitudeImage
	// TODO à compléter ...

}

/*
 * Optimal dft size for current source image
 * @return the current optimal dft size
 */
int CvSimpleDFT::getOptimalDftSize() const
{
	return optimalDFTSize;
}

/*
 * Get current log scale factor
 * @return the current log scale factor
 */
double CvSimpleDFT::getLogScaleFactor() const
{
	return logScaleFactor;
}

/*
 * Setting the log scale factor
 * @param logScaleFactor the new log scale factor
 */
void CvSimpleDFT::setLogScaleFactor(double logScaleFactor)
{
	if (logScaleFactor > maxLogScaleFactor)
	{
		this->logScaleFactor = maxLogScaleFactor;
	}
	else if (logScaleFactor < minLogScaleFactor)
	{
		this->logScaleFactor = minLogScaleFactor;
	}
	else
	{
		this->logScaleFactor = logScaleFactor;
	}
}

// --------------------------------------------------------------------
// Utility methods
// --------------------------------------------------------------------
/*
 * Modify image to obtain reverse frequencies on the Fourier transform
 * (low frequencies at the center of the image and high frequencies on
 * the border), or modify image obtained from reverse Fourier transform
 * with reversed frequencies.
 * @param imgIn source image
 * @param imgOut destination image
 */
template <typename T>
void CvSimpleDFT::frequencyShift(Mat & imgIn, Mat & imgOut)
{
	int i, j;

	for (i = 0; i < imgIn.rows; i++)
	{
		for (j = 0; j < imgIn.cols; j++)
		{
			imgOut.at<T> (i, j) = imgIn.at<T> (i, j) * (T)pow(-1.0, i + j);
		}
	}
}

/*
 * Log scale T valued image
 * @param imgIn input image
 * @param imgOut output image
 * @param scaleFactor such as
 * \f$ imgOut = scaleFactor \times \log(1 + imgIn)\f$
 */
template <typename T>
void CvSimpleDFT::logScaleImg(const Mat & imgIn, Mat & imgOut,
	const T scaleFactor)
{
	MatConstIterator_<T> inIt = imgIn.begin<T>();
	MatConstIterator_<T> inItEnd = imgIn.end<T>();
	MatIterator_<T> outIt = imgOut.begin<T>();
	MatIterator_<T> outItEnd = imgOut.end<T>();
	for (; inIt != inItEnd && outIt != outItEnd; ++inIt, ++outIt)
	{
		(*outIt) = scaleFactor * (T)log(1.0 + (*inIt));
	}
}
