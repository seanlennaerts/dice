// dice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

int thresholdSlider, thresholdSliderMax;
int cannyThreshold1, cannyThreshold1Max;
int cannyRatio = 3; //canny recommended upper:lower ration between 2:1 and 3:1
int blurSize, blurSizeMax;
int diceSize, diceSizeMax;

void on_change(int value, void*) {
	printf("%d\n", value);
}

int main()
{
	VideoCapture cap;
	Mat frame;
	Mat originalFrame;

	if (!cap.open(0)) return -1;

	cap.set(CAP_PROP_EXPOSURE, 0.0); //this is not supported by all webcams
	//cap.set(CAP_PROP_AUTO_EXPOSURE, 0.25);
	//cap.set(CAP_PROP_FPS, 10.0);

	// Get backgroundFrame
	Mat emptyFrame;
	namedWindow("frame", true);
	namedWindow("originalFrame", true);
	thresholdSlider = 150;
	thresholdSliderMax = 255;
	cannyThreshold1 = 2;
	cannyThreshold1Max = 100;
	blurSize = 3;
	blurSizeMax = 100;
	diceSize = 100;
	diceSizeMax = 3000;
	
	// Janik: uncommented the loop because I've manually set the exposure in line 28
	//for (int i = 0; i < 24*3; i++) {
		cap >> emptyFrame;
	//}
	cvtColor(emptyFrame, emptyFrame, CV_BGR2GRAY);
	for (;;) {
		imshow("frame", emptyFrame);
		if (waitKey(30) == 27) break;
	}
	
	createTrackbar("Threshold", "frame", &thresholdSlider, thresholdSliderMax, on_change);
	createTrackbar("Blur", "frame", &blurSize, blurSizeMax, on_change);
	createTrackbar("Canny1", "frame", &cannyThreshold1, cannyThreshold1Max, on_change);
	createTrackbar("DiceSize", "frame", &diceSize, diceSizeMax, on_change);

	


	for (;;)
	{	
		cap >> frame;
		originalFrame = frame.clone();
		cvtColor(frame, frame, CV_BGR2GRAY);
		absdiff(frame, emptyFrame, frame);
		threshold(frame, frame, thresholdSlider, 255, cv::THRESH_BINARY | CV_THRESH_OTSU);
		blur(frame, frame, Size(blurSize, blurSize));
		Canny(frame, frame, cannyThreshold1, cannyThreshold1 * cannyRatio, 3, false);

		std::vector<std::vector<Point>> diceContours;
		std::vector<Vec4i> diceHierarchy;
		findContours(frame.clone(), diceContours, diceHierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//iterate over contours
		for (int i = 0; i < diceContours.size(); i++) {
			double diceContourArea = contourArea(diceContours[i]);

			if (diceContourArea > diceSize) {
				Rect diceBoundsRect = boundingRect(Mat(diceContours[i]));
				Mat diceROI = frame(diceBoundsRect);

				//draw bounding rect
				Scalar color = Scalar(0, 153, 255);
				rectangle(originalFrame, diceBoundsRect.tl(), diceBoundsRect.br(), color, 2, 8, 0);

				imshow("originalFrame", originalFrame);
			}
		}

		imshow("frame", frame);
		if (waitKey(300) == 27) break; // stop capturing by pressing ESC 
	}
	 
	//the camera will be closed automatically upon exit
	return 0;
}
