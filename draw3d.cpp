/*
	Kishore Reddy and Akhil Aji Kumar

	Compile using cmake file

	use the makefiles provided

	Draw 3d Objects file

*/

#include <cstdio>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <aruco/aruco.h>
#include <opencv2/highgui.hpp>


using namespace cv;
using namespace std;

Mat camMat = Mat::zeros(3,3, CV_32FC1);
std::vector<float> distortLine;

//Read file and save camera distortion in a vector
void readFile(std::string filename){
    std::string line;
	std::ifstream nameFileout;

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
                ::distortLine.push_back(value);
                if (ss.peek() == ',' || ss.peek() == ' ')
                    ss.ignore();
            }

        }

        std::cout << camMat << "\n";
        std::cout << "distort:\n";
        for (i=0;i<::distortLine.size();i++){
            std::cout << ::distortLine[i] << ", ";
        }
        std::cout << "\n";

	    
	}
	nameFileout.close();

}

// draw 3d cube on chessboard
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


//draw 3d pyramid 
Mat drawPyramid(Mat frame, float x, float y, float z){
	cv::Mat gray;
	std::vector<Point2f> corners_p;
	std::vector<std::vector<Point2f>> cornerList_p;
	std::vector<Point3f> pointSet_p;
	std::vector<std::vector<Point3f>> pointList_p; 

	//size of the pattern on the checkerboard
    Size patternSize = Size(9,6);
	for (int y=0; y>-6; y--){
		for (int x=0; x<9; x++){
			pointSet_p.push_back(Point3f(x,y,0));
		}
	}

	cvtColor(frame, gray, COLOR_BGR2GRAY);


	bool patternFound = findChessboardCorners(gray, patternSize, corners_p);

	if (patternFound){
		cornerSubPix(gray,corners_p,Size(11,11),Size(-1,-1),
				TermCriteria(TermCriteria::EPS+ TermCriteria::MAX_ITER, 30, 0.1));
	}

	Mat tvec;
	Mat rvec;
	std::vector<Point2f> outsidecorners_p;
	std::vector<Point> pts;
	std::vector<Point2f> outputcorners_p;
	std::vector<Point2f> outputExtra_p;
	std::vector<Point2f> outputAxis_p;
	std::vector<Point2f> outputExternal_p;
	std::vector<Point2f> midOut;
	

	//the points where we want our pyramid to be drawn
	std::vector<Point3f> externalcorners_p{Point3f(-1,1,0), Point3f(-1,-6,0), Point3f(9,-6,0), Point3f(9,1,0)};
	std::vector<Point3f> refaxis_p{Point3f(0,0,0), Point3f(2,0,0), Point3f(0,2,0), Point3f(0,0,2)};
	std::vector<Point3f> pyramidPoints{Point3f(x+4,y-3,z+0), Point3f(x+4,y-6,z+0), Point3f(x+8,y-6,z+0), Point3f(x+8,y-3,z+0), Point3f(x+6,y-4.5,z+7)};
	std::vector<Point3f> extraPoints_p{Point3f(x+6,y-4.5,z+7), Point3f(x+6,y-4.5,z+0), Point3f(x+4,y-3,z+0), Point3f(x+4,y-6,z+0), Point3f(x+8,y-6,z+0), Point3f(x+8,y-3,z+0)};
	
	
	if (corners_p.size() == pointSet_p.size() && patternFound){
		//gives us rvec and tvec
		solvePnP(pointSet_p, corners_p, ::camMat, ::distortLine, rvec, tvec);
		std::cout << "\ntvec\n" << tvec << "\nrvec\n" << rvec << "\n";
		projectPoints(externalcorners_p, rvec, tvec, ::camMat, ::distortLine, outsidecorners_p);
		
		//draw a circle at each external corner
		for (int i=0; i<outsidecorners_p.size(); i++){
			circle(frame, outsidecorners_p.at(i), 5, (0,0,0));
			pts.push_back(outsidecorners_p.at(i));
		}

		//draws a big white rectangle so that it doesn't detect the pattern in the next recursion
		fillConvexPoly( frame, pts, Scalar({ 255, 255, 255 }));

		//recursion
		drawPyramid(frame, x, y, z);
		
		//project points to image coordinates so that we can draw them and they'll make sense
		projectPoints(extraPoints_p, rvec, tvec, ::camMat, ::distortLine, outputExtra_p);   
		projectPoints(refaxis_p, rvec, tvec, ::camMat, ::distortLine, outputAxis_p);           
		projectPoints(pyramidPoints, rvec, tvec, ::camMat, ::distortLine, outputcorners_p);

		line(frame, outputAxis_p.at(0), outputAxis_p.at(1), Scalar({255,0,0}), 4);
		line(frame, outputAxis_p.at(0), outputAxis_p.at(2), Scalar({0,255,0}), 4);
		line(frame, outputAxis_p.at(0), outputAxis_p.at(3), Scalar({0,0,255}), 4);

		//draw all the points
		for (int i=0; i<outputcorners_p.size()-1; i++){
			line(frame, outputcorners_p.at(i), outputcorners_p.at(i+1), Scalar({255,125,0}), 4);
		}
		line(frame, outputcorners_p.at(0), outputcorners_p.at(3), Scalar({255,125,0}), 4);

		for (int i=2;i<6;i++){
			line(frame,outputExtra_p.at(1), outputExtra_p.at(i), Scalar({0,125,255}), 4);
		}
		line(frame, outputExtra_p.at(0), outputExtra_p.at(1), Scalar({0,125,255}), 4);

		
		for (int i=0; i<outputcorners_p.size()-1; i++){
			line(frame, outputcorners_p.at(i), outputcorners_p.at(4), Scalar({255,125,0}), 4);
		}
	}
	return frame;
}


// Main function to draw 3d cube and detect aruco marker
int main(int argc, char *argv[]) {
	
    readFile("writeout.txt");
    std::cout << "camMat\n" << ::camMat << "\n";
            
    cv::VideoCapture *capdev;
	char label[256];
	int quit = 0;
	int frameid = 0;
	char buffer[400];
	std::vector<int> pars;

	pars.push_back(5);

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

	cv::namedWindow("Video", 1); // identifies a window?
	//cv::namedWindow("Video with Coverings",1);
    cv::Mat frame;
	// cv::Mat frame2;
    
	//variables to move around the pyramid
	int x = 0;
	int y = 0;
	int z = 0;
            
	for(;!quit;) {
		*capdev >> frame; // get a new frame from the camera, treat as a stream

		if( frame.empty() ) {
		  printf("frame is empty\n");
		  break;
		}
		
		// frame = drawPyramid(frame, float(x), float(y), float(z));

		frame = drawCube(frame, float(x), float(y), float(z));
		imshow("Video 1", frame);
		
		// imshow("Video 2", frame);

		aruco::MarkerDetector MDetector;
		//read input image
		// cv::Mat InImage = frame;

		//detect corners for aruco
		MDetector.setDictionary("ARUCO_MIP_36h12");

		//detect Markers for each one, draw infro an dits boubndaris
		for(auto m:MDetector.detect(frame)){
			cout<< "Marker corners : " << m << endl;
			m.draw(frame);
		}
		
		imshow("Aruco" , frame);

		int key = cv::waitKey(10);
		std::cout << x <<"\n"<< y << "\n";
		
		switch(key) {
			case 'q':
				quit = 1;
				break;
				
			case 'c': // capture a photo if the user hits c
				sprintf(buffer, "../data/%s.%03d.png", label, frameid++);
				cv::imwrite(buffer, frame, pars);
				printf("Image written: %s\n", buffer);
				break;
			
			//the following 6 cases are for moving the cube/pyramid around
			case 'k':
			{
				y++;
				break;
			}
			case 'i':
			{
				y--;
				break;
			}
			case 'j':
			{
				x--;
				break;
			}
			case 'l':
			{
				x++;
				break;
			}
			case 'o':
			{				
				z--;
				break;
			}
			case 'u':
			{
				z++;
				break;
			}
			
			case 'a':
			{
				cout<< "entering aruco" << endl;
				
				aruco::MarkerDetector MDetector;
				//read input image
				cv::Mat InImage = frame;

				//detect corners for aruco
				MDetector.setDictionary("ARUCO_MIP_36h12");

				//detect Markers for each one, draw infro an dits boubndaris
				for(auto m:MDetector.detect(InImage)){
					cout<< m << endl;
					m.draw(InImage);
				}
				
				imshow("Aruco" , InImage);
				while (true) { 
				
                
                char key = cv::waitKey(0);
                if( key == 'q') {
                    break;
                }  
    
            }
				
				

				cout<< "exiting aruco" << endl;
			}
		}

	}

	// terminate the video capture
	printf("Terminating\n");
	delete capdev;

	return(0);
}
