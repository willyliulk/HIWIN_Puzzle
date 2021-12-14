#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include <ctime>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

#define _DEBUG 1

namespace HIWIN_Puzzle{
	class RotationFinder {
	private:
		double angle_result;
		cv::Mat puzzle;
		cv::Point2d centroid_distance;
		cv::Point2d centroid_shape;
		void DEBUG_MSG(string s);
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
}