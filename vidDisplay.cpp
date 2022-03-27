/*
	Kishore Reddy and Akhil Aji Kumar

	Compile using cmake file

	use the makefiles provided

	Camera calibration file

*/
#include <cstdio>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/calib3d/calib3d_c.h>
#include "function.h"

using namespace cv;

// Create a camera Calibration matrix, distortion and error
int main(int argc, char *argv[]) {

	cv::VideoCapture *capdev;
	char label[256];
	int quit = 0;
	int frameid = 0;
	char buffer[256];
	std::vector<int> pars;

	pars.push_back(10);

	if( argc < 2 ) {
	    printf("Usage: %s <label>\n", argv[0]);
	    exit(-1);
	}

	// open the video device
	capdev = new cv::VideoCapture(0);

	if( !capdev->isOpened() ) {
		printf("Unable to open video device\n");
		return(-1);
	}

	strcpy(label, argv[1]);

	cv::Size refS( (int) capdev->get(cv::CAP_PROP_FRAME_WIDTH ),
		       (int) capdev->get(cv::CAP_PROP_FRAME_HEIGHT));

	printf("Expected size: %d %d\n", refS.width, refS.height);

	cv::namedWindow("Video", 1); // identifies a window
    cv::Mat frame;
    cv::Mat gray;
	std::vector<Point2f> corner_set;
	std::vector<std::vector<Point2f>> cornerList;
	std::vector<Point3f> pointSet;
	std::vector<std::vector<Point3f>> pointList; 

    Size patternSize = Size(9,6);
	
	for (int y=0; y>-6; y--){
		for (int x=0; x<9; x++){
			pointSet.push_back(Point3f(x,y,0));
			
		}
	}

	// // Use static images or pre-captured video to create 3d objects.
	// string path = "/home/kishore/PRCV/Project_3/Training_data/1.jpg";
            
	// Mat frame = imread(path , 1);
		
	for(;!quit;) {
		*capdev >> frame; // store frame of the video capture device into a mat object and loop over to create a video


		if( frame.empty() ) {
		  printf("frame is empty\n");
		  break;
		}

		cvtColor(frame, gray, COLOR_BGR2GRAY);

        //From OpenCV Documentation
		bool patternFound = findChessboardCorners(gray, patternSize, corner_set);

		if (patternFound){
		    cornerSubPix(gray,corner_set,Size(11,11),Size(-1,-1),
		            TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		}
		Mat frameCopy= frame.clone();

		//Draw corners
		drawChessboardCorners(frame, patternSize, Mat(corner_set), patternFound);
		imshow("Video", frame);
		int key = cv::waitKey(10);
		
		switch(key) {
			case 'q':
				quit = 1;
				break;
				
			// capture a photo if the user hits c
			case 'c': 
				sprintf(buffer, "../data/%s.%03d.png", label, frameid++);
				cv::imwrite(buffer, frameCopy, pars);
				printf("Image written: %s\n", buffer);
				break;


			//this case creates the camera calibration matrix
			case 's':
				{
					
					if (corner_set.size() != 54)
						break;

					// save all the corners and point set into a vector
					cornerList.push_back(corner_set);
					pointList.push_back(pointSet);

					sprintf(buffer, "../data/Calibrate.%s.%03d.png", label, frameid++);
					cv::imwrite(buffer, frame, pars);
					printf("Calibration Image written: %s\n", buffer);

					std::vector<double> distCoeffs;
					std::vector<Mat> rvecs;
					std::vector<Mat> tvecs;

					//need ten corners in order to create the matrix
					if (cornerList.size() >= 10){
						float data[3][3] = {{1,0,float(frame.cols)/2},{0,1,float(frame.rows)/2},{0,0,1}};
						Mat cameraMatrix = Mat( 3,3 , CV_32FC1, data);
						Size imageSize = Size(frame.cols, frame.rows);

						//Calibra camera using calibrateCamera function
						double error = calibrateCamera( pointList, cornerList, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_FIX_ASPECT_RATIO);
						std::cout << "cameraMatrix: " << cameraMatrix << "\n" << "distortion coefficients: ";
						for (int i=0; i<distCoeffs.size(); i++){
							std::cout << distCoeffs.at(i) << ", ";
						}
						std::cout << "\nerror: " << error << "\n";
					}
					break;
				}

			//write the camera matrix and the distortion coeffecients to the file
			case 'w':
				{
				std::vector<double> distCoeffs;
				std::vector<Mat> rvecs;
				std::vector<Mat> tvecs;

				//only happens if there are at least 10 corners
				if (cornerList.size() >= 10){
					float data[3][3] = {{1,0,float(frame.cols)/2},{0,1,float(frame.rows)/2},{0,0,1}};
					Mat cameraMatrix = Mat( 3,3 , CV_32FC1, data);
					Size imageSize = Size(frame.cols, frame.rows);
					double error = calibrateCamera( pointList, cornerList, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_FIX_ASPECT_RATIO);
					std::cout << "cameraMatrix: " << cameraMatrix << "\n" << "distortion coefficients: ";
					
					for (int i=0; i<distCoeffs.size(); i++){
						std::cout << distCoeffs.at(i) << ", ";
					}
					
					std::cout << "\nerror: " << error << "\n";
					std::string filename = "writeout.txt";
					std::ofstream myfile;
					myfile.open (filename);
					
					//write cameraMatrix to file
					myfile << cameraMatrix;
					myfile << "\n";

					//write distortion coefficients to file
					for (int  i=0;i<distCoeffs.size(); i++){
						myfile <<  distCoeffs.at(i);
						if (i!=distCoeffs.size()-1){
							myfile << ", ";
						}
					}
					myfile << "\n";
					myfile.close();
					
				}  	
				break;
			}
		}
	}

	// terminate the video capture
	printf("Terminating\n");
	delete capdev;
	return(0);
}
