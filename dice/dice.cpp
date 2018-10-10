// dice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
int main()
{
	//Mat img = imread("lena10.jpg");
	//namedWindow("image", WINDOW_NORMAL);
	//imshow("image", img);
	//waitKey(0);
	namedWindow("frame", true);
	VideoCapture cap;
	// open the default camera, use something different from 0 otherwise;
	// Check VideoCapture documentation.
	if (!cap.open(1))
		return 0;

	
	Mat emptyFrame;
	for (int i = 0; i < 24*5; i++) {
		cap >> emptyFrame;
	}
	cvtColor(emptyFrame, emptyFrame, CV_BGR2GRAY);
	for (;;) {
		imshow("frame", emptyFrame);
		if (waitKey(30) == 27) break;
	}
	Mat frame;

	for (;;)
	{
		
		cap >> frame;
		if (frame.empty()) break; // end of video stream
		//std::cout << "rows:" << frame.rows << "cols: " << frame.cols;
		cvtColor(frame, frame, CV_BGR2GRAY);
		absdiff(frame, emptyFrame, frame);
		threshold(frame, frame, 150, 255, cv::THRESH_BINARY | CV_THRESH_OTSU);
		Canny(frame, frame, 2, 2 * 2, 3, false);
		imshow("frame", frame);
		if (waitKey(30) == 27) break; // stop capturing by pressing ESC 
	}
	 
	//the camera will be closed automatically upon exit
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
