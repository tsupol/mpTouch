#pragma once

#include "ofMain.h"
#include "mpImage.h"
#include "ofEvents.h"
#include "ofxSimpleTimer.h"

#include <Winspool.h>
#include "tchar.h"
#include "WinBase.h"
//#import "C:\Projects\Memoprinter\Memoprinter\bin\Debug\Memoprinter.tlb"
#import "C:\Projects\Memoprinter\Memoprinter\bin\Release\Memoprinter.tlb"
//#import "C:\Projects\Memoprinter\Memoprinter\bin\Debug\Memoprinter.tlb"

class mpPrinter;

struct mpPrinterEvent{
	bool loaded;
	mpPrinter *	image;
	mpPrinterEvent(){
		loaded = true;
		image = NULL;
	}
};

struct instaUser {
	mpImage* image;
	string id;
	string username;
};

struct imageBlock {
	mpImage* image;
	string id;
	int x;
	int y;
	bool isPrinting;
	string username;
	string user_id;
};

struct printJob {
	string id;
	string jodId;
	string imageId;
	string createdTime;
	string fileName;
	string tag;
	int refId;
	int totalPrint;
	int brightness;
	int printNumber;
	bool isSynced;
	bool bInQueue;
	bool bAdded;
	unsigned long addedTime;
	imageBlock*	imageRef;
};

struct mpFrame {

	ofImage background;
	ofImage overlay;

	ofImage fullBackground;
	ofImage fullOverlay;

	ofRectangle rect;

	float frameWidth;
	float frameHeight;
	float imageX;
	float imageY;
	float ratio;
	float imageWidth;
	float imageHeight;

};

class mpPrinter : public ofThread {

	public:

		mpPrinter();	
		virtual ~mpPrinter();
		void mpPrinter::addToPrintQueue(imageBlock* _imageBlock, ofImage* _image, mpFrame* _frame, int _numPrint, int _brightness, int signature);
		void mpPrinter::init();
		void mpPrinter::getPrintStatus();
		void mpPrinter::threadedFunction();
		void mpPrinter::addToPrintQueue(printJob* pj);
		int mpPrinter::getPrintJobCount();
		void mpPrinter::print(string filename, int numPrint, string jobName);

		bool mpPrinter::updatePrintQueue();
		void mpPrinter::printAddedHandler(int &args);
		/*void mpPrinter::getPrintQueue();
		void mpPrinter::getprintedList();*/
		
		deque<printJob>	printQueue;
		deque<printJob>	printedList;
		ofEvent<std::string>	printUpdateEvent;

		// --- Timer
		ofxSimpleTimer	updateTimer;
		void			updateTimerHandler( int &args ) ;

	private:
		ofImage*		image;
		string			whatToDo;
		string			imageId;
		int				brightness;
		mpFrame*		frame;
		int				numPrint;
		int				virtualPrintJob;
		int				eventNum;
		vector<string>	printingList;
		imageBlock*		imageRef;
		int				lastSignature;
};


