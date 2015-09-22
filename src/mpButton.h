/*
 *  mpButton.h
 *  Created by Golan Levin on 2/24/12.
 *
 */


#pragma once

#include "ofMain.h"

class mpButton {

	public:
			
		mpButton();
		~mpButton();

		void	setup (float inx, float iny, float inw, float inh, string inLabel, int type = 1);
		void	clear();

		void	draw(ofTrueTypeFont* ttf);
		void	mouseMoved(ofMouseEventArgs& event);
		void	mouseDragged(ofMouseEventArgs& event);
		void	mousePressed(ofMouseEventArgs& event);
		void	mouseReleased(ofMouseEventArgs& event);
	
		void	setLabelString (string str);
		void	setColor (int c1, int c2);
		void	setToggle (bool tog);
		bool	getToggle ();

		void	show();
		void	hide();

		bool	getClicked();

		float	x;
		float	y; 
		float	width; 
		float	height;
		bool	bInverse;
	
	protected:
		
		ofRectangle box; 
		ofTrueTypeFont* font;

		bool	bShow;

		string	labelString; 

		int		color1, color2;
		int		lastTime;
	
	private:
	
		bool	bWasSetup;
		bool	bHasFocus;
		bool	bToggle;
		bool	bClicked;
		int		type;
};
