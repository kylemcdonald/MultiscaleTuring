#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	void psSetup();
	void psDraw();
	void psKeyPressed();
	void psMouseReleased();

	void setup() {
		psSetup();
	}
	void update() {
	}
	void draw() {
		psDraw();
	}
	void keyPressed(int key) {
		this->key = key;
		psKeyPressed();
	}
	void mouseReleased(int x, int y, int button) {
		this->button = button;
		psMouseReleased();
	}
	
	int key;
	int button;
};
