/*
 ============================================================================
 Name        : clicker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "clicker.h"
CreateLoggerProc createLogger = NULL;
LogProc logProc = NULL;
LogPtrProc logPtrProc = NULL;
DeleteLoggerProc deleteLogger = NULL;
GetLogPathProc GetLogPath = NULL;
GetLogFnameProc GetLogFname = NULL;
HWND hwndTarget = INVALID_HANDLE_VALUE;
LRESULT CALLBACK MouseProc(int    nCode, WPARAM wParam, LPARAM lParam) {
	MOUSEHOOKSTRUCT* ptr = (MOUSEHOOKSTRUCT*) lParam;
	logPtr("MouseProc",(char*)"MouseProc enter hwnd=",(unsigned)(*ptr).hwnd);
	if ((*ptr).hwnd == hwndTarget) {
		char st[MAX_PATH],st2[MAX_PATH];
		switch (wParam) {
			case WM_LBUTTONUP:
				strcpy(st,"WM_LBUTTONUP");
				break;
			case WM_LBUTTONDOWN:
				strcpy(st,"WM_LBUTTONDOWN");
				break;
		}
		sprintf(st2,"%d %s x=%ld y=%ld",wParam,st,(*ptr).pt.x,(*ptr).pt.y);
		logLine("MouseEvent",st2);
	}
	logPtr("MouseProc","MouseProc exit hwnd=",(unsigned)(*ptr).hwnd);
	return CallNextHookEx(0,nCode,wParam,lParam);
}

void logLine(const char* tag, char* msg) {
	if (logProc)
		logProc(CLICKER_LOG_NAME,tag,msg);
}

void logPtr(char* tag, char* msg, unsigned ptr) {
	if (logPtrProc)
		logPtrProc(CLICKER_LOG_NAME,tag,msg,ptr);
}
void  logClicks(int argc, char* argv[]) {
	if (argc < LOG_PARAMS_NUM)
		printf("Illegal parameters number\n");
	else {
		char* p2;
		uint64_t p = strtol(argv[2],&p2,10);
		DWORD threadId = GetCurrentThreadId();
		hwndTarget = (HWND) p;
		HHOOK result = SetWindowsHookEx(WH_MOUSE,MouseProc,NULL,threadId);
		if (!(result))
			printf("Error during SetWindowHookEx call code=%d",GetLastError());
		else
			logLine("LOG_CLICKS","logClicks enter");
	}
}

void clickIt(int argc, char* argv[]) {

}

void addList(WndList** listPtr, WndListItem* newItem) {
#ifdef _SAVE_WND_INFO_
	*listPtr = (WndList*) malloc(sizeof(WndList));
	(**listPtr).head = newItem;
	(**listPtr).tail = newItem;
#endif
}

WndListItem* createNewItem(HWND hwnd, char* name) {
#ifdef _SAVE_WND_INFO_
	WndListItem* newItem = (WndListItem*) malloc(sizeof(WndListItem));
	(*newItem).next =  NULL;
	(*newItem).hwnd = hwnd;
	(*newItem).name = name;
	return newItem;
#else
	return NULL;
#endif
}
void addNewItem(WndList* list, HWND hwnd, char* name) {
#ifdef _SAVE_WND_INFO_
	WndListItem* newItem = createNewItem(hwnd,name);
	(*(*list).tail).next = newItem;
#endif
}
void addItem(WndList** listPtr, HWND hwnd, char* name) {
#ifdef _SAVE_WND_INFO_
	if (!(*listPtr)) {
		addList(listPtr,createNewItem(hwnd,name));
	}
	else {
		addNewItem(*listPtr,hwnd,name);

	}
#endif
}
BOOL CALLBACK EnumWindowsProc( HWND  hwnd, LPARAM lParam) {
	BOOL result = TRUE;
#ifdef _SAVE_WND_INFO_
	WndList** listPtr = (WndList**) lParam;
#endif
	char name[MAX_PATH];
	GetWindowText(hwnd,name,MAX_PATH);
	printf("New window added hwnd=%p name=%s\n",hwnd,name);
#ifdef _SAVE_WND_INFO_
	addItem(listPtr,hwnd,name);
#endif
	return result;
}

void enumWindows() {
	static WndList* wndList = NULL;
	BOOL result = EnumWindows(EnumWindowsProc,(LPARAM)&wndList);
	if (!(result))
		printf("Error during enumWindows code:%d",GetLastError());
	else
		printf("EnumWindows started successfully\n");
}


int initFileUtils() {
	int result = TRUE;
	HMODULE hDll = LoadLibrary("libfileutils.dll");
	if (!(hDll))
		result = FALSE;
	else {
		GetLogPath  = (GetLogPathProc) GetProcAddress(hDll,"getLogPath");
		GetLogFname = (GetLogFnameProc) GetProcAddress(hDll,"getLogFname");
		result = getLogPath && getLogFname;
	}
	return result;
}
int initLogger() {
	int result;
	if (!(initFileUtils()))
		result = 0;
	else {
		HMODULE hLogDll = LoadLibrary("liblogger.dll");
		if (!(hLogDll))
			result = 0;
		else {
			char fname[MAX_PATH];
			createLogger = (CreateLoggerProc) GetProcAddress(hLogDll,"createLogger");
			logProc = (LogProc) GetProcAddress(hLogDll,"log");
			logPtrProc = (LogPtrProc) GetProcAddress(hLogDll,"logPtr");
			deleteLogger = (DeleteLoggerProc) GetProcAddress(hLogDll,"deleteLogger");
			GetLogFname(fname,"\\clicker","clicker.log",NULL);
			result = (createLogger) && (logProc) && (logPtrProc) && (deleteLogger);
			if (result)
				createLogger(CLICKER_LOG_NAME,fname);
		}
	}
	return result;
}
int main(int argc, char *argv[]) {
	if (initLogger()) {
		if (argc < PARAMS_NUM)
			printf("Illegal parameters number\n");
		else {
			if (!(strcmp(argv[1],"log")))
				logClicks(argc,argv);
			else
				if(!(strcmp(argv[1],"click")))
					clickIt(argc,argv);
				else
					if (!(strcmp(argv[1],"enumWindows")))
						enumWindows();

		}
	}
	if (deleteLogger)
		deleteLogger(CLICKER_LOG_NAME);
	return EXIT_SUCCESS;
}
