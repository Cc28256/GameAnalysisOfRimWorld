// CcDll.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"
#include "CcDll.h"

/*
1607BBC5 - 8D 4C B1 10           - lea ecx,[ecx+esi*4+10]
1607BBC9 - D9 19                 - fstp dword ptr [ecx]
1607BBCB - 8B 48 30              - mov ecx,[eax+30]
1607BBCE - 41                    - inc ecx
1607BBCF - 89 48 30              - mov [eax+30],ecx
1607BBD2 - 8D 65 F4              - lea esp,[ebp-0C]
1607BBD5 - 5E                    - pop esi
1607BBD6 - 5F                    - pop edi
1607BBD7 - 5B                    - pop ebx
1607BBD8 - C9                    - leave 
1607BBD9 - C3                    - ret 
  */  
char mem_of_fast_research[] = { 0x8D ,0x4C, 0xB1, 0x10,0xD9, 0x19,0x8B, 0x48 ,0x30,0x41,0x89, 0x48, 0x30,0x8D, 0x65, 0xF4,0x5E,0x5F,0x5B,0xC9,0xC3 };

/*调用物品信息的函数特征，他的第二个参数是我能获得的物品结构地址，
这个函数是一个大循环里面的，会一直执行
028F1790    55              push ebp
028F1791    8BEC            mov ebp,esp
028F1793    83EC 08         sub esp,0x8
028F1796    8B4D 0C         mov ecx,dword ptr ss:[ebp+0xC]
028F1799    8B41 34         mov eax,dword ptr ds:[ecx+0x34]
028F179C    8B49 08         mov ecx,dword ptr ds:[ecx+0x8]
028F179F    83EC 04         sub esp,0x4
028F17A2    51              push ecx
028F17A3    50              push eax
028F17A4    FF75 08         push dword ptr ss:[ebp+0x8]
*/
char mem_of_goods[] = { 0x55,0x8B,0xEC,0x83,0xEC,0x08,0x8B,0x4D,0x0C,0x8B,0x41,0x34,0x8B,0x49,0x08,0x83,0xEC,0x04,0x51,0x50,0xFF,0x75,0x08 };


PDWORD g_fastResearchAddress;//用于获取快速建造地址的全局变量

/*
		typedef struct _MEMORY_BASIC_INFORMATION {
		  PVOID  BaseAddress;			// 内存块基地址指针
		  PVOID  AllocationBase;		// VirtualAlloc 函数分配的基地址指针
		  DWORD  AllocationProtect;		// 内存块初始内存保护属性
		  SIZE_T RegionSize;			// 内存块大小
		  DWORD  State;					// 内存块状态（COMMIT、FREE、RESERVE）
		  DWORD  Protect;				// 内存块当前内存保护属性
		  DWORD  Type;					// 内存块类型（IMAGE、MAPPED、PRIVATE）
		} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;


		PAGE_NOACCESS
		PAGE_READONLY
		PAGE_READWRITE
		PAGE_EXECUTE
		PAGE_EXECUTE_READ
		PAGE_EXECUTE_READWRITE
*/
vector<MEMORY_BASIC_INFORMATION> vec;//用于遍历内存地址的容器


//匹配内存中参数特征的位置
long MemoryCmpToFeatureCode(PVOID BaseAddress, long mem_size, PCHAR Strings)
{
	int index = 0;
	//获取特征长度
	int strlens = strlen(Strings);
	for (long j = 0; j < mem_size - strlens; j++)
	{
		index = 0;
		for (int i = 0; i < strlens; i++)
		{
			char* compare = (char*)BaseAddress;
			//判断内存特征与传进来的特征对比
			if (compare[j + i] != Strings[i])
			{
				break;
			}
			index++;
		}
		if (index == strlens)
			return j;
	}
	return -1;
}



//枚举指定进程所有内存块
//assert(hProcess != nullptr);
//参数:
//  hProcess:  要枚举的进程,需拥有PROCESS_QUERY_INFORMATION权限
//  memories:  返回枚举到的内存块数组
//返回:
//  成功返回true,失败返回false.
static bool EnumAllMemoryBlocks(HANDLE hProcess, OUT vector<MEMORY_BASIC_INFORMATION>& memories) {
	// 如果 hProcess 为空则结束运行
	assert(hProcess != nullptr);

	// 初始化 vector 容量
	memories.clear();
	memories.reserve(200);

	// 获取 PageSize 和地址粒度
	SYSTEM_INFO sysInfo = { 0 };
	GetSystemInfo(&sysInfo);
	/*
		typedef struct _SYSTEM_INFO {
		  union {
			DWORD dwOemId;							// 兼容性保留
			struct {
			  WORD wProcessorArchitecture;			// 操作系统处理器体系结构
			  WORD wReserved;						// 保留
			} DUMMYSTRUCTNAME;
		  } DUMMYUNIONNAME;
		  DWORD     dwPageSize;						// 页面大小和页面保护和承诺的粒度
		  LPVOID    lpMinimumApplicationAddress;	// 指向应用程序和dll可访问的最低内存地址的指针
		  LPVOID    lpMaximumApplicationAddress;	// 指向应用程序和dll可访问的最高内存地址的指针
		  DWORD_PTR dwActiveProcessorMask;			// 处理器掩码
		  DWORD     dwNumberOfProcessors;			// 当前组中逻辑处理器的数量
		  DWORD     dwProcessorType;				// 处理器类型，兼容性保留
		  DWORD     dwAllocationGranularity;		// 虚拟内存的起始地址的粒度
		  WORD      wProcessorLevel;				// 处理器级别
		  WORD      wProcessorRevision;				// 处理器修订
		} SYSTEM_INFO, *LPSYSTEM_INFO;
	*/

	//遍历内存
	const char* p = (const char*)sysInfo.lpMinimumApplicationAddress;
	MEMORY_BASIC_INFORMATION  memInfo = { 0 };
	while (p < sysInfo.lpMaximumApplicationAddress) {
		// 获取进程虚拟内存块缓冲区字节数
		size_t size = VirtualQueryEx(
			hProcess,								// 进程句柄
			p,										// 要查询内存块的基地址指针
			&memInfo,								// 接收内存块信息的 MEMORY_BASIC_INFORMATION 对象
			sizeof(MEMORY_BASIC_INFORMATION32)		// 缓冲区大小
		);
		if (size != sizeof(MEMORY_BASIC_INFORMATION32)) { break; }

		// 经过分析我们需要的内存块属性分别是是PAGE_EXECUTE_READWRITE\MEM_COMMIT\MEM_PRIVATE，所以将不必要的内存块排除掉
		if (memInfo.Protect == PAGE_EXECUTE_READWRITE)
			if (memInfo.State == MEM_COMMIT)
				if (memInfo.Type == MEM_PRIVATE)
					memories.push_back(memInfo);// 将内存块信息追加到 vector 数组尾部

		// 移动指针
		p += memInfo.RegionSize;
	}

	return memories.size() > 0;
}


void EnumProcess(HWND huwdlg)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);		// 进程快照句柄
	PROCESSENTRY32 process = { sizeof(PROCESSENTRY32) };							// 接收进程信息的对象

	// 遍历进程
	while (Process32Next(hProcessSnap, &process)) {
		// 找到想要的进程
		if (strcmp(process.szExeFile, "RimWorldWin.exe") == 0) {
			// 获取进程句柄
			HANDLE h_rundll32 = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process.th32ProcessID);
			if (!h_rundll32) { MessageBox(NULL,"OpenProcess failed.", "failed",NULL); }

			int addressOffset = 0;
			// 遍历该进程的内存块
			if (EnumAllMemoryBlocks(h_rundll32, vec)) {
				for (int i = 0; i < vec.size(); i++) {
					// 查找物品特征
					addressOffset = MemoryCmpToFeatureCode(vec[i].BaseAddress, vec[i].RegionSize, mem_of_goods);
					if(addressOffset != -1)//物品函数特征地址查找
					{
						int int_value = (int)vec[i].BaseAddress + (int)addressOffset;
						g_funAddress = (PDWORD)int_value;
						CString temp_value = _T("");   //temp_value用来处理int值
						temp_value.Format(_T("%x"), int_value);//固定格式
						SetDlgItemText(huwdlg, IDC_PHONE_NUMBER, temp_value);
						OnHook(g_funAddress);
					}

					addressOffset = MemoryCmpToFeatureCode(vec[i].BaseAddress, vec[i].RegionSize, mem_of_fast_research);
					if (addressOffset != -1)
					{
						int int_value = (int)vec[i].BaseAddress + (int)addressOffset;
						g_fastResearchAddress = (PDWORD)(int_value+0x9);
						CString temp_value = _T("");   //temp_value用来处理int值
						temp_value.Format(_T("%x"), g_fastResearchAddress);//固定格式
						SetDlgItemText(huwdlg, IDC_FAST_JIANZAO, temp_value);
						MessageBox(NULL, temp_value, "failed", NULL);
					}
				}
			}
			else { MessageBox(NULL, "EnumAllMemoryBlocks failed.","failed", NULL); }
		}

	}
}


//test
DWORD getWeChatInfo(HWND huwdlg)
{

	//CHAR wxid[0x100] = { 0 };
	//sprintf_s(wxid, "%s",0x126DD00);
	//
	//CHAR wxname[0x100] = { 0 };
	//sprintf_s(wxname, "%s", 0x126D8FC);
	//CHAR wxphonenumber[0x100] = { 0 };
	//sprintf_s(wxphonenumber, "%s",0x126D92F);
	//SetDlgItemText(huwdlg,IDC_PHONE_NUMBER,wxphonenumber);
	//SetDlgItemText(huwdlg, IDC_NAME, wxname);
	return true;
}

//修改物品数量
DWORD addGoodsNumber(int number,HWND huwdlg)
{
	char str[10] = { 0 };
	GetDlgItemText(huwdlg, IDC_THIS_GOODS_ADDR, str, 10);
	int address = strtol(str, NULL, 16);
	if (address < 2)
	{
		MessageBox(NULL, "未选中物品", "错误", NULL);
		return 0;
	}
	//MessageBox(NULL, str, "str", NULL);
	//CHAR wxname[0x100] = { 0 };
	//sprintf_s(wxname, "%x", address);
	//MessageBox(NULL, wxname, "test", NULL);

	PGoodsInfo changeGoods = (PGoodsInfo)address;
	changeGoods->GoodsNumber += number;
	g_start = true;
	MessageBox(NULL, "添加成功", "成功", NULL);
	return true;
}

//修改物品耐久
DWORD addGoodsDurable(int number, HWND huwdlg)
{
	char str[10] = { 0 };
	GetDlgItemText(huwdlg, IDC_THIS_GOODS_ADDR, str, 10);
	int address = strtol(str, NULL, 16);
	if (address < 2)
	{
		MessageBox(NULL, "未选中物品", "错误", NULL);
		return 0;
	}
	PGoodsInfo changeGoods = (PGoodsInfo)address;
	if (changeGoods->GoodsDurable == -1)
	{
		MessageBox(NULL, "该物品没有耐久", "错误", NULL);
		return 0;
	}
	
	changeGoods->GoodsDurable += 10;
	g_start = true;
	MessageBox(NULL, "添加成功", "成功", NULL);
	return true;
}


VOID changeFastResearch(HWND huwdlg, char* fastResearchMem)
{
	HANDLE selfProHandle = GetCurrentProcess();
	DWORD dwOffset;
	/*
	0000000000：83 C1 64       add ecx,0x64
	0000000000：89 48 30       mov [eax+0x30],ecx
	0000000000：E9 ?? ?? ?? ?? jmp <&msvcrt._purecall>
	*/
	char changeByte2[] = { 0x83, 0xC1, 0x64, 0x89, 0x48, 0x30, 0xE9,0x00,0x00,0x00,0x00 };
	int addressOffset = 0;
	if (EnumAllMemoryBlocks(selfProHandle, vec)) {
		for (int i = 0; i < vec.size(); i++) {
			// 查找快速建造特征
			addressOffset = MemoryCmpToFeatureCode(vec[i].BaseAddress, vec[i].RegionSize, mem_of_fast_research);
			if (addressOffset != -1)
			{
				int int_value = (int)vec[i].BaseAddress + (int)addressOffset;
				g_fastResearchAddress = (PDWORD)(int_value + 0x9);
				dwOffset = ((DWORD)fastResearchMem - ((DWORD)g_fastResearchAddress + 0X7)) - 0X4;
				
				*(DWORD*)(changeByte2 + 7) = dwOffset;
				memcpy(g_fastResearchAddress, changeByte2, 0xB);


			}
		}
	}
	else 
	{ MessageBox(NULL, "EnumAllMemoryBlocks failed.", "failed", NULL); }

}