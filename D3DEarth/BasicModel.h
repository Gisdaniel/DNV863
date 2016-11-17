#pragma once
class CBasicModel
{
public:
	CBasicModel(void);
	~CBasicModel(void);
	virtual HRESULT Render(IDirect3DDevice9 *m_pd3dDevice)=0;
	virtual void calWorld();

	D3DXMATRIXA16 world;
	map<int,CBasicModel*> submodels;

	CTriangleRenderer* myTri;

	int operate; //���� 0:��ʼ״̬ 1:���� 2:�Ѵ���
	int status; //״̬ 0:��ʼ״̬ 1: ������
	int id;
	int rootid;
	int parentid;

	bool isReceivePick;  //�Ƿ����ʰȡ
	int pickFlagId;  //ʰȡ�ı�־ID��Ӧ��������ָ����־��ʰȡ

	static CBasicModel* find(int rootid, int findid, CTriangleRenderer* myTri);
	CBasicModel* find(int findid);

	UINT recentUseTime;

	int Pick_ModelMesh(IDirect3DDevice9* pd3dDevice, POINT ptCursor, LPD3DXMESH testmesh);
	virtual int Pick_Model(IDirect3DDevice9* pd3dDevice, POINT ptCursor);

	void setOperate(int op); //���ò���״̬�������Ӷ���
	void clearSub(int op);// �����Ӷ�����operate=op����

	//void changeColor(DWORD color);

	int deepOrd;

	D3DXVECTOR3 axis; //��ʼ��ת��
	float angle; //��ʼ��ת�Ƕ�


protected:
	
	LPD3DXMESH mesh;

	bool isInner; //�Ƿ����ڲ�ģ�ͣ��ܸ���������Ӱ�죬�������ֱ�ӿ��ƣ����͵��ǳ�����ͷ

	UINT NumVertices; //������
	UINT PrimCount; //������
	IDirect3DVertexBuffer9 *m_pd3dVB;  //���㻺��
	IDirect3DIndexBuffer9 *m_pd3dIB;  //��������

	bool isDicTexture;
	bool isDicTexture2;
	LPDIRECT3DTEXTURE9	g_pTexture; //����
	LPDIRECT3DTEXTURE9	g_pTexture2; //����

	D3DMATERIAL9 mtrl;  //���ʣ�����ɫ��
	

private:

};

