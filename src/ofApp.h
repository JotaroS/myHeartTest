#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxPostGlitch.h"

#define IMG_SIZE 4
class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxAssimpModelLoader model;
    ofEasyCam cam;
    ofMaterial material;
    
    ofLight pointLight;
    ofLight pointLight2;
    
    vector<ofMesh> heart;
    vector<ofMesh> heart_; //original heart mesh.
    
    vector<ofVec3f> heart_vertices;
    vector< vector <int> > nearest;
    
    ofSoundPlayer 		beat;
    ofSoundPlayer		ow;
    ofSoundPlayer		dog;
    ofSoundPlayer		rooster;
    
    float 				* fftSmoothed;
    // we will draw a rectangle, bouncing off the wall:
    float 				px, py, vx, vy;
    
    int nBandsToGet;
    float prevx, prevy;
    
    ofImage mizukusa;
    
    ofFbo myFbo;
    ofxPostGlitch myGlitch;
    
    vector<float> noises;
    vector<ofImage>images;
};
