#include "mpHttp.h";

using namespace std;

mpHttp::mpHttp(){
	serverUrl = "http://103.245.167.79/memoprint/public/";
}

mpHttp::mpHttp(const string url){
	serverUrl = url;
}

mpHttp::~mpHttp(){
	if (isThreadRunning()){
		try {
			waitForThread(true);
		}catch(Exception &ex) {
			ofLogError("mpImage", "Exception at waitForThread %s\n", ex.displayText().c_str() );
		}
	}
}

void mpHttp::sendMessage(string msg) {
	message = msg;
	startThread(false, false);
}

void mpHttp::threadedFunction() {

	if( lock() ){
		string url = serverUrl+message;
		std::cout << "[sending request] " << message << "\n";

		if (!result.open(url)) {
			std::cout << "[Error] Failed to parse JSON" << "\n";
		} else {
			ofNotifyEvent(mpHttpEvent,result,this);
		}
		unlock();
	}
	stopThread();
}