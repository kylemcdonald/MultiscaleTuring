#pragma once

#include "ofMain.h"
#include "ofxCv.h"
using namespace cv;

typedef vector<float> floats;
typedef vector<unsigned char> bytes;
typedef vector<bool> bools;

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void drawBuffer(floats& grid, int offset);
	void mousePressed(int x, int y, int button);
	void keyPressed(int key);
	
	ofEasyCam cam;
};
