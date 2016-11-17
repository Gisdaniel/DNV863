#pragma once
class CXModel
{
public:
	CXModel(void);
	~CXModel(void);


	LPD3DXMESH              g_pMesh;  //����ģ�Ͷ���
	D3DMATERIAL9*           g_pMeshMaterials;  //����ģ�Ͳ���
	LPDIRECT3DTEXTURE9*     g_pMeshTextures;  //����ģ������
	DWORD                   g_dwNumMaterials;    //����ģ�Ͳ�������

	int modelkey;
	wstring xfile;

	D3DXVECTOR3 axis; //��ʼ��ת��
	float angle; //��ʼ��ת�Ƕ�

	HRESULT InitGeometry();  //��ʼx�ļ��ṩ��ģ��
	void InitCustom(void* pvertices,void* pnormal, int vcount, void* pindex, int icount, void* puv, int uvcount, void* ptexture); //��ʼ��custom���͵���Ϊģ��
	void RemovePathFromFileName(LPSTR fullPath, LPWSTR fileName);

	void Cleanup();

	void Render();

	bool isCustom; //��X��ʽ������custom��ʽ

	//��Ϊcustom����
	//LPD3DXMESH mesh;
	LPDIRECT3DTEXTURE9	g_pTexture; //����
	D3DMATERIAL9 mtrl;  //���ʣ�����ɫ��


};

