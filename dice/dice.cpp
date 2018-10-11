// dice.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

int thresholdSlider, thresholdSliderMax;
int cannyRatio = 3; //canny recommended upper:lower ration between 2:1 and 3:1

void on_change(int value, void*) {
	printf("%d\n", value);
}

int main()
{
	VideoCapture cap;
	Mat src;

	if (!cap.open(0)) return -1;

	cap.set(CAP_PROP_EXPOSURE, -6.0); //this is not supported by all webcams
	cap.set(CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CAP_PROP_FRAME_HEIGHT, 480);

	namedWindow("Binary image", true);
	thresholdSlider = 40;
	thresholdSliderMax = 255;

	createTrackbar("Threshold", "Binary image", &thresholdSlider, thresholdSliderMax, on_change);

	for (;;)
	{
		cap >> src;
		//imshow("src", src);
		
		Mat kernel = (Mat_<float>(3, 3) <<
			1, 1, 1,
			1, -8, 1,
			1, 1, 1);

		Mat imgLaplacian;
		filter2D(src, imgLaplacian, CV_32F, kernel);
		Mat sharp;
		src.convertTo(sharp, CV_32F);
		Mat imgResult = sharp - imgLaplacian;

		//convert back to 8bits gray scale
		imgResult.convertTo(imgResult, CV_8UC3);
		imgLaplacian.convertTo(imgLaplacian, CV_8UC3);

		imshow("new sharped image", imgResult);

		//create binary image from source image
		Mat bw;
		cvtColor(imgResult, bw, COLOR_BGR2GRAY);
		threshold(bw, bw, thresholdSlider, 255, THRESH_BINARY | THRESH_OTSU);
		imshow("Binary image", bw);

		//perform the distance transform algo
		Mat dist;
		distanceTransform(bw, dist, DIST_L2, 3);

		//normalize the distance image for range
		//so we can vis the threshold it
		normalize(dist, dist, 0, 1.0, NORM_MINMAX);
		imshow("distance transofrm image", dist);

		//threshold to obtain peaks
		threshold(dist, dist, 0.4, 1.0, THRESH_BINARY);

		//dilate a bit the dist image
		Mat kernel1 = Mat::ones(3, 3, CV_8U);
		dilate(dist, dist, kernel1);
		imshow("peaks", dist);

		//create the cv8u version of the distance iamge
		Mat dist_8u;
		dist.convertTo(dist_8u, CV_8U);

		//find total markers
		std::vector<std::vector<Point>> contours;
		findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//create the marker image for the watershed algo
		Mat markers = Mat::zeros(dist.size(), CV_32S);

		//draw the foreground markers
		for (size_t i = 0; i < contours.size(); i++) {
			drawContours(markers, contours, static_cast<int>(i), Scalar(static_cast<int>(i) + 1), -1);
		}

		//draw the background marker
		circle(markers, Point(5, 5), 3, Scalar(255), -1);
		imshow("markers", markers * 10000);

		//perform the watershed
		watershed(imgResult, markers);

		Mat mark;
		markers.convertTo(mark, CV_8U);
		bitwise_not(mark, mark);

		//imshow("markers_v2", mark);

		//generate random colors
		std::vector<Vec3b> colors;
		for (size_t i = 0; i < contours.size(); i++) {
			int b = theRNG().uniform(0, 256);
			int g = theRNG().uniform(0, 256);
			int r = theRNG().uniform(0, 256);
			colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
		}

		//create the result image
		Mat dst = Mat::zeros(markers.size(), CV_8UC3);

		//fill labeled objects with random colors
		for (int i = 0; i < markers.rows; i++)
		{
			for (int j = 0; j < markers.cols; j++)
			{
				int index = markers.at<int>(i, j);
				if (index > 0 && index <= static_cast<int>(contours.size()))
				{
					dst.at<Vec3b>(i, j) = colors[index - 1];
				}
			}
		}
		// Visualize the final image
		imshow("Final Result", dst);
		if (waitKey(1) == 27) break; // stop capturing by pressing ESC 
	}
	 
	//the camera will be closed automatically upon exit
	return 0;
}
