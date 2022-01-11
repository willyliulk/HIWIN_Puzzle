#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include <ctime>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

#define _DEBUG 0

namespace HIWIN_Puzzle{
	void DEBUG_MSG(string s);

	class RotationFinder {
	private:
		double angle_result;
		cv::Mat puzzle;
		cv::Point2d centroid_distance;
		cv::Point2d centroid_shape;
	public:
		RotationFinder();

		/**可以直接給入拼圖，省去再寫一次find()的麻煩*/
		RotationFinder(cv::Mat puzIn);

		/**找出拼圖的角度，每一個拼圖會有屬於它自己的角度值，之後請自己對應出這是哪片拼圖*/
		double find(cv::Mat puzzle);

		/**取得計算出的角度，必須要先有給過拼圖圖片才可使用*/
		double get_angle();

		/**取得給入的圖片*/
		cv::Mat get_image();
	};

	class PuzzleFinder
	{
	public:
		PuzzleFinder();
		/*使用這函示給入圖片時會進行初始化，初始化會占用點時間
		若不想在建立物件時耗時，可以使用無參數的建構子
		後面在想初始化時使用函示init()*/
		PuzzleFinder(cv::Mat inImg);
		~PuzzleFinder();

		/*初始化去除背景資訊並建立遮罩，同背景只須執行一次就好，要先用這個才能使用compute(cv::Mat)*/
		void init(cv::Mat);
		/*計算每個拼圖的位置*/
		void compute(cv::Mat);
		/*設定HSV值*/
		void setHSV(cv::Scalar lower, cv::Scalar upper);
		/*設定遮罩ROI*/
		void setROI(cv::Rect);
		/*取得計算結果圖片*/
		vector<cv::Mat> getImages();
		/*取得標記圖片位置與形心的完整圖片，用於除錯*/
		cv::Mat getDebugImg();
		/*取得輸入圖片*/
		cv::Mat getOrigImage();


		
	private:
		cv::Mat img_orig, img_onlyPuzz, img_BW_onlyPuzz;
		cv::Mat mask_ROI, mask_greenBG, mask_wholeBG, mask_noPuzz;
		//x, y, width, height
		cv::Rect rect_ROI = cv::Rect(189, 182, 859, 433);
		cv::Scalar HSV_Upper = cv::Scalar(68, 255, 152),
					HSV_Lower = cv::Scalar(24, 0, 0);
		cv::Size size_orig;
		int halfBox = 50;

		vector<cv::Mat> puzzles;
		cv::Mat img_debug;

		bool modiROI=false, modiHSV=false;;


		/*給入contour 回傳形心*/
		cv::Point Centroid(vector<cv::Point> contour);
	};


}