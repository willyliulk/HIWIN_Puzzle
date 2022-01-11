#include <iostream>
#include <opencv2/opencv.hpp>

#include "HIWINPuzzlePack.h"

using namespace std;
using namespace cv;
int main() {
	Mat imgIn;
	imgIn = imread(R"(C:\Users\liuWilly\Pictures\Camera Roll\WIN_20220106_23_24_19_Pro.jpg)");
	if (imgIn.empty()) {
		cerr << "imread error\n";
		return 1;
	}

	imshow("orig", imgIn);

	//[859 x 433 from(189, 182)]
	//Rect rect = selectROI(imgIn);
	//cout << rect;
	HIWIN_Puzzle::PuzzleFinder pFinder;
	//pFinder.setROI(rect);
	pFinder.init(imgIn);
	pFinder.compute(imgIn);
	Mat show = pFinder.getDebugImg();
	for (auto pic : pFinder.getImages()) {
		imshow("imgs", pic);
		waitKey(0);
	}
	imshow("asadasd", show);
	waitKey(0);
	destroyAllWindows();


/*	vector<Mat> v;
	v = pFinder.getImages();
	for (auto img : v) {
		imshow("a", img);
		waitKey(0);
	}
	destroyAllWindows();*/

/*
	HIWIN_Puzzle::RotationFinder rFinder;
	rFinder.find(imgIn);
	double angle = rFinder.get_angle();
	Point sp, ep;
	sp.x = 50. + 20. * cos(angle);
	sp.y = 50. + 20. * sin(angle);
	ep.x = 50. - 20. * cos(angle);
	ep.y = 50. - 20. * sin(angle);

	arrowedLine(imgIn, sp, ep, Scalar(0, 255, 0));
	imshow("wind", imgIn);
	waitKey(0);
	destroyAllWindows();*/

}

