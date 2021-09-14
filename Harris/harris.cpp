#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "uso: " << argv[0] << " <immagine>" << endl;
		return 1;
	}

	Mat src = imread(argv[1], IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << argv[1] << " non è un'immagine valida." << endl;
		return 2;
	}
	
	// calcolo derivate parziali
	Mat dx, dy;
	Sobel(src, dx, CV_32F, 1, 0);
	Sobel(src, dy, CV_32F, 0, 1);
	
	// calcolo Dx², Dy², DxDy
	Mat dx2, dy2, dxdy;
	pow(dx, 2, dx2);
	pow(dy, 2, dy2);
	multiply(dx, dy, dxdy);

	// applico gaussiana
	GaussianBlur(dx2, dx2, Size(5, 5), 4/6, 4/6);
	GaussianBlur(dy2, dy2, Size(5, 5), 4/6, 4/6);
	GaussianBlur(dxdy, dxdy, Size(5, 5), 4/6, 4/6);
	
	// calcolo indice R
	Mat det = dx2.mul(dy2) - dxdy.mul(dxdy);
	Mat trc = dx2 + dy2;
	Mat trc2;
	pow(trc, 2, trc2);
	
	Mat R = det - 0.04 * trc2;
	
	// normalizzo R
	normalize(R, R, 0, 255, NORM_MINMAX, CV_8U);
	
	// soglio indice R tracciando cerchi sui corner
	Mat out = src;
	cvtColor(src, out, COLOR_GRAY2BGR);
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
			if (R.at<uchar>(i, j) > 140)
				circle(out, Point(j, i), 6, Scalar(0, 0, 255));
			
	imshow("src", src);
	imshow("out", out);
	waitKey();
	return 0;
}
