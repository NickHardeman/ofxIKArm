//
//  ofxIKArm.cpp
//  emptyExample
//
//  Created by Nick Hardeman on 8/30/15.
//

#include "ofxIKArm.h"

//--------------------------------------------------------------
ofxIKArm::ofxIKArm() {
    shoulder    = shared_ptr< ofxJoint >( new ofxJoint() );
    elbow       = shared_ptr< ofxJoint >( new ofxJoint() );
    hand        = shared_ptr< ofxJoint >( new ofxJoint() );
    
    shoulder->localTransform.makeScaleMatrix( 1., 1., 1. );
    elbow->localTransform.makeScaleMatrix( 1., 1., 1. );
    hand->localTransform.makeScaleMatrix( 1., 1., 1.);
    
    elbow->parent   = shoulder;
    hand->parent    = elbow;
    
    shoulder->color.set( 20, 10, 220 );
    elbow->color.set( 60, 230, 0 );
    hand->color.set( 200, 200, 0 );
    
    hand->bIsEnd        = true;
    
    _saveFilePath       = "";
}

//--------------------------------------------------------------
bool ofxIKArm::load( string aPath ) {
    _saveFilePath = aPath;
    if( _saveFilePath == "" ) {
        ofLogError("path empty please pass the load location as an argument to the save function." );
        return false;
    }
    if( !ofFile::doesFileExist(_saveFilePath) ) {
        return false;
    }
    ofBuffer tbuff = ofBufferFromFile( _saveFilePath );
    int lcount = 0;
    for( auto line : tbuff.getLines() ) {
//        cout << line << endl;
        if( lcount == 0 ) {
            shoulder->length = ofToFloat( line );
        } else if( lcount == 1 ) {
            elbow->length = ofToFloat( line );
        } else if( lcount == 2 ) {
            vector< string > tstrings = ofSplitString( line, "," );
            if( tstrings.size() == 3 ) {
                shoulder->localTransform.setTranslation(ofToFloat(tstrings[0]),
                                                        ofToFloat(tstrings[1]),
                                                        ofToFloat(tstrings[2]) );
            }
        }
        lcount++;
    }
    setup( shoulder->localTransform.getTranslation(), shoulder->length, elbow->length );
}

//--------------------------------------------------------------
bool ofxIKArm::save( string aPath ) {
    
    if( aPath != "" ) {
        _saveFilePath = aPath;
    }
    if( _saveFilePath == "" ) {
        ofLogError("path empty please pass the save location as an argument to the save function." );
        return false;
    }
    
    stringstream outStr;
    outStr << shoulder->length << "\n";
    outStr << elbow->length << "\n";
    outStr << shoulder->getGlobalPosition().x << ","<< shoulder->getGlobalPosition().y << "," << shoulder->getGlobalPosition().z;
    ofBuffer tbuff;
    tbuff.append( outStr.str() );
    return ofBufferToFile( _saveFilePath, tbuff );
}

//--------------------------------------------------------------
void ofxIKArm::setup( ofVec3f aGlobalShoulderPos, float aUpperArmLength, float aLowerArmLength ) {
    shoulder->localTransform.setTranslation( aGlobalShoulderPos );
    elbow->localTransform.setTranslation( aUpperArmLength, 0, 0 );
    hand->localTransform.setTranslation( aLowerArmLength, 0, 0 );
    
    shoulder->length    = aUpperArmLength;//shoulder->getGlobalPosition().distance( elbow->getGlobalPosition() );
	elbow->length       = aLowerArmLength;//elbow->getGlobalPosition().distance( hand->getGlobalPosition() );
	armLength           = shoulder->length + elbow->length;
    
    setDrawSize( 1 );
}

//--------------------------------------------------------------
void ofxIKArm::update() {
    
    ofVec3f shoulderPos     = shoulder->getGlobalPosition();
    ofVec3f shoulderDiff    = shoulderPos - target;
    float distToShoulder    = shoulderDiff.length();
    
    
    ofVec3f stargetDiff     = target - shoulder->getGlobalPosition();
    ofVec3f selbowTarDiff   = elbowtarget - shoulder->getGlobalPosition();
    ofVec3f rotAxis         = stargetDiff.getCrossed( selbowTarDiff );
    rotAxis.normalize();
    
    ofQuaternion shoulderQuat;
    ofVec3f shoulderToTarget = target - shoulderPos;
    shoulderToTarget.normalize();
    shoulderQuat.makeRotate( ofVec3f(1,0,0), shoulderToTarget );
    
    float s1 = distToShoulder;
    float s2 = elbow->length;
    float s3 = shoulder->length;
    float fDegrees = (float) acos ( (s2 * s2 + s3 * s3 - s1 * s1) / (2 * s2 * s3) ) * ( 180.0f / 3.1415f );
    fDegrees += 180.;
    
    //    cout << "fDegrees: " << fDegrees << " nan: " << isnan(fDegrees) << endl;
    if( fDegrees < 200 ) {
        fDegrees = 200;
    }
    
    
    if( isnan(fDegrees) ) {
        fDegrees = 200.;
    }
    
    if( distToShoulder >= armLength ) {
        fDegrees = 0.;
    }
    
    ofQuaternion equat;
    equat.makeRotate( fDegrees, ofVec3f(0,0,1) );
    
    
    
    
    
    s2 = s1;
    s1 = elbow->length;
    s3 = shoulder->length;
    float sDegrees = (float) acos ( (s2 * s2 + s3 * s3 - s1 * s1) / (2 * s2 * s3) ) * ( 180.0f / 3.1415f );
    
    //    cout << "sDegrees: " << sDegrees << " nan: " << isnan(sDegrees) << endl;
    
    if( isnan(sDegrees) ) {
        sDegrees = 0.;
    }
    
    if( distToShoulder >= armLength ) {
        sDegrees = 0.;
    }
    
    ofQuaternion squat;
    squat.makeRotate( sDegrees, ofVec3f(0,0,1) );
    
    shoulder->localTransform.setRotate( squat * shoulderQuat );
    elbow->localTransform.setRotate( equat );
    
    ofVec3f shandDiff = shoulder->getGlobalPosition() - hand->getGlobalPosition();
    shandDiff.normalize();
    
    ofVec3f lzaxis = shoulder->localTransform.getRotate() * ofVec3f(0,0,1);
    ofVec3f lsetd = selbowTarDiff;
    ofVec3f tcross = shandDiff.getCrossed( selbowTarDiff );
    ofQuaternion ytrot;
    
    float tcdot = lzaxis.dot( selbowTarDiff );
    //    cout << "tcdot: " << tcdot << endl;
    
    float seangle = tcross.angle( lzaxis )-180;
    ytrot.makeRotate( seangle * ofSign( tcdot ), shandDiff );
    shoulder->localTransform.setRotate( shoulder->localTransform.getRotate() * ytrot );
}

//--------------------------------------------------------------
void ofxIKArm::draw() {
    ofSetColor(40, 0, 180 );
//    ofDrawSphere( elbowtarget, 0.3 );
    
    ofSetColor( 255, 0, 0 );
    ofDrawSphere( target, shoulder->getDrawSize()*0.3 );
    
    ofSetColor( 120 );
//    ofVec3f shand = hand->getGlobalPosition() - shoulder->getGlobalPosition();
//    shand.normalize();
    ofDrawLine( shoulder->getGlobalPosition(), hand->getGlobalPosition() );
//        ofLine( shoulder->getGlobalPosition(), hand->getGlobalPosition() );
//        ofLine( (shoulder->getGlobalPosition()+hand->getGlobalPosition())*0.5, elbow->getGlobalPosition() );
//        ofLine( (shoulder->getGlobalPosition()+hand->getGlobalPosition())*0.5, elbowtarget );
//        ofLine( shoulder->getGlobalPosition(), elbowtarget );
    
    ofSetColor( 255, 220, 0 );
    ofDrawLine( elbowtarget, elbow->getGlobalPosition() );
    ofSetColor( 200, 30, 200 );
    ofDrawSphere( elbowtarget, elbow->getDrawSize()*0.3 );
    
//        ofLine( elbowtarget, ofVec3f(elbowtarget.x, elbowtarget.y, elbowtarget.z + 30 ) );
    
//    ofPushMatrix(); {
//        ofSetColor( 180 );
//        ofRotateY(-90);
//        ofRotateZ(90 );
//        ofDrawGridPlane( 20 );
//    } ofPopMatrix();
    
    
    shoulder->draw();
    elbow->draw();
    hand->draw();
}

//--------------------------------------------------------------
void ofxIKArm::setTarget( ofVec3f aTarget ) {
    target.set( aTarget );
}

//--------------------------------------------------------------
void ofxIKArm::setTarget( float ax, float ay, float az ) {
    target.set( ax, ay, az );
}

//--------------------------------------------------------------
void ofxIKArm::setElbowTarget( ofVec3f aTarget ) {
    elbowtarget.set( aTarget );
}

//--------------------------------------------------------------
void ofxIKArm::setElbowTarget( float ax, float ay, float az ) {
    elbowtarget.set( ax, ay, az );
}

//--------------------------------------------------------------
void ofxIKArm::setShoulderPos( ofVec3f aPos ) {
    shoulderPos = aPos;
    shoulder->localTransform.setTranslation( aPos );
}

//--------------------------------------------------------------
void ofxIKArm::setUpperArmLength( float aLen ) {
    setArmLengths( aLen, elbow->length );
}

//--------------------------------------------------------------
void ofxIKArm::setLowerArmLength( float aLen ) {
	setArmLengths( shoulder->length, aLen );
}

//--------------------------------------------------------------
void ofxIKArm::setArmLengths( float aUpperLen, float aLowerLen ) {
    shoulder->length    = aUpperLen;
    elbow->length       = aLowerLen;
    armLength           = shoulder->length + elbow->length;
    elbow->localTransform.setTranslation( shoulder->length, 0, 0 );
    hand->localTransform.setTranslation( elbow->length, 0, 0 );
    shoulder->setDrawSize( shoulder->getDrawSize(), true );
    elbow->setDrawSize( elbow->getDrawSize(), true );
}

//--------------------------------------------------------------
void ofxIKArm::setDrawSize( float aSize ) {
    shoulder->setDrawSize(aSize);
    elbow->setDrawSize( aSize );
    hand->setDrawSize( aSize );
}

//--------------------------------------------------------------
float ofxIKArm::getArmLength() {
    return armLength;
}

//--------------------------------------------------------------
float ofxIKArm::getUpperArmLength() {
    return shoulder->length;
}

//--------------------------------------------------------------
float ofxIKArm::getLowerArmLength() {
    return elbow->length;
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getTarget() {
    return target;
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getElbowTarget() {
    return elbowtarget;
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getShoulderPos() {
    return shoulderPos;
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getShoulderGlobalPos() {
    return shoulder->getGlobalPosition();
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getElbowGlobalPos() {
    return elbow->getGlobalPosition();
}

//--------------------------------------------------------------
ofVec3f ofxIKArm::getHandGlobalPos() {
    return hand->getGlobalPosition();
}


