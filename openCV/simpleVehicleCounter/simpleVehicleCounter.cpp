/* 
Author : Rajiv Mantena
Email  : mantenarajiv@gmail.com
Date   : January 14, 2017 
Web    : rajivmantena.net
*/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string> 

using namespace cv;

int main(int argc, char** argv)
{	
	// Read video from file
    VideoCapture myCapture("video.mp4");
    VideoCapture myCaptureDup("video.mp4");
    Mat color_frame, gray_frame, background_frame;

    if( !myCapture.isOpened() )
        throw "Error when reading steam_avi";

	// *** Uncomment if you wish to see the video while the program is running
    // namedWindow( "Output", 1);
	
	// Variable init
    int count = 0;
    int tempCounter = 0;
    int array[360][640] = { 0 };
    int frameFactor = 10;
    
	// Loop to evaluate the background frame
	while(1)
    {
		//  Fetch a frame
		myCapture >> color_frame;
		// Convert to Gray-Scale
		cvtColor(color_frame, gray_frame, CV_RGB2GRAY);
		// Print out the details, only once
		if(count == 0){
			std::cout << "Here : " << gray_frame.cols << " " << gray_frame.rows << std::endl ;
			std::cout << "Pixel : " << (int)gray_frame.at<uchar>(1,1) << std::endl ;
			background_frame = gray_frame; 
		}
		// If the video has ended
		if(color_frame.empty())
            break;
		if(count - tempCounter >= frameFactor){
			for(int j=0 ; j<gray_frame.rows ; j++)
				for(int i=0 ; i<gray_frame.cols ; i++) {
					array[j][i] = array[j][i] + gray_frame.at<uchar>(j,i);
				}
			tempCounter = count;
		}
		count++;
    }
	for (int i = 0; i < background_frame.rows; i++){
		for(int j = 0; j < background_frame.cols; j++){
			array[i][j] = array[i][j] * frameFactor/ count;
			background_frame.at<uchar>(i,j) = array[i][j];
		}
    }
	// More variable init
    int time[4] = { 0 };
    int cars[4] = { 0 };
    bool carPresent[4] = { false };
	// Initiate VideoWriter
    VideoWriter myOutput("output.avi",CV_FOURCC('M', 'J', 'P', 'G'), 
               myCaptureDup.get(CV_CAP_PROP_FPS),
               Size(myCaptureDup.get(CV_CAP_PROP_FRAME_WIDTH),
               myCaptureDup.get(CV_CAP_PROP_FRAME_HEIGHT)));
    if (!myOutput.isOpened()) {
        std::cout << "Could not open the output video file for write\n";
    }
    count = 0;
    while(1){
		// Fetch frame and convert to gray_scale
		myCaptureDup >> color_frame;
        cvtColor(color_frame, gray_frame, CV_RGB2GRAY);
		if(color_frame.empty())
            break;
		// Loop to find where there is a mismatch between the current gray_frame and background_frame	
		for (int i = 0; i < background_frame.rows; i++){
            for(int j = 0; j < background_frame.cols; j++){
				// If there is a considerable difference in grayScale values ... Update
				if ((gray_frame.at<uchar>(i,j) - background_frame.at<uchar>(i,j) < 30) 
					&& (gray_frame.at<uchar>(i,j) - background_frame.at<uchar>(i,j) > -30))
						gray_frame.at<uchar>(i,j) = 150;
				else if ((gray_frame.at<uchar>(i,j) - background_frame.at<uchar>(i,j) < 30))
					gray_frame.at<uchar>(i,j) = 0;
				else
					gray_frame.at<uchar>(i,j) = 0;
			}
		}
		// Blur the frame
		blur(gray_frame, gray_frame, Size(10,10));	
		int myCumVal[4] = { 0 };
		// Loops to find out if there is a significant difference in 
		// most of the pixels in the given area ...
		for (int i = 175 ; i < 225 ; i++)
				for (int j = 65 ; j < 115 ; j++){
					myCumVal[0] += gray_frame.at<uchar>(i,j);
				}
		for (int i = 175 ; i < 225 ; i++)
				for (int j = 180 ; j < 225 ; j++){
					myCumVal[1] += gray_frame.at<uchar>(i,j);
				}	
		for (int i = 175 ; i < 225 ; i++)
			for (int j = 275 ; j < 325 ; j++){
			myCumVal[2] += gray_frame.at<uchar>(i,j);
			}
		for (int i = 175 ; i < 225 ; i++)
				for (int j = 375 ; j < 445 ; j++){
					myCumVal[3] += gray_frame.at<uchar>(i,j);
				}
		// Division to normalize all the Cumulative values
		myCumVal[0] /= 2500;
		myCumVal[1] /= 2250;
		myCumVal[2] /= 2500;
		myCumVal[3] /= 3500;
		
		// Actual Car detection 
		// for loop to consider only the lines around row = 200
		for (int n = 199 ; n < 202; n++ ){
		// For the first lane of traffic
		for (int j = 55 ; j < 130 ; j++)
			// If car isn't present 
			if (carPresent[0] == false ){
			// Code to write to the image to later save it as an output video.
				color_frame.at<Vec3b>(Point(j,n))[0] = 255;
				color_frame.at<Vec3b>(Point(j,n))[1] = 0;
				color_frame.at<Vec3b>(Point(j,n))[2] = 0;
				gray_frame.at<uchar>(n,j) = 0;
			}
			// Else, write another color to the mages
			else {
				color_frame.at<Vec3b>(Point(j,n))[0] = 0;
				color_frame.at<Vec3b>(Point(j,n))[1] = 0;
				color_frame.at<Vec3b>(Point(j,n))[2] = 255;
				gray_frame.at<uchar>(n,j) = 255;
			}
		// For the second lane of traffic
		for (int j = 155 ; j < 225 ; j++)
			if (carPresent[1] == false ){
				color_frame.at<Vec3b>(Point(j,n))[0] = 255;
				color_frame.at<Vec3b>(Point(j,n))[1] = 0;
				color_frame.at<Vec3b>(Point(j,n))[2] = 0;
				gray_frame.at<uchar>(n,j) = 0;
			}
			else {
				color_frame.at<Vec3b>(Point(j,n))[0] = 0;
				color_frame.at<Vec3b>(Point(j,n))[1] = 0;
				color_frame.at<Vec3b>(Point(j,n))[2] = 255;
				gray_frame.at<uchar>(n,j) = 255;
			}
		// For the third lane of traffic
		for (int j = 255 ; j < 340 ; j++)
			if (carPresent[2] == false ){
				color_frame.at<Vec3b>(Point(j,n))[0] = 255;
				color_frame.at<Vec3b>(Point(j,n))[1] = 0;
				color_frame.at<Vec3b>(Point(j,n))[2] = 0;
				gray_frame.at<uchar>(n,j) = 0;
			}
			else {
				color_frame.at<Vec3b>(Point(j,n))[0] = 0;
				color_frame.at<Vec3b>(Point(j,n))[1] = 0;
				color_frame.at<Vec3b>(Point(j,n))[2] = 255;
				gray_frame.at<uchar>(n,j) = 255;
			}
		// For the fourth lane of traffic
		for (int j = 365 ; j < 445 ; j++)
			if (carPresent[3] == false ){
				color_frame.at<Vec3b>(Point(j,n))[0] = 255;
				color_frame.at<Vec3b>(Point(j,n))[1] = 0;
				color_frame.at<Vec3b>(Point(j,n))[2] = 0;
				gray_frame.at<uchar>(n,j) = 0;
			}
			else {
				color_frame.at<Vec3b>(Point(j,n))[0] = 0;
				color_frame.at<Vec3b>(Point(j,n))[1] = 0;
				color_frame.at<Vec3b>(Point(j,n))[2] = 255;
				gray_frame.at<uchar>(n,j) = 255;
			}
		}
	// For each of the lane, if a car is detected, (by myCumVal < 130), 
	// increment the counter and set the carPresent flag to TRUE;
	for (int n=0; n < 4; n++){
	if ((myCumVal[n] < 130) && (carPresent[n] == false)){
	    cars[n] += 1;
	    carPresent[n] = true;
	    time[n] = 0;
	}
	// If the car was present a little while ago, is it still there? 
	// If no, turn the carPresent flag back to FALSE
	if ((myCumVal[n] > 140) && (carPresent[n] == true) && (time[n] > 5))
	    carPresent[n] = false;
	time[n]++;
	}
	// Variable init to display on the video
	std::string s[5];
	for (int n = 0; n < 4; n++){
	    std::stringstream ss;
	    ss << cars[n];
	    s[n] = ss.str();
	}
	// Add up the total cars and display it on the video
	std::stringstream ss;
	ss << "Total Vehicles : " << cars[0] + cars[1] + cars[2] + cars[3];
	s[4] = ss.str();
	// More variable init (for writing strings on the video)
	int fontFace = FONT_HERSHEY_DUPLEX;
	Point pt[5] = {Point(85,192), Point(185,192), Point(285,192), Point(393,192), Point(415,35)};
	double fontScale = 0.7;
	int thickness = 1.8;  
	// putText on the output_video
	for (int n = 0; n < 5; n++){
	    putText(color_frame, s[n], pt[n], fontFace, fontScale, Scalar::all(255), thickness,8);
	    putText(gray_frame, s[n], pt[n], fontFace, fontScale, Scalar::all(255), thickness,8);
	}
	
	// *** Uncomment the below lines if you wish to see the video while the program is running
	// imshow("c", color_frame);
	//         or
	// imshow("c", gray_frame);
	// waitKey(20);
	
	cvtColor(gray_frame, gray_frame, CV_GRAY2RGB);
	// If loop to switch between Color and Gray frame during the video playback
	if (count % 35 < 15) myOutput.write(color_frame);
	else myOutput.write(gray_frame);	
	count++;
    }
	// Write the output to the terminal as well.
    std::cout << "Total Vehicles : " << cars[0] + cars[1] + cars[2] + cars[3] << std::endl;
}
