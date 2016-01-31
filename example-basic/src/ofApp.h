#pragma once

#include "ofMain.h"
#include "ofxFft.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void plot(vector<float>& buffer, float scale, float offset);
	void audioIn(ofSoundBuffer& buffer);
	void draw();

	int plotHeight, bufferSize;

	ofxFft* fft;
	
	ofMutex soundMutex;
	vector<float> drawBins, middleBins, audioBins;
};
