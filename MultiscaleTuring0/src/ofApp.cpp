#include "ofApp.h"

#define setup() ofApp::psSetup()
#define draw() ofApp::psDraw()
#define keyPressed() ofApp::psKeyPressed()
#define mouseReleased() ofApp::psMouseReleased()
#define fill ofSetColor
#define rect ofRect
#define translate ofTranslate
#define random ofRandom
#define min MIN
#define max MAX
void size(int width, int height) {
	ofSetWindowShape(width, height);
}
void noStroke() {
	ofFill();
}
void saveFrame(string name = "") {
	ofSaveScreen(name);
}

// - - --

#include "TuringPattern.h"

typedef vector<float> floats;
typedef vector<floats> floatss;

floatss grid;//~
floatss tmp;//~
int num;
int levels;
vector<TuringPattern> patterns;//~
int counter;

void initGrid();//+
void step();//+
void render();//+
void updateGrid();//+

void setup() {
  size(800, 800); 
  num=400;
  levels=5;
  tmp.resize(num, floats(num));
  patterns.resize(levels);
  patterns[0]=TuringPattern(num, 100, 200, 100, 200, 1, 0.05);
  patterns[1]=TuringPattern(num, 50, 100, 50, 100, 1, 0.04);
  patterns[2]=TuringPattern(num, 10, 20, 10, 20, 1, 0.03);
  patterns[3]=TuringPattern(num, 5, 10, 5, 10, 1, 0.02);
  patterns[4]=TuringPattern(num, 2, 4, 2, 4, 1, 0.01);
  initGrid();
  noStroke();
	ofSetBackgroundAuto(false);//+
}

void draw() {
  for (int i=0;i<5;i++) step();
  translate((counter%2)*400, ((counter/2)%2)*400);
  render();
}

void keyPressed() {
  saveFrame("MCB_#####.png");
}

void mouseReleased() {   
  initGrid();
}

void render() {
  for (int i=0;i<num;i++) {
    for (int j=0;j<num;j++) {
      fill(120+120*grid[i][j]);
      rect(i, j, 1, 1);
    }
  }
  counter++;
}

void initGrid() {
  counter=0;
  grid.resize(num, floats(num));
  for (int i=0;i<num;i++) {
    for (int j=0;j<num;j++) {
      grid[i][j]=random(-1, 1);
    }
  }
}

void step() {
  for (int i=0;i<levels;i++) {
    patterns[i].step(grid, tmp);
  }
  updateGrid();
}


//checkout the references in the intro, they're a lot better
//than any comments I can come up with
void updateGrid() {
  float smallest=10;
  float largest=-10;
  for (int i=0;i<num;i++) {
    for (int j=0;j<num;j++) {
      float bestvariation=patterns[0].variations[i][j];
      int bestlevel=0;
      for (int k=1;k<levels;k++) {
        if ((patterns[k].variations[i][j]<bestvariation)) {
          bestlevel=k; 
          bestvariation=patterns[k].variations[i][j];
        }
      }
      if (patterns[bestlevel].activator[i][j]>patterns[bestlevel].inhibitor[i][j]) {
        grid[i][j]+=patterns[bestlevel].stepsize; 
      }
      else {
        grid[i][j]-=patterns[bestlevel].stepsize; 
      }
      largest=max(largest, grid[i][j]);
      smallest=min(smallest, grid[i][j]);
    }
  }
  float range=0.5*(largest-smallest);
  for (int i=0;i<num;i++) {
    for (int j=0;j<num;j++) {
      grid[i][j]=(grid[i][j]-smallest)/range-1;
    }
  }
}


