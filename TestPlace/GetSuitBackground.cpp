#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include <ctime>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>


#define CONTROL_BAR false
#define HAVE_CAM true


using namespace std;
using namespace cv;

int H, S, V;
void Hbar(int h, void*) {
	H = h;
}
void Sbar(int s, void*) {
	S = s;
}
void Vbar(int v, void*) {
	V = v;
}

int main() {
	string camWin = "诀", conWin = "北";
	
#if CONTROL_BAR
	namedWindow(camWin);
	namedWindow(conWin);
	createTrackbar("H value", conWin, &H, 255, Hbar);
	createTrackbar("S value", conWin, &S, 255, Sbar);
	createTrackbar("V value", conWin, &V, 255, Vbar);
#endif

#if HAVE_CAM
	VideoCapture cap(3);
	if (cap.isOpened() == false) {
		cout << "\n~~~~~~~~~~~~~~camera error~~~~~~~~~~~~~~~~\n" << endl;
		return 1;
	}

	Mat camInput;
	bool ret = cap.read(camInput); // or cap >> frame;
	if (!ret) {
		cout << "Can't receive frame (stream end?). Exiting ...\n";
		return 1;
	}
#else // HAVE_CAM
	Mat camInput = imread(R"(C:\Users\liu willy\Desktop\WIN_20210926_17_22_44_Pro.jpg)");
	resize(camInput, camInput, Size(1280, 720));
#endif

	Rect myROI = selectROI(camInput);
	destroyWindow("ROI selector");

	while (true) {
#if HAVE_CAM
		cap >> camInput;
#else

#endif // HAVE_CAM
		Mat HSVimg;
		cvtColor(camInput, HSVimg, COLOR_RGB2HSV);
		Mat mask = Mat::zeros(HSVimg.size(), CV_8U);
		rectangle(mask, myROI, Scalar(1), -1);
		mask(myROI).setTo(255);
		//imshow("mask", mask);

		int channels = 0;
		int histSize[1];
		int histsize = 256;
		Mat histOutH, histOutS, histOutV;
		float range[] = { 0,256 };
		const float* ranges = { range };
		calcHist(&HSVimg, 1, &channels, mask, histOutH, 1, &histsize, &ranges);
		channels = 1;
		calcHist(&HSVimg, 1, &channels, mask, histOutS, 1, &histsize, &ranges);
		channels = 2;
		calcHist(&HSVimg, 1, &channels, mask, histOutV, 1, &histsize, &ranges);




		vector<Mat> HSVimgs({ histOutH , histOutS , histOutV });
		Mat HistImg(400, 256 * 2, CV_8UC3);
		HistImg.setTo(0);

		Mat minColorMat(HSVimg.size(), CV_8UC3);
		cvtColor(minColorMat, minColorMat, COLOR_RGB2HSV);
		int minH, minS, minV;

		int index = 0;
		for (Mat histOut : HSVimgs) {
			
			double greyMax = 0;
			double greyMin = 0;
			Point MaxLoc;
			Point MinLoc;
			minMaxLoc(histOut, &greyMin, &greyMax, &MinLoc, &MaxLoc);

			Scalar color;
			if (index == 0) {
				color = Scalar(255, 0, 0);
				minH = MinLoc.y;
			}
			else if (index == 1) {
				color = Scalar(0, 255, 0);
				minS = MinLoc.y;
			}
			else {
				color = Scalar(0, 0, 255);
				minV = MinLoc.y;
			}
			Point  p1, p2;

			for (size_t i = 0; i < 256; i++) {
				float bin = histOut.at<float>(i);
				int intensity = cvRound(bin / greyMax * 400);
				p2 = Point(i * 2, 400 - intensity);
				if (i == 0) p1 = p2;
				line(HistImg,
					p1,
					p2,
					color);
				p1 = p2;
			}
			
			index++;
		}
		minColorMat.setTo(Scalar(minH, minS, minV));
		imshow("color", minColorMat);
		printf("(\t%d\t%d\t%d)\n", minH, minS, minV);

		imshow("hist", HistImg);

		//cout << "程:" << greyMax << "程:" << greyMin << endl;
		//cout << "程At:" << MaxLoc.y << "程At:" << MinLoc.y << endl;
		rectangle(camInput, myROI, Scalar(0, 255, 0));
		putText(camInput, to_string(minH), Point(0, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
		putText(camInput, to_string(minS), Point(0, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
		putText(camInput, to_string(minV), Point(0, 90), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));

		imshow(camWin, camInput);
		if (waitKey(1) == 'q') break;
	}

}