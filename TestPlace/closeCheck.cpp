#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include <ctime>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

Mat removeTouchBorder(Mat InputMat) {

	uchar white(255);
	// do top and bottom row
	for (int y = 0; y < InputMat.rows; y += InputMat.rows - 1)
	{
		uchar* row = InputMat.ptr<uchar>(y);
		for (int x = 0; x < InputMat.cols; ++x)
		{
			if (row[x] == white)
			{
				cv::floodFill(InputMat, cv::Point(x, y), cv::Scalar(0), (cv::Rect*)0, cv::Scalar(), cv::Scalar(200));
			}
		}
	}
	// fix left and right sides
	for (int y = 0; y < InputMat.rows; ++y)
	{
		uchar* row = InputMat.ptr<uchar>(y);
		for (int x = 0; x < InputMat.cols; x += InputMat.cols - 1)
		{
			if (row[x] == white)
			{
				cv::floodFill(InputMat, cv::Point(x, y), cv::Scalar(0), (cv::Rect*)0, cv::Scalar(), cv::Scalar(200));
			}
		}
	}
	return InputMat;
}

int main() {
	VideoCapture cap(3);
	if (cap.isOpened() == false) {
		cout << "~~~~~~~~~~~~~~~camera~~~~~~~~~~~~~~~~";
		return 1;
	}

	{
		Mat a;
		for (int i = 0; i < 20; i++) {
			cap >> a;
		}
	}

	while (true)
	{
		Mat camIn;
		Mat temp;
		Mat output;
		vector<vector<Point>> contours;
		vector<Point2d> centroids;
		cap >> camIn;
		int Hh = 68, Hl = 24, Sh = 255, Sl = 0, Vh = 139, Vl = 0;
		temp = camIn.clone();
		blur(temp, temp, Size(5,5));
		cvtColor(temp, temp, COLOR_RGB2HSV);
		inRange(temp, Scalar(Hl, Sl, Vl), Scalar(Hh, Sh, Vh), temp);
		temp = 255 - temp;
		morphologyEx(temp, temp, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(3, 3)));
		temp = removeTouchBorder(temp);
		morphologyEx(temp, temp, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(3, 3)));

		findContours(temp, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		for (auto c : contours) {
			Point2d centroid;
			int pNum = 0;
			for (auto p : c) {
				centroid.x += p.x;
				centroid.y += p.y;
				pNum++;
			}
			centroid = centroid / pNum;
			centroids.push_back(centroid);
		}

		output = temp.clone();
		cvtColor(output, output, COLOR_GRAY2RGB);
		cvtColor(output, output, COLOR_RGB2HSV);
		if (contours.empty() == false) {
			int colorStep = 255 / centroids.size();
			for (int i = 0; i < centroids.size(); i++) {
				circle(output, centroids[i], 2, Scalar((colorStep*i), 255, 255), -1);
				drawContours(output, contours, i, Scalar((colorStep*i), 255, 255));
			}

		}
		cvtColor(output, output, COLOR_HSV2RGB);
		imshow("check", output);
		if (waitKey(1) == 'q') break;
	}

	return 0;
}