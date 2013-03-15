#pragma once

typedef vector<float> floats;
typedef vector<floats> floatss;

class TuringPattern {
public://+
  floatss activator;//~
  floatss inhibitor;//~
  floatss variations;//~
  int activatorRadiusx;
  int inhibitorRadiusx;
  int activatorRadiusy;
  int inhibitorRadiusy;
  int variationSamplingRadius;
  float stepsize;
  int num;

	TuringPattern() {}//+
  TuringPattern(int num, int arx, int irx, int ary, int iry, int vsr, float ss) {
    activator.resize(num, floats(num));//~
		inhibitor.resize(num, floats(num));//~
    variations.resize(num, floats(num));//~
    activatorRadiusx=arx;
    inhibitorRadiusx=irx;
    activatorRadiusy=ary;
    inhibitorRadiusy=iry;
    variationSamplingRadius=vsr;
    stepsize=ss;
    this->num=num;//~
  }

  void step(floatss& grid, floatss& tmp) {//~
    diffuse(grid, tmp);
    sampleVariation(tmp);
  }

  void diffuse(floatss& grid, floatss& tmp) {//~
    blur(grid, activator, tmp, activatorRadiusx, activatorRadiusy);
    blur(grid, inhibitor, tmp, inhibitorRadiusx, inhibitorRadiusy);
  }

  void sampleVariation(floatss& tmp) {//~
    comp(activator, inhibitor, variations, tmp, variationSamplingRadius);
  }

  void blur(floatss& source, floatss& dest, floatss& tmp, int radiusx, int radiusy) {//~
    horlineblur(source, tmp, radiusx);
    vrtlineblur(tmp, dest, radiusy);
  }

  void horlineblur (floatss& source, floatss& dest, int radius) {//~
    for (int j = 0; j < num; ++j) {
      float total = 0;
      for (int di = -radius; di <= radius; di++) {
        total += (di>=0)?source[di][j]:0;
      }
      dest[0][j] = total / (radius * 2 + 1);
      for (int i = 1; i < num; i++) {
        total -= (i - radius - 1>=0)?source[i - radius - 1][j]:0;
        total += (i + radius <num)?source[i + radius][j]:0;
        dest[i][j] = total / (radius * 2 + 1);
      }
    }
  }

  void vrtlineblur (floatss& source, floatss& dest, int radius) {//~
    for (int i = 0; i < num;i++) {
      float total = 0;
      for (int dj = -radius; dj <= radius; dj++) {
        total += (dj>=0)?source[i][dj]:0;
      }
      dest[i][0] = total / (radius * 2 + 1);
      for (int j = 1; j < num; j++) {
        total -= (j - radius - 1>=0)?source[i][j - radius - 1]:0;
        total += (j + radius <num)?source[i][j + radius]:0;
        dest[i][j] = total / (radius * 2 + 1);
      }
    }
  }

  void comp(floatss& source1, floatss& source2, floatss& dest, floatss& tmp, int radius) {//~
    horlinecomp(source1, source2, tmp, radius);
    vrtlinecomp(source1, source2, dest, radius);
    for (int i=0;i<num;i++) {
      for (int j=0;j<num;j++) {
        dest[i][j]+=tmp[i][j];
      }
    }
  }

  void horlinecomp (floatss& source1, floatss& source2, floatss& dest, int radius) {//~
    for (int j = 0; j < num; ++j) {
      float total = 0;
      for (int di = -radius; di <= radius; di++) {
        total += (di>=0)?abs(source1[di][j]-source2[di][j]):0;
      }
      dest[0][j] = total / (radius * 2 + 1);
      for (int i = 1; i < num;i++) {
        total -= (i - radius - 1>=0)?abs(source1[i - radius - 1][j]-source2[i - radius - 1][j]):0;
        total += (i + radius <num)?abs(source1[i + radius][j]-source2[i + radius][j]):0;
        dest[i][j] = total / (radius * 2 + 1);
      }
    }
  }

  void vrtlinecomp (floatss& source1, floatss& source2, floatss& dest, int radius) {//~
    for (int i = 0; i < num; ++i) {
      float total = 0;
      for (int dj = -radius; dj <= radius; dj++) {
        total += (dj>=0)?abs(source1[i][dj]-source2[i][dj]):0;
      }
      dest[i][0] = total / (radius * 2 + 1);
      for (int j = 1; j < num; j++) {
        total -= (j - radius - 1>=0)?abs(source1[i][j - radius - 1]-source2[i][j - radius - 1]):0;
        total += (j + radius <num)?abs(source1[i][j + radius]-source2[i][j + radius]):0;
        dest[i][j] = total / (radius * 2 + 1);
      }
    }
  }
};
