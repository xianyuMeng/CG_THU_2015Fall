#include "Bresenham.h"
#include "DDA.h"
#include <functional>
#include <utility>
#include <fstream>

void mouse_callback(int event, int x, int y, int flags, void* param);

void main(){
	Mat img(800, 600, CV_8UC1, Scalar(0));
	std::function<void(Mat &, Point2i, Point2i)> drawer;
	imshow("test", img);
	printf("DDA or Bresenham?\n");
	//std::pair<Mat*, std::function<void(Mat &, Point2i, Point2i)> > param;
	string str, cmd;
	getline(std::cin, str);
	if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
		stringstream s(str);
		s >> cmd;
	}
	if (cmd == "DDA"){
		drawer = DDA::line;
	}
	else if (cmd == "Bresenham"){
		drawer=Bresenham::line;
	}
	else{
		printf("invalid input!\n");
	}

	auto param = std::make_pair(&img, drawer);
	setMouseCallback("test", mouse_callback, (void *)&param);
	waitKey(0);
}

void mouse_callback(int event, int x, int y, int flags, void* param){
	auto pp = (std::pair<Mat*, std::function<void(Mat &, Point2i, Point2i)> > *)param;
	auto img = pp->first;
	auto drawer = pp->second;
	static Point2i lastPoint(-1, -1);
	if (event == CV_EVENT_LBUTTONUP){
		cout << "left" << x << "," << y << endl;
		if (lastPoint != Point2i(-1, -1)){
			// Call your function
			//Bresenham::line(*img, lastPoint, Point2i(x, y));
			//DDA::line(*img, lastPoint, Point2i(x, y));
			drawer(*img, lastPoint, Point2i(x, y));
			imshow("test", *img);
		}
		lastPoint = Point2i(x, y);
	}
}