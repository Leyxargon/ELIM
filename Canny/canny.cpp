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
	
	imshow("src", src);
	
	int minth = 40;
	int maxth = 180;
	
	// applico filtro gaussiano
	Mat gauss;
	GaussianBlur(src, gauss, Size(5, 5), 0, 0);
	
	// calcolo magnitudo e gradiente
	Mat dx, dy;
	Sobel(gauss, dx, CV_32F, 1, 0);
	Sobel(gauss, dy, CV_32F, 0, 1);
	
	Mat dx2, dy2, mag;
	pow(dx, 2, dx2);
	pow(dy, 2, dy2);
	
	sqrt(dx2 + dy2, mag);
	
	normalize(mag, mag, 0, 255, NORM_MINMAX, CV_8U);
	
	// applico la non maxima suppression
	Mat orient;
	phase(dx, dy, orient);
	
	Mat nms = Mat::zeros(mag.size(), CV_8U);
	
	for (int i = 0; i < mag.rows; i++)
		for (int j = 0; j < mag.cols; j++) {
			float angle = orient.at<float>(i, j);
			
			// orizzontale
			if (angle < 22.5 && angle > -22.5 || angle < -157.5 && angle > 157.5) {
				if (mag.at<uchar>(i, j) > mag.at<uchar>(i, j - 1) && mag.at<uchar>(i, j) > mag.at<uchar>(i, j + 1))
					nms.at<uchar>(i, j) = mag.at<uchar>(i, j);
			}
			// verticale
			else if (angle < 112.5 && angle > 67.5 || angle < -67.5 && angle > -112.5) {
				if (mag.at<uchar>(i, j) > mag.at<uchar>(i - 1, j) && mag.at<uchar>(i, j) > mag.at<uchar>(i + 1, j)) 
					nms.at<uchar>(i, j) = mag.at<uchar>(i, j);
			}
			// diagonale sx
			else if (angle < -112.5 && angle > -157.5 || angle < 67.5 && angle > 22.5) {
				if (mag.at<uchar>(i, j) > mag.at<uchar>(i - 1, j - 1) && mag.at<uchar>(i, j) > mag.at<uchar>(i + 1, j + 1))
					nms.at<uchar>(i, j) = mag.at<uchar>(i, j);
			}
			// diagonale dx
			else if (angle < 157.5 && angle > 112.5 || angle < -22.5 && angle > -67.5) {
				if (mag.at<uchar>(i, j) > mag.at<uchar>(i - 1, j + 1) && mag.at<uchar>(i, j) > mag.at<uchar>(i + 1, j - 1))
					nms.at<uchar>(i, j) = mag.at<uchar>(i, j);
			}
		}
		
	// applico thresholding con isteresi
	Mat out = Mat::zeros(nms.size(), CV_8U);
	
	for (int i = 0; i < nms.rows; i++)
		for (int j = 0; j < nms.cols; j++) {
			if (nms.at<uchar>(i, j) > maxth)
				out.at<uchar>(i, j) = 255;
			else if (nms.at<uchar>(i, j) > minth && nms.at<uchar>(i, j) < maxth)
				for (int k = -256; k <= 256; k++)
					for (int l = -256; l <= 256; l++)
						if (i + k >= 0 && j + l >= 0)
							if (nms.at<uchar>(i + k, j + l) > maxth) {
								out.at<uchar>(i, j) = 255;
								break;
							}
		}
		
	imshow("out", out);
	
	waitKey();
	
	return 0;
}
