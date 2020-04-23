#pragma once
#include <windows.h>
#include "StructObj.h"
#include <iostream>
#include <stdio.h>
#include "resource.h"
#include <atlstr.h>
#include <commctrl.h>
#include<vector>
using namespace std;

extern BYTE g_oldBytes[5];          //记录原函数OPcode
extern BYTE g_NewBytes[5]; //记录自己的OPcode

extern PDWORD g_funAddress;

extern HWND g_hwndDlg;
extern HWND g_hList;

extern bool g_start;

int GetGoodsStruct(
	_In_ DWORD unknown,
	_In_ DWORD goodsStrAddr
);

void OnHook(DWORD* oldFunAddr);

void UnHook(DWORD* oldFunAddr);

