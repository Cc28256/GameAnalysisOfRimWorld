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
	PGoodsTypes GoodsType;			//��Ʒ����
	int unknown12;
	int unknown16;
	int GoodsIconId;		//��Ʒ��ͼ
	int unknown24;
	int unknown28;
	int unknown32;
	int GoodsX;			//���� �������ұ��
	int x_null;
	int GoodsY;			//���� ����֮�ϱ��
	int y_null;
	int GoodsNumber;		//��Ʒ����
	int GoodsDurable;		//��Ʒ�;�
}*PGoodsInfo, GoodsInfo;


typedef struct _MyGoods {
	DWORD GoodsAddress;			//��Ʒ��ַ
	long GoodsX;			//���� �������ұ��
	long GoodsY;			//���� ����֮�ϱ��
	int GoodsNumber;		//��Ʒ����
	int GoodsDurable;		//��Ʒ�;�
	wchar_t* GoodName;		//��Ʒ����
}*PMyGoods,MyGoods;

//��ǰ����������о������״̬
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


