/*
 *  mpButton.cpp
 *  Created by Golan Levin on 2/24/12.
 *
 */

#include "mpButton.h"

//----------------------------------------------------
mpButton::mpButton(){
	bWasSetup = false;
	bClicked = false;
	bInverse = false;
}

//----------------------------------------------------
mpButton::~mpButton(){
	clear();
}

//-----------------------------------------------------------------------------------------------------------------------
void mpButton::setup(float inx, float iny, float inw, float inh, string str, int inType){
	x = inx;
	y = iny; 
	width = inw; 
	height = inh;
	box.set(x,y, width, height); 
	
	labelString = str; 
	
	if(!bWasSetup){
		//ofAddListener(ofEvents().draw, this, &mpButton::draw);
		ofAddListener(ofEvents().mouseMoved, this, &mpButton::mouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &mpButton::mousePressed);
		ofAddListener(ofEvents().mouseReleased, this, &mpButton::mouseReleased);
		ofAddListener(ofEvents().mouseDragged, this, &mpButton::mouseDragged);
		bWasSetup = true;
	}

	//color1 = 0x123456;
	if(bInverse) {
		color2 = 0x006a5e;
		color1 = 0xffffff;
	} else {
		color1 = 0x006a5e;
		color2 = 0xffffff;
	}

	bShow = true;
	bToggle = false;

	type = inType;

	lastTime = ofGetElapsedTimeMillis();
}

//----------------------------------------------------
void mpButton::clear(){
	if(bWasSetup){
		//ofRemoveListener(ofEvents().draw, this, &mpButton::draw);
		ofRemoveListener(ofEvents().mouseMoved, this, &mpButton::mouseMoved);
		ofRemoveListener(ofEvents().mousePressed, this, &mpButton::mousePressed);
		ofRemoveListener(ofEvents().mouseReleased, this, &mpButton::mouseReleased);
		ofRemoveListener(ofEvents().mouseDragged, this, &mpButton::mouseDragged);
	}
	bWasSetup = false;
}

//----------------------------------------------------
void mpButton::setLabelString (string str){
	labelString = str;
}

void mpButton::setColor (int c1, int c2){
	color1 = c1;
	color2 = c2;
}

void mpButton::setToggle (bool tog){
	bToggle = tog;
}

bool mpButton::getToggle(){
	return bToggle;
}

//----------------------------------------------------
void mpButton::draw(ofTrueTypeFont* ttf){

	bShow = true;
	
	if(bShow) {
	
		ofPushStyle();
		ofPushMatrix();
		ofTranslate(x,y,0);
	
		if(bToggle || bHasFocus) {
			ofBeginShape();
			
				ofSetHexColor(color1);

				ofVertex(0,0);
				ofVertex(width,0);
				ofVertex(width,height);
				ofVertex(0,height);
		
			ofEndShape(true);
		} else {
			ofBeginShape();
			
				ofSetHexColor(color1);

				ofVertex(0,0);
				ofVertex(width,0);
				ofVertex(width,height);
				ofVertex(0,height);
		
				ofNextContour(true);
		
				ofVertex(2,2);
				ofVertex(width-2,2);
				ofVertex(width-2,height-2);
				ofVertex(2,height-2);
		
			ofEndShape(true);
		}

		float sw = ttf->stringWidth(labelString);
		float sh = ttf->stringHeight(labelString);

		/*ofSetHexColor(0xe56690);
		ofRect((width-sw)/2,(height-sh)/2,sw,sh);*/
		int cax = (width-sw)/2;
		int cah = height-(height-sh)/2;

		if(labelString.length() == 1) {
			//cax = width-(sw+2))/2;
		} 

		if(bToggle || bHasFocus) {
			ofSetHexColor(color2);
		} else {
			ofSetHexColor(color1);
		}
		ttf->drawString(labelString, cax, cah);

		ofPopMatrix();
		ofPopStyle();

		lastTime = ofGetElapsedTimeMillis();
	}
}


//----------------------------------------------------
void mpButton::mouseMoved(ofMouseEventArgs& event){
	bHasFocus = false;
}
void mpButton::mouseDragged(ofMouseEventArgs& event){
	if (bHasFocus){
	}
}
void mpButton::mousePressed(ofMouseEventArgs& event){
	
	if(ofGetElapsedTimeMillis() - lastTime < 500) {
		if (box.inside(event.x, event.y)){
			bHasFocus = true;
		} else {
			bHasFocus = false;
		}
	}
}
void mpButton::mouseReleased(ofMouseEventArgs& event){

	if(ofGetElapsedTimeMillis() - lastTime < 500) {
		if (bHasFocus){
			if(type == 2) {
				if (box.inside(event.x, event.y)){
					bToggle = !bToggle;
				}
			}
			bClicked = true;
		}
		bHasFocus = false;
	}
}

void mpButton::show(){
	bShow = true;
}

void mpButton::hide(){
	bShow = false;
}

bool mpButton::getClicked(){
	bool result = bClicked;
	bClicked = false;
	return result;
}


