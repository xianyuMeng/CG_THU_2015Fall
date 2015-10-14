#include "Bresenham.h"

void Bresenham::line(Mat &img, Point2i &p0, Point2i &p1){
	/*
	boolean steep := abs(y1 - y0) > abs(x1 - x0)
	*/
	bool steep = abs(p1.y - p0.y) > abs(p1.x - p0.x);
	/*
	if steep then
         swap(x0, y0)
         swap(x1, y1)
     if x0 > x1 then
         swap(x0, x1)
         swap(y0, y1)
	*/
	if (steep){
		swap(p0.x, p0.y);
		swap(p1.x, p1.y);
	}
	if (p0.x > p1.x){
		swap(p0.x, p1.x);
		swap(p0.y, p1.y);
	}
	/*
	int deltax := x1 - x0
    int deltay := abs(y1 - y0)
    int error := deltax / 2
    int ystep
    int y := y0
	*/
	int delta_x = p1.x - p0.x;
	int delta_y = abs(p1.y - p0.y);
	int error = delta_x / 2;
	int y_step;
	int y = p0.y;
	/*
	 if y0 < y1 then ystep := 1 else ystep := -1
	*/
	y_step = (p0.y < p1.y) ? 1 : -1;
	/*
	for x from x0 to x1
         if steep then plot(y,x) else plot(x,y)
         error := error - deltay
         if error < 0 then
             y := y + ystep
             error := error + deltax
	*/
	for (int x = p0.x; x <= p1.x; ++x){
		if (steep){
			img.at<uchar>(x, y) = 255;
		}
		else{
			img.at<uchar>(y, x) = 255;
		}
		error -= delta_y;
		if (error < 0){
			y += y_step;
			error += delta_x;
		}
	}
}

void Bresenham::swap(int &a, int &b){
	int c(a);
	a = b;
	b = c;
}