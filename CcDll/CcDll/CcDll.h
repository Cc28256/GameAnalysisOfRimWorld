#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#include "resource.h"
#include <vector>
#include <iostream>
#include <assert.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <atlstr.h>
#include "Hook.h"


using namespace std;




DWORD getWeChatInfo(HWND huwdlg);
void EnumProcess(HWND huwdlg);
DWORD addGoodsNumber(int number, HWND huwdlg);
DWORD addGoodsDurable(int number, HWND huwdlg);
VOID ChangeFastBulid();
VOID ChangeFastResearch(HWND huwdlg, char* fastResearchMem);
bool ChangePeopleMem(char* changePeopleMem);
void ChangePeopleState(char* changePeopleMem, PStateData peopleState,DWORD isFlage);