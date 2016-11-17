#pragma once


class CCamera;
class CMapTile;
class CBasicModel;

#include <map>
#include <vector>
#include <string>
using namespace std;


    ///<summary>��γ����ṹ</summary>
    struct GeoPoint
    {
		GeoPoint()
		{
            Longitude = 0;
            Latitude = 0;
		}
        GeoPoint(double latitude, double longitude)
        {
            Longitude = longitude;
            Latitude = latitude;
        }
        ///<summary>����</summary>
        double Longitude;
        ///<summary>γ��</summary>
        double Latitude;
    };


////===== ��ͼ���ò����ṹ =====
struct STRUCT_EarthPara
{
	STRUCT_EarthPara()
	{
		mapType=����;
		isShowOverlay=false;
		background=D3DCOLOR_ARGB(255, 0, 0, 0);

		tileReadMode=������Ƭ����;
		//tileFileOffsetLI=0; //��Ƭƫ��-����
		//tileFileOffsetXI=0; //��Ƭƫ��-X���ȷ������
		//tileFileOffsetYI=0; //��Ƭƫ��-Yγ�ȷ������
		isCover=false;

		SceneMode=����;
		InputCoordinate=WGS84��������;
		StartLocation=GeoPoint(0,0);
		EndLocation=GeoPoint(0,0);
		StartLayer=0;
		StartIdxX=0;
		StartIdxY=0;
		EndIdxX=0;
		EndIdxY=0;
		UnitLatLen=0;
		UnitLoneLen=0;

		ArrowIntelvar=100;

		isDepthStencil=false;
	}


	EMapType mapType;
	bool isShowOverlay;
	DWORD background;

	ETileReadMode tileReadMode; //��Ƭ��ȡģʽ��ȱʡgoogle web��ʽ���Զ����ļ���ȡ��ʽ, �Զ���web��ʽ
	//int tileFileOffsetLI; //��Ƭƫ��-����
	//int tileFileOffsetXI; //��Ƭƫ��-X���ȷ������
	//int tileFileOffsetYI; //��Ƭƫ��-Yγ�ȷ������
	bool isCover;

	///<summary>����ģʽ����Ϊ���ȹ�ϵ�����м�С��ͼ�ζ�����Ӧʹ�þ���ƽ������������Ƽ�С��ͼ��</summary>
	ESceneMode SceneMode;

	///<summary>�ⲿ������õ�����ϵ</summary>
	EInputCoordinate InputCoordinate;

	///<summary>�ֲ�ƽ�泡���µ���ʼ����</summary>
	GeoPoint StartLocation;
	///<summary>�ֲ�ƽ�泡���µĽ�������</summary>
	GeoPoint EndLocation;

	///<summary>�ֲ�ƽ�泡���µ���Ч��ʼ����</summary>
	int StartLayer;
	int StartIdxX;
	int StartIdxY;
	int EndIdxX;
	int EndIdxY;
	double UnitLatLen;
	double UnitLoneLen;
	double AdjustAspect; 
	double ArrowIntelvar; //������ͷ���

	bool isDepthStencil;  //�Ƿ�������Ȳ���
};


//typedef pair<int,float> VECPAIR;  //ģ��γ��������
typedef pair<int,int> VECPAIR;  //ģ��γ��������
typedef pair<int,UINT> BUFFERPAIR;  //��Ƭ���ʹ��ʱ��������

class CTriangleRenderer : public CRenderer
{
public:
    static HRESULT Create(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter, CRenderer **ppRenderer,UINT uWidth,UINT uHeight);
    ~CTriangleRenderer();

    HRESULT Render();
	
	CCamera *camera;
	void ChangeCameraAspect(float newaspect);
	void ChangeCameraPara(void* para,bool isAni,int duration);
	void ChangeLightPara(int lightNum, void* para);
	void ChangeAmbientLight(UINT color);
	int BeginTransfer();
	void EndTransfer();
	void AddMapTile(int id, int zlayer, int idxx,int idxy, bool isShowTerrain, int terrainSpan, void* pHigh);
	void LoadTile(); //��ʼ����������Ƭ��Դ

	int BeginTransferModel();
	void EndTransferModel();
	//void LoadModel(); //��ʼ����������Ƭ��Դ
	void ChangeProperty(EModelType modeltype,EPropertyType propertytype,int id ,int subid,void* para,int count,void* para2, int count2);
	void AddModel(EModelType modeltype,int id, void* para, void* pmesh, int mcount , void* ptexture, int tcount); //����ģ��
	void AddCustomModel(int id, void* para, void* plocation, void* pvertices,void* pnormal, int vcount, void* pindex, int icount, void* puv, int uvcount, void* ptexture);

	bool isCancleTile; //������ǰ�ж���Ƭ�����߳�
	int statusTile; //��Ƭ����״̬ 0: ����  1: �����ݺͼ����� 2: �ɸ���
	int statusModel;//ģ������״̬ 0: ����  1: �����ݺͼ����� 2: �ɸ���


	//map<int,CBasicModel*> linemodels;  //�����е�model
	//map<int,CBasicModel*> newlinemodels;  //models���ϵ�buffer
	//
	//map<int,CBasicModel*> symbolmodels;  //�����е�model
	//map<int,CBasicModel*> newsymbolmodels;  //models���ϵ�buffer

	//map<int,CBasicModel*> textmodels;  //�����е�model
	//map<int,CBasicModel*> newtextmodels;  //models���ϵ�buffer

	//vector<VECPAIR> vecPolycol;
	//map<int,CBasicModel*> polycolmodels;  //�����е�model
	//map<int,CBasicModel*> newpolycolmodels;  //models���ϵ�buffer

	//map<int,CBasicModel*> areamodels;  //�����е�model
	//map<int,CBasicModel*> newareamodels;  //models���ϵ�buffer


	//map<int,CBasicModel*>* models[5];
	//map<int,CBasicModel*>* newmodels[5];

	vector<VECPAIR> vecModel;
	map<int,CBasicModel*> models;
	map<int,CBasicModel*> newmodels;


	int Pick_Model(POINT ptCursor);// ʰȡģ��
	int Pick_Model(POINT ptCursor,int flagid);// ʰȡģ��

	STRUCT_EarthPara earthpara; //��ͼ����ʾ���Ʋ����ṹ

	CPara para;

	wstring debuginfo;

	D3DMATERIAL9 basicmtrl;

protected:
    HRESULT Init(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter,UINT uWidth,UINT uHeight);

private:
    CTriangleRenderer();
	

	void UpdateMaps();
	void UpdateModels();

    IDirect3DVertexBuffer9 *m_pd3dVB;  //δʹ��

	map<int,CBasicModel*> maps;  //maptile����
	map<int,CBasicModel*> newmaps;  //maptile���ϵ�buffer

	map<int,CBasicModel*> buffermaps;  //maptile��buffer����
		

	//�������
	STRUCT_Light lights[6];

	

	
};
