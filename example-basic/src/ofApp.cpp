#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
	
	plotHeight = 128;
	bufferSize = 2048;
	
	fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING);
	// To use FFTW, try:
	//fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING, OF_FFT_FFTW);
	
	drawBins.resize(fft->getBinSize());
	middleBins.resize(fft->getBinSize());
	audioBins.resize(fft->getBinSize());
	
	// 0 output channels,
	// 1 input channel
	// 44100 samples per second
	// [bins] samples per buffer
	// 4 num buffers (latency)
	
	ofSoundStreamSetup(0, 1, this, 44100, bufferSize, 4);
	
	ofBackground(0, 0, 0);
}

void ofApp::draw() {
	ofSetColor(255);
	ofPushMatrix();
	ofTranslate(16, 16);
	
	soundMutex.lock();
	drawBins = middleBins;
	soundMutex.unlock();
	
	ofDrawBitmapString("Frequency Domain", 0, 0);
	plot(drawBins, -plotHeight, plotHeight / 2);
	ofPopMatrix();
	string msg = ofToString((int) ofGetFrameRate()) + " fps";
	ofDrawBitmapString(msg, ofGetWidth() - 80, ofGetHeight() - 20);
}

void ofApp::plot(vector<float>& buffer, float scale, float offset) {
	ofNoFill();
	int n = buffer.size();
	ofDrawRectangle(0, 0, n, plotHeight);
	glPushMatrix();
	glTranslatef(0, plotHeight / 2 + offset, 0);
	ofBeginShape();
	for (int i = 0; i < n; i++) {
		ofVertex(i, sqrt(buffer[i]) * scale);
	}
	ofEndShape();
	glPopMatrix();
}

void ofApp::audioIn(ofSoundBuffer& buffer) {
	float maxValue = 0;
	for(int i = 0; i < bufferSize; i++) {
		if(abs(buffer[i]) > maxValue) {
			maxValue = abs(buffer[i]);
		}
	}
	for(int i = 0; i < bufferSize; i++) {
		buffer[i] /= maxValue;
	}
	
	fft->setSignal(buffer.getBuffer());
	
	float* curFft = fft->getAmplitude();
	memcpy(&audioBins[0], curFft, sizeof(float) * fft->getBinSize());
	
	maxValue = 0;
	for(int i = 0; i < fft->getBinSize(); i++) {
		if(abs(audioBins[i]) > maxValue) {
			maxValue = abs(audioBins[i]);
		}
	}
	for(int i = 0; i < fft->getBinSize(); i++) {
		audioBins[i] /= maxValue;
	}
	
	soundMutex.lock();
	middleBins = audioBins;
	soundMutex.unlock();
}
