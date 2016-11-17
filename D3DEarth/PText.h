#pragma once
#include "basicmodel.h"
#include "PPolyCol.h"
class CPText :public CBasicModel
{
public:
	~CPText(void);
	static HRESULT Create(IDirect3DDevice9 *m_pd3dDevice, CBasicModel **ppModel, int id , void* ppara, void* pmesh, int mcount, void* ptext, int tcount,CTriangleRenderer* tri);
	HRESULT Init(IDirect3DDevice9 *m_pd3dDevice, void* ppara, void* pmesh, int mcount, void* ptext, int tcount);
    HRESULT Render(IDirect3DDevice9 *m_pd3dDevice);
	static void SetEffect(IDirect3DDevice9 *m_pd3dDevice,CTriangleRenderer* myTri);
	void ChangeProperty(EPropertyType propertytype,void* para,int count);
	D3DXVECTOR3 location;
	
	CPPolyCol* pparent;
	void calWorld();

private:
    CPText(void);
	
	vector<wstring> text;
	
	int texturekey;//���ʱ���

	//DWORD color;
	float scalex; //�����x������������
	float scaley; //�����y������������

	float width; //ʵ��ʹ�ÿ��
	float height; //ʵ��ʹ�ø߶�
	bool isH;
	bool is3D;

	STRUCT_Ani aniTwinkle; //��˸��������;

	HRESULT CreateTexture(IDirect3DDevice9 *m_pd3dDevice);

	RECT getScreenRect(D3DXVECTOR3 loca);
};

