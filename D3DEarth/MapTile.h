#pragma once
#include <string>


class CMapTile :CBasicModel
{

public:
	static wstring mapIP;   //web��ʽ��ַ
	static wstring mapPath;  //·����ʽ  
	static wstring mapPath2;  //·����ʽ�ڶ���ַ  overlay
    static HRESULT Create(IDirect3DDevice9 *m_pd3dDevice, CBasicModel **ppModel ,int id, int zlayer, int idxx,int idxy, bool isShowTerrain, int terrainSpan, void* pHigh, CTriangleRenderer* tri);
	~CMapTile(void);
    HRESULT Render(IDirect3DDevice9 *m_pd3dDevice);
	wstring QuadKey;
	int id;
	int layer,idxX,idxY;

	HRESULT Init();

	static void SetEffect(IDirect3DDevice9 *m_pd3dDevice,CTriangleRenderer* myTri);

	void SetTerrain(bool IsShowTerrain, int TerrainSpan, void* PHigh);
	bool isShowTerrain;
	int terrainSpan;

protected:
    //HRESULT Init(IDirect3DDevice9 *m_pd3dDevice);

private:
	
   	CMapTile(void);

	void CheckTexture(); //�������״̬�������Ƿ���������

	HRESULT GetTexture(IDirect3DDevice9 *m_pd3dDevice); //��ȡ�����ļ�
	HRESULT GetOverlayTexture(IDirect3DDevice9 *m_pd3dDevice); //��ȡ�����ļ�

	EMapType curMapType; 
	bool curIsShowOverlay;

	HRESULT buildPlaneMesh();
	HRESULT buildTerrainMesh(void* pHigh);
	UINT TerrainNumVertices; //������
	UINT TerrainPrimCount; //������
	IDirect3DVertexBuffer9 *Terrain_pd3dVB;  //���㻺��
	IDirect3DIndexBuffer9 *Terrain_pd3dIB;  //��������
	float* pHigh;
};



