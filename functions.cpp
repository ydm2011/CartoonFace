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
    GaussianBlur(temp,edge,Size(3,3),1);//
    bilateralFilter(temp,edge,9,9,7);//keep the edge while denoising
    Laplacian(edge,edge,CV_8U,5);
}

//extract the main face
int extractFace(vector<Rect>& faces)
{
    vector<Rect> result;
    int max_height=0;
    int max_width=0;
    int max_height_pos=0;
    int max_width_pos=0;
    for(int i=0;i<faces.size();++i)
    {
        if(faces[i].width>max_width)
        {
            max_width_pos = i;
            max_width = faces[i].width;
        }
        if(faces[i].height>max_height)
        {
            max_height_pos = i;
            max_height = faces[i].height;
        }
    }
    if(max_width_pos == max_height_pos)
    {
        result.push_back(faces[max_height_pos]);
    }else result.push_back(faces[max_height_pos]);
    faces.clear();
    faces = result;
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
 string pHashValue(Mat& src)
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
string matchProcess(Mat src, map<string,string>& data_set)
{
    string hash_value;
    hash_value = pHashValue(src);
    string data_set_key;
    data_set_key = match(hash_value,data_set);
    return data_set_key;
}

int writeLocations(const vector<Rect>&src,ofstream& path)
{
    assert(path);
    for(int i=0; i< src.size();++i)
    {
        path<<src[i].x<<","<<src[i].y<<","
           <<src[i].height<<","<<src[i].width<<endl;
    }
}

int readPreFeatures(const string& path,map<string,string>& preFeatures)
{
    ifstream in;
    in.open(path.c_str(),ios::in);
    assert(in);
    string line;
    vector<string> result;
    string key;
    while(!in.eof())
    {
        in>>line;
        parseFormat(line,result);
        assert(result.size()!=2);
        preFeatures[result[0]]=result[1];
    }
}
int parseFormat(const std::string& line,std::vector<string>& result)
{
    int temp_pos=0;
    result.clear();
    if(!line.size())
        return -1;
    for(int i=0;i<line.size();++i)
    {
        if(line[i]!=',')
            continue;
        result.push_back(std::string(line,temp_pos,i-temp_pos));
        temp_pos = i+1;
    }
    result.push_back(string(line,temp_pos,line.size()-temp_pos));
    return 0;
}

//load the given parameters:filepath; material path and so on
int preProcessData(const string& file_path,map<string,string>& data_set)
{
    ;
}

//mosaic the part of the match img
int partMosaic(Mat& face_model,const Mat& organs,const Rect& rect)
{
    ;
}


//data path
DataPath::DataPath():features_path("./features.dat"),
    raw_location_path("./raw_location.dat"),
    refine_location_path("./refine_location.dat"),
    picture_path("./photo.jpg")
{
    ;
}












