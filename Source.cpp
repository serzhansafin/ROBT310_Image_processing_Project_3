#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <cmath>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>

using namespace cv;
using namespace std;

char windowName[] = "Displaying window";
char TrackbarName[50];  //at the stage of creating trackbar
int alpha_slider_max = 300; //the maximum valie on the trackbar slider
int alpha_slider = 0, flag = 1; // the minimum limit is zero

int b_c, g_c, r_c;
Mat image, image1;


string window_name = "video";
int filenumber; // Number of file to be saved
string filename;

// functions' declaration
int imageColorSelection();
int videoTracking();
void on_trackbar(int, void*);
void on_mouse(int e, int x, int y, int d, void* ptr);
int imshow_image();
int update_frame();

int main(int argc, char** argv) {
	// instructions for user
	cout << "Choose one of the choices for usage of selective color effect: " << endl;
	cout << "	To choose the implementation of selective color effect on image, please, put 1" << endl;
	cout << "	To choose the implementation of selective color effect on video, please, put 2" << endl;
	int choice;
	cin >> choice;
	// switch to the mode (part of the assignment)
	if (choice == 1) {
		imageColorSelection();
	}
	else if (choice == 2) {
		videoTracking();
	}
	return 0;
}

int imageColorSelection() {  // almost all the useful information were in opencv documentation for implementing ..
	// .. this and further
	sprintf(TrackbarName, "Alpha x %d", alpha_slider_max);
	image = imread("../data/road.jpg", IMREAD_COLOR); // any picture will be read
	// Check for invalid input
	if (image.empty()) {
		cout << "There is no image, please check for the code (if it is included at all) " << std::endl;
		return -1;
	}
	namedWindow(windowName, WINDOW_AUTOSIZE); // it creates a window where picture will be placed.	
	setMouseCallback(windowName, on_mouse, NULL); // so, it sets the callback function for any mouse event
	imshow(windowName, image); // demonstrates the given image
	createTrackbar(TrackbarName, windowName, &alpha_slider, alpha_slider_max, on_trackbar); // it creates trackbar slider
	waitKey(0);
	return 0;
}

void on_trackbar(int, void*) {
	// at each instance of change in trackbar image shoudl be updated
	imshow_image();
}

void on_mouse(int e, int x, int y, int d, void* ptr) {
	// consider only left mouse click
	if (e == EVENT_LBUTTONDOWN) {
		int n1 = image.channels();
		// save chosen pixels as a global static variable
		uchar* ptr = image.data + image.step * y;
		b_c = ptr[n1 * x];
		g_c = ptr[n1 * x + 1];
		r_c = ptr[n1 * x + 2];
	}
}

int imshow_image() {
	uchar* ptr; // pointer for accessing pixels
	int b, g, r;
	double mean;
	image1 = image.clone(); // copied image is needed for further processing the image
	int n = image1.channels(); // channels were recorded to variable)
	if (b_c != NULL) {
		int dist, i, j;
		for (i = 0; i < image1.cols; i++) {
			for (j = 0; j < image1.rows; j++) {
				ptr = image.data + image.step * j;
				b = ptr[n * i];
				g = ptr[n * i + 1];
				r = ptr[n * i + 2];
				// calculate euclidean distance
				dist = sqrt((b - b_c) * (b - b_c) + (g - g_c) * (g - g_c) + (r - r_c) * (r - r_c));
				if (dist > alpha_slider) {
					ptr = image1.data + image1.step * j;
					mean = (b + g + r) / 3;
					ptr[n * i] = mean;
					ptr[n * i + 1] = mean;
					ptr[n * i + 2] = mean;
				}
			}
		}
		imshow(windowName, image1);
		image1 = image.clone();
	}
	return 0;
}

int videoTracking() {  // useful info also was found, searching for the right 
	VideoCapture cap;	// open the default camera
	namedWindow(windowName, WINDOW_AUTOSIZE); // Create a window for display.	
	//set the callback function for any mouse event
	setMouseCallback(windowName, on_mouse, NULL);
	createTrackbar(TrackbarName, windowName, &alpha_slider, alpha_slider_max, NULL); //trackbar slider is created
	
	if (!cap.open(0)) {
		return 0;
	}
	// will be running (real-time)
	for (;;) {
		cap >> image;
		if (image.empty()) {
			break; // stream of the video will end
		}
		if (flag == 0) {
			update_frame();	// frame is updated
		}
		
		if (waitKey(1) == 27) {
			break; // esc will exit from the video stream
		}
	}
	
	return 0;
}

int update_frame() {
	uchar* ptr;
	int b, g, r;
	double mean;
	int n = image.channels();
	if (b_c != 0) {
		int dist;
		int i, j;
		for (i = 0; i < image.cols; i++) {
			for (j = 0; j < image.rows; j++) {
				ptr = image.data + image.step * j;
				b = ptr[n * i];
				g = ptr[n * i + 1];
				r = ptr[n * i + 2];  //the euclidean distance is calculated
				
				dist = sqrt((b - b_c) * (b - b_c) + (g - g_c) * (g - g_c) + (r - r_c) * (r - r_c));
				if (dist > alpha_slider) {
					ptr = image.data + image.step * j;
					mean = (b + g + r) / 3;
					ptr[n * i] = mean;
					ptr[n * i + 1] = mean;
					ptr[n * i + 2] = mean;
				}
			}
		}
	}
	imshow(windowName, image);
	return 0;
}
