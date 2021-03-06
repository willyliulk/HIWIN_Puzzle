#include "HIWINPuzzlePack.h"

void HIWIN_Puzzle::DEBUG_MSG(string s)
{
#if _DEBUG
	cout << "-----------" << s << "-------------\n";
#endif // _DEBUG
}

cv::Mat HIWIN_Puzzle::Transform_Matrix_Count(cv::Mat Camera_Matrix)/*傳入REALSENSE讀取之實際座標位置*/
{
	float Robot_Value[9] = { 0,28,0,0,0,22.5,1,1,1 };/*標記點實際位置*/
	cv::Mat Robot_Matrix = cv::Mat(cv::Size(3, 3), CV_32FC1, Robot_Value);
	cv::Mat Camera_Matrix_Inv = Camera_Matrix.inv();
	cv::Mat Transform_Matrix = Robot_Matrix * Camera_Matrix_Inv;
	return Transform_Matrix;

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
	if (puzzle.rows != 180, puzzle.cols != 180) {
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

	//imshow("test1", puzzle);
	cv::cvtColor(_puzzleCopy, _puzzleCopy, cv::COLOR_RGBA2GRAY);
	_puzz_gray = _puzzleCopy.clone();
	cv::distanceTransform(_puzzleCopy, _puzz_distance, cv::DIST_L2, cv::DIST_MASK_3);
	//imshow("test2", _puzz_distance);
	cv::minMaxLoc(_puzz_distance, &_disp, &_maxInDistance);
	cv::threshold(_puzz_distance, _puzz_distance, 0.7*_maxInDistance, 255, cv::THRESH_BINARY);
	//imshow("test3", _puzz_distance);
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
	//cv::imshow("test4", _puzz_gray);

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
		return cv::Mat::zeros(180, 180, CV_8U);
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
	img_debug = source.clone();
	for (auto contour : contours) {
		DEBUG_MSG("hi");
		if (cv::contourArea(contour) < 5800) {
			DEBUG_MSG("aout");
			continue;
		}
		if (cv::boundingRect(contour).area() > 22000) {
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
		centroids.push_back(centroid);
		boundingBoxes.push_back(tempRect);

		//make debug img
		//img_debug = source.clone();
		cv::drawContours(img_debug, contours, -1, cv::Scalar(0, 0, 255), 2);
		cv::circle(img_debug, centroid, 2, cv::Scalar(0, 255, 0), -1);
		cv::rectangle(img_debug, tempRect, cv::Scalar(0, 255, 0));
		cv::putText(img_debug, to_string(cons), centroid - cv::Point(70, 70), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 0, 255), 2, 8, 0);
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
	cv::rectangle(img_debug, rect_ROI, cv::Scalar(255, 230, 200),3);
	return img_debug.clone();
}

cv::Mat HIWIN_Puzzle::PuzzleFinder::getOrigImage()
{
	if (img_orig.empty()) {
		cerr << "orignal image is empty. please use init(cv::Mat) first\n";
	}
	return img_orig.clone();
}

vector<cv::Point> HIWIN_Puzzle::PuzzleFinder::getCentroids()
{
	return centroids;
}

vector<cv::Rect> HIWIN_Puzzle::PuzzleFinder::getBoundingBoxes()
{
	return boundingBoxes;
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

//-------------------------------------------------------------------------------

HIWIN_Puzzle::MarkerFinder::MarkerFinder()
{
	//parameters->cornerRefinementMethod = cv::aruco::CORNER_REFINE_APRILTAG;
	//parameters->aprilTagQuadDecimate = 2.0;
}

HIWIN_Puzzle::MarkerFinder::~MarkerFinder()
{
}

cv::Mat HIWIN_Puzzle::MarkerFinder::compute(cv::Mat inImage)
{
	inImg = inImage.clone();
	cv::Mat gray = inImage.clone();
	cv::cvtColor(gray, gray, cv::COLOR_RGB2GRAY);

	//灰階反轉
	threshold(gray, gray, 150, 255, CV_THRESH_BINARY_INV);
	
	cv::aruco::detectMarkers(gray, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

	return inImage;


	/*image_u8_t img_header = { gray.cols, gray.rows, gray.cols, gray.data };
	apriltag_family_t *tagFamily = tag36h11_create();
	apriltag_detector_t *detector = apriltag_detector_create();
	apriltag_detector_add_family(detector, tagFamily);
	zarray *detections = apriltag_detector_detect(detector, &img_header);
	detector->quad_decimate = 2.0;

	markerCenters.clear();
	markerCorners.clear();
	markerIds.clear();

	if (zarray_size(detections) == 0) {
		cout << "no found" << endl;
		tag36h11_destroy(tagFamily);
		apriltag_detector_destroy(detector);
		zarray_destroy(detections);
		return inImage;
	}

	for (int i = 0; i < zarray_size(detections); i++) {
		apriltag_detection_t *det;
		zarray_get(detections, i, &det);

		cv::Point2f tagCenter(det->c[0], det->c[1]);
		markerCenters.push_back(tagCenter);

		vector<cv::Point2f> oneMarkerCorner;
		oneMarkerCorner.push_back(cv::Point(det->p[0][0], det->p[0][1]));
		oneMarkerCorner.push_back(cv::Point(det->p[1][0], det->p[1][1]));
		oneMarkerCorner.push_back(cv::Point(det->p[2][0], det->p[2][1]));
		oneMarkerCorner.push_back(cv::Point(det->p[3][0], det->p[3][1]));
		markerCorners.push_back(oneMarkerCorner);

		markerIds.push_back(det->id);
		apriltag_detection_destroy(det);
	}

	tag36h11_destroy(tagFamily);
	apriltag_detector_destroy(detector);
	zarray_destroy(detections);
	return inImage;*/

}

vector<vector<cv::Point2f>> HIWIN_Puzzle::MarkerFinder::getCornerPoints()
{
	return markerCorners;
}

vector<cv::Point2f> HIWIN_Puzzle::MarkerFinder::getMarkerCenters()
{
	markerCenters.clear();
	for (auto c : markerCorners) {
		markerCenters.push_back(cv::Point2f(((c[0].x + c[2].x) / 2),
											((c[0].y + c[2].y) / 2)));
	}
	return markerCenters;
}

cv::Mat HIWIN_Puzzle::MarkerFinder::getDebugImage()
{
	debugImage = inImg.clone();
	for (int i = 0; i < markerIds.size(); i++) {
		cv::putText(debugImage, to_string(markerIds[i]), markerCenters[i], cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
		cv::circle(debugImage, markerCenters[i], 2, cv::Scalar(0, 255, 0), 1);

		cv::circle(debugImage, markerCorners[i][0], 2, cv::Scalar(0, 0, 255), 1);
		cv::circle(debugImage, markerCorners[i][1], 2, cv::Scalar(0, 255, 0), 1);
		cv::circle(debugImage, markerCorners[i][2], 2, cv::Scalar(255, 0, 0), 1);
		cv::circle(debugImage, markerCorners[i][3], 2, cv::Scalar(0, 255, 255), 1);

	}
	return debugImage;
}

vector<int> HIWIN_Puzzle::MarkerFinder::getMarkerIds()
{
	return markerIds;
}

vector<HIWIN_Puzzle::MarkerFinder::markerData_t> HIWIN_Puzzle::MarkerFinder::getMarkerData()
{
	markerDatas.clear();
	getCornerPoints();
	getMarkerCenters();
	getMarkerIds();
	MarkerFinder::markerData_t temp;
	for (int i = 0; i < markerIds.size(); i++) {
		temp.center = markerCenters[i];
		temp.corners = markerCorners[i];
		temp.id = markerIds[i];

		markerDatas.push_back(temp);
	}
	return markerDatas;
}


