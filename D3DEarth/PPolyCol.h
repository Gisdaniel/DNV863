#pragma once
#include "basicmodel.h"
class CPPolyCol :public CBasicModel
{
public:
	static HRESULT Create(IDirect3DDevice9 *m_pd3dDevice, CBasicModel **ppModel,  int id , void* ppara, void* pmesh, int mcount, void* ptexture, int tcount,CTriangleRenderer* tri);
	~CPPolyCol(void);
    HRESULT Render(IDirect3DDevice9 *m_pd3dDevice);
	void calWorld();

	HRESULT Init(IDirect3DDevice9 *m_pd3dDevice,void* ppara, void* pmesh, int mcount, void* ptexture, int tcount);
	void ChangeProperty(EPropertyType propertytype,void* para,int count);

	CPPolyCol* pparent;


	D3DXVECTOR3 location;
	float distanceGround;
	float height;

	int geokey;
	static void SetEffect(IDirect3DDevice9 *m_pd3dDevice,CTriangleRenderer* myTri);
		
private:
	CPPolyCol(void);
	//DWORD color;
	float sizex;
	float sizey;

	float targetHeight; //Ŀ��ֵ
	float divHeight; //�߲�
	UINT starttick; //��ʼʱ�ļ�ʱ��
	bool isAni;
	
	STRUCT_Ani aniScale; //��˸��������;
	STRUCT_Ani aniRotation; //��ת��������;

	float rotateAngle; //��ת�Ƕ�
};

