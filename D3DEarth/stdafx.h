// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
enum EGeometryType {������,����,����};

enum ETileReadMode { ������Ƭ����, �Զ����ļ���Ƭ, �Զ���Web��Ƭ};
enum EAniType {�޶���,��˸,����,����,��������,����,����,��ת};
enum EMapType {����,��·,����,��};
enum EModelType{��ͼ,���,��Դ,����,ͼԪ,������,����,��ֵͼ,����,����,�Զ���ģ��};
enum EPropertyType {����,��ɫ,��С,����,���,�߶�,ƫ��,����,����,����,����,����,�ɼ���,����,��ַ,·��,·��2,����,λ��,�Ƕ�,ģʽ};

enum EInputCoordinate { WGS84��������, ƽ������ };
enum ESceneMode { ����, �ֲ�ƽ�� };
enum EDrawMode { ��ɫģʽ, ����ģʽ, �߿�ģʽ};



#include <windows.h>
#include <ppl.h>

////===== ���ö��������ṹ =====
struct STRUCT_Ani
{
	EAniType aniType;
	bool isDoAni;  //�Ƿ񲥷Ŷ���
	int duration; //��������ʱ��������
	int doCount;  //ѭ��������0��ʾ����
	bool isReverse;  //��ɺ��Ƿ�ת����

	
	int doneCount; //��ִ�д���
	UINT startTick; //��ʼʱ�ļ�ʱ
};


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <assert.h>



//===== ������ʶ���ṹ =====
struct CUSTOMVERTEX_T
{
    D3DXVECTOR3 position; 
	D3DXVECTOR3 normal; 
    FLOAT u, v;
};
#define D3DFVF_CUSTOMVERTEX_T (D3DFVF_XYZ | D3DFVF_NORMAL| D3DFVF_TEX1)

////===== ɫ�ʶ���ṹ ===== ��������
struct CUSTOMVERTEX_C
{
    D3DXVECTOR3 position;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX_C (D3DFVF_XYZ | D3DFVF_DIFFUSE)

////===== ɫ�ʷ��߶���ṹ ===== ��������
struct CUSTOMVERTEX_CN
{
    D3DXVECTOR3 position;
	D3DXVECTOR3 normal; 
	D3DXCOLOR color;
};
#define D3DFVF_CUSTOMVERTEX_CN (D3DFVF_XYZ | D3DFVF_NORMAL| D3DFVF_DIFFUSE)

////===== ���߶���ṹ =====
struct CUSTOMVERTEX_N
{
    D3DXVECTOR3 position;
	D3DXVECTOR3 normal; 
};
#define D3DFVF_CUSTOMVERTEX_N (D3DFVF_XYZ | D3DFVF_NORMAL)

////===== ��������ṹ =====
struct STRUCT_Camera
{
	D3DXVECTOR3 cameraPosition;
	D3DXVECTOR3 cameraLookat;
	D3DXVECTOR3 cameraDirection;
	D3DXVECTOR3 cameraUp;

	float FieldOfView;
	float Near;
	float Far;
};
//===== ��Դ�ṹ =====
struct STRUCT_Light
{
	bool isEnable;
	D3DLIGHT9 light;
};




#include "Helper.h"
#include "Global.h"
#include "Para.h"
#include "RendererManager.h"
#include "Renderer.h"
#include "TriangleRenderer.h"

#include "BasicModel.h"
#include "MapTile.h"
#include "Pline.h"
#include "PSymbol.h"
#include "PCustom.h"
#include "PPolyCol.h"
#include "PArea.h"
#include "PPanel.h"
#include "PText.h"
#include "Camera.h"
#include "XModel.h"

#define IFC(x) { hr = (x); if (FAILED(hr)) goto Cleanup; }
#define IFCOOM(x) { if ((x) == NULL) { hr = E_OUTOFMEMORY; IFC(hr); } }
#define SAFE_RELEASE(x) { if (x) { x->Release(); x = NULL; } }



typedef pair<int,CBasicModel*> EntryMap;
typedef pair<EModelType,pair<int,CBasicModel*>> EntryModelSort;
typedef pair<int,LPDIRECT3DTEXTURE9> EntryTexture;
typedef pair<int,ID3DXMesh*> EntryGeometry;
typedef pair<int,CXModel*> EntryXModel;






////===== ���߲����ṹ =====
struct STRUCT_Line
{
	bool isReceivePick;
	int pickFlagId;
	int deepOrd;
	float thickness;
	D3DMATERIAL9 material;
	DWORD arrowColor;
    float arrowSize;
	bool isInverse;
	STRUCT_Ani aniDraw;
	STRUCT_Ani aniFlow;
	STRUCT_Ani aniTwinkle;
	D3DXVECTOR3 axis;
	float angle;
	int radCount;
};
////===== ��������ṹ =====
struct STRUCT_Area
{
	bool isReceivePick;
	int pickFlagId;
	int deepOrd;
	D3DMATERIAL9 material;
	STRUCT_Ani aniShow;
	D3DXVECTOR3 axis;
	float angle;
};

////===== ͼԪ�����ṹ ͬʱ��Ϊ�������ʹ��=====
struct STRUCT_Symbol
{
	bool isReceivePick;
	int pickFlagId;
	int deepOrd;
	int rootid;
	int parentid;
	int texturekey;
    float scaleX;
    float scaleY;
    float scaleZ;
	bool isH;
	D3DMATERIAL9 material;
	bool isUseColor;
	STRUCT_Ani aniTwinkle;
	STRUCT_Ani aniShow;
	STRUCT_Ani aniScale;
	bool isUseXModel;
    int XMKey;
	float XMScaleAddition;
	D3DXVECTOR3 axis;
	float angle;

};
////===== ���ݶ�������ṹ =====
struct STRUCT_PolyCol
{
	bool isReceivePick;
	int pickFlagId;
	int deepOrd;
	int rootid;
	int parentid;
	D3DMATERIAL9 material;
	float sizex;
	float sizey;
	float height;
	int geokey;
	STRUCT_Ani aniScale;
	STRUCT_Ani aniRotation;
	D3DXVECTOR3 axis;
	float angle;
};


////===== ���ü���������ṹ =====
struct STRUCT_Geometry
{
	EGeometryType geoType;
	float pf1;
	float pf2;
	float pf3;
	int pi1;
	int pi2;
};


////===== �Զ���ģ�Ͳ����ṹ =====
struct STRUCT_Custom
{
	bool isReceivePick;
	int pickFlagId;
	int deepOrd;
	int rootid;
	int parentid;
    float scaleX;
    float scaleY;
    float scaleZ;
	D3DMATERIAL9 material;
	D3DXVECTOR3 axis;
	float angle;
	int texturekey;
	EDrawMode drawMode;
};
