#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include "HIWINPuzzlePack.h"
using namespace std;
using namespace cv;
int main() {
	VideoCapture cap(0);
	cap.set(CAP_PROP_FRAME_WIDTH, 1920);
	cap.set(CAP_PROP_FRAME_HEIGHT, 1080);
	Mat Img;
	HIWIN_Puzzle::MarkerFinder mFinder;
	namedWindow("place");


	/*這些不寫沒關西，class裡有預設值
	可以這樣選邊角定位方法，目前使用AprilTag2論文的方法，但是我個人覺得挺慢的，這邊推薦選用openCV內建的SubPixal演算法
	mFinder.parameters->cornerRefinementMethod = aruco::CORNER_REFINE_APRILTAG;
	mFinder.parameters->cornerRefinementMethod = aruco::CORNER_REFINE_SUBPIX;

	可以這樣選Tag資料集
	mFinder.dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_APRILTAG_36h11);
	mFinder.dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);
	*/

	for (int i = 0; i < 20; i++) {
		cap >> Img;
	}

	do {
		cap >> Img;

		HIWIN_Puzzle::Sharpen(Img, Img);

		//Mat Img = imread("hav.png");
		Img = mFinder.compute(Img);

		Mat show=mFinder.getDebugImage();
		//for (auto c : mFinder.getCornerPoints()) {
		//	cout << c << endl;
		////	座標順序為由紅框點順時針轉一圈
		//}

		for (auto md : mFinder.getMarkerData()) {
			circle(show, md.center, 5, Scalar(255, 150, 0), -1);
			putText(show, to_string(md.id), md.center, FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 255), 3);
		}

		resize(show, show, Size(1920, 1080) / 2);
		imshow("place", show);
	} while(waitKey(20) != 'q');

	cap.release();

	cout << "finish" << endl;
	return 0;
}