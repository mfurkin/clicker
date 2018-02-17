/*
 * clicker.h
 *
 *  Created on: 16 февр. 2018 г.
 *      Author: Алёна
 */

#ifndef CLICKER_H_
#define CLICKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wtypes.h>
#include <windef.h>
#include <winuser.h>
#include "fileutils_types.h"
#include "LoggerTypes.h"

#define PARAMS_NUM 2
#define LOG_PARAMS_NUM 3
#define CLICKER_LOG_NAME "clicker"
void logLine(const char* tag, char* msg);
void logPtr(char* tag, char* msg, unsigned ptr);
typedef struct WndListItemStruct {
	HWND hwnd;
	char name[MAX_PATH];
	struct WndListItemStruct* next;
} WndListItem;
typedef struct {
	WndListItem* head,*tail;
} WndList;


#endif /* CLICKER_H_ */
