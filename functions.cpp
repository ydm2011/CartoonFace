#include "functions.h"
using namespace std;
using namespace cv;
//resize the ROi
void resizeRoi(const Mat& src,
               const vector<Rect>& rois,
               vector<Rect>& dst_rois,
               float width_rate,
               float height_rate)
{
    vector<Rect>::const_iterator iter = rois.begin();
    Rect temp_roi;
    for( ;iter != rois.end();++iter)
    {
        int x = iter->x -width_rate/2.0*(iter->width) +0.5;
        if(x<0)
        {
            temp_roi.x = 0;
        }else  if(x<src.cols) temp_roi.x = x;

        int y = iter->y - height_rate/2.0*(iter->height)+0.5;
        if(y<0)
        {
            temp_roi.y = 0;
        }else if(y<src.rows) temp_roi.y = y;

        int width = iter->width + width_rate*(iter->width)+0.5;
        if(temp_roi.x + width < src.cols)
        {
            if(width>0)
                temp_roi.width = width;
            else temp_roi.width = 0;
        } else temp_roi.width = src.cols - temp_roi.x;

        int height = iter->height + height_rate*(iter->height)+0.5;
        if(temp_roi.y + height < src.rows)
        {
            if(height>0)
                temp_roi.height = height;
            else temp_roi.height = 0;
        }else temp_roi.height = src.rows - temp_roi.y;
        dst_rois.push_back(temp_roi);
    }
}
//get the binary  edge image  of the src image
void getSkeletonOfRoi(const Mat& src,Mat& edge)
{
    Mat temp;
    medianBlur(src,temp,3);//erase the pot dirty pixels
    GaussianBlur(temp,temp,Size(3,3),1);//
    bilateralFilter(temp,edge,9,9,7);//keep the edge while denoising
    Laplacian(edge,edge,CV_8U,5);
}
//void refine the detect result
int refineface(const Rect& face,const vector<Rect>& organs,int threshold)
{
    //refine the faces
    if(face.width<threshold||face.height<threshold)
        return -1;
    if(organs.empty())
        return -2;
    return 0;
}
int refineOrgans(vector<Rect>& eyes,vector<Rect>& mouths,vector<Rect>& noses)
{
    Rect temp;
    //int loca;
    if(eyes.size()==0)
        return -1;
    //refine the eyes
    if(eyes.size()==1&&mouths.size()!=0)
    {
        temp = eyes[0];
        temp.x = eyes[0].x+ mouths[0].x + mouths[0].width/2-eyes[0].width;
        temp.y = eyes[0].y;
    }
    if(eyes.size()>=3)
    {
        return -2;
    }
    //refine mouth
    temp = Rect();
    if(mouths.size()!=1&&!mouths.empty())
    {
        for(int i=0;i<mouths.size();++i)
        {
            if(mouths[i].y>eyes[0].y&&mouths[i].x>(eyes[0].x<eyes[1].x?eyes[0].x:eyes[1].x))
            {
                temp = mouths[i];
            }
        }
        mouths.clear();
        mouths.push_back(temp);
    }
    //refine nose
    temp = Rect();
    if(noses.size()!=1&&!noses.empty())
    {
        for(int i=0;i<noses.size();++i)
        {
            if(noses[i].y>eyes[0].y&&noses[i].x>(eyes[0].x<eyes[1].x?eyes[0].x:eyes[1].x))
            {
                temp = noses[i];
            }
        }
        noses.clear();
        noses.push_back(temp);
    }
    //erase the overide
    int local_overide;
    if(noses[0].y+noses[0].height>mouths[0].y+mouths[0].height/2)
    {
        local_overide = noses[0].y+noses[0].height-mouths[0].y;
        local_overide /=2;
        if(noses[0].height-local_overide>0)
            noses[0].height -= local_overide;
        else noses[0].height = local_overide;
        local_overide = noses[0].y + noses[0].height- mouths[0].y;
        if(local_overide>0)
        {
            local_overide /=2;
            mouths[0].y += local_overide;
            mouths[0].height -= local_overide;
        }
    }
    return 0;
}

//write the position to the file
void writePosition(Rect& rect,ofstream& out)
{
    out<<rect.x<<",";
    out<<rect.y<<",";
    out<<rect.width<<",";
    out<<rect.height<<",";
}

//pHash algorithm
 string pHashValue(const Mat &src)
 {
     Mat img ,dst;
     string rst(64,'\0');
     double dIdex[64];
     double mean = 0.0;
     int k = 0;
     if(src.channels()==3)
     {
         cvtColor(src,src,CV_BGR2GRAY);
         img = Mat_<double>(src);
     }
     else
     {
         img = Mat_<double>(src);
     }

     //1.resize the img
     resize(img, img, Size(8,8));

      //2.dct transform
     dct(img, dst);

     //3. the mean of the coefficients of the dct
     const uchar* ptr;// = dst.ptr<uchar>()
     for (int i = 0; i < 8; ++i)
     {
         ptr = dst.ptr<uchar>(i);
         for (int j = 0; j < 8; ++j)
         {
             dIdex[k] = ptr[j];
             mean += ptr[j]/64;
             ++k;
         }
     }

     //4.caculate the hash value
     for (int i =0;i<64;++i)
     {
         if (dIdex[i]>=mean)
         {
             rst[i]='1';
         }
         else
         {
             rst[i]='0';
         }
     }
     return rst;
 }

 //hamming distance
int hanmingDistance(const string &str1,const string &str2)
{
    if((str1.size()!=64)||(str2.size()!=64))
       return -1;
    int difference = 0;
    for(int i=0;i<64;i++)
    {
        if(str1[i]!=str2[i])
            difference++;
    }
    return difference;
}
//get the nearest hamming distance from the dataset
string match(const string& hash_target,const map<string,string>& hash_datas)
{
    map<string,string>::const_iterator iter = hash_datas.begin();
    string key;
    int min_distace=64;
    int distance;
    for( ; iter!=hash_datas.end();++iter)
    {
        distance = hanmingDistance(hash_target,iter->second);
        if(min_distace>distance)
        {
            min_distace = distance;
            key = iter->first;
        }
    }
    return key;
}

//matchProcess
string matchProcess(const Mat& src,map<string,string>& data_set)
{
    string hash_value;
    hash_value = pHashValue(src);
    string data_set_key;
    data_set_key = match(hash_value,data_set);
    return data_set_key;
}
//load the given parameters:filepath; material path and so on
int preProcessData(const string& file_path,map<string,string>* data_set)
{
    ;
}

//mosaic the part of the match img
int partMosaic(Mat& face_model,const Mat& organs,const Rect& rect)
{
    Mat image_roi = face_model(rect);
    organs.copyTo(image_roi);
}














