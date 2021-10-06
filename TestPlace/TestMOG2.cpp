#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include <ctime>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#define SAVE_PATH R"(D:\比賽\2021 上銀機器手臂\test mtr\puzzles\)"
#define SELECT_HSV false
#define SELECT_AREA false

#if SELECT_HSV
int Hh = 255, Hl = 0, Sh = 255, Sl = 0, Vh = 255, Vl = 0;

void HhBar(int i, void*) {}
void HlBar(int i, void*) {}
void ShBar(int i, void*) {}
void SlBar(int i, void*) {}
void VhBar(int i, void*) {}
void VlBar(int i, void*) {}

#else
int Hh = 68, Hl = 24, Sh = 255, Sl = 0, Vh = 139, Vl = 0;
#endif // SELECT_HSV

#if SELECT_AREA
int Amax = 8000, Amin = 2000;
#else
int Amax = 7000, Amin = 3000;
#endif // SELECT_AREA


using namespace std;
using namespace cv;

Point Centroid(vector<Point> contour) {
	double x = 0, y = 0;
	double s = contour.size();
	for (auto p : contour) {
		x += p.x;
		y += p.y;
	}
	return Point(x / s, y / s);
}

int main() {
	VideoCapture cap(3);
	if (cap.isOpened() == false) {
		cout << "~~~~~~~~~~~~~~~camera~~~~~~~~~~~~~~~~";
		return 1;
	}

#if SELECT_HSV
	namedWindow("control");
	createTrackbar("Hh", "control", &Hh, 255, HhBar);
	createTrackbar("Hl", "control", &Hl, 255, HlBar);
	createTrackbar("Sh", "control", &Sh, 255, ShBar);
	createTrackbar("Sl", "control", &Sl, 255, SlBar);
	createTrackbar("Vh", "control", &Vh, 255, VhBar);
	createTrackbar("Vl", "control", &Vl, 255, VlBar);
#endif // SELECT_HSV

#if SELECT_AREA
	namedWindow("area control");
	createTrackbar("max", "area control", &Amax, 10000);
	createTrackbar("min", "area control", &Amin, 10000);
#endif // SELECT_AREA

	String savePath = SAVE_PATH;

	Mat camIn, camOrig;


	for (int i = 0; i < 20; i++) {
		cap >> camIn;
	}

	cap >> camIn;

	Rect myROI(48, 20, 567, 400);
	Mat outROI(Size(camIn.cols, camIn.rows), CV_8UC1);
	outROI.setTo(255);
	///myROI = selectROI(camIn);
	rectangle(outROI, myROI, 0, -1);
	//imshow("roi", outROI);
	cout << "~~~~~~~~~~~~~" << myROI << "~~~~~~~~~~~~~~~~~~~~~~" << endl;

	Mat outGreenMat;
	{
		Mat HSVOutput,
			inRangeOutput,
			morphologyOutput,
			inverOutput;
		vector<vector<Point>> contours;

		cvtColor(camIn, HSVOutput, COLOR_RGB2HSV);
		inRange(HSVOutput, Scalar(Hl, Vl, Sl), Scalar(Hh, Sh, Vh), inRangeOutput);
		camIn.setTo(0, inRangeOutput);
		morphologyEx(inRangeOutput, morphologyOutput, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(15, 15)));

		findContours(morphologyOutput, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		fillPoly(morphologyOutput, contours, Scalar(255, 255, 255));
		bitwise_not(morphologyOutput, inverOutput);
		bitwise_or(inverOutput, outROI, outGreenMat);
	}
	//imshow("out green", outGreenMat);

	while (true)
	{
		Mat HSVOutput,
			inRangeOutput,
			mask;
		Mat greenMat, camblack;

		cap >> camIn;

		camOrig = camIn.clone();

		cvtColor(camIn, HSVOutput, COLOR_RGB2HSV);
		inRange(HSVOutput, Scalar(Hl, Vl, Sl), Scalar(Hh, Sh, Vh), inRangeOutput);
		bitwise_or(inRangeOutput, outGreenMat, mask);
		camIn.setTo(0, mask);
		inRangeOutput.copyTo(greenMat);
		bitwise_not(mask, camblack);

		//------------find rect------------
		Mat cannyOutout;
		vector<vector<Point>> contours;
		vector<Rect> rects;

		Canny(camblack, cannyOutout, 200, 300);
		findContours(camblack, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		double x, y;
		for (auto contour : contours) {
			//Rect tempRect = boundingRect(contour);
			//if (tempRect.area() < Amin) continue;
			//if (tempRect.area() > Amax) continue;
			//if (tempRect.tl().x <= 20 ||
			//	tempRect.tl().y <= 20 ||
			//	tempRect.br().x >= camIn.cols - 10 ||
			//	tempRect.br().y >= camIn.rows - 10) continue;
			//rects.push_back(tempRect);
			//rectangle(camIn, tempRect, Scalar(0, 255, 0));
			int halfBox = 50;
			if (contourArea(contour) < 150) continue;
			if (boundingRect(contour).area() > 7000)continue;
			//fillPoly(camIn, contour, Scalar(0, 255, 0));
			Point centroid = Centroid(contour);
			Rect tempRect = Rect(centroid.x - halfBox,
				centroid.y - halfBox,
				halfBox * 2,
				halfBox * 2);
			circle(camIn, centroid, 2, Scalar(0, 255, 0), -1);
			rectangle(camIn, tempRect, Scalar(0, 255, 0));

			//Mat subMat= camblack()
		}
		//-------------find rect------------

		imshow("green mat", camblack);
		imshow("cam", camIn);

		char key = waitKey(10);
		if (key == 'q') {
			break;
		}
		if (key == 'y') {
			int i = 1;
			//double x, y;
			for (auto contour : contours) {
				//Rect tempRect = boundingRect(contour);
				//if (tempRect.area() < Amin) continue;
				//if (tempRect.area() > Amax) continue;
				//if (tempRect.tl().x <= 20 ||
				//	tempRect.tl().y <= 20 ||
				//	tempRect.br().x >= camIn.cols - 10 ||
				//	tempRect.br().y >= camIn.rows - 10) continue;
				//rects.push_back(tempRect);
				//rectangle(camIn, tempRect, Scalar(0, 255, 0));
				int halfBox = 50;
				if (contourArea(contour) < 150) continue;
				if (boundingRect(contour).area() > 7000)continue;
				//fillPoly(camIn, contour, Scalar(0, 255, 0));
				Point centroid = Centroid(contour);
				Rect tempRect = Rect(centroid.x - halfBox,
					centroid.y - halfBox,
					halfBox * 2,
					halfBox * 2);
				//circle(camIn, centroid, 2, Scalar(0, 255, 0), -1);
				//rectangle(camIn, tempRect, Scalar(0, 255, 0));

				Mat subMatMask = camOrig.clone();
				subMatMask.setTo(255);

				fillPoly(subMatMask, contour, Scalar(0, 0, 0));

				Mat colMat = camOrig.clone();
				colMat.setTo(0, subMatMask);
				Mat subOut;
				colMat(tempRect).copyTo(subOut);
				//subOut.setTo(0, subMatMask);

				//imshow("1", subOut);
				//Mat subMatOut;
				//camOrig.copyTo(subMatOut, subMatMask);
				imwrite(savePath + to_string(i) + ".png", subOut);

				//Mat subMat= camblack()
				i++;
				//break;
			}
			break;
		}

	}

	waitKey(0);
	return 0;
}