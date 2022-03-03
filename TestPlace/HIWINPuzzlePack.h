#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include <ctime>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <apriltag.h>
#include <tag36h11.h>

using namespace std;

#define _DEBUG 0

namespace HIWIN_Puzzle {
	void DEBUG_MSG(string s);

	cv::Mat Transform_Matrix_Count(cv::Mat Camera_Matrix);

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
		/*取得形心*/
		vector<cv::Point> getCentroids();
		/*取得包圍框*/
		vector<cv::Rect> getBoundingBoxes();


	private:
		cv::Mat img_orig, img_onlyPuzz, img_BW_onlyPuzz;
		cv::Mat mask_ROI, mask_greenBG, mask_wholeBG, mask_noPuzz;

		//你可能會需要調的參數
		//x, y, width, height
		cv::Rect rect_ROI = cv::Rect(189, 182, 859, 433);
		cv::Scalar HSV_Upper = cv::Scalar(68, 255, 152),
			HSV_Lower = cv::Scalar(24, 0, 0);
		cv::Size size_orig;
		int halfBox = 90;

		vector<cv::Mat> puzzles;
		vector<cv::Point> centroids;
		vector<cv::Rect> boundingBoxes;
		cv::Mat img_debug;

		bool modiROI = false, modiHSV = false;


		/*給入contour 回傳形心*/
		cv::Point Centroid(vector<cv::Point> contour);
	};


	class MarkerFinder
	{
	public:
		MarkerFinder();
		~MarkerFinder();

		/*輸入圖片找Tag*/
		cv::Mat compute(cv::Mat inImage);
		/*取得所有Tag角點*/
		vector<vector<cv::Point2f>> getCornerPoints();
		/*取得所有Tag中心點*/
		vector<cv::Point2f> getMarkerCenters();
		/*取得所有Tag Debug圖*/
		cv::Mat getDebugImage();
		/*取得所有Tag ID*/
		vector<int> getMarkerIds();

		typedef struct markerData_t
		{
			cv::Point2f center;
			vector<cv::Point2f> corners;
			int id;
		};

		/*取得所有Tag 資訊*/
		vector<markerData_t> getMarkerData();

		cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
		cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_APRILTAG_36h11);
	private:
		vector<vector<cv::Point2f>> markerCorners, rejectedCandidates;

		cv::Mat inImg, debugImage;
		vector<cv::Point2f> markerCenters;
		std::vector<int> markerIds;
		vector<markerData_t> markerDatas;

	};

}