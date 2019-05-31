#include  "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include<stdint.h>
#include<iostream>

using namespace cv;
using namespace std;

string detectColor(int row, int col, Mat img){
	//BGR Coloring System
	int red = img.at<Vec3b>(col,row).val[2];
	int green = img.at<Vec3b>(col, row).val[1];
	int blue = img.at<Vec3b>(col, row).val[0];

	if (red == 255 && green == 0 && blue == 0)
		return "red";
	else if (red == 255 && green == 255 && blue == 0)
		return "yellow";
	else if (red == 255 && green == 0 && blue == 255)
		return "magenta";
	else if (red == 0 && green == 255 && blue == 0)
		return "green";
	else if (red == 0 && green == 255 && blue == 255)
		return "cyan";
	else if (red == 0 && green == 0 && blue == 255)
		return "blue";
	else if (red == 255 && green == 255 && blue == 255)
		return "white";

	return "";
}

int main(int argc,char* argv[]){

	if (argc ==2){
		string imgPath = argv[1];
		cout << "ashraf";
		Mat colored_noisyImage = imread(imgPath, CV_LOAD_IMAGE_COLOR);
		cout << colored_noisyImage << endl;
		imshow("Filtered Image", colored_noisyImage);
		waitKey();
		Mat gray, median, gaussian, img, thresholded, HSV_img;

		// Removing noise by gaussian, median blurring and thresholding
		GaussianBlur(colored_noisyImage, gaussian, Size(11, 11), 0, 0, BORDER_DEFAULT);
		medianBlur(gaussian, median, 9);
		threshold(median, img, 128, 255, 0);

		imshow("Filtered Image", img);
		imwrite("FilteredImage.bmp", img);

		// Convert the image to grayscale
		cvtColor(img, gray, CV_BGR2GRAY);

		// thresholding (whatever pixel greater than 10 will be 255, otherwise => 0)
		threshold(gray, thresholded, 10, 255, THRESH_BINARY);
		imshow("Thresholded Image", thresholded);

		// each contour is a vector of points.
		vector<vector<Point>> contours;
		findContours(thresholded, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

		//Draw the contours
		int countAll = 0, countTriangles = 0, countSqaures = 0, countCircles = 0, countRectangles = 0;
		int shape_X, shape_Y;
		vector<Point> approximation;
		vector<Rect> boundRectangles;
		string color;

		for (size_t i = 0; i < contours.size(); i++){
			boundRectangles.push_back(boundingRect(contours[i]));

			// select shapes with (area > 200) to avoid very small noisy shapes
			if (contourArea(contours[i])>200){
				countAll++;
				approxPolyDP(contours[i], approximation, 5.2, true);

				if (approximation.size() == 3){
					drawContours(img, contours, i, Scalar(76, 255, 166), 2);
					countTriangles++;
					shape_X = boundRectangles[i].x;
					shape_Y = boundRectangles[i].y;

					color = detectColor(shape_X + boundRectangles[i].width / 2, shape_Y + boundRectangles[i].height / 2, img);
					putText(img, "Triangle(" + color + ")", Point2f(shape_X, shape_Y), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
				}
				else if (approximation.size() >= 4 && approximation.size() <= 5){
					float ratio, width = (float)boundRectangles[i].width, height = (float)boundRectangles[i].height;

					ratio = height >= width ? height / width : width / height;

					if (ratio > 1.05){
						drawContours(img, contours, i, Scalar(76, 255, 166), 2);
						countRectangles++;
						shape_X = boundRectangles[i].x;
						shape_Y = boundRectangles[i].y;

						color = detectColor(shape_X + boundRectangles[i].width / 2, shape_Y + boundRectangles[i].height / 2, img);
						putText(img, "Rectangle(" + color + ")", Point2f(shape_X, shape_Y), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
					}
					else{
						drawContours(img, contours, i, Scalar(76, 255, 166), 2);
						countSqaures++;
						shape_X = boundRectangles[i].x;
						shape_Y = boundRectangles[i].y;

						color = detectColor(shape_X + boundRectangles[i].width / 2, shape_Y + boundRectangles[i].height / 2, img);
						putText(img, "Square(" + color + ")", Point2f(shape_X, shape_Y), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
					}
				}
				else{
					drawContours(img, contours, i, Scalar(76, 255, 166), 2);
					countCircles++;
					shape_X = boundRectangles[i].x;
					shape_Y = boundRectangles[i].y;

					color = detectColor(shape_X + boundRectangles[i].width / 2, shape_Y + boundRectangles[i].height / 2, img);
					putText(img, "Circle(" + color + ")", Point2f(shape_X, shape_Y), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
				}
			}
		}

		cout << "Number of shapes: " << countAll << endl;
		cout << countTriangles << " Triangles" << endl;
		cout << countSqaures << " Squares" << endl;
		cout << countRectangles << " Rectangles" << endl;
		cout << countCircles << " Circles" << endl;

		imshow("Output Image", img);
		imwrite("Output Image.bmp", img);
		waitKey();
	}
	else{
		cout << "Error loading the input image!" << endl;
	}
	return 0;
}