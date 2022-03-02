#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include "HIWINPuzzlePack.h"

using namespace std;
using namespace cv;

void aaa(int *a) {
	cout << a[0] << endl << a[1] << endl;
}

int main() {

	int a[] = { 1, 2 };
	aaa(vector<int>{3,4}.data());




	//Mat image = imread("test.jpg");
	//Mat wb;
	//resize(image, image, Size(image.size[1], image.size[0]) / 4);
	//Mat imgRGB = image.clone();

	//cvtColor(image, image, COLOR_RGB2HSV);
	//inRange(image, Scalar(35, 0, 50), Scalar(75, 255, 255), image);
	//morphologyEx(image, image, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(8, 8)));

	//cv::Mat labels, stats, centroids;
	//int nConnect = cv::connectedComponentsWithStats(image, labels, stats, centroids);

	//double max;
	//minMaxLoc(labels, (double *)0, &max);
	//cout << max << endl;

	//Mat show(imgRGB.size[0], imgRGB.size[1], CV_8UC3);
	//cvtColor(show, show, COLOR_RGB2HSV);
	//for (int i = 0; i < max; i++) {

	//}


	//imshow("saa", stats);
	//waitKey(0);
	//destroyAllWindows();
	return 0;
}