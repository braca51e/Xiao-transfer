#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string.h>
#include <vector>
#include <math.h>
#include <conio.h>
#include <iostream>

using namespace cv;

#define WND_NAME_RES "Result Picture"
#define WND_NAME_SOURCE "Source Picture"

struct ct_image
{
	std::string source;
	std::string target;
	std::string result;
};

ct_image images[] = {
					{"images/1/img_1.jpg", "images/1/img_1.jpg", "images/1/img_1_2_cv.jpg"}
					};

bool makeCTXiao(ct_image images);
void GetTRS(Mat input, Mat& T, Mat& R, Mat& S);
void GetSRT(Mat input, Mat& T, Mat& R, Mat& S);
Mat AddChannel(Mat mat);
Mat RemoveChannel(Mat mat);

int main()
{
	unsigned img_pack = 0;
	makeCTXiao(images[img_pack]);
	/*if(makeCTXiao(images[img_pack]))
	{
		Mat res_pic = imread(images[img_pack].result);
		imshow(WND_NAME_RES, res_pic);
		waitKey(0);
	}*/
	_getch();
	return 0;
}
bool makeCTXiao(ct_image images)
{
	Mat imgs = imread(images.source);
	Mat imgt = imread(images.target);
	imgs.convertTo(imgs, CV_32FC3, 1 / 255.0);
	imgt.convertTo(imgt, CV_32FC3, 1 / 255.0);
	Mat src_T, src_R, src_S;
	Mat tar_T, tar_R, tar_S;
	GetTRS(imgs, src_T, src_R, src_S);
	GetSRT(imgt, tar_T, tar_R, tar_S);
	Mat mega = src_T * src_R * src_S * tar_S * tar_R * tar_T; // with same image it should be eye;
	std::cout << "Mega\n" << mega << std::endl;
	std::cout << "src_T\n" << src_T << std::endl;
	std::cout << "src_R\n" << src_R << std::endl;
	std::cout << "src_S\n" << src_S << std::endl;

	std::cout << "tar_S\n" << tar_S << std::endl;
	std::cout << "tar_R\n" << tar_R << std::endl;
	std::cout << "tar_T\n" << tar_T << std::endl;

	Mat img_4 = AddChannel(imgt);
	std::cout << img_4.at<Vec4f>(0, 0) << std::endl;
	imshow(WND_NAME_RES, img_4);
	waitKey(0);
	Mat result;
	transform(img_4, result, mega);
	std::cout << result.at<Vec4f>(0, 0) << std::endl; // enormous values
	imshow(WND_NAME_RES, result);
	waitKey(0);
	/*
	//imwrite(images.result, result);
	imgt.convertTo(imgt, CV_8UC3);
	imshow(WND_NAME_RES, img_4);
	waitKey(0);
	//result.convertTo(result, CV_8UC3);
	imshow(WND_NAME_RES, result);
	waitKey(0);*/
	return true;
}
void GetTRS(Mat input, Mat& T, Mat& R, Mat& S)
{
	Mat cov, means;
	calcCovarMatrix(input.reshape(1, input.cols * input.rows), cov, means, CV_COVAR_NORMAL | CV_COVAR_ROWS, CV_32F);
	Mat U, A, VT;
	SVD::compute(cov, A, U, VT);
	T = Mat::eye(4, 4, CV_32FC1);
	R = Mat::eye(4, 4, CV_32FC1);
	S = Mat::eye(4, 4, CV_32FC1);

	Rect roi(0, 0, 3, 3);
	U.copyTo(R(roi));
	
	T.at<float>(0, 3) = means.at<float>(0, 0);
	T.at<float>(1, 3) = means.at<float>(0, 1);
	T.at<float>(2, 3) = means.at<float>(0, 2);

	S.at<float>(0, 0) = A.at<float>(0, 0);
	S.at<float>(1, 1) = A.at<float>(1, 0);
	S.at<float>(2, 2) = A.at<float>(2, 0);
}
void GetSRT(Mat input, Mat& T, Mat& R, Mat& S)
{
	Mat cov, means;
	calcCovarMatrix(input.reshape(1, input.cols * input.rows), cov, means, CV_COVAR_NORMAL | CV_COVAR_ROWS, CV_32F);
	Mat U, A, VT;
	SVD::compute(cov, A, U, VT);
	T = Mat::eye(4, 4, CV_32FC1);
	R = Mat::eye(4, 4, CV_32FC1);
	S = Mat::eye(4, 4, CV_32FC1);
	Rect roi(0, 0, 3, 3);
	invert(U, R(roi));

	T.at<float>(0, 3) = -means.at<float>(0, 0);
	T.at<float>(1, 3) = -means.at<float>(0, 1);
	T.at<float>(2, 3) = -means.at<float>(0, 2);

	S.at<float>(0, 0) = 1/sqrt(A.at<float>(0, 0));
	S.at<float>(1, 1) = 1/sqrt(A.at<float>(1, 0));
	S.at<float>(2, 2) = 1/sqrt(A.at<float>(2, 0));
}
Mat AddChannel(Mat mat)
{
	Mat img = Mat::ones(mat.size(), CV_32FC4);
	int from_to[] = {0,0, 1,1, 2,2};
	mixChannels(mat, img, from_to, 3);
	return img; 
}
Mat RemoveChannel(Mat mat)
{
	std::vector<Mat> channels;
	split(mat, channels);
	channels.resize(3);
	Mat img;
	merge(channels, img);
	return img;
}