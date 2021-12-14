#include "HIWINPuzzlePack.h"

void HIWIN_Puzzle::RotationFinder::DEBUG_MSG(string s)
{
#if _DEBUG
	cout << s;
#endif // _DEBUG
}

HIWIN_Puzzle::RotationFinder::RotationFinder()
{
}

HIWIN_Puzzle::RotationFinder::RotationFinder(cv::Mat puzIn)
{
	cout << "you are using overload RotationFinder constructor.\n";
	this->find(puzIn);
}


double HIWIN_Puzzle::RotationFinder::find(cv::Mat puzzle)
{
	DEBUG_MSG("you are at RotationFinder::find(cv::Mat puzzle)");
	if (puzzle.channels() != 3) {
		cerr << "error at HIWIN_Puzzle::RotationFinder::find \n";
		cerr << "please giva 3 channels RGB Image\n";
		cerr << "you give " << puzzle.channels() << " channels image\n";
		return -1;
	}
	if (puzzle.rows != 100, puzzle.cols != 100) {
		cerr << "error at HIWIN_Puzzle::RotationFinder::find \n";
		cerr << "please giva a 100x100 image\n";
		cerr << "you give " << puzzle.rows << "x" << puzzle.cols << "\n";
		return -1;
	}

	this->puzzle = puzzle.clone();

	cv::Mat _puzzleCopy = puzzle.clone();
	cv::Mat _puzz_distance;
	cv::Mat _puzz_gray;
	double _maxInDistance, _disp;

	cv::cvtColor(_puzzleCopy, _puzzleCopy, cv::COLOR_RGBA2GRAY);
	_puzz_gray = _puzzleCopy.clone();
	cv::distanceTransform(_puzzleCopy, _puzz_distance, cv::DIST_L2, cv::DIST_MASK_3);
	cv::minMaxLoc(_puzz_distance, &_disp, &_maxInDistance);
	cv::threshold(_puzz_distance, _puzz_distance, 0.7*_maxInDistance, 255, cv::THRESH_BINARY);
	_puzz_distance.convertTo(_puzz_distance, CV_8UC1);

	cv::Mat labels, stats, centroids;
	int nConnect = cv::connectedComponentsWithStats(_puzz_distance, labels, stats, centroids);
	cout << "we get " << nConnect << " connect space\n";
	cout << "cnetrouds:\n";
	cout << centroids << endl;
	cout << centroids.type() << endl;
	cout << centroids.at<double>(1,0) << endl;
	cout << centroids.at<double>(1, 1) << endl;
	this->centroid_distance.x = centroids.at<double>(1, 0);
	this->centroid_distance.y = centroids.at<double>(1, 1);

	vector<vector<cv::Point>> contours;
	cv::findContours(_puzz_gray, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	for (auto contour : contours) {
		cv::Moments m = cv::moments(contour);
		this->centroid_shape.x = m.m10 / m.m00;
		this->centroid_shape.y = m.m01 / m.m00;
	}

	cv::drawContours(_puzz_gray, contours, -1, 255);
	cv::circle(_puzz_gray, centroid_shape, 1, 255, -1);
	cv::circle(_puzz_gray, centroid_distance, 1, 0, -1);
	this->angle_result = atan2(centroid_shape.x - centroid_distance.x,
							   centroid_shape.y - centroid_distance.y);
	cv::imshow("apple", _puzz_gray);

	cout << centroid_distance << endl;
	cout << centroid_shape << endl;
	return angle_result;
}

double HIWIN_Puzzle::RotationFinder::get_angle()
{
	DEBUG_MSG("you are at RotationFinder::get_angle()");
	if (this->puzzle.empty()) {
		cerr << "you need to pass a puzzle using find() first.\n";
		return -99999;
	}


	return angle_result;
}

cv::Mat HIWIN_Puzzle::RotationFinder::get_image()
{
	DEBUG_MSG("you are at RotationFinder::get_image()");
	if (this->puzzle.empty()) {
		cerr << "you need to pass a puzzle using find() first.\n";
		return cv::Mat::zeros(100,100, CV_8U);
	}
	return this->puzzle.clone();
}
