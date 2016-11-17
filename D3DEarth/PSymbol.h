#pragma once
#include "basicmodel.h"
#include "PPolyCol.h"
class CPSymbol :public CBasicModel
{
public:
	~CPSymbol(void);
	static HRESULT Create(IDirect3DDevice9 *m_pd3dDevice, CBasicModel **ppModel, int id , void* ppara, void* pmesh, int mcount, void* ptexture, int tcount,CTriangleRenderer* tri);
	HRESULT Init(IDirect3DDevice9 *m_pd3dDevice, void* ppara, void* pmesh, int mcount, void* ptexture, int tcount);
	void ChangeProperty(EModelType modeltype, EPropertyType propertytype,void* para,int count);
    HRESULT Render(IDirect3DDevice9 *m_pd3dDevice);
	static void SetEffect(IDirect3DDevice9 *m_pd3dDevice,CTriangleRenderer* myTri);

	D3DXVECTOR3 location;
	
	CPPolyCol* pparent;
	void calWorld();

	int Pick_Model(IDirect3DDevice9* pd3dDevice, POINT ptCursor);

private:
    CPSymbol(void);
	
	int texturekey;
	float scaleX;
	float scaleY;
	float scaleZ;
	float saveX;
	float saveY;
	float saveZ;
	bool isH;
	//DWORD color;
	bool isUseColor;
	bool isUseXModel;  //�Ƿ�ʹ��.X 3Dģ��
	int XMKey; //.X 3Dģ�ͼ������������
	float XMScaleAddition; //3Dģ��ʱ�ĸ��ӱ���ϵ��

	STRUCT_Ani aniTwinkle; //��˸��������;
	STRUCT_Ani aniShow; //������ʾ��������;
	STRUCT_Ani aniScale; //���Ŷ�������;

	float progress;//ͼ����ʾ�İٷֱȽ���

	float savealpha;
};

