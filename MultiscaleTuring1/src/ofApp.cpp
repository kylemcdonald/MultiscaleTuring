#include "ofApp.h"

typedef vector<float> floats;

floats grid;
int num;
vector<TuringPattern> patterns;
ofImage buffer;

void updateGrid() {
  float smallest=10;
  float largest=-10;
	int n = num * num;
  for (int i = 0; i < n; i++) {
		// this is the magic
		// use the stepsize from the level with the least variation
		// in other words: take the scale at which the current value is most representative
		// and walk a similar amount much in that direction. 
		float bestvariation=patterns[0].variations[i];
		int bestlevel=0;
		for (int k=1;k<patterns.size();k++) {
			if ((patterns[k].variations[i]<bestvariation)) {
				bestlevel = k;
				bestvariation = patterns[k].variations[i];
			}
		}
		if (patterns[bestlevel].activator[i]>patterns[bestlevel].inhibitor[i]) {
			grid[i]+=patterns[bestlevel].ss; 
			//grid[i]=+patterns[bestlevel].ss; 
		}
		else {
			grid[i]-=patterns[bestlevel].ss; 
			//grid[i]=-patterns[bestlevel].ss; 
		}
		
		largest=max(largest, grid[i]);
		smallest=min(smallest, grid[i]);
	}
	// map everything from min/max onto -1 to +1
	float range=0.5*(largest-smallest);
	for (int i=0;i<n;i++) {
		grid[i]=(grid[i]-smallest)/range-1;
	}
}

void step() {
  for (int i = 0; i < patterns.size(); i++) {
    patterns[i].step(grid);
  }
  updateGrid();
}

void ofApp::setup() {
  num=512;
	int n = num * num;
	
	float scale = .009;
	float base = .008;
	patterns.clear();
	for(int i = 0; i < 6; i++) {
		int baseSize = (int) powf(2.5, i);
		patterns.push_back(TuringPattern(num, baseSize, baseSize * 2, log(baseSize) * scale + base));
	}
  
  grid.resize(n);
  for (int i=0;i<n;i++) {
		grid[i]=ofRandom(-1, 1);
  }
	
	buffer.allocate(num, num, OF_IMAGE_GRAYSCALE);
}

void ofApp::update() {
	step();
	
	unsigned char* pixels = buffer.getPixels();
	int n = num * num;
  for (int i = 0; i < n; i++) {
		pixels[i] = 128 + 100*grid[i];
  }	
	buffer.update();
}

void ofApp::draw() {
	buffer.draw(0, 0);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}

void ofApp::mousePressed(int x, int y, int button) {
	//buffer.saveImage(ofToString(ofGetFrameNum()) + ".png");
	setup();
}