#pragma once

#include "ofxCv.h"
using namespace cv;
using namespace ofxCv;

typedef vector<float> floats;

class TuringPattern {
public:
  floats activator, inhibitor, variations;
	floats tmp;
  int ar, ir, n;
	float ss;
	
	TuringPattern(int n, int ar, int ir, float ss) : 
	ar(ar), ir(ir), ss(ss), n(n) {
    activator.resize(n * n);
		inhibitor.resize(n * n);
    variations.resize(n * n);
		tmp.resize(n * n);
	}
	
  void step(floats& src) {
		Mat srcMat(n, n, CV_32FC1, &(src[0]));
		Mat activatorMat(n, n, CV_32FC1, &(activator[0]));
		Mat inhibitorMat(n, n, CV_32FC1, &(inhibitor[0]));
		Mat variationsMat(n, n, CV_32FC1, &(variations[0]));
		
		blur(srcMat, activatorMat, ar);
    blur(srcMat, inhibitorMat, ir);
		absdiff(activatorMat, inhibitorMat, variationsMat);
  }
	
  void blur(Mat& src, Mat& dst, int radius) {
		//cv::GaussianBlur(src, dst, cv::Size(radius * 2 + 1, radius * 2 + 1), 0, 0);
		cv::blur(src, dst, cv::Size(radius * 2 + 1, radius * 2 + 1));
		
		/*
		Mat tmpMat(n, n, CV_32FC1, &(tmp[0]));
		IplImage tmpIpl = tmpMat;
		IplImage srcIpl = src;
		IplImage dstIpl = dst;
		
		cvLogPolar(&srcIpl, &dstIpl, cvPoint2D32f(n / 2, n / 2), n / 6, CV_INTER_LINEAR);
		cv::blur(dst, tmpMat, cv::Size(radius * 2 + 1, radius * 2 + 1));
		cvLogPolar(&tmpIpl, &dstIpl, cvPoint2D32f(n / 2, n / 2), n / 6, CV_INTER_LINEAR + CV_WARP_INVERSE_MAP);
		*/
	}
};
