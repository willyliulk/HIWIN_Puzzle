#include "HIWINPuzzlePack.h"

void HIWIN_Puzzle::DEBUG_MSG(string s)
{
#if _DEBUG
	cout << "-----------" << s << "-------------\n";
#endif // _DEBUG
}

HIWIN_Puzzle::RotationFinder::RotationFinder()
{
}

HIWIN_Puzzle::RotationFinder::RotationFinder(cv::Mat puzIn)
{
	DEBUG_MSG("you are using overload RotationFinder constructor.");
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
	//imshow("test", _puzz_distance);
	cv::minMaxLoc(_puzz_distance, &_disp, &_maxInDistance);
	cv::threshold(_puzz_distance, _puzz_distance, 0.7*_maxInDistance, 255, cv::THRESH_BINARY);
	_puzz_distance.convertTo(_puzz_distance, CV_8UC1);

	cv::Mat labels, stats, centroids;
	int nConnect = cv::connectedComponentsWithStats(_puzz_distance, labels, stats, centroids);

#if _DEBIG
	cout << "we get " << nConnect << " connect space\n";
	cout << "cnetrouds:\n";
	cout << centroids << endl;
	cout << centroids.type() << endl;
	cout << centroids.at<double>(1, 0) << endl;
	cout << centroids.at<double>(1, 1) << endl;
#endif // _DEBIG

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
	//cv::imshow("apple", _puzz_gray);

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
		return cv::Mat::zeros(100, 100, CV_8U);
	}
	return this->puzzle.clone();
}

//----------------------Puzzle Finder------------------------
HIWIN_Puzzle::PuzzleFinder::PuzzleFinder()
{
}

HIWIN_Puzzle::PuzzleFinder::PuzzleFinder(cv::Mat inImg)
{
	img_orig = inImg.clone();
}

HIWIN_Puzzle::PuzzleFinder::~PuzzleFinder()
{
}

void HIWIN_Puzzle::PuzzleFinder::init(cv::Mat source)
{
	img_orig = source.clone();

	DEBUG_MSG("init human select ROI region.\n");
	if (modiROI == false) {
		cerr << "rect_ROI is empty. use default ROI region \n";
		cerr << "ROI value (x, y, width, height):\n";
		cerr << rect_ROI << endl;
	}

	if (modiHSV == false) {
		cerr << "You have'n change HSV value. Use default value.\n";
		cerr << "HSV range:\n";
		cerr << "Upper Bound:\n";
		cerr << HSV_Upper << endl;
		cerr << "Lower Bound:\n";
		cerr << HSV_Lower << endl;
	}
	mask_ROI = cv::Mat::ones(img_orig.rows, img_orig.cols, CV_8U);
	mask_ROI = 255 * mask_ROI;
	cv::rectangle(mask_ROI, rect_ROI, 0, -1);

	DEBUG_MSG("init greenBG mask.\n");
	cv::Mat img_inProcess = source.clone();
	cv::medianBlur(img_inProcess, img_inProcess, 5);
	cv::cvtColor(img_inProcess, img_inProcess, cv::COLOR_RGB2HSV);
	cv::inRange(img_inProcess, HSV_Lower, HSV_Upper, img_inProcess);
#if _DEBUG
	cout << HSV_Lower << HSV_Upper << endl;

#endif // _DEBUG

	cv::Mat kernal = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(15, 15));
	cv::morphologyEx(img_inProcess, img_inProcess, cv::MORPH_CLOSE, kernal);


	vector<vector<cv::Point>> contours;
	cv::findContours(img_inProcess, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	cv::fillPoly(img_inProcess, contours, cv::Scalar(255, 255, 255));
	cv::bitwise_not(img_inProcess, img_inProcess);

	mask_greenBG = img_inProcess.clone();
	DEBUG_MSG("merge ROI mask and greenBG mask\n");
	cv::bitwise_or(img_inProcess, mask_ROI, mask_wholeBG);

	DEBUG_MSG("init finish\n");
}

void HIWIN_Puzzle::PuzzleFinder::compute(cv::Mat source)
{
	DEBUG_MSG("puzzle finder conpute");
	if (mask_wholeBG.empty()) {
		cerr << "please use init(cv::Mat source) first.\n";
		return;
	}
	DEBUG_MSG("conputing \n");
	cv::Mat img_inProcess = source.clone();
	size_orig = cv::Size(source.cols, source.rows);
	cv::cvtColor(img_inProcess, img_inProcess, cv::COLOR_RGB2HSV);
	cv::inRange(img_inProcess, HSV_Lower, HSV_Upper, img_inProcess);
	cv::Mat kernal = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
	cv::morphologyEx(img_inProcess, img_inProcess, cv::MORPH_CLOSE, kernal);
	DEBUG_MSG("apply mask\n");
	cv::bitwise_or(img_inProcess, mask_wholeBG, mask_noPuzz);

	img_onlyPuzz = source.clone();
	img_onlyPuzz.setTo(0, mask_noPuzz);

	cv::bitwise_not(mask_noPuzz, img_BW_onlyPuzz);

	vector<vector<cv::Point>> contours;
	vector<cv::Rect> rects;
	int cons = 1;

	//cv::imshow("test", img_BW_onlyPuzz);
	//cv::waitKey(0);


	cv::findContours(img_BW_onlyPuzz, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	DEBUG_MSG("crop puzzles piece");
	DEBUG_MSG(to_string(contours.size()));
	for (auto contour : contours) {
		DEBUG_MSG("hi");
		if (cv::contourArea(contour) < 150) {
			DEBUG_MSG("aout");
			continue;
		}
		if (cv::boundingRect(contour).area() > 20000) {
			DEBUG_MSG("bout");
			continue;
		}

		cv::Point centroid = Centroid(contour);
		cv::Rect tempRect(centroid.x - halfBox,
			centroid.y - halfBox,
			halfBox * 2,
			halfBox * 2);
		if (centroid.x - halfBox<0 || centroid.x + halfBox > size_orig.width ||
			centroid.y - halfBox<0 || centroid.y + halfBox > size_orig.height) continue;
		cv::Mat subMask_OnePuzzOut;
		subMask_OnePuzzOut = cv::Mat::ones(size_orig, CV_8U);
		cv::fillPoly(subMask_OnePuzzOut, contour, cv::Scalar(0));
		cv::Mat subImg_crop = img_orig.clone();
		subImg_crop.setTo(0, subMask_OnePuzzOut);
		cv::Mat subImg_onePuzz;
		subImg_crop(tempRect).copyTo(subImg_onePuzz);

		puzzles.push_back(subImg_onePuzz.clone());

		//make debug img
		img_debug = source.clone();
		cv::drawContours(img_debug, contours, -1, cv::Scalar(0, 0, 255), 2);
		cv::circle(img_debug, centroid, 2, cv::Scalar(0, 255, 0), -1);
		cv::rectangle(img_debug, tempRect, cv::Scalar(0, 255, 0));
		cv::putText(img_debug, to_string(cons), centroid - cv::Point(50, 50), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 150, 0));
		cons++;
	}
	DEBUG_MSG("conputing finish\n");
}

void HIWIN_Puzzle::PuzzleFinder::setHSV(cv::Scalar lower, cv::Scalar upper)
{
	HSV_Lower = lower;
	HSV_Upper = upper;
	modiROI = true;
}

void HIWIN_Puzzle::PuzzleFinder::setROI(cv::Rect ROI)
{
	rect_ROI = ROI;
	modiHSV = true;
}

vector<cv::Mat> HIWIN_Puzzle::PuzzleFinder::getImages()
{
	if (puzzles.empty()) {
		cerr << "Result is empty. \nPleases use compute(cv::Mat source) first.\n";
		vector<cv::Mat> nothing;
		nothing.push_back(cv::Mat::zeros(halfBox * 2, halfBox * 2, CV_8U));
		return nothing;
	}


	return puzzles;
}

cv::Mat HIWIN_Puzzle::PuzzleFinder::getDebugImg()
{
	if (img_debug.empty()) {
		cerr << "DebugImg  empty. please use compute(cv::Mat) first.";
		return cv::Mat::zeros(halfBox * 2, halfBox * 2, CV_8U);
	}
	return img_debug.clone();
}

cv::Mat HIWIN_Puzzle::PuzzleFinder::getOrigImage()
{
	if (img_orig.empty()) {
		cerr << "orignal image is empty. please use init(cv::Mat) first\n";
	}
	return img_orig.clone();
}

cv::Point HIWIN_Puzzle::PuzzleFinder::Centroid(vector<cv::Point> contour)
{
	//Moments  mu = moments(contour, false);
	int sumx = 0;
	int sumy = 0;
	for (auto p : contour) {
		sumx += p.x;
		sumy += p.y;
	}
	return cv::Point(sumx / contour.size(), sumy / contour.size());
}
