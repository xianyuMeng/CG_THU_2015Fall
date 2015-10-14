#include "DDA.h"

void DDA::line(Mat &img, Point2i &p0, Point2i &p1){
	float m = float(p1.y - p0.y) / float(p1.x - p0.x);
	if (abs(m) <= 1){
		float y = (p1.x < p0.x) ? p1.y : p0.y;
		if (p1.x < p0.x){
			swap(p0.x, p1.x);
		}
		else{}
		for (int x = p0.x; x <= p1.x; ++x){
			img.at<uchar>(round(y), x) = 255;
			y += m;
		}
	}
	else{
		float x = (p1.y < p0.y) ? p1.x : p0.x;
		if (p1.y < p0.y)
			swap(p0.y, p1.y);
		else{}
		for (int y = p0.y; y <= p1.y; ++y){
			img.at<uchar>(y, round(x)) = 255;
			x += 1.0 / m;
		}
	}
}

void DDA::swap(int &a, int &b){
	int c(a);
	a = b;
	b = c;
}