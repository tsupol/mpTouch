#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	/*HWND AppWindow = GetActiveWindow();
			SetWindowPos(AppWindow, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);*/
			
	//ofSetupOpenGL(1024,768, OF_WINDOW);			// <-------- setup the GL context
	ofSetupOpenGL(1920,1080, OF_WINDOW);
	//ofSetupOpenGL(1600,1000, OF_FULLSCREEN);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new ofApp());

}
