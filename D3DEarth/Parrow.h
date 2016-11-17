#pragma once
#include "basicmodel.h"
class CParrow :	CBasicModel
{
public:
	static HRESULT Create(IDirect3DDevice9 *m_pd3dDevice, CBasicModel **ppModel, int id , CPline *parent, CTriangleRenderer* tri, int idx, int count);
	~CParrow(void);
	HRESULT Render(IDirect3DDevice9 *m_pd3dDevice);
	HRESULT Init(IDirect3DDevice9 *m_pd3dDevice, int id , CPline *parent);
	void initBaseWorld();

private:
    CParrow(void);

	CPline* parent;

	D3DXMATRIXA16 baseWorld;
	D3DXVECTOR3 location;

	void updateWorld(); //���㶯��world

    D3DXVECTOR3 lastPoint;
    D3DXVECTOR3 lastDir;  //��ͷ����
	int idx; //��ͷ���
	int count; //��ͷ��

};

