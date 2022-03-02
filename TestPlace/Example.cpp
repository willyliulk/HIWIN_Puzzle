#include <iostream>
#include <opencv2/opencv.hpp>

#include "HIWINPuzzlePack.h"

using namespace std;
using namespace cv;
int main() {
	Mat imgIn;

	//從相機讀圖
	//VideoCapture cap(0);
	//if (cap.isOpened() == false) {
	//	cout << "camera error\n";
	//}
	//else {
	//	
	//	for (int i = 0; i < 20; i++) {
	//		cap >> imgIn;
	//	}
	//
	//cap >> imgIn;
	
	//從檔案讀圖
	imgIn = imread(R"(C:\Users\liuWilly\Pictures\Camera Roll\WIN_20220106_23_24_19_Pro.jpg)");
	//imgIn = imread("2-4.png");

	HIWIN_Puzzle::PuzzleFinder pFinder;

	Rect roi;
	//用ROI框出拼圖
	roi = selectROI(imgIn);
	pFinder.setROI(roi);

	//這樣也行
	//pFinder.setROI(selectROI(imgIn));

	pFinder.init(imgIn);
	pFinder.compute(imgIn);

	for (int i = 0; i < pFinder.getCentroids().size(); i++) {
		imshow("puzzle piece", pFinder.getImages()[i]);
		cout << "centoids : \t" << pFinder.getCentroids()[i] << endl;
		cout << "bounding box : \t" << pFinder.getBoundingBoxes()[i] << endl;
		cout << endl;
		waitKey(0);
	}


//-----------範例1---------------
	//VideoCapture cap(0);
	//if (cap.isOpened() == false) {
	//	cout << "camera error\n";
	//}
	//else {
	//	
	//	for (int i = 0; i < 20; i++) {
	//		cap >> imgIn;
	//	}

	//	cap >> imgIn;
	//	if (imgIn.empty()) {
	//		cerr << "imread error\n";
	//	}
	//	else {

	//		imshow("orig", imgIn);

	//		HIWIN_Puzzle::PuzzleFinder pFinder;
	//		//設定ROI範例
	//		/*
	//		Rect roi;
	//		roi = selectROI(imgIn);
	//		pFinder.setROI(roi);
	//		*/

	//		//設定HSV範例
	//		/*
	//		Scalar upper(255, 255, 255), lower(0, 0, 0);
	//		pFinder.setHSV(upper, lower); //方法1
	//		pFinder.setHSV(Scalar(255, 255, 255), Scalar(0, 0, 0)); //方法2
	//		*/

	//		//讀圖片不會慢的範例
	//		pFinder.init(imgIn);
	//		while (1) {
	//			cap >> imgIn;
	//			pFinder.compute(imgIn);
	//			Mat show = pFinder.getDebugImg();

	//			for (auto centroid : pFinder.getCentroids()) {

	//			}

	//			imshow("asadasd", show);
	//			if (waitKey(20) == 'q') {
	//				break;
	//			}
	//		}

	//		//取得圖片範例
	//		/*
	//		for (auto pic : pFinder.getImages()) {
	//			imshow("imgs", pic);
	//			waitKey(0);
	//		}
	//		*/
	//		destroyAllWindows();
	//	}
	//}
//----------範例1結束-------------------

//----------範例2--------------------
	//HIWIN_Puzzle::RotationFinder rFinder;
	//rFinder.find(imgIn);
	//double angle = rFinder.get_angle();
	//Point sp, ep;
	//sp.x = 50. + 20. * cos(angle);
	//sp.y = 50. + 20. * sin(angle);
	//ep.x = 50. - 20. * cos(angle);
	//ep.y = 50. - 20. * sin(angle);

	//arrowedLine(imgIn, sp, ep, Scalar(0, 255, 0));
	//imshow("wind", imgIn);
	//waitKey(0);
	//destroyAllWindows();
//-----------範例2結束-----------------
}

