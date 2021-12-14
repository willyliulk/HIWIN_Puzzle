#include <iostream>
#include <opencv2/opencv.hpp>

#include "HIWINPuzzlePack.h"

using namespace std;
using namespace cv;
int main() {
	Mat imgIn;
	imgIn = imread(R"(2-4.png)");
	if (imgIn.empty()) {
		cerr << "imread error\n";
		return 1;
	}



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
	destroyAllWindows();
}

