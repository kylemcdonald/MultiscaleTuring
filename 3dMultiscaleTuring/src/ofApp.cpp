#include "ofApp.h"

bool drawTiles = false;

int n, wh, side, levels;
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
  side = 64;
	float base = 2;
	levels = logf(side) / logf(base);
	float stepScale = .09;
	float stepOffset = .008;
	
	// allocate space
	wh = side * side;
	n = side * side * side;
	radii.resize(levels);
	stepSizes.resize(levels);
	grid.resize(n);
	diffusionLeft.resize(n);
	diffusionRight.resize(n);
	variation.resize(n);
	bestVariation.resize(n);
	bestLevel.resize(n);
	direction.resize(n);
	buffer.allocate(side, side, OF_IMAGE_COLOR_ALPHA);
	
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
	
	unsigned char* pixels = buffer.getPixels();
	for(int i = 0; i < wh; i++) {
		for(int j = 0; j < 4; j++) {
			pixels[i * 4 + j] = 255;
		}
	}
	ofEnableBlendMode(OF_BLENDMODE_ADD);
}

void blur(floats& from, floats& to, floats& buffer, int w, int h, int d, int radius) {
  // build integral image
	int i = 0;
	bool hx, hy, hz;
	hz = false;
	for(int z = 0; z < d; z++) {
		hy = false;
		for(int y = 0; y < h; y++) {
			hx = false;
			for(int x = 0; x < w; x++) {
				buffer[i] = from[i];
				
				if(hx) {
					buffer[i] += buffer[i - 1];
					if(hy) {
						buffer[i] -= buffer[i - 1 - w];
						if(hz) {
							buffer[i] += buffer[i - 1 - w - wh];
						}
					}
					if(hz) {
						buffer[i] -= buffer[i - 1 - wh];
					}
				}
				if(hy) {
					buffer[i] += buffer[i - w];
					if(hz) {
						buffer[i] -= buffer[i - w - wh];
					}
				}
				if(hz) {
					buffer[i] += buffer[i - wh];
				}
				
				/*
				 int swb = i - 1 - 0 - 0;
				 int nwb = i - 1 - w - 0;
				 int nwf = i - 1 - w - wh;
				 int swf = i - 1 - 0 - wh;
				 int neb = i - 0 - w - 0;
				 int nef = i - 0 - w - wh;
				 int sef = i - 0 - 0 - wh;
				 if(x > 0) {
				 buffer[i] += buffer[swb];
				 }
				 if(y > 0) {
				 buffer[i] += buffer[neb];
				 }
				 if(z > 0) {
				 buffer[i] += buffer[sef];
				 }
				 if(x > 0 && y > 0 && z > 0) {
				 buffer[i] += buffer[nwf];
				 }
				 if(x > 0 && y > 0) {
				 buffer[i] -= buffer[nwb];
				 }
				 if(x > 0 && z > 0) {
				 buffer[i] -= buffer[swf];
				 }
				 if(y > 0 && z > 0) {
				 buffer[i] -= buffer[nef];
				 }
				 */
				i++;
				hx = true;
			}
			hy = true;
		}
		hz = true;
	}
	// do lookups
	i = 0;
	for(int z = 0; z < d; z++) {
		int minz = max(0, z - radius);
		int maxz = min(z + radius, d - 1);
		int minzi = minz * wh;
		int maxzi = maxz * wh;
		for(int y = 0; y < h; y++) {
			int miny = max(0, y - radius);
			int maxy = min(y + radius, h - 1);
			int minyi = miny * w;
			int maxyi = maxy * w;
			for(int x = 0; x < w; x++) {
				int minx = max(0, x - radius);
				int maxx = min(x + radius, w - 1);		
				int volume = (maxx - minx) * (maxy - miny) * (maxz - minz);
				
				int nwf = minzi + minyi + minx;
				int nef = minzi + minyi + maxx;
				int swf = minzi + maxyi + minx;
				int sef = minzi + maxyi + maxx;
				int nwb = maxzi + minyi + minx;
				int neb = maxzi + minyi + maxx;
				int swb = maxzi + maxyi + minx;
				int seb = maxzi + maxyi + maxx;
				
				to[i] = (+buffer[nwb]
								 -buffer[neb]
								 -buffer[swb]
								 +buffer[seb]
								 -buffer[nwf]
								 +buffer[nef]
								 +buffer[swf]
								 -buffer[sef]) / volume;
				i++;
			}
		}
	}
}

void ofApp::update() {
	floats* activator = &grid;
	floats* inhibitor = &diffusionRight;
	
	for(int level = 0; level < levels - 1; level++) {		
		// blur activator into inhibitor
		int radius = radii[level];
		blur(*activator, *inhibitor, variation, side, side, side, radius);
		
		// absdiff between activator and inhibitor
		for(int i = 0; i < n; i++) {
			variation[i] = fabsf((*activator)[i] - (*inhibitor)[i]);
		}
		
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

void ofApp::drawBuffer(floats& grid, int offset) {
	unsigned char* pixels = buffer.getPixels();
  for (int i = 0; i < wh; i++) {
		float cur = grid[i + offset * wh];
		pixels[i * 4 + 3] = (cur + 1) * 4;
  }	
	buffer.update();
	buffer.draw(0, 0);//, ofGetWidth(), ofGetHeight());
}

void ofApp::draw() {
	ofBackground(0);

	cam.begin();
	ofScale(5, 5, 5);
	ofTranslate(-side / 2, -side / 2, -side / 2);
	//ofSetMinMagFilters(GL_NEAREST, GL_NEAREST);
	for(int z = 0; z < side; z++) {
		ofPushMatrix();
		ofTranslate(0, 0, z);
		drawBuffer(grid, z);
		ofPopMatrix();
	}
	cam.end();
	
	if(drawTiles) {
		int i = 0;
		for(int y = 0; y < 8; y++) {
			for(int x = 0; x < 8; x++) {
				ofPushMatrix();
				ofTranslate(x * side, y * side);
				drawBuffer(grid, i++);
				ofPopMatrix();
			}
		}
	}
	
	//ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}

void ofApp::mousePressed(int x, int y, int button) {
	//setup();
}

void ofApp::keyPressed(int key) {
	if(key == ' ') {
		buffer.saveImage(ofToString(ofGetFrameNum()) + ".png");
	}
	if(key == '\t') {
		setup();
	}
}