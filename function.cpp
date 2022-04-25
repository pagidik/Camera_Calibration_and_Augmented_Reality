/*
	Kishore Reddy and Akhil Aji Kumar

	Compile using cmake file

	use the makefiles provided

	Function file

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
using namespace std;


void readFile(std::string filename){
    std::string line;
	std::ifstream nameFileout;
    Mat camMat = Mat::zeros(3,3, CV_32FC1);
    std::vector<float> distortLine;
    

	nameFileout.open(filename);
	if (!nameFileout){
		std::cout << "unable to open\n";
		exit(1);
	}

    bool loopMat = true;
    int i = 0;
    int j = 0;
	while (std::getline(nameFileout, line))
	{
		std::stringstream ss(line);        
        float value;
       
        ss.ignore(); // ignore '['
        
        while ((ss >> value)){
            if (loopMat){ //camera matrix
                camMat.at<float>(j,i) = value;
                if (ss.peek() == ',' || ss.peek() == ' '){
                    ss.ignore();
                    i++;
                }
                else if (ss.peek() == ';'){ // new row
                    ss.ignore();
                    j++;
                    i=0;
                }
                else if (ss.peek() == ']'){ // break at end of matrix
                    loopMat = false;
                }
            }
            
            else{ // distort
                distortLine.push_back(value);
                if (ss.peek() == ',' || ss.peek() == ' ')
                    ss.ignore();
            }

        };

        std::cout << camMat << "\n";
        std::cout << "distort:\n";
        for (i=0;i<distortLine.size();i++){
            std::cout << distortLine[i] << ", ";
        }
        std::cout << "\n";

	    
	}
	nameFileout.close();

}

Mat drawCube(Mat frame, float x, float y, float z){
	cv::Mat gray;
	std::vector<Point2f> corners;
	std::vector<std::vector<Point2f>> cornerList;
	std::vector<Point3f> pointSet;
	std::vector<std::vector<Point3f>> pointList; 

	//size of the pattern on the checkerboard
    Size patternSize = Size(9,6);
	for (int y=0; y>-6; y--){
		for (int x=0; x<9; x++){
			pointSet.push_back(Point3f(x,y,0));
		}
	}

	cvtColor(frame, gray, COLOR_BGR2GRAY);

	//copied a lot of this from documentation
	bool patternFound = findChessboardCorners(gray, patternSize, corners);

	if (patternFound){
		cornerSubPix(gray,corners,Size(11,11),Size(-1,-1),
				TermCriteria(TermCriteria::EPS+ TermCriteria::MAX_ITER, 30, 0.1));
	}

	Mat tvec;
	Mat rvec;
	std::vector<Point2f> outsideCorners;
	std::vector<Point> pts;
	std::vector<Point2f> outputCorners;
	std::vector<Point2f> outputExtra;
	std::vector<Point2f> outputAxis;
	std::vector<Point2f> outputExternal;
	std::vector<Point2f> midOut;

	//the points where we want our pyramid to be drawn
	std::vector<Point3f> externalCorners{Point3f(-1,1,0), Point3f(-1,-6,0), Point3f(9,-6,0), Point3f(9,1,0)};
	std::vector<Point3f> refaxis{Point3f(0,0,0), Point3f(2,0,0), Point3f(0,2,0), Point3f(0,0,2)};
	std::vector<Point3f> cubePoints{Point3f(x+4,y-3,z+0), Point3f(x+4,y-6,z+0), Point3f(x+8,y-6,z+0), Point3f(x+8,y-3,z+0)};
	std::vector<Point3f> extraPoints{Point3f(x+4,y-3,z+4), Point3f(x+4,y-6,z+4), Point3f(x+8,y-6,z+4), Point3f(x+8,y-3,z+4)};
	
	
	if (corners.size() == pointSet.size() && patternFound){
		//gives us rvec and tvec
		solvePnP(pointSet, corners, ::camMat, ::distortLine, rvec, tvec);
		std::cout << "\ntvec\n" << tvec << "\nrvec\n" << rvec << "\n";
		projectPoints(externalCorners, rvec, tvec, ::camMat, ::distortLine, outsideCorners);
		
		//draw a circle at each external corner
		for (int i=0; i<outsideCorners.size(); i++){
			circle(frame, outsideCorners.at(i), 5, (0,0,0));
			pts.push_back(outsideCorners.at(i));
		}

		//draws a big white rectangle so that it doesn't detect the pattern in the next recursion
		fillConvexPoly( frame, pts, Scalar({ 255, 255, 255 }));

		//recursion
		drawCube(frame, x, y, z);
		
		//project points to image coordinates so that we can draw them and they'll make sense
		projectPoints(externalCorners, rvec, tvec, ::camMat, ::distortLine, outputExternal);
		projectPoints(refaxis, rvec, tvec, ::camMat, ::distortLine, outputAxis);  
		projectPoints(extraPoints, rvec, tvec, ::camMat, ::distortLine, outputExtra);            
		projectPoints(cubePoints, rvec, tvec, ::camMat, ::distortLine, outputCorners);

		//draw 3d axis

	
		line(frame, outputAxis.at(0), outputAxis.at(1), Scalar({255,0,0}), 4);
		line(frame, outputAxis.at(0), outputAxis.at(2), Scalar({0,255,0}), 4);
		line(frame, outputAxis.at(0), outputAxis.at(3), Scalar({0,0,255}), 4);
		


		//draw all the points

		for (int i=0; i<outputCorners.size()-1; i++){
			line(frame, outputCorners.at(i), outputCorners.at(i+1), Scalar({255,0,0}), 4);
		}
		line(frame, outputCorners.at(0), outputCorners.at(3), Scalar({255,0,0}), 4);

		for (int i=0;i<outputExtra.size() - 1;i++){
			line(frame, outputExtra.at(i), outputExtra.at(i+1), Scalar({255,0,0}), 4);
		}
		line(frame, outputExtra.at(0), outputExtra.at(3), Scalar({255,0,0}), 4);

		
		for (int i=0; i<outputCorners.size(); i++){
			line(frame, outputCorners.at(i), outputExtra.at(i), Scalar({255,0,0}), 4);
		}

	}
	return frame;
}


