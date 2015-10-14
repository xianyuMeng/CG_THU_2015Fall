#ifndef _INCLUDEDDAHEADER
#define _INCLUDEDDAHEADER

#include <cstdio>
#include <opencv2\opencv.hpp>
#include "opencv2\core\core.hpp"       // Mat is defined here.
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\highgui\highgui.hpp"

using namespace cv;
using namespace std;

class DDA{
public:
	DDA();
	virtual ~DDA();
	static void line(Mat &img, Point2i &p0, Point2i &p1);
	static void swap(int &a, int &b);
};

#endif