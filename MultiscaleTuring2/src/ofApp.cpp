#include "ofApp.h"

typedef vector<float> floats;

floats grid;
int num, levels;
vector<floats> diffusion;
vector<float> stepSizes;
vector<int> radii;

ofImage buffer;

void updateGridFromDiffusion() {
  float smallest = 10;
  float largest = -10;
	int n = num * num;
  for (int i = 0; i < n; i++) {
		float bestVariation = 0;
		int bestLevel = 0;
		for(int level = 0; level < diffusion.size() - 1; level++) {
			float curVariation = fabsf(diffusion[level][i] - diffusion[level + 1][i]);
			if(level == 0 || curVariation < bestVariation) {
				bestLevel = level;
				bestVariation = curVariation;
			}
		}
		float curActivator = diffusion[bestLevel][i];
		float curInhibitor = diffusion[bestLevel + 1][i];
		if(curActivator > curInhibitor) {
			grid[i] += stepSizes[bestLevel];
		}	else {
			grid[i] -= stepSizes[bestLevel]; 
		}
		
		largest = max(largest, grid[i]);
		smallest = min(smallest, grid[i]);
	}
	float range = (largest - smallest) / 2;
	for (int i = 0; i < n; i++) {
		grid[i] = ((grid[i] - smallest) / range) - 1;
	}
}

void updateDiffusion() {
	Mat srcMat(num, num, CV_32FC1, &(grid[0]));
	for(int i = 0; i < diffusion.size(); i++) {
		Mat curMat(num, num, CV_32FC1, &(diffusion[i][0]));
		int radius = radii[i];
		cv::blur(i == 0 ? srcMat : Mat(num, num, CV_32FC1, &(diffusion[i - 1][0])),
						 curMat, cv::Size(radius * 2 + 1, radius * 2 + 1));
	}
}

void ofApp::setup() {
  num=4096;
	int n = num * num;
	
	levels = 9;
	radii.resize(levels);
	stepSizes.resize(levels);
	diffusion.resize(levels, floats(n));
	float scale = .009;
	float base = .008;
	for(int i = 0; i < levels; i++) {
		int radius = (int) powf(2.5, i);
		radii[i] = (radius);
		stepSizes[i] = log(radius) * scale + base;
	}
  
  grid.resize(n);
  for (int i = 0; i < n; i++) {
		grid[i] = ofRandom(-1, 1);
  }
	
	buffer.allocate(num, num, OF_IMAGE_GRAYSCALE);
}

void ofApp::update() {
	updateDiffusion();
	updateGridFromDiffusion();
}

void drawBuffer(floats& grid) {
	unsigned char* pixels = buffer.getPixels();
	int n = num * num;
  for (int i = 0; i < n; i++) {
		pixels[i] = 128 + 100*grid[i];
  }	
	buffer.update();
	buffer.draw(0, 0);
}

void ofApp::draw() {
	drawBuffer(grid);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
	
	ofTranslate(0, 256);
	/*
	for(int i = 0; i < diffusion.size(); i++) {
		drawBuffer(diffusion[i]);
		ofTranslate(num, 0);
	}*/
}

void ofApp::mousePressed(int x, int y, int button) {
	buffer.saveImage(ofToString(ofGetFrameNum()) + ".png");
	setup();
}

void ofApp::mouseMoved(int x, int y) {
	/*
	 for(int i = 0; i < levels; i++) {
	 int radius = (int) powf(ofMap(x, 0, ofGetWidth(), 1.5, 4), i);
	 radii[i] = (radius);
	 }
	 */
}