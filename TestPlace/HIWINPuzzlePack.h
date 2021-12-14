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

		/**�i�H�������J���ϡA�٥h�A�g�@��find()���·�*/
		RotationFinder(cv::Mat puzIn);

		/**��X���Ϫ����סA�C�@�ӫ��Ϸ|���ݩ󥦦ۤv�����׭ȡA����Цۤv�����X�o�O��������*/
		double find(cv::Mat puzzle);

		/**���o�p��X�����סA�����n�������L���ϹϤ��~�i�ϥ�*/
		double get_angle();

		/**���o���J���Ϥ�*/
		cv::Mat get_image();
	};
}