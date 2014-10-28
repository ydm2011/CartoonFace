#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <iostream>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <map>
#include <fstream>
using namespace cv;
using namespace std;
enum Organs{EYE,NOSE,MOUTH};
void resizeRoi(const Mat& src,
               const vector<Rect>& rois,
               vector<Rect>& dst_rois,
               float width_rate=0.1,
               float height_rate=0.25);

void getSkeletonOfRoi(const Mat& src,Mat& edge);

//void refine the detect result
int refineface(const Rect& face,const vector<Rect>& organs,int threshold=30);

int refineOrgans(vector<Rect>& eyes,vector<Rect>& mouths,vector<Rect>& noses);

//write the position to the file
void writePosition(Rect& rect,ofstream& out);

//pHash algorithm
 string pHashValue(Mat &src);

 //hamming distance
int hanmingDistance(const string &str1,const string &str2);

//get the nearest hamming distance from the dataset
string match(const Mat& src,const map<string,string>& hash_datas);

//matchProcess
string matchProcess(const Mat& src,map<string,string>& data_set);

//preprocess data set
int preProcessData(const string file_path,map<string,string>* data_set);

//mosaic the part of the match img
int partMosaic(Mat& face_model, const Mat &organs, const Rect& rect);

#endif // FUNCTIONS_H
