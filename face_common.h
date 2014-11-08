
#ifndef __FACE_COMMON_360_H__
#define __FACE_COMMON_360_H__

#include <vector>
using namespace std;

namespace face_360
{
	
struct FaceDetectParam
{
	bool detect_roll_face;		// true表示检测旋转30度左右的脸，但是检测时间会翻倍
	bool detect_profile_face;	// true表示检测侧脸，但是检测时间会翻倍
	bool detect_horizontal_face;// true表示检测横着的脸，但是检测时间会增加2倍
	
	double min_face_ratio;	// 最小检测到的人脸宽度与图片最短边长的比例，必须小于1
	double scale_step;	// 检测框缩放步长，必须大于1，越接近1速度越慢，精度越高
	int stride_step;	// 检测框移动步长，必须大于等于1，越接近1速度越慢，精度越高

	FaceDetectParam():detect_roll_face(false),
		detect_profile_face(false), detect_horizontal_face(false),
		min_face_ratio(0.02), scale_step(1.15), stride_step(2){}
};

enum FaceDirection
{
	FaceDirectionVertical = 0,
	FaceDirectionLeft = 1,
	FaceDirectionRight = 2
};

struct FaceRect{
	int left, right, top, bottom;	// 人脸位置
	float confidence;				// 人脸置信度
	FaceDirection direction;		// 头的朝向
};

struct FacePose{
	float pt5[10];
	float yaw, pitch, roll;
};

struct FaceGender{
	int is_male;
	float confidence;
};

struct FaceGlass{
	int have_glass;
	float confidence;
};

struct FaceSmile{
	int is_simling;
	float confidence;
};

struct FacePoints{
	vector<float> key_point_5;	
	vector<float> face_shape_19;
	vector<float> left_brow_10;
	vector<float> right_brow_10;
	vector<float> left_eye_12;
	vector<float> right_eye_12;
	vector<float> nose_12;
	vector<float> mouse_outside_12;
	vector<float> mouse_inside_8;
        vector<float> all_points_96;
};

enum Face360Error
{
	Face360Error_OK = 0,
	Face360Error_ModelError = -1,
	Face360Error_UnvalidatedInput = -10,
	Face360Error_UnvalidatedImage = -11,
	Face360Error_Error = -100
};

}
#endif
