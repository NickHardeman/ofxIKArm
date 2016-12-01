//
//  ofxIKArm.h
//  emptyExample
//
//  Created by Nick Hardeman on 8/30/15.
//

#pragma once
#include "ofxJoint.h"

class ofxIKArm {
public:
    
    ofxIKArm();
    
    bool load( string aPath );
    bool save( string aPath="" );
    void setup( ofVec3f aGlobalShoulderPos, float aUpperArmLength, float aLowerArmLength );
    void update();
    void draw();
    
    void setTarget( ofVec3f aTarget );
    void setTarget( float ax, float ay, float az );
    void setElbowTarget( ofVec3f aTarget );
    void setShoulderPos( ofVec3f aPos );
    
    void setUpperArmLength( float aLen );
    void setLowerArmLength( float aLen );
    void setArmLengths( float aUpperLen, float aLowerLen );
    
    void setElbowTarget( float ax, float ay, float az );
    void setDrawSize( float aSize );
    
    float getArmLength();
    float getUpperArmLength();
    float getLowerArmLength();
    
    ofVec3f getTarget();
    ofVec3f getElbowTarget();
    ofVec3f getShoulderPos();
    
    ofVec3f getShoulderGlobalPos();
    ofVec3f getElbowGlobalPos();
    ofVec3f getHandGlobalPos();
    
protected:
    string _saveFilePath;
    ofVec3f shoulderPos;
    shared_ptr<ofxJoint> shoulder;
    shared_ptr<ofxJoint> elbow;
    shared_ptr<ofxJoint> hand;
    
    ofVec3f elbowtarget, target;
    
    float armLength;
};
