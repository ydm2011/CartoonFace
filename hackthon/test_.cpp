
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp""
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
using namespace cv;
using namespace std;
int test3_main( )
{
    Mat src;
    src = imread("/home/daoming/Desktop/test2.jpeg", CV_LOAD_IMAGE_COLOR);
    Mat gray;
    cvtColor(src, gray, CV_BGR2GRAY);
    threshold(gray, gray,200, 255,THRESH_BINARY_INV); //Threshold the gray
    imshow("gray",gray);int largest_area=0;
    int largest_contour_index=0;
    Rect bounding_rect;
    vector<vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    findContours( gray, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
    // iterate through each contour.
    for( int i = 0; i< contours.size(); i++ )
    {
        //  Find the area of contour
        double a=contourArea( contours[i],false);
        if(a>largest_area){
            largest_area=a;cout<<i<<" area  "<<a<<endl;
            // Store the index of largest contour
            largest_contour_index=i;
            // Find the bounding rectangle for biggest contour
            bounding_rect=boundingRect(contours[i]);
        }
    }
    Scalar color( 255,255,255);  // color of the contour in the
    //Draw the contour and rectangle
    drawContours( gray, contours,largest_contour_index, color, CV_FILLED,8,hierarchy);
    rectangle(src, bounding_rect,  Scalar(0,255,0),2, 8,0);
    namedWindow( "Display window", CV_WINDOW_AUTOSIZE );
    imshow( "Display window", gray );
    waitKey(0);
    return 0;
}
