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


