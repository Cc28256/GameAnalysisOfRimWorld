#pragma once



typedef struct _GoodsString {
	int unknown1;
	int unknown2;
	int Goodsstrlen;
}*PGoodsString, GoodsString;

typedef struct _GoodsTypes {
	int unknown1;
	int unknown2;
	PGoodsString goodsString;
}*PGoodsTypes, GoodsTypes;

/*
EC 57 BB 11 - 00 00 00 00 - 90 F4 B4 18 - 00 00 00 00 
00 00 00 00 - 00 95 9F 16 - 38 77 17 18 - 83 4E 00 00
00 00 00 00 - 52 00 00 00 - 00 00 00 00 - 6B 00 00 00
00 00 00 00 - 32 00 00 00 - 96 00 00 00
*/


typedef struct _GoodsInfo{
	int unknown0;
	int unknown4;
	PGoodsTypes GoodsType;			//物品类型
	int unknown12;
	int unknown16;
	int GoodsIconId;		//物品贴图
	int unknown24;
	int unknown28;
	int unknown32;
	int GoodsX;			//座标 从左至右变大
	int x_null;
	int GoodsY;			//座标 从下之上变大
	int y_null;
	int GoodsNumber;		//物品数量
	int GoodsDurable;		//物品耐久
}*PGoodsInfo, GoodsInfo;


typedef struct _MyGoods {
	DWORD GoodsAddress;			//物品地址
	long GoodsX;			//座标 从左至右变大
	long GoodsY;			//座标 从下之上变大
	int GoodsNumber;		//物品数量
	int GoodsDurable;		//物品耐久
	wchar_t* GoodName;		//物品名称
}*PMyGoods,MyGoods;

//当前的人物、快速研究建造的状态
typedef struct _StateData {
	bool comfort;
	bool mood;
	bool food;
	bool rest;
	bool joy;
	bool beauty;
	bool space;
	bool research;
	bool build;
}*PtateData, StateData;


