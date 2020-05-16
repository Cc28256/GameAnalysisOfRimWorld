#include "stdafx.h"
#include "Hook.h"


BYTE g_oldBytes[5];          //记录原函数OPcode
BYTE g_NewBytes[5] = { 0xE9 }; //记录自己的OPcode

HWND g_hList;
PDWORD g_funAddress;
PDWORD pGoods;
HWND g_hwndDlg;
DWORD flag = 0;
int index_flag = 0;
bool g_start = true;
LVITEM li = { 0 };


CHAR* wcharToCHAR(WCHAR *s) {
	int w_nlen = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, false);
	CHAR *ret = (CHAR*)malloc(w_nlen * sizeof(CHAR));
	memset(ret, 0, w_nlen);
	WideCharToMultiByte(CP_ACP, 0, s, -1, ret, w_nlen, NULL, false);
	return ret;
}

void OnHook(DWORD* oldFunAddr)
{
	//MessageBoxW;
	//保存原OPcode
	memcpy(g_oldBytes, oldFunAddr,5);

	//计算jmp之后的偏移量
	//（偏移量 = 我的函数地址 - 目标函数地址 - 指令长度（5））
	DWORD dwOffset = (DWORD)GetGoodsStruct -
		(DWORD)oldFunAddr - 5;
	*(DWORD*)(g_NewBytes + 1) = dwOffset;

	//因代码段没有可写属性，所以需要修改内存属性
	DWORD dwOldAttrubet;
	VirtualProtect(oldFunAddr,
		5,
		PAGE_EXECUTE_READWRITE,
		&dwOldAttrubet);
	//修改代码
	memcpy(oldFunAddr, g_NewBytes, 5);

	//恢复原来的内存属性
	VirtualProtect(oldFunAddr,
		5,
		dwOldAttrubet,
		&dwOldAttrubet);

}

void UnHook(DWORD* oldFunAddr)
{
	DWORD dwOldAttrubet;
	VirtualProtect(oldFunAddr,
		5,
		PAGE_EXECUTE_READWRITE,
		&dwOldAttrubet);
	//修改代码
	memcpy(oldFunAddr, g_oldBytes, 5);

	//恢复原来的内存属性
	VirtualProtect(oldFunAddr,
		5,
		dwOldAttrubet,
		&dwOldAttrubet);

}

bool CmpCirculation(DWORD cflag)
{
	if (flag == cflag)
	{
		index_flag = 0;
		g_start = false;
		flag = 0;
		return false;
	}
	if (index_flag == 0)
	{
		//清空所有数据	
		SendMessage(g_hList, LVM_DELETEALLITEMS, 0, 0);
		flag = cflag;
	}
	index_flag++;
	return true;
}


void GoodsOperator(DWORD goodsStrAddr)
{
	
	if (!g_start)
		return;

	if (!CmpCirculation(goodsStrAddr))
		return;

	
	//获得ListView列数	

	//HWND hWndHeader;
	//LVITEM lvItem;
	//hWndHeader=(HWND)SendMessage(g_hList, IDC_LIST1, 0,0);
	//int nCols=SendMessage(hWndHeader, HDM_GETITEMCOUNT, 0, 0);		//删除ListView所有列	
	//while(nCols)	
	//{		
	//	SendMessage(g_hList,LVM_DELETECOLUMN,0,(LPARAM)&lvItem);
	//	nCols--;	
	//}

	PGoodsInfo goodA = (PGoodsInfo)goodsStrAddr;
	li.mask = LVIF_TEXT;
	
	if (goodA->GoodsType->goodsString->Goodsstrlen > 3)
	{
		DWORD address = (DWORD)goodA->GoodsType->goodsString;
		wchar_t* wGoodsName = (wchar_t*)(address + 0xC);
		char* GoodsName = wcharToCHAR(wGoodsName);
		//名称、座标x、y、数量、耐久、地址
		li.pszText = GoodsName;
		li.cchTextMax = goodA->GoodsType->goodsString->Goodsstrlen + 1;
		li.iItem = index_flag - 1;//行号
		ListView_InsertItem(g_hList,&li);

		CHAR GoodsX[0x100] = { 0 };
		sprintf_s(GoodsX, "%d", goodA->GoodsX);
		ListView_SetItemText(g_hList, index_flag - 1, 1, GoodsX);

		CHAR GoodsY[0x100] = { 0 };
		sprintf_s(GoodsY, "%d", goodA->GoodsY);
		ListView_SetItemText(g_hList, index_flag - 1, 2, GoodsY);

		CHAR GoodsNumber[0x100] = { 0 };
		sprintf_s(GoodsNumber, "%d", goodA->GoodsNumber);
		ListView_SetItemText(g_hList, index_flag - 1, 3, GoodsNumber);

		CHAR GoodsDurable[0x100] = { 0 };
		sprintf_s(GoodsDurable, "%d", goodA->GoodsDurable);
		ListView_SetItemText(g_hList, index_flag - 1, 4, GoodsDurable);

		CHAR goodsAddr[0x100] = { 0 };
		sprintf_s(goodsAddr, "%x", goodA);
		ListView_SetItemText(g_hList, index_flag - 1, 5, goodsAddr);
		UpdateWindow(g_hwndDlg);
		free(GoodsName);
		GoodsName = NULL;
	}
	return;
}


int GetGoodsStruct(
	_In_ DWORD unknown,
	_In_ DWORD goodsStrAddr
)
{
	GoodsOperator(goodsStrAddr);

	//为了正常执行我们这里先卸载钩子，然后调用原函数，在
	UnHook(g_funAddress);
	int(*p)(int a, int b);
	p = (int(*)(int ,int))g_funAddress;
	
	//在调用真正的MessageBoxW函数
	int ret = p(unknown, goodsStrAddr);
	//最后在把钩子装上
	OnHook(g_funAddress);

	return ret;

}