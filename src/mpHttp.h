#pragma once

#include "ofMain.h"
#include "ofEvents.h"
#include "ofxSimpleHttp.h"
#include "ofxJSON.h"

class mpHttp : public ofThread {

	public:
		mpHttp();
		mpHttp(const std::string serverUrl);

		virtual ~mpHttp();

		void threadedFunction();
		void sendMessage( string msg );

		ofEvent<ofxJSONElement> mpHttpEvent;

		string message;
		string serverUrl;

	private:

		ofxJSONElement result;
		
};