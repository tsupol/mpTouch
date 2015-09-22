#pragma once

#include "ofMain.h"
#include "mpImage.h"
#include "ofxTween.h"
#include "ofxSimpleTimer.h"
#include "ofxJSON.h"
#include "mpButton.h"
#include "ofxXmlSettings.h"
#include "ofxSimpleSlider.h"
#include "ofxLibwebsockets.h"
#include "mpPrinter.h"
#include "mpHttp.h"

//#include <Windows.h>
//#include <Objbase.h>
//#include <WinError.h>
//#include <Winspool.h>
//#include <XpsPrint.h>
// Import the type library.

/***************************************************************

 oscChatSystemExample

 - this app demonstrates using the ofxOsc addon to create a
 simple chat room
 - it sends chat messages to a server running in the same app
 - optionally, it can talk to another instance of this app on another IP.
 - get the IP address of the second computer
 - in ofApp.cpp inside the setup() function, change the value of
 clientDestination to the other IP as shown in the comment.

 ***************************************************************/
//bool compare_by_word(const instaUser& lhs, const instaUser& rhs) {
//    return lhs.username < rhs.username;
//}

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void setBrightness(int x);

		bool isClicking();

		// Networking
		string serverUrl;
		void sendMessage( string msg );
		bool bGotAllMedia;

		mpHttp mpServer;
		void onMessage( ofxJSONElement &result ) ;

		// Font

		ofTrueTypeFont	font; // standard font
		ofTrueTypeFont	titleFont; // font for some info in the title line
		ofTrueTypeFont	monoFont;

		//----------------------------------------

		// GUI
		vector<imageBlock> images;

		// Easing
		
		ofxEasingQuart	easingquart;
		ofxEasingLinear easinglinear;
		ofxTween		multitween,alphatween1,alphatween2;

		unsigned		delay;
		unsigned		duration;

		// MP
		string			instatag;
		string			begintime;
		string			eventId;
		ofxJSONElement	result;
		ofxJSONElement	jsender;

		// Lightbox
		ofTexture		texScreen;
		ofFbo			fbo1,fbo2,imageFbo;
		ofPixels		imagePixels;
		ofImage			colorImg;
		ofShader		shader, shaderv, shaderh, bgShader, filterShader, circleShader;
		int				appState;

		mpButton		btnNumPrint[9];
		mpButton		btnPrint, btnClose;
		ofxTween		bgTween;

		ofTexture		imageTexture;

		ofxSimpleSlider	brightnessSlider;
		ofImage			brightnessImg1, brightnessImg2;

		// Options
		bool			bShowPrintQueue, bShowPrinted, bShowOtherImage, bInitialized;
		string			printBgColor;

	private:

		// 3D
		ofCamera		cam;
		ofLight			light;
		// Scrolling

		float			momentum;
		unsigned long	lastMdTime;
		bool			bMousePressed, bMouseChanged;
		ofxTween		momenTween;

		ofVec2f			mouseLast;
		float			scrollPos;
		int				lastScrollPos, targetPos;
		int				mousePressedTime;
		float			maxScroll;

		ofVec2f			mousePressPos;

		float			msDivider, msDelayDivider;

		// Gui

		int				imgWidth, imgHeight, blockWidth, blockHeight, maxId;
		int				printImgX, printImgY;
		int				ipc, headerHeight;
		float			imgPadding;
		int				selImgIdx;
		float			drawX, drawY, drawFramesX;
		bool			bUseImageBg, bSpeedUp, bInfoText, bFirstLoad;
		int				guiStyle;
		int				state2Signature;
		int				moveDuration, newCount;
		string			debugString,printString,connectString;

		ofImage			imgBackground;
		ofImage			imgLogo;

		// Events

		bool			hasFocus, isClicked;
		vector<mpFrame>	frames;
		ofxXmlSettings	XML;

		// Frame

		int				numFrames;
		int				currentFrameIdx;

		// Websocket

		string			pendingMessage;
		bool			isConnected;

		mpPrinter		mprinter;
		ofxLibwebsockets::ClientOptions options;

		// Timers

		ofxSimpleTimer	updateTimer;
		void			updateTimerHandler( int &args ) ;

		// Notify

		void			notifyError( string msg );

		// Print

		void			onPrintUpdate( string &args ) ;
		ofImage			printerImg;
		
		// Miscs
		ofImage			screenGrabber;

		// Users
		vector<instaUser>	users;
		ofImage			circleMask;

		// adjustments
		float		incPadX, incPadY, incZoom;

		// test Git 555

		int asdfasdf;
};

