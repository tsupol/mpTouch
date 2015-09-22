
#include "mpImage.h"
//#if (USE_OPENCV_TO_RESIZE)
//#endif


mpImage::mpImage(){
	pendingTexture = false;
	timeOut = 10;
	alpha = 1;
	alphaRiseSpeed = 0.05; //20 frames to full alpha
	imageLoaded = false;
	pendingNotification = false;
	readyToDraw = false;
	problemLoading = false;
	resizeAfterLoad = false;
	errorLoaded = false;
	clickable = true;
	bLoadError = false;
	compression = OF_COMPRESS_NONE;
}

mpImage::~mpImage(){
	if (isThreadRunning()){
		try {
			waitForThread(true);
		}catch(Exception &ex) {
			ofLogError("mpImage", "Exception at waitForThread %s\n", ex.displayText().c_str() );
		}
	}
}

void mpImage::resizeIfNeeded(){
	if (resizeAfterLoad){

		int w = getWidth();
		int h = getHeight();
		int largestSide = MAX(w, h);
		if(largestSide > maxSideSize){ //we need resize!
			float scale = maxSideSize / (float)largestSide;
			//float t1 = ofGetElapsedTimef();
			int newW = w * scale;
			int newH = h * scale;
			//#if USE_OPENCV_TO_RESIZE
			if (type == OF_IMAGE_COLOR){
				originalImage;
				originalImage.setUseTexture(false);
				originalImage.allocate(w, h);
				originalImage.setFromPixels(getPixels(), w, h);
				ofxCvColorImage cvImgsmall;
				cvImgsmall.setUseTexture(false);
				cvImgsmall.allocate(newW, newH);
				cvImgsmall.scaleIntoMe(originalImage, CV_INTER_AREA);
				setFromPixels(cvImgsmall.getPixels(), newW, newH, OF_IMAGE_COLOR);
				
			}else{
				resize(newW, newH); //TODO opencv resizing is much faster!
			}
			//#else
			//resize(newW, newH); //TODO opencv resizing is much faster!
			//#endif
			//ofLog() << "time resize: " + ofToString( ofGetElapsedTimef() - t1 );
		}
	}
}

void mpImage::constrainImageSize(int largestSide){
	maxSideSize = largestSide;
	resizeAfterLoad = true;
}

void mpImage::threadedFunction(){

	#ifdef TARGET_OSX
	pthread_setname_np("mpImage");
	#endif

	if( lock() ){

		switch (whatToDo) {
			case SAVE:
				ofSaveImage(getPixelsRef(), fileName, quality);
				break;

			case LOAD:{
				alpha = 0.0f;
				//float t1 = ofGetElapsedTimef();
				loadImageBlocking(fileName);
				//ofLog() << "time to load: " << ofGetElapsedTimef() - t1;
				}break;

			case LOAD_HTTP:
				alpha = 0;
				ofxSimpleHttp http;
				http.setTimeOut(timeOut);
				ofxSimpleHttpResponse response;
				for(int t=0; t<5; t++) {
					response = http.fetchURLtoDiskBlocking(url, IMG_DOWNLOAD_FOLDER_NAME);
					if(response.ok) break;
				}

				
				if (response.ok){

					setUseTexture(false);
					bool loaded = loadImage(response.absolutePath);
					//cout << "loaded = " << loaded << "\nurl = " << url << "\n";
					if (loaded){
						resizeIfNeeded();
						imageLoaded = true;
						bLoadError = false;
					}else{
						//string res = "no file";
						//ofNotifyEvent(imageLoadedStatus, res, this); 
						ofLog(OF_LOG_ERROR, "loadHttpImageBlocking() failed to load from disk (%d) > %s\n", response.status, url.c_str() );
						load404();
					}
				}else{
					ofLog(OF_LOG_ERROR, "loadHttpImageBlocking() failed to download (%d) > %s\n", response.status, url.c_str() );
					load404();
				}
				break;
		}
		unlock();

	} else {
		ofLogError("mpImage::threadedFunction Can't %s %s, thread is already running", whatToDo == SAVE ? "Save" : "Load",  fileName.c_str() );
	}
	
	stopThread();

	#if  defined(TARGET_OSX) || defined(TARGET_LINUX) /*I'm not 100% sure of linux*/
	pthread_detach( pthread_self() ); //this is a workaround for this issue https://github.com/openframeworks/openFrameworks/issues/2506
	#endif

}


void mpImage::loadImageThreaded(string fileName_){
	alpha = 0.0;
	whatToDo = LOAD;
	fileName = fileName_;
	readyToDraw = false;
	problemLoading = false;
	startThread(true, false);
}


void mpImage::loadImageBlocking(string fileName){
	imageLoaded = false;
	whatToDo = LOAD;
	problemLoading = false;
	setUseTexture(false);
	alpha = 0.0;
	bool loaded = loadImage(fileName);
	if (!loaded){
		ofLogError() << "mpImage:: img couldnt load!" << endl;
		problemLoading = true;
		// load dummy image
		load404();

	}else{
		resizeIfNeeded();
		imageLoaded = true;
	}
	pendingTexture = true;
	originalImage.setUseTexture(false);
		originalImage.allocate(width, height);
		originalImage.setFromPixels(getPixels(), width, height);
}

void mpImage::load404(){
	bLoadError = true;
	string dfileName = ofToDataPath("imgs/notfound.jpg");
	bool bb = loadImage(dfileName);
	resizeIfNeeded();
	imageLoaded = true;
	clickable = false;
}

bool mpImage::loadHttpImageBlocking(string url_){
	alpha = 0;
	whatToDo = LOAD_HTTP;
	url = url_;
	readyToDraw = false;
	problemLoading = false;
	setUseTexture(false);
	ofxSimpleHttp http;
	http.setTimeOut(timeOut);
	ofxSimpleHttpResponse response = http.fetchURLBlocking(url);
	if (response.ok){
		ofDirectory dir;
		dir.open(ofToDataPath(IMG_DOWNLOAD_FOLDER_NAME, false));
		if ( !dir.exists()){
			dir.create();
		}
		string filePath = ofToDataPath( (string)IMG_DOWNLOAD_FOLDER_NAME + "/" + response.fileName, false );
		FILE * file = fopen( filePath.c_str(), "wb");
		fwrite (response.responseBody.c_str() , 1 , response.responseBody.length() , file );
		fclose( file);
	}else{
		ofLog(OF_LOG_ERROR, "loadHttpImageBlocking() failed (%d) > %s\n", response.status, url.c_str() );
		return false;
	}
	imageLoaded = false;
	bool ok = loadImage((string)IMG_DOWNLOAD_FOLDER_NAME + "/" + response.fileName);
	if(ok){
		resizeIfNeeded();
		imageLoaded = true;
	}
	pendingTexture = true;
	return ok;
}


void mpImage::loadHttpImageThreaded(string url_){
	alpha = 0;
	whatToDo = LOAD_HTTP;
	url = url_;
	pendingTexture = true;
	imageLoaded = false;
	problemLoading = false;
	readyToDraw = false;
	setUseTexture(false);
	startThread(true, false);
}


bool mpImage::isReadyToDraw(){
	return readyToDraw;
}


bool mpImage::arePixelsAvailable(){
	return imageLoaded;
}


void mpImage::updateTextureIfNeeded(){
	if (pendingTexture){
		if (!problemLoading){
			setUseTexture(true);
			tex.setCompression(compression);
			tex.allocate(getPixelsRef());
			//tex.setTextureMinMagFilter(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
			ofImage::update();
			readyToDraw = true;
			pendingTexture = false;
		}
		pendingNotification = true; //texture is loaded, notify owner!
	}
}


void mpImage::saveThreaded(string where, ofImageQualityType quality_){
	whatToDo = SAVE;
	this->fileName = where;
	this->quality = quality_;
	startThread(false, false);   // !blocking, !verbose
};


void mpImage::setTexCompression(ofTexCompression c){
	compression = c;
}

void mpImage::update(){

	if(imageLoaded){
		updateTextureIfNeeded();
	}

	if(pendingNotification){
		mpImageEvent event;
		event.image = this;
		if(problemLoading){
			event.loaded = false;
			imageLoaded = false;
		}
		ofNotifyEvent( imageReadyEvent, event, this );
		pendingNotification = false;
	}
}


void mpImage::draw(float _x, float _y, bool fadeInOnDelayedLoad){
	mpImage::draw(_x, _y, getPixelsRef().getWidth(), getPixelsRef().getHeight(), fadeInOnDelayedLoad );
}


void mpImage::draw(float _x, float _y, float _w, float _h, bool fadeInOnLoad){

	if (imageLoaded && fadeInOnLoad && alpha < 1.0f){
		if (whatToDo == LOAD_HTTP || whatToDo == LOAD){
			alpha += alphaRiseSpeed;
			if(alpha > 1.0) alpha = 1.0;
		}

		ofPushStyle();
			ofSetColor(255,255,255, 255 * alpha);
			ofImage::draw(_x, _y, _w, _h);
		ofPopStyle();

	}else{
		if(tex.bAllocated()){
			ofImage::draw(_x, _y, _w, _h);
		}
	}
}

