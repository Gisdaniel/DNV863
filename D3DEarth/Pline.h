#pragma once
#include "renderer.h"



class CPline : CBasicModel
{
public:
	static HRESULT Create(IDirect3DDevice9 *m_pd3dDevice, CBasicModel **ppModel, int id ,void* ppara, void* pdots, int dotcount,CTriangleRenderer* tri);
	~CPline(void);
    HRESULT Render(IDirect3DDevice9 *m_pd3dDevice);

	HRESULT Init(IDirect3DDevice9 *m_pd3dDevice,void* ppara, void* pdots, int dotcount);
	void ChangeProperty(EModelType modeltype, EPropertyType propertytype,void* para,int count);

	D3DXVECTOR3* points;

	int pointCount;

	void showFlow(bool isshow);
	DWORD arrowColor;
	float arrowSize; 
	bool isInverse;

	int radCount; //�����ķ�Ƭ��

	STRUCT_Ani aniFlow; //������������
	float lenth;  //��·����
    D3DXVECTOR3* dirs; //�߶ι�һ������
    float* lens; //�߶γ���


	static void SetEffect(IDirect3DDevice9 *m_pd3dDevice,CTriangleRenderer* myTri);

private:
	CPline(void);

	STRUCT_Ani aniDraw; //���ƶ�������
	STRUCT_Ani aniTwinkle; //��˸��������;
	//DWORD color;
	float thickness; //�߿�
	float savealpha; //ԭʼɫ��Alpahֵ

	void buildMesh();
	void getPosition(float progress, int* passcount, D3DXVECTOR3* pos);  //����progress�����㶯���ߵ���λ��
	void buildVerticBuffer(int passcount,  D3DXVECTOR3 pos);  //�������ƶ����Ķ���buffer

	CUSTOMVERTEX_N* vertices;  //���ƶ����ö���
	WORD* idxes; //���ƶ���������

	float progress;//ͼ����ʾ�İٷֱȽ���
};

