// CNNTest.cpp : Defines the entry point for the console application.
//

#include <math.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <time.h>
#include <dirent.h>
using namespace std;


#include <opencv2/opencv.hpp>
using namespace cv;

//#include "../src/face_common.h"
#include "face_cartoon_api.h"
using namespace face_360;



void draw_link_points(cv::Mat &img, std::vector<float> &pts, cv::Scalar cor)
{
	int n = pts.size();
        //cout << "draw points num = " << n/2 << endl;
	for(int i=0; i<n/2; i++) {
        cv::circle(img, cv::Point(pts[i*2+0], pts[i*2+1]), 1, cor, 1);
	}	
	for(int i=1; i<n/2; i++) {
		;//cv::line(img, cv::Point(pts[i*2-2], pts[i*2-1]), cv::Point(pts[i*2+0], pts[i*2+1]), cor);
	}	
	//cv::line(img, cv::Point(pts[0], pts[1]), cv::Point(pts[n-2], pts[n-1]), cor);
}

int test_api_1_pic(const char *picfile,const char *savefile,face_360::FacePoints& pts,double scale)
{
	string pic_file  = picfile;
    string save_file = savefile;
	Mat imgc = imread(pic_file, 1), rgb;
	Mat imgg = imread(pic_file, 0), gray;

	if(scale < 1) {
		cv::Size dsize(imgg.cols * scale, imgg.rows * scale);
		resize(imgg, gray, dsize);
		resize(imgc, rgb, dsize);
	} else {
		gray = imgg;
		rgb = imgc;
	}


	face_360::FaceAPI api;
    if (api.init("/home/daoming/faceapi/faceapi/face-api-0506/data") != Face360Error_OK) {
                cout << "init data error! " << endl;
		return -1;
	}
	face_360::FaceDetectParam param = api.get_face_detect_param();
	param.detect_roll_face = true;
	//param.detect_profile_face = true;
	param.detect_horizontal_face = true;
	api.set_face_detect_param(param);

	vector<face_360::FaceRect> rects;
    clock_t t1 = clock();
    if (api.set_image((char *)gray.data, gray.cols, gray.rows, gray.step) != Face360Error_OK)
    {
        cout << "face detect error! " << endl;
		return -1;
	}
    if (api.detect(rects) != Face360Error_OK)
    {
        cout << "face detect error! " << endl;
        return -1;
    }
    clock_t t2 = clock();
    double time1 = ((double) (t2 - t1))/CLOCKS_PER_SEC;
    cout << "detect time = " << time1 << ", face number = " << rects.size() << endl;

    t1 = clock();

	char buf[1024];
	for (int i = 0; i < rects.size(); ++i) {
		Rect recti(rects[i].left, rects[i].top, rects[i].right - rects[i].left, rects[i].bottom - rects[i].top);
                
        t1 = clock();
		face_360::FacePose pose;
		if (api.get_pose(rects[i], pose) != Face360Error_OK) {
            cout << "get pose error!" << endl;
			return -1;
		}
        t2 = clock();
        cout << "pose time = " << ((double) (t2 - t1))/CLOCKS_PER_SEC << endl;

        //rectangle(rgb, recti, Scalar(0, 255, 0), 1);
		
		if(rects[i].direction == face_360::FaceDirectionVertical)
			sprintf(buf, "V[%0.2f,%0.2f,%0.2f]", pose.yaw, pose.pitch, pose.roll);
		if(rects[i].direction == face_360::FaceDirectionLeft)
			sprintf(buf, "L[%0.2f,%0.2f,%0.2f]", pose.yaw, pose.pitch, pose.roll);
		if(rects[i].direction == face_360::FaceDirectionRight)
			sprintf(buf, "R[%0.2f,%0.2f,%0.2f]", pose.yaw, pose.pitch, pose.roll);
		//cv::putText(rgb, buf, cv::Point(rects[i].left, rects[i].top), 0, 0.4, cv::Scalar(110,255,0),1,CV_AA);
		for(int j=0; j<5; j++) {
			;//cv::circle(rgb, cv::Point(pose.pt5[j*2+0], pose.pt5[j*2+1]), 1, cv::Scalar(0,255,255), 1);
		}

        t1 = clock();
        //face_360::FacePoints pts;
		if (api.get_face_points(rects[i], pose, pts) != Face360Error_OK){
                        cout << "get points error!" << endl;
			return -1;
		}
        t2 = clock();
        cout << "face points time = " << ((double) (t2 - t1))/CLOCKS_PER_SEC << endl;
        /*
        t1 = clock();
        for(int j=0; j<10; j++) {
            if (api.get_face_points(rects[i], pose, pts) != Face360Error_OK){
                   cout << "get points error!" << endl;
                   return -1;
            }
		}
        t2 = clock();
        cout << "face points 10 faces time = " << ((double) (t2 - t1))/CLOCKS_PER_SEC << endl;
         */
		draw_link_points(rgb, pts.face_shape_19, cv::Scalar(255,0,0));
		draw_link_points(rgb, pts.left_brow_10, cv::Scalar(255,0,0));
		draw_link_points(rgb, pts.right_brow_10, cv::Scalar(255,0,0));
		draw_link_points(rgb, pts.left_eye_12, cv::Scalar(255,0,0));
		draw_link_points(rgb, pts.right_eye_12, cv::Scalar(255,0,0));
		draw_link_points(rgb, pts.nose_12, cv::Scalar(255,0,0));
		draw_link_points(rgb, pts.mouse_inside_8, cv::Scalar(255,0,0));
		draw_link_points(rgb, pts.mouse_outside_12, cv::Scalar(255,0,0));
		for(int j=0; j<5; j++) {
			cv::circle(rgb, cv::Point(pts.key_point_5[j*2+0], pts.key_point_5[j*2+1]), 1, cv::Scalar(0,0,255), 1);
		}
			
	}
        imwrite(save_file, rgb);
	//imshow("fd", rgb);
	//waitKey(0);
        cout << "done!" << endl;
	return 0;
}

int test_dir(char *testdir, char *savedir)
{
    string test_dir = testdir;
    string save_dir = savedir;
    
    DIR *dir;
    struct dirent entry;
    struct dirent *res;
    int return_code;

    face_360::FaceAPI api;
    if (api.init("../../data") != Face360Error_OK) {
            cout << "init data error! " << endl;
            return -1;
    }
    face_360::FaceDetectParam param = api.get_face_detect_param();
    param.detect_roll_face = true;
    //param.detect_profile_face = true;
    //param.detect_horizontal_face = true;
    api.set_face_detect_param(param);

    if ((dir = opendir(test_dir.c_str())) == NULL) {
        cout << "test dir fail: " << test_dir << endl;
        return -1;
    }
        
    int pic_num = 0;
    for (return_code = readdir_r(dir, &entry, &res);
                res != NULL && return_code == 0;
                return_code = readdir_r(dir, &entry, &res)){
        if (entry.d_type == DT_DIR) continue;

        string pic_file  = test_dir + "/" + entry.d_name;
        string save_file = save_dir + "/" + entry.d_name;

        Mat imgc = imread(pic_file, 1), rgb;
        Mat imgg = imread(pic_file, 0), gray;

        int max_wid = 700;
        double scale = max_wid * 1.0 / max(imgg.rows, imgg.cols);
        if(scale < 1) {
                cv::Size dsize(imgg.cols * scale, imgg.rows * scale);
                resize(imgg, gray, dsize);
                resize(imgc, rgb, dsize);
        } else {
                gray = imgg;
                rgb = imgc;
        }

        vector<face_360::FaceRect> rects;
        if (api.set_image((char *)gray.data, gray.cols, gray.rows, gray.step) != Face360Error_OK
	    || api.detect(rects) != Face360Error_OK) {
                cout << "detect error: " << pic_file << endl;
                continue;
        }

        for (int i = 0; i < rects.size(); ++i) {
            Rect recti(rects[i].left, rects[i].top, rects[i].right - rects[i].left, rects[i].bottom - rects[i].top);

            face_360::FacePose pose;
            if (api.get_pose(rects[i], pose) != Face360Error_OK) {
                cout << "pose error: " << pic_file << endl;
                   continue;
            }

            face_360::FacePoints pts;
            if (api.get_face_points(rects[i], pose, pts) != Face360Error_OK){
                 cout << "points error: " << pic_file << endl;
                    continue;
            }

            rectangle(rgb, recti, Scalar(0, 255, 0), 1);
            draw_link_points(rgb, pts.face_shape_19, cv::Scalar(255,0,0));
            draw_link_points(rgb, pts.left_brow_10, cv::Scalar(255,0,0));
            draw_link_points(rgb, pts.right_brow_10, cv::Scalar(255,0,0));
            draw_link_points(rgb, pts.left_eye_12, cv::Scalar(255,0,0));
            draw_link_points(rgb, pts.right_eye_12, cv::Scalar(255,0,0));
            draw_link_points(rgb, pts.nose_12, cv::Scalar(255,0,0));
            draw_link_points(rgb, pts.mouse_inside_8, cv::Scalar(255,0,0));
            draw_link_points(rgb, pts.mouse_outside_12, cv::Scalar(255,0,0));
            for(int j=0; j<5; j++) {
               cv::circle(rgb, cv::Point(pts.key_point_5[j*2+0], pts.key_point_5[j*2+1]), 1, cv::Scalar(0,0,255)
, 1);
            }
		/*if(0 == i) {
			cerr << "{\"rect\":\"" << rects[i].left << "," << rects[i].top << "," << rects[i].right << "," << rects[i].bottom << "\"";
			cerr << ",";
			cerr << "\"width\":\"" << rgb.cols << "\",\"height\":\"" << rgb.rows << "\",";
			std::vector<float> ppts = pts.face_shape_19;
			cerr << "\"face_shape\":\"";
			for(int m = 0; m < 19; m++) {
				if(18 == m)
					cerr << ppts[2*m+0] << "," << ppts[2*m+1] << "\"";
				else
					cerr << ppts[2*m+0] << "," << ppts[2*m+1] << ",";
			}
			cerr << ",";
			ppts = pts.left_brow_10;
			cerr << "\"left_brow\":\"";
			for(int m = 0; m < 10; m++) {
				if(9 == m)
                                        cerr << ppts[2*m+0] << "," << ppts[2*m+1] << "\"";
                                else
					cerr << ppts[2*m+0] << "," << ppts[2*m+1] << ",";
			}
			cerr << ",";
			ppts = pts.right_brow_10;
			cerr << "\"right_brow\":\"";
			for(int m = 0; m < 10; m++) {
				if(9 == m)
                                        cerr << ppts[2*m+0] << "," << ppts[2*m+1] << "\"";
                                else
					cerr << ppts[2*m+0] << "," << ppts[2*m+1] << ",";
			}
			cerr << ",";
			ppts = pts.left_eye_12;
			cerr << "\"left_eye\":\"";
			for(int m = 0; m < 12; m++) {
				if(11 == m)
                                        cerr << ppts[2*m+0] << "," << ppts[2*m+1] << "\"";
                                else
					cerr << ppts[2*m+0] << "," << ppts[2*m+1] << ",";
			}
			cerr << ",";
			ppts = pts.right_eye_12;
			cerr << "\"right_eye\":\"";
			for(int m = 0; m < 12; m++) {
				if(11 == m)
                                        cerr << ppts[2*m+0] << "," << ppts[2*m+1] << "\"";
                                else
					cerr << ppts[2*m+0] << "," << ppts[2*m+1] << ",";
			}
			cerr << ",";
			ppts = pts.nose_12;
			cerr << "\"nose\":\"";
			for(int m = 0; m < 12; m++) {
				if(11 == m)
                                        cerr << ppts[2*m+0] << "," << ppts[2*m+1] << "\"";
                                else
					cerr << ppts[2*m+0] << "," << ppts[2*m+1] << ",";
			}
			cerr << ",";
			ppts = pts.mouse_inside_8;
			cerr << "\"mouse_inside\":\"";
			for(int m = 0; m < 8; m++) {
				if(7 == m)
                                        cerr << ppts[2*m+0] << "," << ppts[2*m+1] << "\"";
                                else
					cerr << ppts[2*m+0] << "," << ppts[2*m+1] << ",";
			}
			cerr << ",";
			ppts = pts.mouse_outside_12;
			cerr << "\"mouse_outside\":\"";
			for(int m = 0; m < 12; m++) {
				if(11 == m)
                                        cerr << ppts[2*m+0] << "," << ppts[2*m+1] << "\"";
                                else
					cerr << ppts[2*m+0] << "," << ppts[2*m+1] << ",";
			}
			cerr << ",";
			ppts = pts.key_point_5;
			cerr << "\"key_point\":\"";
			for(int m = 0; m < 5; m++) {
				if(4 == m)
                                        cerr << ppts[2*m+0] << "," << ppts[2*m+1] << "\"";
                                else
					cerr << ppts[2*m+0] << "," << ppts[2*m+1] << ",";
			}
			cerr << "}" << endl;
		}*/
        } // end of each faces in 1 pic
        pic_num ++;
        cout << "pic num: " << pic_num << endl;
        if(pic_num % 100 == 0) cout << "pic num:" << pic_num << endl;
        imwrite(save_file, rgb);
    } // end of each pics

    cout << endl << "all done, pic num: " << pic_num << endl;
    return 0;
}



