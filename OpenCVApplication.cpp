#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>
#include <iostream>

#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

wchar_t* projectPath;


Mat_<Vec3b> testBGR2HSV(Mat src){
	int height = src.rows;
	int width = src.cols;

	// HSV components
	Mat H = Mat(height, width, CV_8UC1);
	Mat S = Mat(height, width, CV_8UC1);
	Mat V = Mat(height, width, CV_8UC1);

	// Defining pointers to each matrix of the individual components H, S, V 
	uchar* Hpointer = H.data;
	uchar* Spointer = S.data;
	uchar* Vpointer = V.data;

	/*
		cv.COLOR_BGR2HSV
		convert RGB / BGR to HSV(hue saturation value) with H range 0..180 if 8 bit image, color conversions
	*/
	Mat hsvImg;
	cvtColor(src, hsvImg, COLOR_BGR2HSV);

	uchar* hsvData = hsvImg.data;

	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){

			int hindex = i * width * 3 + j * 3;
			int gindex = i * width + j;

			// transform from [0, 360] to [0, 255]
			Hpointer[gindex] = hsvData[hindex] * 255 / 180;	
			Spointer[gindex] = hsvData[hindex + 1];			
			Vpointer[gindex] = hsvData[hindex + 2];

			//read the documentation and found out tht Value and Saturatioan are in [0,255] range
			//but Hue is in [0,360] range and if the image is an 8 bit one, than it is in range [0,180]
			//since I am wotking with 8 bit frames from the camera, The normalisation is 255/180
		}
	}

	cv::Scalar lower_blue(75, 50, 15);
	cv::Scalar upper_blue(130, 255, 255);

	cv::Scalar lower_white(0, 0, 50);
	cv::Scalar upper_white(20, 150, 255);

	cv::Scalar lower_red1(0, 50, 50);
	cv::Scalar upper_red1(10, 255, 255);

	cv::Scalar lower_red2(170, 50, 50);
	cv::Scalar upper_red2(180, 255, 255);

	cv::Scalar lower_yellow(20, 100, 100);
	cv::Scalar upper_yellow(30, 255, 255);

	cv::Scalar lower_green(35, 50, 50);
	cv::Scalar upper_green(85, 255, 255);

	cv::Scalar lower_orange(10, 100, 100);
	cv::Scalar upper_orange(20, 255, 255);


	Mat blueMask, resultBlue, whiteMask, resultWhite, redMask, resultRed, orangeMask, resultOrange, yellowMask, resultYellow, greenMask, resultGreen;
	Mat_<Vec3b> dst(src.rows, src.cols);
	dst.setTo(Vec3b(0, 0, 0));

	inRange(hsvImg, lower_blue, upper_blue, blueMask);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (blueMask.at<uchar>(i,j) == 255) {
				dst(i, j) = Vec3b(255, 0, 0);
			}
		}
	}

	inRange(hsvImg, lower_white, upper_white, whiteMask);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (whiteMask.at<uchar>(i, j) == 255) {
				dst(i, j) = Vec3b(255, 255, 255);
			}
		}
	}

	cv::Mat mask_red1, mask_red2;
	cv::inRange(hsvImg, lower_red1, upper_red1, mask_red1);
	cv::inRange(hsvImg, lower_red2, upper_red2, mask_red2);
	cv::bitwise_or(mask_red1, mask_red2, redMask);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (mask_red2.at<uchar>(i, j) == 255) {
				dst(i, j) = Vec3b(0, 0, 255);
			}
		}
	}


	inRange(hsvImg, lower_yellow, upper_yellow, yellowMask);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (yellowMask.at<uchar>(i, j) == 255) {
				dst(i, j) = Vec3b(0, 255, 255);
			}
		}
	}

	inRange(hsvImg, lower_green, upper_green, greenMask);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (greenMask.at<uchar>(i, j) == 255) {
				dst(i, j) = Vec3b(0, 255, 0);
			}
		}
	}

	inRange(hsvImg, lower_orange, upper_orange, orangeMask);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (orangeMask.at<uchar>(i, j) == 255) {
				dst(i, j) = Vec3b(0, 165, 255);
			}
		}
	}

	//imshow("BlueMask", blueMask);
	//imshow("WhiteMask", whiteMask);
	//imshow("RedMask", redMask);
	//imshow("YellowMask", yellowMask);
	//imshow("GreenMask", greenMask);
	//imshow("OrangeMask", orangeMask);

	/*
	imshow("H", H);
	imshow("S", S);
	imshow("V", V);
	*/

	imshow("DST", dst);

	return dst;
}


Vec3b checkFillColor(int lower_i, int upper_i, int lower_j, int upper_j, Mat_<Vec3b> primaryColors) {
	int blueCounter = 0;
	int redCounter = 0;
	int whiteCounter = 0;
	int yellowCounter = 0;
	int orangeCounter = 0;
	int greenCounter = 0;

	for (int i = lower_i; i < upper_i; i++) {
		for (int j = lower_j; j < upper_j; j++) {
			if (primaryColors(i, j) == Vec3b(255, 0, 0)) {
				blueCounter++;
			}
			if (primaryColors(i, j) == Vec3b(255, 255, 255)) {
				whiteCounter++;
			}
			if (primaryColors(i, j) == Vec3b(0, 0, 255)) {
				redCounter++;
			}
			if (primaryColors(i, j) == Vec3b(0, 255, 255)) {
				yellowCounter++;
			}
			if (primaryColors(i, j) == Vec3b(0, 255, 0)) {
				greenCounter++;
			}
			if (primaryColors(i, j) == Vec3b(0, 165, 255)) {
				orangeCounter++;
			}
		}
	}

	if (blueCounter > redCounter && blueCounter > whiteCounter && blueCounter > yellowCounter && blueCounter > orangeCounter && blueCounter > greenCounter) {
		return Vec3b(255, 0, 0);
	}
	if (redCounter > blueCounter && redCounter > whiteCounter && redCounter > yellowCounter && redCounter > orangeCounter && redCounter > greenCounter) {
		return Vec3b(0, 0, 255);
	}
	if (whiteCounter > blueCounter && whiteCounter > yellowCounter && whiteCounter > redCounter && whiteCounter > orangeCounter && whiteCounter > greenCounter) {
		return Vec3b(255, 255, 255);
	}
	if (yellowCounter > blueCounter && yellowCounter > whiteCounter && yellowCounter > redCounter && yellowCounter > orangeCounter && yellowCounter > greenCounter) {
		return Vec3b(0, 255, 255);
	}
	if (orangeCounter > blueCounter && orangeCounter > whiteCounter && orangeCounter > redCounter && orangeCounter > yellowCounter && orangeCounter > greenCounter) {
		return Vec3b(0, 165, 255);
	}
	if (greenCounter > blueCounter && greenCounter > whiteCounter && greenCounter > redCounter && greenCounter > orangeCounter && greenCounter > yellowCounter) {
		return Vec3b(0, 255, 0);
	}
	return 0;
}

Mat_<Vec3b> dysplayCube(450, 600);

int main() 
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
    projectPath = _wgetcwd(0, 0);

	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - camera input\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d",&op);
		
		cv::Mat image_src = imread("Images/WIN_20240410_01_12_49_Pro.jpg", CV_8SC3);
		VideoCapture cap(0);

		switch (op)
		{
		case 1:
			
			if (!cap.isOpened()) {
				cout << "Error opening video stream or file" << endl;
				return -1;
			}

			while (1) {
				cv::Mat frame;
				cap >> frame;
				if (frame.empty())
					break;

				int treshold = 90;

				int middle_i = frame.rows/2;
				int middle_j = frame.cols/2;

				//detects the color
				Mat_<Vec3b> primaryColors(frame.rows, frame.cols);
				primaryColors = testBGR2HSV(frame);

				//for showing the black borders
				for (int i = middle_i - treshold; i < middle_i + treshold; i++) {
					for (int j = middle_j - treshold; j < middle_j + treshold; j++) {
						if (i == middle_i - treshold || i == middle_i - treshold + 60 || i == middle_i + treshold - 60 || i == middle_i + treshold - 1)
							frame.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
						if (j == middle_j - treshold || j == middle_j - treshold + 60 || j == middle_j + treshold - 60 || j == middle_j + treshold - 1)
							frame.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
					}
				}

				imshow("frame", frame);

				int afisare_offset_i = 0;
				int afisare_offset_j = 0;

				char c = (char)waitKey(25);

				if (c == 'A' || c == 'a') {
					printf("Alb\n");
					
					afisare_offset_i = 300;
					afisare_offset_j = 150;

					//stanga sus
					Vec3b color = 0;
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {

							dysplayCube(i, j) = color;
						}
					}

					//stanga mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//stanga jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc sus

					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta sus
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					imshow("DysplayCube", dysplayCube);
				}

				if (c == 'S' || c == 's') {
					printf("Rosu\n");

					afisare_offset_i = 150;
					afisare_offset_j = 150;

					//stanga sus
					Vec3b color = 0;
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {

							dysplayCube(i, j) = color;
						}
					}

					//stanga mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//stanga jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc sus

					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta sus
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					imshow("DysplayCube", dysplayCube);
				}

				if (c == 'D' || c == 'd') {
					printf("Albastru\n");

					afisare_offset_i = 150;
					afisare_offset_j = 0;

					//stanga sus
					Vec3b color = 0;
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50+afisare_offset_i; i++) {
						for (int j = 0+afisare_offset_j; j < 50+afisare_offset_j; j++) {

							dysplayCube(i, j) = color;
						}
					}

					//stanga mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 50+afisare_offset_i; i < 100+afisare_offset_i; i++) {
						for (int j = 0+afisare_offset_j; j < 50+afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//stanga jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 100+afisare_offset_i; i < 150+afisare_offset_i; i++) {
						for (int j = 0+afisare_offset_j; j < 50+afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc sus

					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 0+afisare_offset_i; i < 50+afisare_offset_i; i++) {
						for (int j = 50+afisare_offset_j; j < 100+afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 50+afisare_offset_i; i < 100+afisare_offset_i; i++) {
						for (int j = 50+afisare_offset_j; j < 100+afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 100+afisare_offset_i; i < 150+afisare_offset_i; i++) {
						for (int j = 50+afisare_offset_j; j < 100+afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta sus
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 0+afisare_offset_i; i < 50+afisare_offset_i; i++) {
						for (int j = 100+afisare_offset_j; j < 150+afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 50+afisare_offset_i; i < 100+afisare_offset_i; i++) {
						for (int j = 100+afisare_offset_j; j < 150+afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 100+afisare_offset_i; i < 150+afisare_offset_i; i++) {
						for (int j = 100+afisare_offset_j; j < 150+afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					imshow("DysplayCube", dysplayCube);
				}
				
				if (c == 'F' || c == 'f') {
					printf("Portocaliu\n");

					afisare_offset_i = 150;
					afisare_offset_j = 450;

					//stanga sus
					Vec3b color = 0;
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {

							dysplayCube(i, j) = color;
						}
					}

					//stanga mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//stanga jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc sus

					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta sus
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					imshow("DysplayCube", dysplayCube);
				}

				if (c == 'G' || c == 'g') {
					printf("Verde\n");

					afisare_offset_i = 150;
					afisare_offset_j = 300;

					//stanga sus
					Vec3b color = 0;
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {

							dysplayCube(i, j) = color;
						}
					}

					//stanga mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//stanga jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc sus

					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta sus
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					imshow("DysplayCube", dysplayCube);
				}

				if (c == 'H' || c == 'h') {
					printf("Galben\n");

					afisare_offset_i = 0;
					afisare_offset_j = 150;

					//stanga sus
					Vec3b color = 0;
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {

							dysplayCube(i, j) = color;
						}
					}

					//stanga mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//stanga jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 1, middle_j - treshold + 60, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 0 + afisare_offset_j; j < 50 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc sus

					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//mijloc jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j - treshold + 61, middle_j + treshold - 60, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 50 + afisare_offset_j; j < 100 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta sus
					color = checkFillColor(middle_i - treshold + 1, middle_i - treshold + 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 0 + afisare_offset_i; i < 50 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta mijloc
					color = checkFillColor(middle_i - treshold + 61, middle_i + treshold - 60, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 50 + afisare_offset_i; i < 100 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					//dreapta jos
					color = checkFillColor(middle_i + treshold - 59, middle_i + treshold, middle_j + treshold - 59, middle_j + treshold, primaryColors);

					for (int i = 100 + afisare_offset_i; i < 150 + afisare_offset_i; i++) {
						for (int j = 100 + afisare_offset_j; j < 150 + afisare_offset_j; j++) {
							dysplayCube(i, j) = color;
						}
					}

					imshow("DysplayCube", dysplayCube);
				}

				if (c == 27) {
					cap.release();
					destroyAllWindows();
					break;
				}
			}
			break;
		}
	}
	while (op!=0);
	return 0;
}