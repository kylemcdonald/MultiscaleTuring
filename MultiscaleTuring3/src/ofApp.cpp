#include "ofApp.h"

typedef vector<float> floats;
typedef vector<unsigned char> bytes;
typedef vector<bool> bools;

int n, side, levels;
floats grid;
floats diffusionLeft, diffusionRight, variation;
floats bestVariation;
bytes bestLevel;
bools direction;

vector<float> stepSizes;
vector<int> radii;
ofImage buffer;

void ofApp::setup() {
	// initialize settings
	side = 512;
	float base = ofRandom(1.5, 2.5);
	levels = logf(side) / logf(base);
	float stepScale = ofRandom(-.003, +.003) + .009;
	float stepOffset = ofRandom(-.003, +.003) + .008;
	
	// allocate space
	n = side * side;
	radii.resize(levels);
	stepSizes.resize(levels);
	grid.resize(n);
	diffusionLeft.resize(n);
	diffusionRight.resize(n);
	variation.resize(n);
	bestVariation.resize(n);
	bestLevel.resize(n);
	direction.resize(n);
	buffer.allocate(side, side, OF_IMAGE_GRAYSCALE);
	
	// determines the shape of the patterns
	for(int i = 0; i < levels; i++) {
		int radius = (int) powf(base, i);
		radii[i] = radius;
		float diameterRatio = (float) ((2 * radius) + 1) / side;
		cout << i << ":" << (int) (100 * diameterRatio) << "%" << endl;
		stepSizes[i] = log(radius) * stepScale + stepOffset;
	}
  
	// initialize the grid with noise
  for (int i = 0; i < n; i++) {
		grid[i] = ofRandom(-1, +1);
  }
	// initialize with a few pixels
	for(int i = 0; i < 8; i++) {
		grid[ofRandom(0, side) * side + ofRandom(0, side)] = ofRandom(-.01, +.01);
	}
}

void ofApp::update() {
	floats* activator = &grid;
	floats* inhibitor = &diffusionRight;
	
	Mat variationMat(side, side, CV_32FC1, &(variation[0]));
	
	for(int level = 0; level < levels - 1; level++) {
		Mat activatorMat(side, side, CV_32FC1, &((*activator)[0]));
		Mat inhibitorMat(side, side, CV_32FC1, &((*inhibitor)[0]));
		
		// blur activator into inhibitor
		int radius = radii[level];
		cv::Size kernelSize(radius * 2 + 1, radius * 2 + 1);
		cv::blur(activatorMat, inhibitorMat, kernelSize);
		//cv::GaussianBlur(activatorMat, inhibitorMat, kernelSize, 0);
		
		// absdiff between activator and inhibitor
		cv::absdiff(activatorMat, inhibitorMat, variationMat);
		
		if(level == 0) {
			// save bestLevel and bestVariation
			for(int i = 0; i < n; i++) {
				bestVariation[i] = variation[i];
				bestLevel[i] = level;
				direction[i] = (*activator)[i] > (*inhibitor)[i];
			}
			activator = &diffusionRight;
			inhibitor = &diffusionLeft;
		} else {
			// check/save bestLevel and bestVariation
			for(int i = 0; i < n; i++) {
				if(variation[i] < bestVariation[i]) {
					bestVariation[i] = variation[i];
					bestLevel[i] = level;
					direction[i] = (*activator)[i] > (*inhibitor)[i];
				}
			}
			swap(activator, inhibitor);
		}
	}
	
	// update grid from bestLevel
	float smallest = 10;
	float largest = -10;
	for(int i = 0; i < n; i++) {
		float curStep = stepSizes[bestLevel[i]];
		if(direction[i]) {
			grid[i] += curStep;
		} else {
			grid[i] -= curStep;
		}
		smallest = min(smallest, grid[i]);
		largest = max(largest, grid[i]);
	}
	
	// normalize to [-1, +1]
	float range = (largest - smallest) / 2;
	for (int i = 0; i < n; i++) {
		grid[i] = ((grid[i] - smallest) / range) - 1;
	}
}

template <class T>
void drawBuffer(vector<T>& grid) {
	unsigned char* pixels = buffer.getPixels();
  for (int i = 0; i < n; i++) {
		pixels[i] = 128 + 128 * grid[i];
  }	
	buffer.update();
	buffer.draw(0, 0);//, ofGetWidth(), ofGetHeight());
}

void ofApp::draw() {
	drawBuffer(grid);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}

void ofApp::mousePressed(int x, int y, int button) {
	setup();
}

void ofApp::keyPressed(int key) {
	if(key == ' ') {
		buffer.saveImage(ofToString(ofGetFrameNum()) + ".png");
	}
}