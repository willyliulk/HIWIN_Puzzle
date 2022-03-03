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

		/**�i�H�������J���ϡA�٥h�A�g�@��find()���·�*/
		RotationFinder(cv::Mat puzIn);

		/**��X���Ϫ����סA�C�@�ӫ��Ϸ|���ݩ󥦦ۤv�����׭ȡA����Цۤv�����X�o�O��������*/
		double find(cv::Mat puzzle);

		/**���o�p��X�����סA�����n�������L���ϹϤ��~�i�ϥ�*/
		double get_angle();

		/**���o���J���Ϥ�*/
		cv::Mat get_image();
	};

	class PuzzleFinder
	{
	public:
		PuzzleFinder();
		/*�ϥγo��ܵ��J�Ϥ��ɷ|�i���l�ơA��l�Ʒ|�e���I�ɶ�
		�Y���Q�b�إߪ���ɯӮɡA�i�H�ϥεL�Ѽƪ��غc�l
		�᭱�b�Q��l�ƮɨϥΨ��init()*/
		PuzzleFinder(cv::Mat inImg);
		~PuzzleFinder();

		/*��l�ƥh���I����T�ëإ߾B�n�A�P�I���u������@���N�n�A�n���γo�Ӥ~��ϥ�compute(cv::Mat)*/
		void init(cv::Mat);
		/*�p��C�ӫ��Ϫ���m*/
		void compute(cv::Mat);
		/*�]�wHSV��*/
		void setHSV(cv::Scalar lower, cv::Scalar upper);
		/*�]�w�B�nROI*/
		void setROI(cv::Rect);
		/*���o�p�⵲�G�Ϥ�*/
		vector<cv::Mat> getImages();
		/*���o�аO�Ϥ���m�P�Τߪ�����Ϥ��A�Ω󰣿�*/
		cv::Mat getDebugImg();
		/*���o��J�Ϥ�*/
		cv::Mat getOrigImage();
		/*���o�Τ�*/
		vector<cv::Point> getCentroids();
		/*���o�]���*/
		vector<cv::Rect> getBoundingBoxes();


	private:
		cv::Mat img_orig, img_onlyPuzz, img_BW_onlyPuzz;
		cv::Mat mask_ROI, mask_greenBG, mask_wholeBG, mask_noPuzz;

		//�A�i��|�ݭn�ժ��Ѽ�
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


		/*���Jcontour �^�ǧΤ�*/
		cv::Point Centroid(vector<cv::Point> contour);
	};


	class MarkerFinder
	{
	public:
		MarkerFinder();
		~MarkerFinder();

		/*��J�Ϥ���Tag*/
		cv::Mat compute(cv::Mat inImage);
		/*���o�Ҧ�Tag���I*/
		vector<vector<cv::Point2f>> getCornerPoints();
		/*���o�Ҧ�Tag�����I*/
		vector<cv::Point2f> getMarkerCenters();
		/*���o�Ҧ�Tag Debug��*/
		cv::Mat getDebugImage();
		/*���o�Ҧ�Tag ID*/
		vector<int> getMarkerIds();

		typedef struct markerData_t
		{
			cv::Point2f center;
			vector<cv::Point2f> corners;
			int id;
		};

		/*���o�Ҧ�Tag ��T*/
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