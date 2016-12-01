//
//  ofxJoint.h
//  emptyExample
//
//  Created by Nick Hardeman on 8/25/15.
//

#pragma once
#include "ofMain.h"

class ofxJoint {
public:
    ofxJoint() {
        drawSize    = -1.4903920981;
        bIsEnd      = false;
        
        outlineMesh.setMode( OF_PRIMITIVE_LINES );
    }
    
    ofMatrix4x4 getGlobalTransform() {
        if( parent ) {
            return localTransform * parent->getGlobalTransform();
        }
        return localTransform;
    }
    
    ofVec3f getGlobalPosition() {
        return getGlobalTransform().getTranslation();
    }
    
    void draw() {
        
        ofPushMatrix(); {
            ofMultMatrix( getGlobalTransform() );
            
            ofDrawAxis( drawSize );
            
            if( !bIsEnd ) {
                
                ofSetColor( color );
                ofDrawLine( 0,0,0, length,0,0 );
                
                ofSetColor( 255 );
                outlineMesh.draw();
                
            }
        } ofPopMatrix();
    }
    
    void setDrawSize( float aSize, bool bForceUpdate=false ) {
        
        if( aSize != drawSize || bForceUpdate ) {
            outlineMesh.clear();
            
            float tlen = aSize;
            float xlen = tlen * 1.5;
                        
            ofVec3f p1, p2, p3, p4;
            p1.set( xlen, -tlen, tlen );
            p2.set( xlen, -tlen, -tlen );
            p3.set( xlen, tlen, -tlen );
            p4.set( xlen, tlen, tlen );
            
            outlineMesh.addVertex( p1 ); outlineMesh.addVertex( p2 );
            outlineMesh.addVertex( p2 ); outlineMesh.addVertex( p3 );
            outlineMesh.addVertex( p3 ); outlineMesh.addVertex( p4 );
            outlineMesh.addVertex( p4 ); outlineMesh.addVertex( p1 );
            
            ofVec3f ept(length, 0, 0 );
            outlineMesh.addVertex( p1 ); outlineMesh.addVertex( ept );
            outlineMesh.addVertex( p2 ); outlineMesh.addVertex( ept );
            outlineMesh.addVertex( p3 ); outlineMesh.addVertex( ept );
            outlineMesh.addVertex( p4 ); outlineMesh.addVertex( ept );
            
            ept.set( 0,0,0 );
            outlineMesh.addVertex( p1 ); outlineMesh.addVertex( ept );
            outlineMesh.addVertex( p2 ); outlineMesh.addVertex( ept );
            outlineMesh.addVertex( p3 ); outlineMesh.addVertex( ept );
            outlineMesh.addVertex( p4 ); outlineMesh.addVertex( ept );
        }
        
        drawSize = aSize;
    }
    
    float getDrawSize() { return drawSize; }
    
    bool bIsEnd;
    float length;
    float drawSize;
    
    ofColor color;
    
    ofMatrix4x4 localTransform;
    int index;
    shared_ptr< ofxJoint > parent;
    
    ofMesh outlineMesh;
};
