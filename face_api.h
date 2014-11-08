
#pragma once 
#ifndef __FACE_API_360_H__
#define __FACE_API_360_H__
  

#ifdef DLL_IMPLEMENT  
#define DLL_API __declspec(dllexport)  
#else  
#define DLL_API __declspec(dllimport)  
#endif  
  

#include <vector>
#include "face_common.h"
using namespace std;

namespace face_360
{

class FaceAPI{
public:
	FaceAPI();
	~FaceAPI();

	// 初始化所有模型的路径，只需调用一次
	Face360Error init(const char *model_dir);

	// 设置图像，每次检测新图像的时候先调用一次
	Face360Error set_image(const char *img_data, int width, int height, int width_step);

	// 人脸检测部分
	FaceDetectParam get_face_detect_param();
	Face360Error set_face_detect_param(FaceDetectParam param);
	Face360Error detect(vector<FaceRect> &rect_vec);
	
	// 人脸属性部分
	Face360Error get_pose(FaceRect rect, FacePose &pose);
	Face360Error get_gender(FaceRect rect, FaceGender &gender);
	//bool get_glass(FaceRect rect, FaceGlass &glass);
	//bool get_smile(FaceRect rect, FaceSmile &smile);
	Face360Error get_face_points(FaceRect rect, FacePose pose, FacePoints &points);

	// 人脸特征部分

private:
	void *impl_;
};


}
#endif
