#include "ofApp.h"

ofMesh mesh;
//--------------------------------------------------------------
void ofApp::setup(){
    
    beat.load("sound/heartbeat.mp3");
    mizukusa.load("mizukusa.png");
    // for that purpose:
    fftSmoothed = new float[8192];
    for (int i = 0; i < 8192; i++){
        fftSmoothed[i] = 0;
    }
    
    nBandsToGet = 128;
    
    ofSetFrameRate(60);
    
    model.loadModel("untitled.obj");
    ofEnableLighting();
    ofEnableAlphaBlending();
    
    material.setShininess(255);
    material.setDiffuseColor(ofColor(255,255,255));
    material.setSpecularColor(ofColor(255,255,0));
    material.setAmbientColor(ofColor(255,0,0));
    
    mesh = model.getMesh(0);
    
    for (int i=0; i < model.getMeshCount();i++){
        heart.push_back(model.getMesh(i));
    }
    
    pointLight2.setDiffuseColor( ofFloatColor(.85, .0, .0) );
    pointLight2.setSpecularColor( ofFloatColor(1.f, 1.f, 0.6f));
    
    pointLight.setDiffuseColor( ofFloatColor(.85, .0, .0) );
    pointLight.setSpecularColor( ofFloatColor(1.f, 1.f, 0.6f));
    
    //
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    vector<ofMeshFace> triangles = mesh.getUniqueFaces();
    mesh.setFromTriangles(triangles,true);
    
    for(int i=0; i < model.getMeshCount();i++){
        //heart[i].setMode(OF_PRIMITIVE_TRIANGLES);
        vector<ofMeshFace> triangles = heart[i].getUniqueFaces();
        heart[i].setFromTriangles(triangles,true);
        
    }
    
    //clone object
    heart_ = heart;
    
    
    ofSetBackgroundColor(0);
    
    cam.setDistance(500.0f);
    
    heart_vertices= heart[0].getVertices();
    for(int i=0; i < 3;i++){
        for(int j=0; j < heart[i].getNumVertices(); j+=10){
            //heart[i].setVertex(j,heart_[i].getVertex(j)*(ofNoise(ofGetFrameNum()/100.0f,j)*0.1+1));
            ofVec3f v = heart[i].getVertex(j);
            vector <int> list;
            for(int k=0; k < heart[i].getNumVertices(); k++){
                if(v.distance(heart[i].getVertex(k))<0.1){
                    list.push_back(k);
                }
            }
            nearest.push_back(list);
        }
    }
    
    myFbo.allocate(ofGetWidth(),ofGetHeight());
    myGlitch.setup(&myFbo);
    
    beat.play();
    
    for(int i=0; i <6;i++){
        noises.push_back(0.0);
    }
    for(int i=0; i<IMG_SIZE; i++){
        ofImage img;
        img.load("img"+ofToString(i)+".png");
        images.push_back(img);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    pointLight.setPosition((ofGetWidth()*.8)+ cos(ofGetElapsedTimef()*.5)*(ofGetWidth()*.8), ofGetHeight()/2, 500);
    pointLight2.setPosition((ofGetWidth()*.8)+ cos(ofGetElapsedTimef()*.5)*(ofGetWidth()*.8), -ofGetHeight()/2, -500);
    heart_vertices[100].set(1400,1400,1400);
    int cnt =0;
    for(int i=0; i < 3;i++){
        for(int j=0; j < heart[i].getNumVertices(); j+=10){
            for(int k=0; k < nearest[cnt].size();k++){
                heart[i].setVertex(nearest[cnt][k],heart_[i].getVertex(nearest[cnt][k])*(ofNoise(ofGetFrameNum()/100.0f,j)*fftSmoothed[0]*0.4+1));
            }
            cnt++;
        }
    }
    
    float * val = ofSoundGetSpectrum(nBandsToGet);		// request 128 values for fft
    for (int i = 0;i < nBandsToGet; i++){
        
        // let the smoothed calue sink to zero:
        fftSmoothed[i] *= 0.96f;
        
        // take the max, either the smoothed or the incoming:
        if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];
        
    }
    if(!beat.isPlaying())beat.play();
    
    
    myFbo.begin();
    ofClear(0,0,0,255);
    
    


    cam.begin();

    ofPushMatrix();
    ofTranslate(0, 0,-400);
    ofEnableAlphaBlending();
    for(int key=0; key<IMG_SIZE; key++){
        float n = ofNoise(key,ofGetFrameNum()/80.0f);
        if(n>0.9) {
            ofSetColor(255,255,255,255);
            images[key].draw(-ofGetWidth()/2, -ofGetHeight()/2, ofGetWidth(), ofGetHeight());
            cout<<key<<endl;
        }
    }
    ofDisableAlphaBlending();
    //mizukusa.draw(ofPoint(-ofGetWidth()/2, -ofGetHeight()/2,1500) ,ofGetWidth(), ofGetHeight());
    ofPopMatrix();
    ofEnableDepthTest();
    
    ofEnableLighting();
    pointLight.enable();
    pointLight2.enable();
    
    ofPushMatrix();
    {
        
        
        ofPushMatrix();
        {
            
            ofScale(0.3,0.3);
            ofTranslate(ofGetWidth()/2*0.8,ofGetHeight()/2*0.8);
            ofRotateY(ofGetFrameNum()/3.0f);
            
            for(int i=0; i < 3; i++){
                //heart[i].draw();
            }
        }ofPopMatrix();
        material.end();
        ofScale(2.6,2.6);
        ofRotateY(ofGetFrameNum()/3.0f);
        ofRotateX(ofGetFrameNum()/1.8f);
        material.begin();
        for(int i=0; i < 3; i++){
            heart[i].draw();
        }
        
    }
    ofPopMatrix();
    ofDisableLighting();
    cam.end();

    myFbo.end();
    
    for(int i=0; i < 6; i++){
        noises[i] = ofNoise(i+1,ofGetFrameNum()/100.0f);
    }
    
    for(int key=0; key<6; key++){
        if(noises[key]>0.9 && key == 0) myGlitch.setFx(OFXPOSTGLITCH_GLOW	, true); else if(key==0) myGlitch.setFx(OFXPOSTGLITCH_GLOW	, false);
        if(noises[key]>0.8 && key == 1) myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER	, true); else  if(key==1)  myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER	, false);
        if(noises[key]>0.8 && key == 2) myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, true); else  if(key==2)  myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, false);
        if(noises[key]>0.8 && key == 3) myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN	, true); else  if(key==3)  myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN	, false);
        if(noises[key]>0.8 && key == 4) myGlitch.setFx(OFXPOSTGLITCH_NOISE	, true); else  if(key==4)  myGlitch.setFx(OFXPOSTGLITCH_NOISE	, false);
        if(noises[key]>0.8 && key == 5) myGlitch.setFx(OFXPOSTGLITCH_TWIST	, true); else  if(key==5)  myGlitch.setFx(OFXPOSTGLITCH_TWIST	, false);
        
    }
    
}


//--------------------------------------------------------------
void ofApp::draw(){
    /* Apply effects */
    myGlitch.generateFx();
    /* draw normal view */
    ofSetColor(255);
    myFbo.draw(0, 0);
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
    if (key == '1') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, true);
    if (key == '2') myGlitch.setFx(OFXPOSTGLITCH_GLOW			, true);
    if (key == '3') myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, true);
    if (key == '4') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, true);
    if (key == '5') myGlitch.setFx(OFXPOSTGLITCH_TWIST			, true);
    if (key == '6') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, true);
    if (key == '7') myGlitch.setFx(OFXPOSTGLITCH_NOISE			, true);
    if (key == '8') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, true);
    if (key == '9') myGlitch.setFx(OFXPOSTGLITCH_SWELL			, true);
    if (key == '0') myGlitch.setFx(OFXPOSTGLITCH_INVERT			, true);
    
    if (key == 'q') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
    if (key == 'w') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE	, true);
    if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE	, true);
    if (key == 'r') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE	, true);
    if (key == 't') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT	, true);
    if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT	, true);
    if (key == 'u') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT	, true);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == '1') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, false);
    if (key == '2') myGlitch.setFx(OFXPOSTGLITCH_GLOW			, false);
    if (key == '3') myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, false);
    if (key == '4') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, false);
    if (key == '5') myGlitch.setFx(OFXPOSTGLITCH_TWIST			, false);
    if (key == '6') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, false);
    if (key == '7') myGlitch.setFx(OFXPOSTGLITCH_NOISE			, false);
    if (key == '8') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, false);
    if (key == '9') myGlitch.setFx(OFXPOSTGLITCH_SWELL			, false);
    if (key == '0') myGlitch.setFx(OFXPOSTGLITCH_INVERT			, false);
    
    if (key == 'q') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, false);
    if (key == 'w') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE	, false);
    if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE	, false);
    if (key == 'r') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE	, false);
    if (key == 't') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT	, false);
    if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT	, false);
    if (key == 'u') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT	, false);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
