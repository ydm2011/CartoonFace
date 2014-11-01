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
class DataPath{
public:
    DataPath();
    string picture_path;
    string features_path;
    string raw_location_path;
    string refine_location_path;
};

void resizeRoi(const Mat& src,
               const vector<Rect>& rois,
               vector<Rect>& dst_rois,
               float width_rate=0.1,
               float height_rate=0.25);

void getSkeletonOfRoi(const Mat& src,Mat& edge);

//void refine the detect result
int extractFace(vector<Rect>& faces);
int refineface(const Rect& face,const vector<Rect>& organs,int threshold=30);

int refineOrgans(vector<Rect>& eyes,vector<Rect>& mouths,vector<Rect>& noses);

//write the position to the file
void writePosition(Rect& rect,ofstream& out);

//pHash algorithm
 string pHashValue(Mat& src);

 //hamming distance
int hanmingDistance(const string &str1,const string &str2);

//get the nearest hamming distance from the dataset
string match(const string& src_phash,const map<string,string>& hash_datas);

//matchProcess
string matchProcess(Mat src, map<string,string>& data_set);

//preprocess data set
int preProcessData(const string file_path,map<string,string>* data_set);

//mosaic the part of the match img
int partMosaic(Mat& face_model, const Mat &organs, const Rect& rect);

int parseFormat(const std::string& line, std::vector<string> &result);

int readPreFeatures(const string& path,map<string,string>& preFeatures);

int writeLocations(const vector<Rect>&src, ofstream &path);

int findLocations(const DataPath& path);
//allignment the face size
//int alignmentFaceSize(Rect,Mat src2,)

class GetLocation{
public:
    int findLocations(const DataPath &path);
    string getMatchKey(const string& src_phash,DataPath& path);
    int getAllMatchKey(DataPath& path,map<string,string>& key_pairs);
private:
};

#endif // FUNCTIONS_H
