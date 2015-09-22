#include "mpPrinter.h";

using namespace std;
using namespace Memoprinter;

mpPrinter::mpPrinter(){
	virtualPrintJob = 0;
}

mpPrinter::~mpPrinter(){
	if (isThreadRunning()){
		try {
			waitForThread(true);
		}catch(Exception &ex) {
			ofLogError("mpImage", "Exception at waitForThread %s\n", ex.displayText().c_str() );
		}
	}
}

void mpPrinter::init(){
	updateTimer.setup( 500 );
	ofAddListener( updateTimer.TIMER_COMPLETE , this, &mpPrinter::updateTimerHandler ) ;
	updateTimer.start( true );
	eventNum = 1;
}

void mpPrinter::addToPrintQueue(imageBlock* _imageRef, ofImage* _image, mpFrame* _frame, int _numPrint, int _brightness, int signature) {

	// Prevent Double Add
	if(lastSignature == signature) return;
	image = _image;
	frame = _frame;
	numPrint = _numPrint;
	brightness = _brightness;
	imageRef = _imageRef;
	imageId = imageRef->id;
	lastSignature = signature;

	//getFilePath(), getFileName()
	whatToDo = "save";
	startThread(false, false);   // !blocking, !verbose

}

void mpPrinter::threadedFunction() {

	if( lock() ){
		if(whatToDo == "save") {
			bool isAdded = false;
			unsigned long startTime = ofGetElapsedTimeMillis();

			int width = frame->fullBackground.getWidth();
			int height = frame->fullBackground.getHeight();
			
			float ratio = (float)width/(float)height;

			int w = frame->imageWidth;
			int h = frame->imageHeight;
			int ox = frame->imageX;
			int oy = frame->imageY;

			image->resize(w,h);
			int totaPixel = width * height;

			ofPixels pix = frame->fullBackground.getPixelsRef();
			ofPixels pix2 = image->getPixelsRef();

			for(int x=0; x < w; x++) {
				for(int y=0; y < h; y++) {
					pix.setColor(x+ox,y+oy,pix2.getColor(x,y));
				}
			}
			if(frame->fullOverlay.bAllocated()) {
				ofPixels pix3 = frame->fullOverlay.getPixelsRef();
				for(int i=0; i<totaPixel; i++) {
					int is = i*3;
					int id = i*4;
					double a = pix3[id+3] / 255.0;
					//cout << "r1: " << pix[is] << " r2: " << pix3[id] << " a2: " << pix3[id+3] << endl;
					pix[is] = pix[is] * 1 + pix3[id] * a - 1 * a * pix[is];
					pix[is+1] = pix[is+1] * 1 + pix3[id+1] * a - 1 * a * pix[is+1];
					pix[is+2] = pix[is+2] * 1 + pix3[id+2] * a - 1 * a * pix[is+2];
				}
			}

			// make it very bright
			if(false) {
				for(int x=0; x < width; x++) {
					for(int y=0; y < height; y++) {
						if(ofRandom(10) >= 1) pix.setColor(x,y,255);
						else pix.setColor(x,y,pix.getColor(x,y) + 180);
					}
				}
			}

			cout << "ratio " << ratio << " - " << width << " - " << height << endl;

			// padding for printer
			ofPixels dest;

			if(ratio > 1) {
				int destw = width*1.032; //1.015
				int desth = height*1.012;//1.035
				int py = (destw-width)/2;
				int px = ((desth-height)/2)-height/900;
				dest.allocate(desth, destw, OF_IMAGE_COLOR);
				dest.setColor(ofColor(255,255));

				for(int x=0; x < height; x++) {
					for(int y=0; y < width; y++) {
						dest.setColor(x+px,y+py,pix.getColor(width-y,x));
					}
				}

			} else {
				int destw = width*1.012; //1.015
				int desth = height*1.032;//1.035
				int px = (destw-width)/2;
				int py = ((desth-height)/2)-height/900;
				dest.allocate(destw,desth, OF_IMAGE_COLOR);
				dest.setColor(ofColor(255,255));

				for(int x=0; x < width; x++) {
					for(int y=0; y < height; y++) {
						dest.setColor(x+px,y+py,pix.getColor(x,y));
					}
				}
			}
			
			unsigned long ct = ofGetSystemTime();
			string createdTime = ofToString(ct);
			string jobId = imageId+"_"+createdTime+"_"+ofToString(brightness);
			string fileName = "tempImg/"+jobId+".png";
			fileName = ofToDataPath(fileName);
			ofSaveImage(dest, fileName);
			cout << "image saved to " << fileName << "[" << ofGetElapsedTimeMillis()-startTime << "]" << endl;
			for(int i=1; i<=numPrint; i++) {
				printJob pj;
				pj.brightness = brightness;
				pj.totalPrint = numPrint;
				pj.createdTime = createdTime;
				pj.imageId = imageId;
				pj.printNumber = i;
				pj.id = jobId+"_"+ofToString(i);
				pj.fileName = jobId+".png";
				pj.isSynced = false;
				pj.imageRef = imageRef;
				pj.imageRef->isPrinting = true;
				pj.bInQueue = false;
				pj.addedTime = ct;
				pj.bAdded = false;
				addToPrintQueue(&pj);
			}
			string estr = ofToString("add");
			ofNotifyEvent(printUpdateEvent,estr,this);
			unlock();
			/*if(numPrint > 0) {
				if (!updatePrintQueue()) {
					string estr = ofToString("cannot open printer");
					ofNotifyEvent(printUpdateEvent,estr,this);
				}
			}*/
			
		}
	}
	stopThread();
}

void mpPrinter::addToPrintQueue(printJob* pj) {

	// Check for duplicate
	for(int i=0; i<printQueue.size(); i++) {
		if(printQueue[i].id == pj->id) return;
	}

	// Adding to actual print jobs
	if(printQueue.size() >= 2) {
		if (!updatePrintQueue()) {
			string estr = ofToString("cannot open printer");
			ofNotifyEvent(printUpdateEvent,estr,this);
		}
	} else {
		pj->bAdded = true;
		if(getPrintJobCount() == 0) {
			pj->bInQueue = true;
		}
		print(pj->fileName, 1, pj->id);
	}
	printQueue.push_back(*pj);
}
bool mpPrinter::updatePrintQueue() {


	HANDLE	hPrinter;
	TCHAR   szPrinter[256];
	DWORD   cchBuffer = 255;
	PRINTER_INFO_2  *pPrinterData;
	BYTE    pdBuffer[16384];
	BOOL    bReturn = FALSE;

	DWORD   cbBuf = sizeof (pdBuffer);
	DWORD   cbNeeded = 0;
	DWORD	dwBufsize=0;
	pPrinterData = (PRINTER_INFO_2 *)&pdBuffer[0];

	// get the default printer name
	bReturn = GetDefaultPrinter(
		szPrinter,
		&cchBuffer);

	if(!bReturn) return false;

	bReturn = OpenPrinter( szPrinter, &hPrinter, NULL );

	if(!bReturn) return false;

	//Getting some info from printer - particularly how many jobs
	GetPrinter(hPrinter, 2, NULL, 0, &dwBufsize);
	PRINTER_INFO_2* pinfo = (PRINTER_INFO_2*)malloc(dwBufsize);
	long result = GetPrinter(hPrinter, 2,(LPBYTE)pinfo, dwBufsize, &dwBufsize);
	DWORD numJobs = pinfo->cJobs;
	free(pinfo);//free now

	JOB_INFO_1 *pJobInfo=0;
	DWORD bytesNeeded = 0, jobsReturned=0;

	//Get info about jobs in queue.
	EnumJobs(hPrinter, 0, numJobs, 1, (LPBYTE)pJobInfo, 0,&bytesNeeded,&jobsReturned);
	pJobInfo = (JOB_INFO_1*) malloc(bytesNeeded);
	EnumJobs(hPrinter, 0, numJobs, 1, (LPBYTE)pJobInfo, bytesNeeded,&bytesNeeded,&jobsReturned);

	int printCount = 0;

	printingList.clear();

	for(int count =0; count < jobsReturned; count ++) {
		/*cout << "pPrinterName :" << pJobInfo[count].pPrinterName << endl;
		cout << "szPrinter :" << szPrinter << endl;
		cout << " == ? :" << (pJobInfo[count].pPrinterName == szPrinter) << endl;*/

		std::stringstream ss;
		ss << pJobInfo[count].pPrinterName;

		if(ss.str() == szPrinter) {
			printCount++;
			std::stringstream ss2;
			ss2 << pJobInfo[count].pDocument;
			printingList.push_back(ss2.str());
		}
	}
	
	/*cout << "---------------------- " << printingList.size() << endl;
	cout << "----------------------+ " << printCount << endl;*/
	bool isUpdate = false;
	int emptySeat = 0;

	//cout << "--- check ----" << endl;

	if(printQueue.size() > 0) {
		int lastPidx = -1;
		for(int i=0; i<printQueue.size(); i++) {
			if(printQueue[i].bAdded) lastPidx = i;
			else break;
		}
		if(lastPidx == -1) {
			string estr = "bAdded = -1 but printQueue.size() = "+ofToString(printQueue.size());
			ofNotifyEvent(printUpdateEvent,estr,this);
		}

		//cout << 1 << endl;
		// Blind Check In Actual Print Jobs
		for(int i=0; i<=lastPidx; i++) {
			for(int c=0; c<printingList.size(); c++) {
				if(printingList[c] == printQueue[i].id) {
					printQueue[i].bInQueue = true;
					isUpdate = true;
					//cout << "bInQueue: " << printQueue[i].id << endl;
					break;
				}
			}
		}

		//cout << 2 << endl;
		// Age
		/*unsigned long ctime = ofGetSystemTime();
		for(int i=0; i<=lastPidx; i++) {
			if(!printQueue[i].bInQueue) {
				if(ctime - printQueue[i].addedTime > 160000) {
					string estr = ofToString(printQueue[i].id+" has wait for too long. resend to printer");
					ofNotifyEvent(printUpdateEvent,estr,this);
					printQueue[i].addedTime = ctime;
					printQueue[i].bInQueue = true;
					print(printQueue[i].fileName, 1, printQueue[i].id);
					cout << "age: " << printQueue[i].id;
					emptySeat--;
					isUpdate = true;
				}
			}
		}*/
		//cout << 3 << endl;
		// Remove finished job
		for(int i=0; i<=lastPidx && i<printQueue.size(); i++) {
			bool found = false;
			for(int c=0; c<printingList.size(); c++) {
				if(printingList[c] == printQueue[i].id) {
					found = true;
					break;
				}
			}
			if(!found && printQueue[i].bInQueue) {
				printJob pj = printQueue[i];
				cout << "finished: " << printQueue[i].id << endl;
				pj.isSynced = false;
				if(pj.printNumber == pj.totalPrint) {
					pj.imageRef->isPrinting = false;
				}
				printedList.push_back(pj);
				//printQueue.pop_front();
				//cout << "q = " << ofToString(printQueue.size()) << ", b = " << ofToString(printQueue.empty()) << ", i = " << i << endl;
				//printQueue.pop_back();
				printQueue.erase(printQueue.begin()+i);
				emptySeat++;
				cout << "-----remove" << endl;
				isUpdate = true;
			}
		}
		//cout << 4 << endl;
		// Add print job if available
		if(emptySeat > 0) {
			cout << "-----emptySeat: "  << emptySeat << endl;
			int a=0;
			for(int i=0; a<emptySeat && a<2 && i<printQueue.size(); i++) {
				bool found = false;
				cout << "-----find: " << printQueue[i].id << endl;
				for(int c=0; c<printingList.size(); c++) {
					if(printingList[c] == printQueue[i].id) {
						cout << "-----found: " << endl;
						found = true;
						break;
					}
				}
				if(!found && !printQueue[i].bAdded) {
					cout << "-----add" << endl;
					printQueue[i].bAdded = true;
					a++;
					print(printQueue[i].fileName, 1, printQueue[i].id);
					cout << "add: " << printQueue[i].id;
					isUpdate = true;
				}
			}
		}
	}

	//isUpdate = true;
	if(isUpdate) { 
		string estr = ofToString("update");
		ofNotifyEvent(printUpdateEvent,estr,this);
	}
	//cout << "--------------" << endl;

	free(pJobInfo);//free now
	//Finished with the printer
	ClosePrinter( hPrinter );
}

void mpPrinter::print(string filename, int numPrint, string jobName) {

	string fileName = "tempImg/"+filename;
	fileName = ofToDataPath(fileName);
	// Initialize COM.
	HRESULT hr = CoInitialize(NULL);

	// Create the interface pointer.

	MemoprinterControllerPtr pICalc(__uuidof(MemoprinterClass));

	long lResult = 0;

	// Call the Add method.
	lResult = pICalc->AddPrintJob(fileName.c_str(), numPrint, jobName.c_str());

	//wprintf(L"The result is %d", lResult);

	// Uninitialize COM.
	CoUninitialize();

}

int mpPrinter::getPrintJobCount() {

	HANDLE	hPrinter;
	TCHAR   szPrinter[256];
	DWORD   cchBuffer = 255;
	PRINTER_INFO_2  *pPrinterData;
	BYTE    pdBuffer[16384];
	BOOL    bReturn = FALSE;

	DWORD   cbBuf = sizeof (pdBuffer);
	DWORD   cbNeeded = 0;
	DWORD	dwBufsize=0;
	pPrinterData = (PRINTER_INFO_2 *)&pdBuffer[0];

	// get the default printer name
	bReturn = GetDefaultPrinter(
		szPrinter,
		&cchBuffer);

	if(!bReturn) return false;

	bReturn = OpenPrinter( szPrinter, &hPrinter, NULL );

	if(!bReturn) return false;

	//Getting some info from printer - particularly how many jobs
	GetPrinter(hPrinter, 2, NULL, 0, &dwBufsize);
	PRINTER_INFO_2* pinfo = (PRINTER_INFO_2*)malloc(dwBufsize);
	long result = GetPrinter(hPrinter, 2,(LPBYTE)pinfo, dwBufsize, &dwBufsize);
	DWORD numJobs = pinfo->cJobs;
	free(pinfo);//free now

	JOB_INFO_1 *pJobInfo=0;
	DWORD bytesNeeded = 0, jobsReturned=0;

	//Get info about jobs in queue.
	EnumJobs(hPrinter, 0, numJobs, 1, (LPBYTE)pJobInfo, 0,&bytesNeeded,&jobsReturned);
	pJobInfo = (JOB_INFO_1*) malloc(bytesNeeded);
	EnumJobs(hPrinter, 0, numJobs, 1, (LPBYTE)pJobInfo, bytesNeeded,&bytesNeeded,&jobsReturned);

	int printCount = 0;

	printingList.clear();

	for(int count =0; count < jobsReturned; count ++) {
		/*cout << "pPrinterName :" << pJobInfo[count].pPrinterName << endl;
		cout << "szPrinter :" << szPrinter << endl;
		cout << " == ? :" << (pJobInfo[count].pPrinterName == szPrinter) << endl;*/

		std::stringstream ss;
		ss << pJobInfo[count].pPrinterName;

		if(ss.str() == szPrinter) {
			printCount++;
			std::stringstream ss2;
			ss2 << pJobInfo[count].JobId;
			printingList.push_back(ss2.str());
		}
	}

	free(pJobInfo);
	ClosePrinter( hPrinter );
	return printCount;
}

void mpPrinter::getPrintStatus() {
	
	HANDLE	hPrinter;
	TCHAR   szPrinter[256];
	DWORD   cchBuffer = 255;
	PRINTER_INFO_2  *pPrinterData;
	BYTE    pdBuffer[16384];
	BOOL    bReturn = FALSE;

	DWORD   cbBuf = sizeof (pdBuffer);
	DWORD   cbNeeded = 0;
	DWORD	dwBufsize=0;
	pPrinterData = (PRINTER_INFO_2 *)&pdBuffer[0];

	// get the default printer name
	bReturn = GetDefaultPrinter(
		szPrinter,
		&cchBuffer);

	bReturn = OpenPrinter( szPrinter, &hPrinter, NULL );

	//Getting some info from printer - particularly how many jobs
	GetPrinter(hPrinter, 2, NULL, 0, &dwBufsize);
	PRINTER_INFO_2* pinfo = (PRINTER_INFO_2*)malloc(dwBufsize);
	long result = GetPrinter(hPrinter, 2,(LPBYTE)pinfo, dwBufsize, &dwBufsize);
	DWORD numJobs = pinfo->cJobs;
	free(pinfo);//free now
 
	
	if ( numJobs == 0)
	{
		cout << "No printer jobs found." << endl;
	}

	else //Some Jobs in queue
	{

		JOB_INFO_1 *pJobInfo=0;
		DWORD bytesNeeded = 0, jobsReturned=0;

		//Get info about jobs in queue.
		EnumJobs(hPrinter, 0, numJobs, 1, (LPBYTE)pJobInfo, 0,&bytesNeeded,&jobsReturned);
		pJobInfo = (JOB_INFO_1*) malloc(bytesNeeded);
		EnumJobs(hPrinter, 0, numJobs, 1, (LPBYTE)pJobInfo, bytesNeeded,&bytesNeeded,&jobsReturned);

		//Loop and delete each waiting job
		for(int count =0; count < jobsReturned; count ++)
		{
			cout << "=======" << endl;
			cout << "JobId = " << pJobInfo[count].JobId << endl;
			if(pJobInfo[count].pStatus != NULL) {
				cout << "pStatus = " << pJobInfo[count].pStatus << endl;
			}
			cout << "Status = " << pJobInfo[count].Status << endl;
			cout << "PagesPrinted = " << pJobInfo[count].PagesPrinted << endl;
			cout << "TotalPages = " << pJobInfo[count].TotalPages << endl;
			cout << "Position = " << pJobInfo[count].Position << endl;
			cout << "Priority = " << pJobInfo[count].Priority << endl;
			/*SYSTEMTIME st = pJobInfo[count].Submitted;
			CString cstrMessage;
	 
			cstrMessage.Format( "%d-%02d-%02d %02d:%02d:%02d.%03d", 
								st.wYear,
								st.wMonth, 
								st.wDay,                       
								st.wHour, 
								st.wMinute, 
								st.wSecond,
								st.wMilliseconds );
			
			cout << "Submitted = " << cstrMessage << endl;
			cout << "=======" << endl;*/

			/*cout << "Deleting JobID  " << pJobInfo[count].JobId;
			if ( SetJob(hPrinter, pJobInfo[count].JobId,0,NULL, JOB_CONTROL_DELETE) !=0)
			{
				cout << "...... Deleted OK" << endl;
			}
			else
			{
				cout << "...... Failed to Delete" << endl;
			}*/
		}

		free(pJobInfo);//free now
	}


	//Finished with the printer
	ClosePrinter( hPrinter );
}

string gen_random(const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	string str;
	for (int i = 0; i < len; ++i) {
		str[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return str;
}

void mpPrinter::updateTimerHandler(int &args) {
	updatePrintQueue();
}