#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/superres/optical_flow.hpp>


//取得するフレーム数-1(差分をとるから)
#define f_num 60
/*
//オプティカルフローのベクトルの絶対値の閾値(0〜255)
#define vec_th 120
//人と認識する面積の閾値(オーダーよくわからん)
#define area 200
//背景と人を区別するオプティカルフローの頻度(フレーム数に対する％)
#define freq 15
*/
//imshowのon/off 0でオフ、1でオン
#define flag 0

using namespace cv;
using namespace cv::superres;
using namespace std;

int main(int argc, char *argv[]){
  VideoCapture cap;
	cap.open(1);
  int vec_th = stoi(argv[1]);
  int area = stoi(argv[2]);
  int freq = stoi(argv[3]);
 //f_num+1フレームをbuff.aviとしてローカルに保存
	VideoWriter output;
	output.open("buff.avi",CV_FOURCC('M','J','P','G'),30,cv::Size(640,480));
	int k = 0;
	Mat frame;
	while(k < f_num+1){
		cap >> frame;
		if(frame.empty()){
			break;
		}
		output << frame;
		k++;
	}
	output.release();
	cap.release();

	// 動画ファイルの読み込み
	VideoCapture capture = VideoCapture("./buff.avi");
	//オプティカルフロー計算オブジェクトの生成
	Ptr<DenseOpticalFlowExt> opticalFlow = superres::createOptFlow_Farneback();

	// 前のフレームを保存しておく
	Mat prev;
	capture >> prev;
	Size s = prev.size();

	//オプティカルフローのベクトルを保管する二次配列
	int mat[s.height][s.width];
	for(int i=0;i<s.height;i++){
		for(int j=0;j<s.width;j++){
			mat[i][j] = 0;
		}
	}

	for(int k=0;k<f_num;k++){
		// 現在のフレームを保存
		Mat curr;
		capture >> curr;

		// オプティカルフローの計算
		Mat flowX, flowY;
		opticalFlow->calc(prev, curr, flowX, flowY);

		//  オプティカルフローを極座標に変換（角度は[deg]）
		Mat magnitude, angle;
		cartToPolar(flowX, flowY, magnitude, angle, true);

		//オプティカルフローの絶対値を0~255に規格化
		Mat absflow;
		normalize(magnitude, absflow, 0, 255, NORM_MINMAX);
		//閾値vec_thで2値画像に変換
		threshold(absflow,absflow,vec_th,255,CV_THRESH_BINARY);
		absflow.convertTo(absflow, CV_8UC1);

		// 表示
		#if flag
		imshow("input", curr);
		imshow("flow",absflow);
		waitKey(1);
		#endif

		//オプティカルフローベクトルの保管
		for(int i=0;i<s.height;i++){
			unsigned char *ptr;
			ptr = absflow.ptr<uchar>(i);
			for(int j=0;j<s.width;j++){
				double r = ((double)ptr[0]);
				if(r > 0){
					mat[i][j]++;
				}
				ptr++;
			}
		}

		// 前のフレームを保存
		prev = curr;
	}
	//f_numフレーム分のオプティカルフローで出現度が高いものだけ抽出
	Mat filter = Mat((int)(s.height),(int)(s.width),CV_8UC1);
	for(int i=0;i<s.height;i++){
		unsigned char *ptr;
		ptr = filter.ptr<uchar>(i);
		for(int j=0;j<s.width;j++){
			if(mat[i][j] >= f_num*freq*1.0/100){
				*ptr = (uchar)(255);
			}else{
				*ptr = (uchar)(0);
			}
			ptr++;
		}
	}

	#if flag
	imshow("filter",filter);
	#endif

	//２値画像から領域抽出
	vector<vector<Point> > contours;
	findContours(filter, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	int count = 0;
	for(int j=0;j<contours.size();j++){
		if(contourArea(contours.at(j)) > area){
			count++;
		}
	}
	cout  << count << endl;
	remove("buff.avi");
	#if flag
		waitKey(-1);
	#endif
	return 0;
}
