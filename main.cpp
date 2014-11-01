#include "functions.h"
using namespace std;
using namespace cv;
int main(int argc, char**argv)
{
    //this will replace by the qt;
    Mat src = imread("/home/daoming/Desktop/test.jpg");
    //imshow("src",src);
    //waitKey(0);

    //
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
   /*
    Mat test_face = imread("/home/daoming/Desktop/face/womanface1.png");
    string test_face_hash = pHashValue(test_face);
    ofstream out;
    out.open("/home/daoming/Desktop/1.dat",ios::out);
    out<<"womanface1"<<","<<test_face_hash;
    out.close();*/
    //detect faces
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
        rectangle(src_gray,*iter,Scalar( 255, 0, 255 ));
        rectangle(src_gray,*(iter+1),Scalar( 255, 0, 255 ));
        imshow("src_gray",src_gray);
        waitKey(0);
        //eyes region
        eyes_cascade.detectMultiScale(face_roi,eyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(20, 20));
        //erase the faked face
        if(!refineface(*iter,eyes,20))
            continue;
        //mouth region
        mouth_cascade.detectMultiScale(face_roi,mouths, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));

        //nose region
        nose_cascade.detectMultiScale(face_roi,noses, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));

        refineOrgans(eyes,mouths,noses);
        rectangle(face_roi,noses[0],Scalar( 255, 0, 255 ));
        rectangle(face_roi,mouths[0],Scalar( 255, 0, 255 ));
        imshow("Detec_face1",face_roi);
        waitKey(0);
    }
    DataPath path;
    Mat edge_test;
    iter = fix_roi.begin();

    //getSkeletonOfRoi(src_gray(*iter),edge_test);
    //imshow("src",edge_test);
    //waitKey(0);
    //read
    //use the fake data
    map<string,string> data_set;
    readPreFeatures(path.features_path,data_set);
    //ofstream in;
    //in.open();
    //data_set["womanface1"] = test_face_hash;

    //get the best match;
    string cartoon_face;
    cartoon_face = matchProcess(face_roi,data_set);

    /*
    //alignment
    resize(test_face,test_face,face_roi.size());

    Mat test_eye = imread("/home/daoming/Desktop/organs/eye_big1_pencil.png");
    resize(test_eye,test_eye,Size(eyes[0].width,eyes[0].height));
    partMosaic(test_face,test_eye,eyes[0]);*/

    cout << "Hello World!" << endl;
    return 0;
}
