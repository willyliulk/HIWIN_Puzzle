// TestPlace.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <string>
#include <vector>

#include <ctime>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>


#define HAVE_CAM true


using namespace std;
using namespace cv;

typedef vector<vector<Point>> Contours;


void myConvexHull(Mat &inMat, Mat &outMat, vector<vector<Point>> &outHulls);
void myMaskByContour(Mat &inMat, Mat &outMat, vector<vector<Point>> contours);

#if 1
int Hh = 85, Hl = 21, Sh = 113, Sl = 49, Vh = 168, Vl = 49;
int L = 70, T = 70, R = 130, B = 130;
#endif

int main()
{
#if HAVE_CAM
	VideoCapture cap(3);
	if (!cap.isOpened()) {
		cout << "\n\nCannot open camera\n\n";
		return 1;
	}
#endif

	clock_t t1, t2;
	int i = 0;

	Mat camInput;
	Mat backGround = imread("background.jpg");
	if (backGround.empty()) {
		cout << "background.jpg not found" << endl;
		return 1;
	}


	//namedWindow("滑條", WINDOW_AUTOSIZE);
	//createTrackbar("L", "滑條", &L, 300);
	//createTrackbar("T", "滑條", &T, 300);
	//createTrackbar("R", "滑條", &R, 300);
	//createTrackbar("B", "滑條", &B, 300);
	//createTrackbar("Hh", "滑條", &Hh, 255);
	//createTrackbar("Hl", "滑條", &Hl, 255);
	//createTrackbar("Sh", "滑條", &Sh, 255);
	//createTrackbar("Sl", "滑條", &Sl, 255);
	//createTrackbar("Vh", "滑條", &Vh, 255);
	//createTrackbar("Vl", "滑條", &Vl, 255);


	//namedWindow("live", WINDOW_AUTOSIZE); // 命名一個視窗，可不寫
	while (true) {
		t2 = clock();
		Mat showOutput,
			thresholeOutput,
			maskOutput,
			ImaskOutput,
			morphologyOutput;

		//Mat cvtColorOutput,
		//	thresholdOutput,
		//	morphologyOutput,
		//	morphologyOutput2,
		//	convexHullOutput,
		//	maskOutput;
		//Contours RIOtoMask;

		//Mat minusOutput,
		//	cvtOutput,
		//	thresholdOutput;

		//Mat bgModel, fgModle;
		//Mat mask;
		//Rect rect(10, 100, 380, 180);
		Mat cvtOutput;

#if HAVE_CAM
		// 擷取影像
		bool ret = cap.read(camInput); // or cap >> frame;
		if (!ret) {
			cout << "Can't receive frame (stream end?). Exiting ...\n";
			break;
		}
		//resize(camInput, camInput, Size(1920, 1080));

#else
		camInput = imread(R"(C:\Users\liu willy\Desktop\WIN_20210926_17_22_44_Pro.jpg)");
		resize(camInput, camInput, Size(1280, 720));
#endif

		// 彩色轉灰階
		//cvtColor(camInput, cvtColorOutput, COLOR_RGB2GRAY);
		//threshold(cvtColorOutput, thresholdOutput, 86, 255, THRESH_BINARY_INV);
		//morphologyEx(thresholdOutput, morphologyOutput, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(15, 15)));
		//morphologyEx(morphologyOutput, morphologyOutput2, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(30, 30)));
		//myConvexHull(morphologyOutput2, convexHullOutput, RIOtoMask);
		//myMaskByContour(convexHullOutput, maskOutput, RIOtoMask);
		//morphologyEx(maskOutput, showOutput, MORPH_TOPHAT, getStructuringElement(MORPH_RECT, Size(30, 30)), Point(-1, -1), 15);

		//subtract(camInput, backGround, minusOutput);
		//cvtColor(minusOutput, cvtOutput, COLOR_RGB2GRAY);
		//threshold(cvtOutput, thresholdOutput, 20, 255, THRESH_BINARY);

		//grabCut(camInput, mask, rect, bgModel, fgModle, 1);
		//Mat binMask(mask.size(), CV_8SC1);
		//binMask = mask & 1;
		//camInput.copyTo(showOutput, binMask);


		cvtColor(camInput, cvtOutput, COLOR_RGB2HSV);
		inRange(cvtOutput, Scalar(Hl, Sl, Vl), Scalar(Hh, Sh, Vh), thresholeOutput);
		bitwise_not(thresholeOutput, ImaskOutput);
		camInput.copyTo(maskOutput, ImaskOutput);
		//printf("%d %d %d %d %d %d\n", Hl, Sl, Vl, Hh, Sh, Vh);
		morphologyEx(maskOutput, morphologyOutput, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		showOutput = morphologyOutput;

		Mat extract = showOutput(Rect(L, T, R, B));
		Mat extractGrey;
		Contours c;
		vector<Point2f> vtxTemp;
		vector<vector<Point2f>> vtxs;
		
		cvtColor(extract, extractGrey, COLOR_RGB2GRAY);
		findContours(extractGrey, c, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		//drawContours(extract, c, -1, Scalar(0, 255, 0));
		for (auto con : c) {
			//Rect b = boundingRect(con);
			//rectangle(extract, b, Scalar(0, 255, 0));	
			RotatedRect rb = minAreaRect(con);
			Point2f vtx[4];
			rb.points(vtx);
			for (int i = 0; i < 3;i++) {
				line(extract, vtx[i], vtx[i + 1], Scalar(255, 0, 0));
			}
			line(extract, vtx[0], vtx[3], Scalar(255, 0, 0));
		}

		//drawContours(extract, c, -1, Scalar(0, 0, 255));
		
		
		// 顯示圖片
		imshow("live", showOutput);
		imshow("raw", camInput);
		imshow("extrect", extract);


		// 按下 q 鍵離開迴圈
		if (waitKey(1) == 'q') {
			cout << "--------------end--------\n";
			destroyAllWindows();
			//imwrite("background.jpg", camInput);
			break;
		}
		cout << "process time=" << double(clock() - t2) / CLOCKS_PER_SEC << endl;

	}
	// VideoCapture 會自動在解構子裡釋放資源
	return 0;
}



/*凸包和移除處碰邊界的contour*/
void myConvexHull(Mat &inMat, Mat &outMat, vector<vector<Point>> &outHulls) {
	inMat.copyTo(outMat);
	vector<vector<Point>> contours;
	vector<vector<Point>> hulls;
	vector<Point> hull;
	findContours(inMat, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (auto coutour : contours) {
		//找 convexHull
		convexHull(coutour, hull);

		//移除觸碰邊界的東東
		bool touchBorder = false;
		for (Point p : hull) {
			if (p.x <= 0 || p.x >= outMat.cols) touchBorder = true;
			else if (p.y <= 0 || p.y >= outMat.rows) touchBorder = true;
		}
		if (touchBorder == false) {
			outHulls.push_back(hull);
		}

	}

	drawContours(outMat, hulls, -1, Scalar(0, 255, 0));
}


void myMaskByContour(Mat &inMat, Mat &outMat, vector<vector<Point>> contours) {
	inMat.copyTo(outMat);
	Mat outMask = inMat.clone();
	fillPoly(outMask, contours, Scalar(0, 0, 0));
	outMat.setTo(0, outMask);
	//imshow("mid", outMat);
}

void myChangeBright(Mat &inMat, int changeL) {
	cvtColor(inMat.clone(), inMat, COLOR_RGB2HLS);

}

