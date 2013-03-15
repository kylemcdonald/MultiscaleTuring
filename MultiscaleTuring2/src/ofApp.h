#pragma once

#include "ofMain.h"
#include "ofxCv.h"
using namespace cv;

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void mousePressed(int x, int y, int button);
	void mouseMoved(int x, int y);
};
