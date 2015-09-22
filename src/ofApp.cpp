#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	// Change 5555

	// Another Change

	ofSetWindowPosition(300,0);
	ofSetWindowShape(1720,1080);
	ofSetFullscreen(false);

	ofSetFrameRate(60);
	ofSetVerticalSync(true);

	ofEnableAlphaBlending();
	ofEnableAntiAliasing(); 
	ofBackground(155);

	bInitialized = false;
	state2Signature = 0;

	guiStyle = 1;
	bSpeedUp = true;
	bInfoText = false;

	// load fonts to display stuff
	font.loadFont("fonts/Quicksand-Light.otf", 48, true, true, true);
	monoFont.loadFont("fonts/Batang.ttf", 36, true, true, true);
	//titleFont.loadFont("futura_book.otf", 20);

	// Enable some logging information
	ofSetLogLevel(OF_LOG_SILENT);
	// ofSetLogLevel(OF_LOG_NOTICE);

	// Easing
	delay = 0;
	duration = 1000;
	moveDuration = 2000;
	newCount = 0;

	// Touch
	scrollPos = 0;
	maxScroll = 0;
	targetPos = 0;
	momentum = 0;
	lastMdTime = 0;
	maxId = 0;
	bFirstLoad = true;

	// Lightbox
	texScreen.allocate(ofGetWidth(),ofGetHeight(),GL_RGB);
	fbo1.allocate(1920,1080,GL_RGB);
	fbo2.allocate(1920,1080,GL_RGB);
	imageFbo.allocate(640,640,GL_RGB);
	imageTexture.allocate(640,640,GL_RGB);
	shaderv.load("shaders/blur.vert", "shaders/blurv.frag");
	shaderh.load("shaders/blur.vert", "shaders/blurh.frag");
	shader.load("shaders/blur.vert", "shaders/darker.frag");
	filterShader.load("shaders/blur.vert", "shaders/basicfilter.frag");
	bgShader.load("shaders/bg.vert", "shaders/bg.frag");
	appState = 1;

	circleShader.load("shaders/circle.vert", "shaders/circle.frag");
	circleMask.loadImage("imgs/circlemask.png");

	mousePressedTime = 0;
	bMousePressed = false;
	bGotAllMedia = false;


	imgLogo.loadImage("imgs/logo.png");

	// Frame
	if( XML.loadFile("FrameSettings.xml") ){
		cout << "FrameSettings.xml loaded!" << "\n";
	}else{
		cout << "unable to load FrameSettings.xml check data/ folder" << "\n";
	}

	instatag = XML.getValue("settings:tag", "no");
	eventId = XML.getValue("settings:event_id", "1");
	begintime = XML.getValue("settings:begintime", "100");
	bShowPrintQueue = XML.getValue("settings:show_print_queue", false);
	bShowPrinted = XML.getValue("settings:show_printed", false);
	bShowOtherImage = XML.getValue("settings:show_other_image", false);
	msDivider = XML.getValue("touch:speed_divider", 3);
	msDelayDivider = XML.getValue("touch:delay_divider", 150);

	// Background

	printBgColor = XML.getValue("settings:print_background", "blur");
	bUseImageBg = false;
	if(printBgColor.find(".")) {
		bUseImageBg = true;
		imgBackground.loadImage("backgrounds/"+printBgColor);
	}
	
	XML.pushTag("frames", 0);
	numFrames = XML.getNumTags("frame");

	cout << "numFrames" << numFrames << "\n";

	if(numFrames > 0) {

		if(numFrames > 1) {
			drawX = 300;
			drawY = 100;
			drawFramesX = 100;
		} else {
			drawX = 200;
			drawY = 100;
			drawFramesX = 0;
		}

		float beginY = 0;
		for(int i = 0; i < numFrames; i++) {
			XML.pushTag("frame", i);

			mpFrame frame;

			frame.frameWidth = XML.getValue("background:width", 2400);
			frame.frameHeight = XML.getValue("background:height", 3600);
			frame.imageX = XML.getValue("image:x", 0);
			frame.imageY = XML.getValue("image:y", 0);
			frame.imageWidth = XML.getValue("image:width", frame.frameWidth-(frame.imageX*2));
			frame.imageHeight = XML.getValue("image:height", frame.imageWidth);

			frame.ratio = frame.frameWidth/frame.frameHeight;
			float height = 980-drawY*2;

			frame.fullBackground.loadImage("frames/"+XML.getValue("background:src", "frame.png"));
			frame.background = frame.fullBackground;
			frame.background.resize(height*frame.ratio, height);
			string overlayPath = XML.getValue("overlay:src", "none");
			if(overlayPath == "none") {
			} else {
				frame.fullOverlay.loadImage("frames/"+overlayPath);
				frame.overlay = frame.fullOverlay;
				frame.overlay.resize(height*frame.ratio, height);
			}
			
			float drawWidth = 100;
			float drawHeight = drawWidth/frame.ratio;
			frame.rect.set(drawFramesX, beginY+drawY, drawWidth, drawHeight);
			beginY += 50+drawHeight;

			frames.push_back(frame);

			cout << "Frame " << i << " loaded." << "\n";

			cout << "rect = " << drawFramesX << "," << beginY+drawY << "," << drawWidth << "," << drawHeight << "\n";

			XML.popTag();
			
		}
	} 

	currentFrameIdx = 0;

	// --- Brightness Init

	brightnessSlider.setup(drawX, 1080-100-23, 520, 30, 20, 180, 100, false, false);
	brightnessImg1.loadImage("imgs/brightness_d.png");
	brightnessImg2.loadImage("imgs/brightness_b.png");

	//brightnessImg1.loadImage();

	// --- Button Init

	int btnH = 60;
	int btnW = 60;
	int startH = 1086; //1320;
	int startV = 280;

	for(int i = 0; i < 9; i++) {
		btnNumPrint[i].setup(startH+150*(i%3), startV+150*floor(i/3), 100, 100, ofToString(i+1), 2);
	}
	btnPrint.setup(startH, startV+150*3, 400, 100, "PRINT");
	btnPrint.bInverse = true;
	btnClose.setup(startH, startV+150*4, 400, 100, "CLOSE");
	isClicked = false;

	// Layout Settings
	int fsHeight = 1080;
	ipc = 3; // Items Per Column
	imgPadding = 0;
	headerHeight = 100;
	blockWidth = (fsHeight - headerHeight - (imgPadding*2)) / ipc;
	blockHeight = blockWidth;
	imgWidth = blockWidth - imgPadding*2;
	imgHeight = imgWidth;
	
	bool bb = printerImg.loadImage("imgs/printer.png");
	printImgX = (imgWidth - printerImg.width)/2.0;
	printImgY = (imgHeight - printerImg.height)/2.0;

	bInitialized = true;
	// ----- End Websocket

	// 3D
	cam.setFov(60);

	// --- Printer Init
	ofAddListener( mprinter.printUpdateEvent , this, &ofApp::onPrintUpdate ) ;
	mprinter.init();

	// Http Request
	serverUrl = "http://103.245.167.79/memoprint/public/";
	mpServer.serverUrl = serverUrl;
	ofAddListener( mpServer.mpHttpEvent, this, &ofApp::onMessage ) ;
	mpServer.sendMessage("event/"+eventId);
	
	// --- Timer Init
	updateTimer.setup( 3000 );
	ofAddListener( updateTimer.TIMER_COMPLETE , this, &ofApp::updateTimerHandler ) ;
	updateTimer.start( true );

	// --- Adjustments
	incPadX = 0;
	incPadY = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
			
	for(int i = 0; i < (int)images.size(); i++) {
		images[i].image->update();
	}
	for(int i = 0; i < (int)users.size(); i++) {
		users[i].image->update();
	}
	// Buttons
	
	if(appState == 2) {
		
		int cpa = -1;
		for(int i = 0; i < 9; i++) {
			if(btnNumPrint[i].getClicked()) {
				cpa = i;
			}
		}
		if(cpa > -1) {
			for(int i = 0; i < 9; i++) {
				if(i == cpa) {
					btnNumPrint[i].setToggle(true);
				} else {
					btnNumPrint[i].setToggle(false);
				}
			}
		}
		if(btnClose.getClicked()) {
			bgTween.setParameters(easingquart,ofxTween::easeOut,255,0,300,delay);
			texScreen.loadScreenData(0,0,ofGetWidth(),ofGetHeight());
			appState = 3;
		} else if(btnPrint.getClicked()) {
			int numCopy = 1;
			for(int i = 0; i < 9; i++) {
				if(btnNumPrint[i].getToggle()) {
					numCopy = i+1;
					break;
				}
			}
			imageFbo.begin();
			filterShader.begin();
			filterShader.setUniform1f("amt", brightnessSlider.getValue()/100);
				images[selImgIdx].image->originalImage.setUseTexture(true);
				images[selImgIdx].image->originalImage.draw(0,0);
			filterShader.end();
			imageFbo.end();
			imageFbo.readToPixels(imagePixels);
			colorImg.setFromPixels(imagePixels);
			
			mprinter.addToPrintQueue(&images[selImgIdx],&colorImg,&frames[currentFrameIdx], numCopy, brightnessSlider.getValue(), state2Signature);

			// --- Closing
			bgTween.setParameters(easingquart,ofxTween::easeOut,255,0,300,delay);
			texScreen.loadScreenData(0,0,ofGetWidth(),ofGetHeight());
			appState = 3;
		}
	} else if(appState == 3) {
		if(bgTween.isCompleted()) {
			appState = 1;
		}
	}

	// --- Timer Update
	updateTimer.update();
	mprinter.updateTimer.update();

}

//--------------------------------------------------------------
void ofApp::draw(){

	debugString = "";

	ofSetColor(155);
	bgShader.begin();
	bgShader.setUniform1f("windowWidth", ofGetWidth());
	bgShader.setUniform1f("windowHeight", ofGetHeight());
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
	bgShader.end();

	ofSetColor(0,0,0,180);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());

	// Header
	ofSetColor(0,0,0,180);
	ofRect(0, 0, ofGetWidth(), headerHeight);
	ofSetColor(255);
	imgLogo.draw(40,20,49,60);

	

	if(guiStyle == 1) {

		// Touch
		maxScroll = (ceil(images.size()*1.0/ipc))*blockWidth - (ofGetWidth() - imgPadding*2);
		//cout << "maxScroll = " << maxScroll << endl;
		if(maxScroll < 0) maxScroll = 0;

		if(appState == 2) {
			scrollPos = scrollPos + momenTween.update();
			bMouseChanged = false;
			bMousePressed = false;
			targetPos = scrollPos;
		} else {
			if (bMouseChanged && !bMousePressed) {
				momentum = (targetPos-scrollPos)/msDivider;
				momenTween.setParameters(easingquart, ofxTween::easeOut, momentum, 0, duration, delay);
			}
			if(bMousePressed) {
				if(bMouseChanged) {
					targetPos = scrollPos;
				}
				scrollPos = scrollPos + ((ofGetLastFrameTime()*1000*(targetPos-scrollPos))/msDelayDivider);
			} else {
				scrollPos = scrollPos + momenTween.update();
				if(scrollPos > maxScroll) scrollPos = maxScroll;
			}
			if(bMouseChanged) {
				bMouseChanged = false;
			}
				
			if(scrollPos < 0) scrollPos = 0;
			else if(scrollPos > maxScroll) scrollPos = maxScroll;
		}

		float startH = imgPadding;
		float startV = headerHeight + startH;
		int lidx = (int)images.size() - 1;

		if(multitween.isCompleted()) {
			debugString += "multitween completed!";
			for(int i = 0; i <= lidx; i++) {
				if(images[lidx-i].isPrinting) {
					ofPushStyle();
					ofSetColor(150,150,150,150);
					images[lidx-i].image->draw(images[lidx-i].x-scrollPos,images[lidx-i].y);
					ofSetColor(255,255,255,255);
					printerImg.draw(images[lidx-i].x-scrollPos+printImgX,images[lidx-i].y+printImgY);
					ofPopStyle();
				} else {
					images[lidx-i].image->draw(images[lidx-i].x-scrollPos,images[lidx-i].y);
				}
			}
		} else {
			debugString += "multitween!";
			if (lidx >= 0) {
				multitween.update();
				multitween.getTarget(1);
			}
			float alpha1 = alphatween1.update();
			float alpha2 = alphatween2.update();
			for(int i = 1; i <= lidx; i++) {
				if(i%ipc < newCount && i > newCount) {
					if(alphatween1.isCompleted()) {
						ofSetColor(255,255,255,alpha2);
						images[lidx-i].image->draw(images[lidx-i].x-scrollPos,multitween.getTarget(i*2+1));
						ofSetColor(255);
					} else {
						ofSetColor(255,255,255,alpha1);
						images[lidx-i].image->draw(images[lidx-i+newCount].x-scrollPos,images[lidx-i+newCount].y);
						ofSetColor(255);
					}
				} else if(i >= newCount) {
					if(i%ipc < newCount) {
						ofSetColor(255,255,255,multitween.getTarget(i*2));
						images[lidx-i].image->draw(images[lidx-i].x-scrollPos,multitween.getTarget(i*2+1));
						ofSetColor(255);
					} else {
						images[lidx-i].image->draw(images[lidx-i].x-scrollPos,multitween.getTarget(i*2+1));
					}

				}
			}
		}
	} else if (guiStyle == 2) {

		scrollPos = scrollPos + momenTween.update();
		bMouseChanged = false;
		bMousePressed = false;
		targetPos = scrollPos;

		// 3D
		ofEnableDepthTest();
		//ofEnableNormalizedTexCoords();

		float movementSpeed = .1;
		float cloudSize = ofGetWidth() / 2;
		float maxBoxSize = 100;
		float spacing = 1;

		float boxDepth = 10;
	
		ofPushStyle();
		//cam.disableMouseInput();
		//cam.setFov(30);
		
		cam.setPosition(1920/2-imgPadding,1080/2-imgPadding,2020);
		cam.setFov(30);
		cam.begin();

		/*float tx = -ofGetWidth() / 2;
		float ty = -ofGetHeight() / 2;*/
		float tx = blockWidth/2;
		float ty = blockHeight/2;
		float tz = -blockWidth/2;
	
		//for(int i = 0; i < images.size(); i++) {
		//	ofPushMatrix();
		//
		//	float t = (ofGetElapsedTimef() + i * spacing) * movementSpeed;
		//	ofVec3f pos(
		//		ofSignedNoise(t, 0, 0),
		//		ofSignedNoise(0, t, 0),
		//		ofSignedNoise(0, 0, t));
		//
		//	float boxSize = maxBoxSize * ofNoise(pos.x, pos.y, pos.z);
		//
		//	pos *= cloudSize;
		//	ofTranslate(pos);
		//	ofRotateX(pos.x);
		//	ofRotateY(pos.y);
		//	ofRotateZ(pos.z);

		//	images[i].image->draw(0,0,boxSize,boxSize);
		//
		//	/*images[i].image->bind();
		//	ofFill();
		//	ofSetColor(255);
		//	ofDrawBox(boxSize,boxSize,boxDepth);
		//	images[i].image->unbind();*/
		//
		//	/*ofNoFill();
		//	ofSetColor(ofColor::fromHsb(sinf(t) * 128 + 128, 255, 255));
		//	ofDrawBox(boxSize * 1.1f);*/
		//
		//	ofPopMatrix();
		//}

		int maxBox = 18;
		float ef = ofGetElapsedTimef();
		float bw = blockWidth;
		float iw = imgWidth;
		for(int i = 0; i<maxBox && i<images.size(); i++) {
			ofPushMatrix();
			ofTranslate((tx+(floor(i/ipc)*bw)),ty+floor(i%ipc)*bw,0);
			ofRotateY(ef*50);
			ofTranslate(-tx,-ty,0);

		
			//float t = (ofGetElapsedTimef() + i * spacing) * movementSpeed;
			//ofVec3f pos(
			//	ofSignedNoise(t, 0, 0),
			//	ofSignedNoise(0, t, 0),
			//	ofSignedNoise(0, 0, t));
		
			//float boxSize = maxBoxSize * ofNoise(pos.x, pos.y, pos.z);
		
			//pos *= cloudSize;
			////ofTranslate(pos);
			//ofRotateX(pos.x);
			//ofRotateY(pos.y);
			//ofRotateZ(pos.z);

			images[i].image->draw(imgPadding,imgPadding,iw,iw);
		
			/*images[i].image->bind();
			ofFill();
			ofSetColor(255);
			ofDrawBox(imgWidth,imgWidth,imgWidth);
			images[i].image->unbind();*/
		
			/*ofNoFill();
			ofSetColor(ofColor::fromHsb(sinf(t) * 128 + 128, 255, 255));
			ofDrawBox(boxSize * 1.1f);*/
		
			ofPopMatrix();
		}
		cam.end();
		ofPopStyle();
		ofDisableDepthTest();
		ofDisableNormalizedTexCoords();

	}

	//scrollPos = scrollPos + (ofGetLastFrameTime()*100*(targetPos-scrollPos)/12);
	
	
	if( appState == 1 ) {
		if(bShowPrintQueue) {
			int lidx = mprinter.printQueue.size()-1;
			int dx = ofGetWidth() - 100;
			for(int i=0; i<=lidx; i++) {
				mprinter.printQueue[lidx-i].imageRef->image->draw(dx,20,60,60);
				dx -= 70;
			}
		} else {
			int strw = font.stringWidth(instatag);
			font.drawString(instatag,(1920 - 40) - strw, 20+48);
			monoFont.drawString("#",(1920 - 50) - strw - (monoFont.stringWidth("#")), 20+48);
		}
	} else if( appState == 3) {
		ofSetColor(255,255,255,bgTween.update());
		texScreen.draw(0,0,ofGetWidth(),ofGetHeight());
	} else if( appState == 2) {

		// Lightbox
		if(printBgColor == "blur") {
			texScreen.loadScreenData(0,0,ofGetWidth(),ofGetHeight());
			fbo2.begin();
			texScreen.draw(0,0,ofGetWidth(),ofGetHeight());
			fbo2.end();

			for (int i=0; i<8; i++) {
				fbo1.begin();
				shaderh.begin();
				shaderh.setUniform1f("amt", 5); 
				fbo2.draw(0,0);
				shaderh.end();
				fbo1.end();
			
				fbo2.begin();
				shaderv.begin();
				shaderv.setUniform1f("amt", 5);
				fbo1.draw(0,0);
				shaderv.end();
				fbo2.end();
			}

			shader.begin();
			shader.setUniform1f("amt", 0.22);
			shader.setUniform1f("alpha", bgTween.update());
			fbo2.draw(0,0);
			shader.end();
		} else if(bUseImageBg) {
			ofSetColor(255,255,255, floor(bgTween.update()*255));
			imgBackground.draw(0,0,ofGetWidth(),ofGetHeight());
			ofSetHexColor(255);
		} else {
			ofSetColor(ofColor(ofHexToInt(printBgColor)), floor(bgTween.update()*255));
			ofRect(0,0,ofGetWidth(),ofGetHeight());
			ofSetHexColor(255);
		}

		// Draw Frames

		if(frames.size() > 0) {

			if(frames.size() > 1) {

				// Chooser

				for(int i = 0; i < frames.size(); i++) {

					float frameWidth = frames[i].frameWidth;
					float frameHeight = frames[i].frameHeight;
					float imageX = frames[i].imageX;
					float imageY = frames[i].imageY;
					
					float drawWidth = 100;
					float ratio = frames[i].ratio;
					float drawHeight = drawWidth/ratio;

					float scale = drawWidth/frameWidth;

					if(i != currentFrameIdx) {
						ofSetColor(255,255,255,150);
					} else {
						ofSetColor(209,122,144);
						int lineWidth = 5;
						ofRect(frames[i].rect.x-lineWidth, frames[i].rect.y-lineWidth, drawWidth+lineWidth*2, drawHeight+lineWidth*2);
						ofSetColor(255,255,255,255);
					}
					frames[i].background.draw(frames[i].rect.x, frames[i].rect.y, drawWidth, drawHeight);
					images[selImgIdx].image->draw(drawFramesX+imageX*scale,frames[i].rect.y+imageY*scale,frames[i].imageWidth*scale,frames[i].imageHeight*scale);
					if(frames[i].overlay.isAllocated()) {
						frames[i].overlay.draw(frames[i].rect.x, frames[i].rect.y, drawWidth, drawHeight);
					}
					if(i != currentFrameIdx) {
						ofSetColor(255,255,255,255);
					}
				}
			}

			// Draw Preview Frames

			int i = currentFrameIdx;

			float frameWidth = frames[i].frameWidth;
			float frameHeight = frames[i].frameHeight;
			float imageX = frames[i].imageX;
			float imageY = frames[i].imageY;
			float imageWidth = frames[i].imageWidth;
			float imageHeight = frames[i].imageHeight;
			
			float ratio = frames[i].ratio;

			if(ratio < 1) {

				float drawHeight = ofGetHeight()-(drawY*2)-100;
				float drawWidth = drawHeight*ratio;

				float scale = drawHeight/frameHeight;

				// cout << drawX+drawWidth << "\n";

				frames[i].background.draw(drawX, drawY, drawWidth, drawHeight);
				filterShader.begin();
				filterShader.setUniform1f("amt", brightnessSlider.getValue()/100);
					images[selImgIdx].image->originalImage.setUseTexture(true);
					images[selImgIdx].image->originalImage.draw(drawX+incPadX+imageX*scale,drawY+incPadY+imageY*scale,imageWidth*scale*incZoom,imageHeight*scale*incZoom);
				filterShader.end();
				//setBrightness(brightnessSlider.getValue());
				//imageTexture.draw(drawX+imageX*scale,drawY+imageY*scale,imageWidth*scale,imageHeight*scale);
				if(frames[i].overlay.isAllocated()) {
					frames[i].overlay.draw(drawX, drawY, drawWidth, drawHeight);
				}

			} else {

				float drawHeight1 = ofGetHeight()-(drawY*2)-100;
				float drawWidth = drawHeight1/ratio;
				float drawHeight = drawWidth/ratio;

				float scale = drawHeight/frameHeight;

				frames[i].background.draw(drawX, drawY+((drawHeight1-drawHeight)/2), drawWidth, drawHeight);
				filterShader.begin();
				filterShader.setUniform1f("amt", brightnessSlider.getValue()/100);
					images[selImgIdx].image->originalImage.setUseTexture(true);
					images[selImgIdx].image->originalImage.draw(drawX+imageX*scale,(drawY+((drawHeight1-drawHeight)/2))+imageY*scale,imageWidth*scale,imageHeight*scale);
				filterShader.end();
				if(frames[i].overlay.isAllocated()) {
					frames[i].overlay.draw(drawX, drawY+((drawHeight1-drawHeight)/2), drawWidth, drawHeight);
				}

			}
		}
		
		// Brightness
		brightnessImg1.draw(drawX - 52, 1080-100-32, 48,48);
		brightnessImg2.draw(drawX + 528, 1080-100-32, 48,48);

		// GUI
		ofSetHexColor(0x123456);
		ofRect(btnNumPrint[0].x, 208, 400, 2);
		ofSetColor(255);

		// Buttons

		int sw = font.stringWidth("AMOUNT");
		int sh = font.stringHeight("AMOUNT");
	
		ofSetHexColor(0x123456);
		font.drawString("AMOUNT",1086+((400-sw)/2),100+sh);
		ofSetColor(255);

		for(int i = 0; i < 9; i++) {
			btnNumPrint[i].draw(&font);
		}
		btnPrint.draw(&font);
		btnClose.draw(&font);

		// Search
		if(bShowOtherImage) {
			sh = 1720;
			int sv = 100;

			/*for(int i=0; i<users.size(); i++) {
				if(users[i].id == images[selImgIdx].user_id) {
					circleShader.begin();
					circleShader.setUniformTexture("imageMask", circleMask.getTextureReference(), 1);
					users[i].image->draw(sh,100,100,100);
 
					circleShader.end();
					break;
				}
			}*/

			//font.drawString(images[selImgIdx].username,1086+((400-sw)/2),100+sh);

			string username = images[selImgIdx].username;
			for(int i=0; i<images.size(); i++) {
				if(images[i].username == username) {
					if(images[i].id == images[selImgIdx].id) {
						ofSetColor(209,122,144,255);
						int lineWidth = 5;
						ofRect(sh-lineWidth, sv-lineWidth, 100+lineWidth*2, 100+lineWidth*2);
						ofSetColor(255);
						images[i].image->draw(sh,sv,100,100);
					} else {
						ofSetColor(255,255,255,150);
						images[i].image->draw(sh,sv,100,100);
					}
					sv += 125;
				}
				ofSetColor(255);
			}
		}

		//font.drawString("9",500.5,500.5);
	}

	// Slider
	brightnessSlider.setVisible((appState == 2));

	if(bInfoText) {
		stringstream ss;
		ss << "Framerate: " << ofToString(ofGetFrameRate(),0) << "\n";
		ss << "mouseX: " << mouseX << endl << "mouseY:"<< mouseY << endl;
		ss << connectString << endl;
		ss << debugString << endl;
		ss << printString << endl;
		
		ofDrawBitmapString(ss.str().c_str(), 20, 20);
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	string file = "img.jpg";
	string url = "http://scontent-b.cdninstagram.com/hphotos-xfa1/t51.2885-15/10843904_1745274612363555_330395318_n.jpg";

	if(key=='1'){
	} else if(key=='2') {
		//sendMessage("{\"request\":\"getallimage\",\"tag\":\""+instatag+"\"}");
		//sendMessage("{\"request\":\"getrandomimage\",\"tag\":\""+instatag+"\",\"amount\":\""+ofToString(floor(ofRandom(1,1)))+"\",\"begintime\":\""+begintime+"\"}");
	} else if(key=='3') {
		//images.clear();
		//multitween.setParameters(easingquart,ofxTween::easeInOut,0,0,0,0);
		//sendMessage("{\"request\":\"getallimage\",\"tag\":\""+instatag+"\",\"begintime\":\""+begintime+"\"}");
	} else if(key=='4') {
		if(guiStyle == 1) guiStyle = 2;
		else guiStyle = 1;
		//mprinter.getPrintStatus();
	} else if(key=='5') {
		screenGrabber.grabScreen(0,0,ofGetWidth(),ofGetHeight());
		screenGrabber.saveImage("screenshots/"+ofToString(ofGetSystemTime())+".png");
	} else if(key=='6') {
		bInfoText = !bInfoText;
	} else if(key=='f') {
		ofToggleFullscreen();
		texScreen.allocate(ofGetWidth(),ofGetHeight(),GL_RGB);
	} else if(key=='s') {
		incPadY += 5;
	} else if(key=='w') {
		incPadY -= 5;
	} else if(key=='d') {
		incPadX += 5;
	} else if(key=='a') {
		incPadX -= 5;
	} else if(key=='q') {
		incZoom += 0.05;
	} else if(key=='e') {
		incZoom -= 0.05;
	} 
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
	if(!(abs(x-mousePressPos.x) < 10 && abs(y-mousePressPos.y) < 10))
		hasFocus = false;
	// mouse = ofVec2f(x, y);
	//mouseLast = mouse;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

	if(!(abs(x-mousePressPos.x) < 10 && abs(y-mousePressPos.y) < 10)) {
		hasFocus = false;
	}

	if( appState == 1 ){
		ofVec2f mouse = ofVec2f(x, y);
		ofVec2f mouseVel = mouseLast - mouse;
		unsigned long currMdTime = ofGetElapsedTimeMillis();
		int et = currMdTime - lastMdTime;
		if(et < 1) et = 1;
		//momentum = (mouseVel.x)/et;
		//cout << momentum << endl;
		lastMdTime = currMdTime;
		//mouseLast = mouse;
		//cout << "mouseVel.x = " << mouseVel.x << "\n";
		targetPos = (lastScrollPos + mouseVel.x);
		int limit = ((ceil(images.size()*1.0/ipc))*blockWidth)-ofGetWidth()+(imgPadding*2);
		if(targetPos > limit) targetPos = limit;
		if (targetPos < 0) targetPos = 0;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

	hasFocus = true;
	if(appState == 1) {
		bMousePressed = true;
		bMouseChanged = true;
	}

	if( appState == 1 ){
		ofVec2f mouse = ofVec2f(x, y);
		mouseLast = mouse;
		lastScrollPos = scrollPos;
	}
	mousePressedTime = ofGetElapsedTimeMillis();
	mousePressPos.set(x,y);

	//cout << "mouselast!" << "\n";
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
	if(appState == 1) {
		bMousePressed = false;
		bMouseChanged = true;
	}
	//cout << momentum << endl
	

	if(hasFocus) isClicked = true;
	else isClicked = false;

	if( appState == 1 ){
		if(isClicking()) {
			if(images.size() > 0) {
				bgTween.setParameters(easingquart,ofxTween::easeOut,0,1,duration,delay);
				// Buttons
				for(int i = 0; i < 9; i++) {
					btnNumPrint[i].setToggle(false);
				}
				btnNumPrint[0].setToggle(true);

				// Get Select Image
				if(y > headerHeight + (imgPadding/2)) {
					selImgIdx = (int)images.size() - 1 - (floor((x-imgPadding+scrollPos)/blockWidth)*ipc + floor((y-(headerHeight + imgPadding))/blockHeight));
					//cout << "selImgIdx" << selImgIdx << endl;
					if(selImgIdx >= 0 && selImgIdx < images.size()) {
						if(!images[selImgIdx].image->clickable || images[selImgIdx].isPrinting) selImgIdx = -1;
						if(selImgIdx > images.size()-1 || selImgIdx < 0) selImgIdx = -1;
					} else {
						selImgIdx = - 1;
					}
				} else {
					selImgIdx = - 1;
				}

				//cout << "selImgIdx" << selImgIdx << endl;
				if(selImgIdx != -1) {
					brightnessSlider.resetValue();
					state2Signature = ofGetElapsedTimeMillis();
					appState = 2;
					
					// adjustment
					incPadX = 0;
					incPadY = 0;
					incZoom = 1;
				}
			}
			//selectImage.draw(300, (ofGetHeight()-612)/2, 612, 612);
		}
	} else if( appState == 2 ){
		if(isClicking()) {
			for(int i = 0; i < numFrames; i++) {
				if(frames[i].rect.inside(x, y)) {
					currentFrameIdx = i;
					break;
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

// --- Timer handlers
void ofApp::updateTimerHandler( int &args ) {

	/*if(images.size() > 0) {
		sendMessage("{\"request\":\"getnewimage\",\"tag\":\""+instatag+"\",\"minid\":\""+ofToString(maxId)+"\",\"begintime\":\""+begintime+"\"}");
	} else {
		sendMessage("{\"request\":\"getallimage\",\"tag\":\""+instatag+"\",\"begintime\":\""+begintime+"\"}");
	}
	cout << "check for update ..." << endl;*/
	mpServer.sendMessage("event/"+eventId+"/"+ofToString(maxId));
}


bool ofApp::isClicking() {
	bool isd = isClicked;
	/*int timeElapse = ofGetElapsedTimeMillis() - mousePressedTime;
	if(timeElapse < 200){
		if(ABS(targetPos - lastScrollPos) < 100) {
			isd = true;
		}
	} else if(timeElapse < 800) {
		if(ABS(targetPos - lastScrollPos) < 10) {
			isd = true;
		}
	}*/
	isClicked = false;
	return isd;
}

void ofApp::setBrightness(int x){
 
	 unsigned char * pix = images[selImgIdx].image->getPixels();
	 int numPix = images[selImgIdx].image->width * images[selImgIdx].image->height * 3;

	 for(int i = 0; i <numPix; i+=3){
		 pix[i] += x;
		 pix[i+1] += x;
		 pix[i+2] += x;
		 if(pix[i] < 0) pix[i] = 0;
		 else if(pix[i] > 255) pix[i] = 255;

		 if(pix[i+1] < 0) pix[i+1] = 0;
		 else if(pix[i+1] > 255) pix[i+1] = 255;

		 if(pix[i+2] < 0) pix[i+2] = 0;
		 else if(pix[i+2] > 255) pix[i+2] = 255;
	 }
	 imageTexture.loadData(pix,640,640,GL_RGB);
}

void ofApp::sendMessage( string msg ){

	string url = serverUrl+msg;
	//bool parsingSuccessful = result.open(url);

	cout << "[sending request] " << msg << "\n";
	
	if (!result.open(url)) {
		cout << "[Error] Failed to parse JSON" << "\n";
	} else {
		//cout<<"got message :"<<result["header"]<<endl;
		if(result["header"] == "media") {
			if(result["type"] == "all") {
				if(bGotAllMedia) return;
			}

			int total = result["data"].size();
			newCount = 0;
			if(total > 0) {
				for(int i=total-1; i>=0; i--) {
					string id = result["data"][i]["id"].asString();
					if(result["type"] != "random") {
						bool found = false; 
						for(int i=0; i<images.size(); i++) {
							if(id == images[i].id) {
								found = true;
								break;
							}
						}
						if(found) continue;
					}
					newCount++;
					imageBlock mpi;
					if(maxId < ofToInt(id)) maxId = ofToInt(id);
					mpi.id = id;
					mpi.image = new mpImage;
					mpi.image->constrainImageSize(imgWidth);
					mpi.isPrinting = false;
					mpi.username = result["data"][i]["username"].asString();
					mpi.user_id = result["data"][i]["user_id"].asString();
					images.push_back(mpi);
					string s = result["data"][i]["img_high"].asString();
					size_t f = s.find("https:");
					if (f != std::string::npos) s.replace(f, std::string("https:").length(), "http:");
					images[(int)images.size() - 1].image->loadHttpImageThreaded(s);

					// --- user
					bool found = false;
					string user_id = result["data"][i]["user_id"].asString();
					for(int j=0; j<users.size(); j++) {
						if(users[j].id == user_id) {
							found = true;
							break;
						}
					}
					if(!found) {
						instaUser iu;
						iu.username = result["data"][i]["username"].asString();
						iu.id = user_id;
						iu.image = new mpImage;
						users.push_back(iu);
						string s = result["data"][i]["profile_picture"].asString();
						size_t f = s.find("https:");
						if (f != std::string::npos) s.replace(f, std::string("https:").length(), "http:");
						users[(int)users.size() - 1].image->loadHttpImageThreaded(s);
					}
				}
				int lidx = (int)images.size() - 1;	
				for(int i=0; i<=lidx; i++) {
					float startH = imgPadding;
					float startV = headerHeight + startH;
					for(int i = 0; i <= lidx; i++)  {
						images[lidx-i].x = floor(i/ipc)*blockWidth+imgPadding+startH;
						images[lidx-i].y = (i%ipc)*blockWidth+imgPadding+startV;
					}
				}

				cout << "*** total image : " << images.size() << endl;
				cout << "*** img position : " << images[images.size()-1].x << endl;
				//std::sort(users.begin(), users.end(), compare_by_word);

				if(result["type"] == "all") {
					bGotAllMedia = true;
				}

				if(!bFirstLoad) {
					alphatween1.setParameters(easingquart,ofxTween::easeInOut,255,0,moveDuration/2,0);
					alphatween2.setParameters(easingquart,ofxTween::easeInOut,0,255,moveDuration/2,moveDuration/2);
					multitween.setParameters(easingquart,ofxTween::easeInOut,0,0,moveDuration,delay);
					multitween.addValue(0,0);
					for(int i = 1; i < newCount; i++) {
						multitween.addValue(images[lidx-i].x,images[lidx-i].x);
						multitween.addValue(images[lidx-i].y,images[lidx-i].y);
					}
					for(int i = newCount; i <= lidx; i++) {
						multitween.addValue(0,255);
						multitween.addValue(images[lidx-i].y-blockHeight*newCount,images[lidx-i].y);
					}
					multitween.start();
				}
				bFirstLoad = false;
			}
		}
	}
}

void ofApp::onMessage( ofxJSONElement &result ){

	//cout<<"got message :"<<result["header"]<<endl;
	if(result["header"] == "media") {
		if(result["type"] == "all") {
			if(bGotAllMedia) return;
		}

		int total = result["data"].size();
		newCount = 0;
		if(total > 0) {
			for(int i=total-1; i>=0; i--) {
				string id = result["data"][i]["id"].asString();
				if(result["type"] != "random") {
					bool found = false; 
					for(int i=0; i<images.size(); i++) {
						if(id == images[i].id) {
							found = true;
							break;
						}
					}
					if(found) continue;
				}
				newCount++;
				imageBlock mpi;
				if(maxId < ofToInt(id)) maxId = ofToInt(id);
				mpi.id = id;
				mpi.image = new mpImage;
				mpi.image->constrainImageSize(imgWidth);
				mpi.isPrinting = false;
				mpi.username = result["data"][i]["username"].asString();
				mpi.user_id = result["data"][i]["user_id"].asString();
				images.push_back(mpi);
				string s = result["data"][i]["img_high"].asString();
				size_t f = s.find("https:");
				if (f != std::string::npos) s.replace(f, std::string("https:").length(), "http:");
				images[(int)images.size() - 1].image->loadHttpImageThreaded(s);

				// --- user
				bool found = false;
				string user_id = result["data"][i]["user_id"].asString();
				for(int j=0; j<users.size(); j++) {
					if(users[j].id == user_id) {
						found = true;
						break;
					}
				}
				if(!found) {
					instaUser iu;
					iu.username = result["data"][i]["username"].asString();
					iu.id = user_id;
					iu.image = new mpImage;
					users.push_back(iu);
					string s = result["data"][i]["profile_picture"].asString();
					size_t f = s.find("https:");
					if (f != std::string::npos) s.replace(f, std::string("https:").length(), "http:");
					users[(int)users.size() - 1].image->loadHttpImageThreaded(s);
				}
			}
			int lidx = (int)images.size() - 1;	
			for(int i=0; i<=lidx; i++) {
				float startH = imgPadding;
				float startV = headerHeight + startH;
				for(int i = 0; i <= lidx; i++)  {
					images[lidx-i].x = floor(i/ipc)*blockWidth+imgPadding+startH;
					images[lidx-i].y = (i%ipc)*blockWidth+imgPadding+startV;
				}
			}

			cout << "*** total image : " << images.size() << endl;
			cout << "*** img position : " << images[images.size()-1].x << endl;
			//std::sort(users.begin(), users.end(), compare_by_word);

			if(result["type"] == "all") {
				bGotAllMedia = true;
			}

			if(!bFirstLoad) {
				alphatween1.setParameters(easingquart,ofxTween::easeInOut,255,0,moveDuration/2,0);
				alphatween2.setParameters(easingquart,ofxTween::easeInOut,0,255,moveDuration/2,moveDuration/2);
				multitween.setParameters(easingquart,ofxTween::easeInOut,0,0,moveDuration,delay);
				multitween.addValue(0,0);
				for(int i = 1; i < newCount; i++) {
					multitween.addValue(images[lidx-i].x,images[lidx-i].x);
					multitween.addValue(images[lidx-i].y,images[lidx-i].y);
				}
				for(int i = newCount; i <= lidx; i++) {
					multitween.addValue(0,255);
					multitween.addValue(images[lidx-i].y-blockHeight*newCount,images[lidx-i].y);
				}
				multitween.start();
			}
			bFirstLoad = false;
		}
	}
}

void ofApp::notifyError( string msg ){
	string message = ("{\"request\":\"error\",\"tag\":\""+instatag+"\",\"data\":\""+msg+"\"}");
	sendMessage(message);
	cout << "Error: " << msg << endl;
}

void ofApp::onPrintUpdate( string &args ) {
	printString = "";
	if(args == "update" || args == "add") {
		jsender.empty();
		jsender["request"] = "print";
		jsender["tag"] = instatag;
		int cq = 0;
		for(int i = 0; i<mprinter.printQueue.size(); i++) {
			try {
				if(!mprinter.printQueue[i].isSynced) {
					jsender["add"][cq]["jobId"] = mprinter.printQueue[i].id;
					jsender["add"][cq]["imageId"] = mprinter.printQueue[i].imageId;
					jsender["add"][cq]["createdTime"] = mprinter.printQueue[i].createdTime;
					jsender["add"][cq]["fileName"] = mprinter.printQueue[i].fileName;
					jsender["add"][cq]["totalPrint"] = ofToString(mprinter.printQueue[i].totalPrint);
					jsender["add"][cq]["brightness"] = ofToString(mprinter.printQueue[i].brightness);
					jsender["add"][cq]["status"] = "0";
					jsender["add"][cq++]["printNumber"] = ofToString(mprinter.printQueue[i].printNumber);
					mprinter.printQueue[i].isSynced = true;
				}
			} catch (out_of_range&) {
				cout << "Error: " << "printQueue index out of range" << endl;
			}
			printString += mprinter.printQueue[i].id+": a = "+ofToString(mprinter.printQueue[i].bAdded)+", c = "+ofToString(mprinter.printQueue[i].bInQueue)+"\n";
		}
		int cp = 0;
		/*cout << "--------------------------------------------------" << endl;
		cout << jsender.getRawString() << endl;*/
		for(int i = mprinter.printedList.size()-1; i>=0; i--) {
			try {
				if(!mprinter.printedList[i].isSynced) {
					jsender["print"][cp]["jobId"] = mprinter.printedList[i].id;
					jsender["print"][cp]["imageId"] = mprinter.printedList[i].imageId;
					jsender["print"][cp]["createdTime"] = mprinter.printedList[i].createdTime;
					jsender["print"][cp]["fileName"] = mprinter.printedList[i].fileName;
					jsender["print"][cp]["totalPrint"] = ofToString(mprinter.printedList[i].totalPrint);
					jsender["print"][cp]["brightness"] = ofToString(mprinter.printedList[i].brightness);
					jsender["print"][cp]["status"] = "1";
					jsender["print"][cp++]["printNumber"] = ofToString(mprinter.printedList[i].printNumber);
					mprinter.printedList[i].isSynced = true;
				}
			} catch (out_of_range&) {
				cout << "Error: " << "printQueue index out of range" << endl;
			}
			printString += "printed: "+mprinter.printedList[i].id+"\n";
		}
		if(cp+cq > 0) {
			//sendMessage(jsender.getRawString());
		}
		//sendMessage("{\"request\":\"haha\",\"tag\":\""+instatag+"\",\"data\":\"nodata\"}");
	} else {
		// notifyError(args);
	}
}