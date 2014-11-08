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
        eyes.push_back(temp);
    }
    if(eyes.size()>=3)
    {
        return -2;
    }
    //refine mouth
    temp = Rect();
    vector<Rect> temp_mouths;
    if(mouths.size()!=1&&!mouths.empty())
    {
        for(int i=0;i<mouths.size();++i)
        {
            if(mouths[i].y>eyes[0].y&&mouths[i].x>(eyes[0].x<eyes[1].x?eyes[0].x:eyes[1].x))
            {
                temp = mouths[i];
                temp_mouths.push_back(temp);
            }
        }
        int position=-1;
        if(temp_mouths.size()!=1&&temp_mouths.size()!=0)
        {
            int temp_pos=temp_mouths[0].height+temp_mouths[0].y;
            position=0;
            for(int i=1; i< temp_mouths.size(); ++i)
            {
                if(temp_pos>temp_mouths[i].height+temp_mouths[i].y)
                {
                    temp_pos = temp_mouths[i].height+temp_mouths[i].y;
                    position = i;
                }
            }
        }
        mouths.clear();
        if(position!=-1)
            mouths.push_back(temp_mouths[position]);
        else mouths.push_back(temp);
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
    if(noses[0].y+noses[0].height>mouths[0].y)
    {
        local_overide = noses[0].y+noses[0].height-mouths[0].y;
        local_overide /=2;
        if(noses[0].height-local_overide>0)
            noses[0].height -= local_overide;
        else noses[0].height = local_overide;
        local_overide *= 2;
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
     const double* ptr;// = dst.ptr<uchar>()
     for (int i = 0; i < 8; ++i)
     {
         ptr = dst.ptr<double>(i);
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
string match(const string& src_phash, const map<string,string>& hash_datas)
{
    map<string,string>::const_iterator iter = hash_datas.begin();
    string key;
    int min_distace=64;
    int distance;
    for( ; iter!=hash_datas.end();++iter)
    {
        distance = hanmingDistance(src_phash,iter->second);
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
    return 0;
}

int readPreFeatures(const string& path,map<string,string>& preFeatures)
{
    ifstream in;
    in.open(path.c_str(),ios::in);
    assert(in);
    string line;
    vector<string> result;
    string key;
    preFeatures.clear();
    while(!in.eof())
    {
        in>>line;
        parseFormat(line,result);
        assert(result.size()!=2);
        preFeatures[result[0]]=result[1];
    }
    return 0;
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

//data path
DataPath::DataPath():features_path("./features/"),
    raw_location_path("./raw_location.dat"),
    refine_location_path("./refine_location.dat"),
    picture_path("./photo.jpg")
{
    ;
}

//get all raw locations
int findAllLocation(const DataPath& path)
{
    //this will replace by the qt;
    Mat src = imread(path.picture_path.c_str());
    if(src.empty())
        return -1;
    std::vector<Rect> faces;
    Mat src_gray;

    cvtColor(src, src_gray, CV_BGR2GRAY);

    medianBlur(src_gray,src_gray,3);//denosie
    equalizeHist(src_gray, src_gray);//

    //classify
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    CascadeClassifier mouth_cascade;
    CascadeClassifier nose_cascade;
    //the training data
    string face_cascade_name = "/home/daoming/Qtproject/hackthon/haarcascade_frontalface_alt2.xml";
    string eyes_cascade_name = "/home/daoming/Qtproject/hackthon/haarcascade_eye_tree_eyeglasses.xml";
    string mouth_cascade_name = "/home/daoming/Qtproject/hackthon/haarcascade_mcs_mouth.xml";
    string nose_cascade_name = "/home/daoming/Qtproject/hackthon/haarcascade_mcs_nose.xml";
    if(!face_cascade.load(face_cascade_name))
    {
        cout<<"Error loading "<<face_cascade_name<<endl;
        return -1;
    }
    if(!eyes_cascade.load(eyes_cascade_name))
    {
        cout<<"Error loading "<<eyes_cascade_name<<endl;
        return -1;
    }
    if(!mouth_cascade.load(mouth_cascade_name))
    {
        cout<<"Error loading "<<mouth_cascade_name<<endl;
        return -1;
    }
    if(!nose_cascade.load(nose_cascade_name))
    {
        cout<<"Error loading "<<eyes_cascade_name<<endl;
        return -1;
    }

    face_cascade.detectMultiScale(src_gray,faces,1.1,2,0|CV_HAAR_SCALE_IMAGE,Size(30,30));
    if(faces.size()!=1)
    {
        extractFace(faces);
    }
    //fix the ROi
    vector<Rect> fix_roi;
    resizeRoi(src_gray,faces,fix_roi,0.1,0.35);
    if(faces.empty())
    {
        cout<<"Can't detect the faces"<<endl;
        return -1;
    }
    //get the eyes mouth nose on the face
    vector<Rect> eyes;
    vector<Rect> mouths;
    vector<Rect> noses;

    Mat face_roi;
    vector<Rect>::iterator iter = fix_roi.begin();
    for( ; iter != fix_roi.end(); ++iter)
    {
        //face region
        face_roi = src_gray(*iter);

        //eyes region
        eyes_cascade.detectMultiScale(face_roi,eyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(10, 10));
        //erase the faked face
        if(refineface(*iter,eyes,10))
            continue;
        //mouth region
        mouth_cascade.detectMultiScale(face_roi,mouths, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));

        //nose region
        nose_cascade.detectMultiScale(face_roi,noses, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));

        refineOrgans(eyes,mouths,noses);
    }
    iter = fix_roi.begin();
    ofstream out_locations;
    out_locations.open(path.raw_location_path.c_str(),ios::out);
    writeLocations(faces,out_locations);
    writeLocations(eyes,out_locations);
    writeLocations(noses,out_locations);
    writeLocations(mouths,out_locations);
    out_locations.close();
    return 0;
}
Rect stringToRect(vector<string>& rect_iterm)
{
    Rect rect;
    rect.x = atoi(rect_iterm[0].c_str());
    rect.y = atoi(rect_iterm[1].c_str());
    rect.height =atoi(rect_iterm[2].c_str());
    rect.width = atoi(rect_iterm[3].c_str());
    return rect;
}

//get locations
int GetLocation::findLocations(const DataPath &path)
{
    findAllLocation(path);
    return 0;
}
//
string GetLocation::getMatchKey(const string& src_phash, DataPath &path)
{
    map<string,string> data_set;
    if(!readPreFeatures(path.features_path,data_set))
            return "";
    return match(src_phash,data_set);
}

int GetLocation::getAllMatchKey(DataPath& path,map<string,string>& key_pairs)
{
    ifstream in;
    in.open(path.refine_location_path.c_str());
    if(!in)
        return -1;
    map<string,string> data_set_face;
    map<string,string> data_set_eye;
    map<string,string> data_set_mouth;
    map<string,string> data_set_nose;

    string line;
    vector<string> result;
    result.resize(10);
    Rect rect;
    Mat img = imread(path.picture_path.c_str());
    int i=0;
    string key;
    while(!in.eof())
    {
        in>>line;
        parseFormat(line,result);
        rect = stringToRect(result);

        switch(i){
        case 0:
            if(!readPreFeatures(path.features_path+"face.dat",data_set_face))
               return -1;
            key = matchProcess(img(rect),data_set_face);
            key_pairs["face"] = key;
            break;
        case 1:
            if(!readPreFeatures(path.features_path+"eye.dat",data_set_eye))
               return -1;
            key = matchProcess(img(rect),data_set_eye);
            key_pairs["left_eye"] = key;
            break;
        case 2:
            if(!readPreFeatures(path.features_path+"eye.dat",data_set_eye))
               return -1;
            key = matchProcess(img(rect),data_set_eye);
            key_pairs["right_eye"] = key;
            break;
        case 3:
            if(!readPreFeatures(path.features_path+"mouth.dat",data_set_mouth))
               return -1;
            key = matchProcess(img(rect),data_set_mouth);
            key_pairs["mouth"] = key;
            break;
        case 4:
            if(!readPreFeatures(path.features_path+"nose.dat",data_set_nose))
               return -1;
            key = matchProcess(img(rect),data_set_nose);
            key_pairs["nose"] = key;
            break;
        default:
            break;
        }
    }
    in.close();
    return 0;
}
