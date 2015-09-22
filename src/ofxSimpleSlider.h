/*
 *  ofxSimpleSlider.h
 *  Created by Golan Levin on 2/24/12.
 *
 */


#pragma once

#include "ofMain.h"

class ofxSimpleSlider {

	public:
			
		ofxSimpleSlider();
		~ofxSimpleSlider();

		void	setup (float inx, float iny, float inw, float inh, float loVal, float hiVal, float initialPercent, bool bVert, bool bDrawNum);
		void	clear();

		void	draw(ofEventArgs& event);
		void	mouseMoved(ofMouseEventArgs& event);
		void	mouseDragged(ofMouseEventArgs& event);
		void	mousePressed(ofMouseEventArgs& event);
		void	mouseReleased(ofMouseEventArgs& event);

		void	setVisible(bool _visible);
	
		float	getValue();
		float	getLowValue();
		float	getHighValue();
		float	getPercent();
		
		void	setLowValue(float lv);
		void	setHighValue(float hv);
		void	setPercent(float p);
		void	setNumberDisplayPrecision(int prec);
		void	setLabelString (string str);
		void	updatePercentFromMouse(int mx, int my); 

		void	resetValue();
	
	protected:
		
		float	x;
		float	y; 
		float	width; 
		float	height;
		ofRectangle box; 
		int		numberDisplayPrecision;
	
		bool	bVertical;
		bool	bDrawNumber;
		bool	bHasFocus; 
	
		float	lowValue;
		float	highValue;
		float	percent;
		float	initialValue;
	
		string	labelString; 
	
	private:
	
		bool	bWasSetup;
		bool	isVisible;
};
