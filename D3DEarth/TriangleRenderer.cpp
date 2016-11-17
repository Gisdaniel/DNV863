//+-----------------------------------------------------------------------------
//
//  CTriangleRenderer
//
//      Subclass of CRenderer that renders a single, spinning triangle
//
//------------------------------------------------------------------------------

#include "StdAfx.h"
#include <process.h> 
#include <algorithm>
//===========================================================
// ���̴߳�����Ƭmesh�Ͳ�������
//===========================================================
static unsigned __stdcall
LoadTileThread(void* pArguments)
{
	CTriangleRenderer* pthis=(CTriangleRenderer*) pArguments;

	pthis->LoadTile();

	_endthreadex(   0   );  
    return   0; 
}


int cmp(const VECPAIR& x, const VECPAIR& y) {return x.second<y.second;}
//+-----------------------------------------------------------------------------
//
//  Member:
//      CTriangleRenderer ctor
//
//------------------------------------------------------------------------------
CTriangleRenderer::CTriangleRenderer() : CRenderer(), m_pd3dVB(NULL) 
{
	debuginfo=L"";
}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CTriangleRenderer dtor
//
//------------------------------------------------------------------------------
CTriangleRenderer::~CTriangleRenderer()
{
    SAFE_RELEASE(m_pd3dVB);

}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CTriangleRenderer::Create
//
//  Synopsis:
//      Creates the renderer
//
//------------------------------------------------------------------------------
HRESULT 
CTriangleRenderer::Create(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter, CRenderer **ppRenderer,UINT uWidth,UINT uHeight)
{
	


    HRESULT hr = S_OK;

    CTriangleRenderer *pRenderer = new CTriangleRenderer();
    IFCOOM(pRenderer);

    IFC(pRenderer->Init(pD3D, pD3DEx, hwnd, uAdapter, uWidth,uHeight));

    *ppRenderer = pRenderer;
    pRenderer = NULL;


Cleanup:
    delete pRenderer;

    return hr;
}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CTriangleRenderer::Init
//
//  Synopsis:
//      Override of CRenderer::Init that calls base to create the device and 
//      then creates the CTriangleRenderer-specific resources
//
//------------------------------------------------------------------------------
HRESULT 
CTriangleRenderer::Init(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter,UINT uWidth,UINT uHeight)
{

    HRESULT hr = S_OK;

	statusTile=0;
	statusModel=0;

	//models[0]=&linemodels;
	//newmodels[0]=&newlinemodels;
	//models[1]=&symbolmodels;
	//newmodels[1]=&newsymbolmodels;
	//models[2]=&polycolmodels;
	//newmodels[2]=&newpolycolmodels;
	//models[3]=&areamodels;
	//newmodels[3]=&newareamodels;
	//models[4]=&textmodels;
	//newmodels[4]=&newtextmodels;


	earthpara.isShowOverlay=true;
	earthpara.mapType=����;

	for (int i=0; i<sizeof(lights)/sizeof(STRUCT_Light); i++)  //��ʼ����Դ����
	{
		ZeroMemory(&(lights[i]), sizeof(STRUCT_Light));
	
	}


	D3DXVECTOR3 POS=D3DXVECTOR3(2177.739,4094.09,4388.384);

	CCamera::Create(POS,D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DX_PI / 4, 1.0f, 1000.0f,1.0f, &camera);
 

    // Call base to create the device and render target
    IFC(CRenderer::Init(pD3D, pD3DEx, hwnd, uAdapter,uWidth,uHeight));

	IFC(m_pd3dDevice->SetTransform(D3DTS_VIEW, &camera->view));
	IFC(m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &camera->projection));

    // Set up the global state
    IFC(m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));

	//D3DCOLOR ambientColor=D3DCOLOR_XRGB(128,128,128);
	//IFC(m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, true));
	//m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, ambientColor);
	


	//��ʼ�����������������ֵ��豸
	D3DXCreateFont(m_pd3dDevice,20,8,0,0,0,0,0,0,0, L"Arial", &para.g_pFont2D);	
	D3DXCreateFont(m_pd3dDevice,50,20,4,0,0,0,0,0,0, L"Microsoft YaHei", &para.g_pFont3D);	

	//��ʼ����������
    ZeroMemory( &basicmtrl, sizeof(D3DMATERIAL9) );
	basicmtrl.Diffuse.r = basicmtrl.Ambient.r = 1;//1.0f;
    basicmtrl.Diffuse.g = basicmtrl.Ambient.g =1;// 0.0f;
    basicmtrl.Diffuse.b = basicmtrl.Ambient.b =1;// 0.0f;
    basicmtrl.Diffuse.a = basicmtrl.Ambient.a =1;// 1.0f;

Cleanup:
    return hr;
}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CTriangleRenderer::Render
//
//  Synopsis:
//      Renders the rotating triangle
//
//------------------------------------------------------------------------------
HRESULT
CTriangleRenderer::Render()
{
    HRESULT hr = S_OK;
	//============== �������� ==================
  /*  m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	for (int i=0; i<8; i++)
	{
		if (lights[i].isEnable)
			m_pd3dDevice->SetLight( i, &lights[i].light );
	    m_pd3dDevice->LightEnable( i, lights[i].isEnable );
	}*/



	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE); //��һ�������ߣ�ʹģ�ͷ��߲�������Ӱ��



	//���û�������
	m_pd3dDevice->SetMaterial( &basicmtrl );



	//  �������
	if (camera->isAni)
	{
		float progress=(float)(GetTickCount()-camera->startTick)/camera->duration;
		if (progress>=1)
		{
			progress=1;
			camera->isAni=false;
		}
		camera->aniCamera(progress);
		m_pd3dDevice->SetTransform(D3DTS_VIEW, &camera->view);
		m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &camera->projection);
	}



    IFC(m_pd3dDevice->BeginScene());

	IFC(m_pd3dDevice->Clear(
        0,
        NULL,
		D3DCLEAR_TARGET| D3DCLEAR_ZBUFFER, //zh��Ȳ���
        earthpara.background,  // NOTE: Premultiplied alpha!D3DCOLOR_ARGB(255, 255, 0, 0)
        1.0f,
        0
        ));



	//============ ������Ƭ


	if (this->earthpara.mapType!=��)
	{
		CMapTile::SetEffect(m_pd3dDevice,this);
		if (statusTile==2) //��Ƭ������ɣ�����maps
		{
			UpdateMaps();
			statusTile=0;
		}
		for (auto ent=maps.begin();ent!=maps.end();ent++)//ѭ��������Ƭ
		{
			ent->second->Render(m_pd3dDevice);
		}
	}


	//=========== ����ģ��
	if (statusModel==2) //ģ��������ɣ�����models
	{
		UpdateModels();
		statusModel=0;
	}


	////=========== ��������
	//CPArea::SetEffect(m_pd3dDevice,this);
	//for (auto ent=areamodels.begin();ent!=areamodels.end();ent++)
	//	ent->second->Render(m_pd3dDevice);

	////=========== ������·
	//CPline::SetEffect(m_pd3dDevice,this);
	//for (auto ent=linemodels.begin();ent!=linemodels.end();ent++)
	//	ent->second->Render(m_pd3dDevice);
	////=========== ����ͼԪ
	//CPInfoPanel::SetEffect(m_pd3dDevice,this);
	//for (auto ent=symbolmodels.begin();ent!=symbolmodels.end();ent++)
	//	ent->second->Render(m_pd3dDevice);

	////=========== ��������
	//CPText::SetEffect(m_pd3dDevice,this);
	//for (auto ent=textmodels.begin();ent!=textmodels.end();ent++)
	//	ent->second->Render(m_pd3dDevice);
	////=========== ���Ƽ�����
	//CPPolyCol::SetEffect(m_pd3dDevice,this);
	//for (auto ent=vecPolycol.begin();ent!=vecPolycol.end();ent++)
	//{
	//	polycolmodels.find(ent->first)->second->Render(m_pd3dDevice);
	//}



	for (auto ent=vecModel.begin();ent!=vecModel.end();ent++)
	{
		models.find(ent->first)->second->Render(m_pd3dDevice);
	}




	IFC(m_pd3dDevice->EndScene());


	//m_pd3dDevice->Present(NULL,NULL,NULL,NULL); 
	//m_pd3dRTS->Release();


	//debug��Ϣ
	//RECT rect;
	//rect.left=0;
	//rect.right=200;
	//rect.top=200;
	//rect.bottom=400;
	//para.g_pFont2D->DrawText(NULL, debuginfo.c_str(), debuginfo.length(), &rect,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_VCENTER, 0xff00ffff);

	
Cleanup:
    return hr;
}


#pragma region �������  

void CTriangleRenderer::ChangeCameraAspect(float newaspect)
{
	camera->Aspect=newaspect;
	camera->Update();

	m_pd3dDevice->SetTransform(D3DTS_VIEW, &camera->view);
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &camera->projection);


}


void 
CTriangleRenderer::ChangeCameraPara(void* pPara,bool isAni,int duration)
{
	STRUCT_Camera* para=(STRUCT_Camera*)pPara;
	if (isAni)
	{
		memcpy(&camera->oldpara,&camera->curpara,sizeof(STRUCT_Camera));
		memcpy(&camera->newpara,para,sizeof(STRUCT_Camera));
		camera->isAni=true;
		camera->startTick=GetTickCount();
		camera->duration=duration;
	}
	else
	{
		memcpy(&camera->curpara,para,sizeof(STRUCT_Camera));
		camera->Update();
		m_pd3dDevice->SetTransform(D3DTS_VIEW, &camera->view);
		m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &camera->projection);
	}
}

#pragma endregion
void 
CTriangleRenderer::ChangeLightPara(int lightNum, void* pPara)
{
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	//����
	STRUCT_Light* ppara=(STRUCT_Light*)pPara;
	memcpy(&(lights[lightNum]),ppara,sizeof(STRUCT_Light));
	//��Ч
	m_pd3dDevice->SetLight( lightNum, &(lights[lightNum].light) );
    m_pd3dDevice->LightEnable( lightNum, lights[lightNum].isEnable );
}
void 
CTriangleRenderer::ChangeAmbientLight(UINT color)
{
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	D3DXCOLOR c=D3DXCOLOR(color);
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, c);
}


#pragma region ��Ƭ�������
//************************************************************
//ģʽ��wpfÿ����begintransfer��endtransfer֮�䴫��������Ƭ����
//����: 
// 1. wpf����begintransfer, ����buffer����ƬoperateΪ��ʼ״̬
// 2. wpf����addmaptile�����Ƭ��������buffer�У���operateΪ1
// 3. wpf����endtransfer, ɾ��buffer�У�operate��Ϊ1����������̣߳���ʼ���¼������Ƭ
// 4. ���߳̽����󣬽��ù�������statusΪ�����
// 5. wpf����renderʱ������������statusΪ����ɣ������buffer��maps
//************************************************************
//============================================================
// ��ʼ������Ƭ���ݣ�ʧ�ܷ���0���ɹ�����1
//============================================================
int
CTriangleRenderer::BeginTransfer()
{
	while (statusTile!=0) 
	{
		isCancleTile=true;//return 0;  //�ɳ��Ը�Ϊ��ǰ���������߳�
		Sleep(10);
	}

	if ((statusTile==0) || (statusTile==1)) //�������Ѵ��ڵ���Ƭ������״̬�ó�ʼ̬
	{
		for (auto ent=newmaps.begin();ent!=newmaps.end();ent++)
		{
			(ent->second)->operate=0;
		}

		return 1;
	}
	else
		return 0;


}
//===========================================================
// ������Ƭ���䣬�����߳�
//===========================================================
void
CTriangleRenderer::EndTransfer()
{
	//===== ����buffer����, 1.ɾ����ʼ״����Ŀ; 2. ����maps�����е���Ϊ�Ѵ���
	auto iter=newmaps.begin();
	while (iter!=newmaps.end()) //ɾ����Ч��Ŀ
	{
		if ((iter->second)->operate==0)
		{
			if (maps.find(iter->first)==maps.end()) //����ʾ�����в����ڵģ�ɾ��������ģ�Ͷ���
				delete iter->second;
			iter=newmaps.erase(iter);
		}
		else
			iter++;
	}
	
	for (auto ent=newmaps.begin();ent!=newmaps.end();ent++) //����maps�����е���Ϊ�Ѵ���
	{
		auto mapsiter=maps.find(ent->first);
		if (mapsiter!=maps.end())
			(ent->second)->operate=2;
	}

	//===== ���̴߳�������
	

	HANDLE   hThread;  
    unsigned   threadID;  
  
	statusTile=1;
    hThread   =   (HANDLE)_beginthreadex(NULL, 0, &LoadTileThread, this, 0, &threadID   );   
	isCancleTile=false;
	CloseHandle(hThread);
}


int cmpBuffer(const BUFFERPAIR& x, const BUFFERPAIR& y) {return x.second>y.second;}
//===========================================================
// ����Maps, ����Ϊbuffer��newmaps, ���µ�maps
//===========================================================
void
CTriangleRenderer::UpdateMaps()
{
	//==ɾ��������Ч����Ƭ, �ڵȴ�������û�е���Ƭ�����Ƴ�
	auto iter=maps.begin();
	while (iter!=maps.end()) 
	{
		if (newmaps.find(iter->first)==newmaps.end())
		{
			if (buffermaps.find(iter->first)==buffermaps.end()) //buffer��û�У���buffer
			{
				//buffer��1000��Ĵ���
				if (buffermaps.size()>1000)
				{
					vector<BUFFERPAIR> vecBuffer;
					for (auto biter=buffermaps.begin();biter!=buffermaps.end();biter++)
						vecBuffer.push_back(make_pair(biter->first,(biter->second)->recentUseTime));
					sort(vecBuffer.begin(),vecBuffer.end(),cmpBuffer);
					//��ɾ��N��
					for (int i=0;i<30;i++)
					{
						delete buffermaps.find(vecBuffer.begin()->first)->second;
						buffermaps.erase(vecBuffer.begin()->first);
						vecBuffer.erase(vecBuffer.begin());
					}
				}

				//��buffer
				buffermaps.insert(EntryMap(iter->first,iter->second));	
				iter->second->recentUseTime=GetTickCount();
			}
			iter=maps.erase(iter);
		}
		else
			iter++;
	}
	//==�����µ���Ƭ���ڵȴ������в���Ϊ1(����)�Ĳ����ӣ�����2(�Ѵ���)����
	for (auto ent=newmaps.begin();ent!=newmaps.end();ent++)
	{
		if ((ent->second)->operate==1)  //��Ϊ���������
		{
			maps.insert(EntryMap(ent->first,ent->second));
		}
	}
}


//===============================================
// ������Ƭmesh, ���������, �����̵߳���
//===============================================
void
CTriangleRenderer::LoadTile()
{

		Concurrency::parallel_for_each(newmaps.begin(),newmaps.end(),[](pair<int,CBasicModel*> ent)
			{
				if (((ent.second)->operate==1) && ((ent.second)->status==0))  //������δ�����
					((CMapTile*)ent.second)->Init();
			}
			);

	////���߳�����
	//for (auto ent=newmaps.begin();ent!=newmaps.end();ent++) //��������
	//{
	//	if (((ent->second)->operate==1) && ((ent->second)->status==0))  //������δ�����
	//		((CMapTile*)ent->second)->Init();
	//	if (isCancleTile)
	//	{
	//		statusTile=0;
	//		return;
	//	}
	//}


	statusTile=2;
}
//===============================================
// �����Ƭ
//===============================================
void CTriangleRenderer::AddMapTile(int id, int zlayer, int idxx,int idxy, bool isShowTerrain, int terrainSpan, void* pHigh)
{

	auto iter= newmaps.find(id);
	if (iter==newmaps.end()) //��������������
	{
		auto bufferiter=buffermaps.find(id);
		if (bufferiter==buffermaps.end()) //buffer��Ҳ�ޣ���ȫ����
		{
			CBasicModel *tmp;
			CMapTile::Create(m_pd3dDevice,&tmp, id,zlayer,idxx,idxy,isShowTerrain,terrainSpan, pHigh,this);
			tmp->operate=1; //�ò���״̬
			newmaps.insert(EntryMap(id,tmp));

			tmp=NULL;
		}
		else  //buffer���ҵ�����ӵ�����������
		{
			newmaps.insert(EntryMap(id,bufferiter->second));
			bufferiter->second->operate=1;
			((CMapTile*)(bufferiter->second))->SetTerrain(isShowTerrain,terrainSpan,pHigh);
			buffermaps.erase(id); //��buffer���Ƴ�
		}
	}
	else
	{
		iter->second->operate=1;

		((CMapTile*)(iter->second))->SetTerrain(isShowTerrain,terrainSpan,pHigh);
	}

}




#pragma endregion

#pragma region ģ�����
//************************************************************
//ģʽ��wpfÿ����begintransfermodel��endtransfermodel֮�䴫������ģ������
//����: 
// 1. wpf����begintransfermodel, ����buffer��ģ��operateΪ��ʼ״̬
// 2. wpf����addmodel���ģ�ͣ�������buffer�У���operateΪ1, ����ȫ�µģ�ֱ�����ɣ�ע���������߳�����ΪΪ��ʹģ�͵���ɾ������Ч��
// 3. wpf����endtransfermodel, ɾ��buffer�У�operate��Ϊ1����, ��������statusΪ�����
// 4. wpf����renderʱ������������statusΪ����ɣ������buffer��maps
//************************************************************

//============================================================
// ��ʼ����ģ�����ݣ�ʧ�ܷ���0���ɹ�����1, �����ͬʱ������model
//============================================================
int
CTriangleRenderer::BeginTransferModel()
{
	while (statusModel==1) 
	{
		Sleep(10);
	}

	if (statusModel==0) //�������Ѵ��ڵ�ģ�ͣ�����״̬�ó�ʼ̬
	{
		//for (int i=0;i<sizeof(newmodels)/sizeof(newmodels[0]);i++)
		//{
		//	for (auto iter=newmodels[i]->begin();iter!=newmodels[i]->end();iter++)
		//	{
		//		iter->second->setOperate(0); //�����Ӷ���
		//	}
		//}
		for (auto iter=newmodels.begin();iter!=newmodels.end();iter++)
			iter->second->setOperate(0);

		return 1;
	}
	else
		return 0;


}
//===========================================================
// ����ģ�ʹ���
//===========================================================
void
CTriangleRenderer::EndTransferModel()
{

	
		//auto showmodels=models[i];  //���ֵ�ģ�ͼ���
		//auto buffermodels=newmodels[i];   //��ӦΪbuffer��ģ�ͼ���


		//===== ����buffer����, 1.ɾ������ʾ������û�еĳ�ʼ״����Ŀ; 2. ����maps�����е���Ϊ�Ѵ���
		auto iter=newmodels.begin();
		while (iter!=newmodels.end()) //ɾ����Ч��Ŀ
		{
			if ((iter->second)->operate==0)
			{
				if (models.find(iter->first)==models.end()) //����ʾ�����в����ڵģ�ɾ��������ģ�Ͷ���
					delete iter->second;

				iter=newmodels.erase(iter);
			}
			else
			{
				iter->second->clearSub(0); //zhע���Ӽ�������ֱ���������ܻ��г�ͻ
				iter++;
			}
		}

		for (auto ent=newmodels.begin();ent!=newmodels.end();ent++) //���ڳ��������е���Ϊ�Ѵ���
		{
			auto mapsiter=models.find(ent->first);
			if (mapsiter!=models.end())
			{
				(ent->second)->operate=2;
				if (mapsiter->second!=ent->second)  //��buffer�ж��������ϵ���ж���ͬ����bufferȡ��ԭϵ���ж���
				{
					delete mapsiter->second;
					mapsiter->second=ent->second;
				}
			}
		}
	
	statusModel=2; //״̬��Ϊ��ɣ��Ա�render����




	//for (int i=0;i<sizeof(models)/sizeof(models[0]);i++)
	//{
	//	auto showmodels=models[i];  //���ֵ�ģ�ͼ���
	//	auto buffermodels=newmodels[i];   //��ӦΪbuffer��ģ�ͼ���


	//	//===== ����buffer����, 1.ɾ������ʾ������û�еĳ�ʼ״����Ŀ; 2. ����maps�����е���Ϊ�Ѵ���
	//	auto iter=buffermodels->begin();
	//	while (iter!=buffermodels->end()) //ɾ����Ч��Ŀ
	//	{
	//		if ((iter->second)->operate==0)
	//		{
	//			if (showmodels->find(iter->first)==showmodels->end()) //����ʾ�����в����ڵģ�ɾ��������ģ�Ͷ���
	//				delete iter->second;

	//			iter=buffermodels->erase(iter);
	//		}
	//		else
	//		{
	//			iter->second->clearSub(0); //zhע���Ӽ�������ֱ���������ܻ��г�ͻ
	//			iter++;
	//		}
	//	}

	//	for (auto ent=buffermodels->begin();ent!=buffermodels->end();ent++) //���ڳ��������е���Ϊ�Ѵ���
	//	{
	//		auto mapsiter=showmodels->find(ent->first);
	//		if (mapsiter!=showmodels->end())
	//			(ent->second)->operate=2;
	//	}
	//}
	//statusModel=2; //״̬��Ϊ��ɣ��Ա�render����

	
}

//===========================================================
// ����Models, ����Ϊbuffer��newmodels, ���µ�models
//===========================================================
void
CTriangleRenderer::UpdateModels()
{
	//for (int i=0;i<sizeof(models)/sizeof(models[0]);i++)
	//{
	//	auto showmodels=models[i];  //���ֵ�ģ�ͼ���
	//	auto buffermodels=newmodels[i];   //��ӦΪbuffer��ģ�ͼ���

	//	//==ɾ��������Ч��ģ��, 1.��buffer��û�е����ɾ��
	//	auto iter=showmodels->begin();
	//	while (iter!=showmodels->end()) 
	//	{
	//		auto newiter=buffermodels->find(iter->first);
	//		if (newiter==buffermodels->end())
	//		{
	//			delete iter->second;
	//			iter=showmodels->erase(iter);
	//		}
	//		else
	//			iter++;
	//	}


	//	//==�����µ�ģ�ͣ���buffer�в���Ϊ1(����)�Ĳ����ӣ�����2(�Ѵ���)����
	//	for (auto iter=buffermodels->begin();iter!=buffermodels->end();iter++)
	//	{
	//		if ((iter->second)->operate==1)  //��Ϊ���������
	//		{
	//			showmodels->insert(EntryMap(iter->first,iter->second));
	//		}
	//	}
	//}

	//// ������Ȳ���ʱ�����Ӵ���polycol������
	//vecPolycol.clear();
	//for (auto iter=polycolmodels.begin();iter!=polycolmodels.end();iter++)
	//{
	//	vecPolycol.push_back(make_pair(iter->first,((CPPolyCol*)(iter->second))->latitude));
	//}
	//sort(vecPolycol.begin(),vecPolycol.end(),cmp);


		//auto showmodels=models[i];  //���ֵ�ģ�ͼ���
		//auto buffermodels=newmodels[i];   //��ӦΪbuffer��ģ�ͼ���

		//==ɾ��������Ч��ģ��, 1.��buffer��û�е����ɾ��
		auto iter=models.begin();
		while (iter!=models.end()) 
		{
			auto newiter=newmodels.find(iter->first);
			if (newiter==newmodels.end())
			{
				delete iter->second;
				iter=models.erase(iter);
			}
			else
				iter++;
		}


		//==�����µ�ģ�ͣ���buffer�в���Ϊ1(����)�Ĳ����ӣ�����2(�Ѵ���)����
		for (auto iter=newmodels.begin();iter!=newmodels.end();iter++)
		{
			if ((iter->second)->operate==1)  //��Ϊ���������
			{
				models.insert(EntryMap(iter->first,iter->second));
			}
		}

	// ������Ȳ���ʱ�����Ӵ���polycol������
	vecModel.clear();
	for (auto iter=models.begin();iter!=models.end();iter++)
	{
		vecModel.push_back(make_pair(iter->first,iter->second->deepOrd));
	}
	sort(vecModel.begin(),vecModel.end(),cmp);

}

//-----------------------------------------------------------
//�����Զ���ģ��
//-----------------------------------------------------------
void
CTriangleRenderer::AddCustomModel(int id, void* para, void* plocation, void* pvertices,void* pnormal, int vcount, void* pindex, int icount, void* puv, int uvcount, void* ptexture)
{
	
		auto iter= newmodels.find(id);
		if (iter==newmodels.end())  //�¶�����ؽ�
		{
			CBasicModel *tmp;
			CPCustom::Create(m_pd3dDevice,&tmp, id,para, plocation,pvertices,pnormal,vcount,pindex,icount,puv,uvcount,ptexture ,this);
			tmp->operate=1; //�ò���״̬
			newmodels.insert(EntryMap(id,tmp));
			tmp=NULL;
		}
		else  //�Ѵ��ڣ����ؽ�
		{
				iter->second->operate=1;
		}
}
//-----------------------------------------------------------
//����ģ��
//-----------------------------------------------------------
void
CTriangleRenderer::AddModel(EModelType modeltype,int id, void* para, void* pmesh, int mcount, void* ptexture, int tcount)
{
	//D3DXVECTOR3* pd=(D3DXVECTOR3*)pmesh;
	
	if (modeltype==����) // ��������
	{
		auto iter= newmodels.find(id);
		if (iter==newmodels.end())
		{
			CBasicModel *tmp;
			CPline::Create(m_pd3dDevice,&tmp, id,para, pmesh, mcount,this);
			tmp->operate=1; //�ò���״̬
			newmodels.insert(EntryMap(id,tmp));
			tmp=NULL;
		}
		else
		{
			iter->second->operate=1;
		}
	}
	else if (modeltype==ͼԪ)  //ˮƽͼԪ����
	{
		auto iter= newmodels.find(id);
		if (iter==newmodels.end())
		{
			CBasicModel *tmp;
			CPSymbol::Create(m_pd3dDevice,&tmp, id,para, pmesh, mcount, ptexture, tcount,this);
			tmp->operate=1; //�ò���״̬
			newmodels.insert(EntryMap(id,tmp));
			tmp=NULL;
		}
		else
		{
			iter->second->operate=1;
		}
	}
	else if (modeltype==������)  //������
	{
		STRUCT_PolyCol* p=(STRUCT_PolyCol*)para;
		CPPolyCol* findobj=(CPPolyCol*)CPPolyCol::find(p->rootid,id, this);

		if (findobj==nullptr)
		{
			CBasicModel *tmp;
			CPPolyCol::Create(m_pd3dDevice,&tmp, id,para, pmesh, mcount, ptexture, tcount,this);
			tmp->operate=1; //�ò���״̬
			
			if (((CPPolyCol*)tmp)->rootid==id)  //�����ݼӵ�newmodels
				newmodels.insert(EntryMap(id,tmp));
			else //�Ǹ���������������Ӽ���
				((CPPolyCol*)tmp)->pparent->submodels.insert(EntryMap(id,tmp));

			tmp=NULL;
		}
		else
		{
			findobj->operate=1;
		}
	}
	else if (modeltype==����)
	{
		STRUCT_Symbol* p=(STRUCT_Symbol*)para;
		if (p->rootid==id) //��������
		{
			auto iter= newmodels.find(id);
			if (iter==newmodels.end())
			{
				CBasicModel *tmp;
				CPText::Create(m_pd3dDevice,&tmp, id,para, pmesh, mcount, ptexture, tcount,this);
				tmp->operate=1; //�ò���״̬
				newmodels.insert(EntryMap(id,tmp));
				tmp=NULL;
			}
			else
			{
				iter->second->operate=1;
			}

		}
		else  //��������������
		{
			CPText* findobj=(CPText*)CPPolyCol::find(p->rootid,id,this);
			if (findobj==nullptr)
			{
				CBasicModel *tmp;
				CPText::Create(m_pd3dDevice,&tmp, id,para, pmesh, mcount, ptexture, tcount,this);
				tmp->operate=1; //�ò���״̬

				((CPText*)tmp)->pparent->submodels.insert(EntryMap(id,tmp));

				tmp=NULL;
			}
			else
			{
				findobj->operate=1;
			}
		}



	}
	else if (modeltype==����)
	{
		auto iter= newmodels.find(id);
		if (iter==newmodels.end())
		{
			CBasicModel *tmp;
			CPArea::Create(m_pd3dDevice,&tmp, id,para, pmesh, mcount, ptexture, tcount,this);
			tmp->operate=1; //�ò���״̬
			newmodels.insert(EntryMap(id,tmp));
			tmp=NULL;
		}
		else
		{
			iter->second->operate=1;
		}
	}
	else if (modeltype==��ֵͼ)  //��ֵͼ���������ͬһ������
	{
		auto iter= newmodels.find(id);
		if (iter==newmodels.end())
		{
			CBasicModel *tmp;
			CPPanel::Create(m_pd3dDevice,&tmp, id,para, pmesh, mcount, ptexture, tcount,this);
			tmp->operate=1; //�ò���״̬
			newmodels.insert(EntryMap(id,tmp));
			tmp=NULL;
		}
		else
		{
			iter->second->operate=1;
		}
	}

}




#pragma endregion


//========================================================
//���ù̶�ö�ٷ�ʽ�޸����ԣ��Ժ���ʱ����Կ���ʵ�ַ���
//========================================================
void 
CTriangleRenderer::ChangeProperty(EModelType modeltype,EPropertyType propertytype,int id ,int subid,void* para,int count, void* para2, int count2)
{
	if (modeltype==������)
	{
		CPPolyCol* obj=(CPPolyCol*)CBasicModel::find(id, subid, this);
		if (obj!=nullptr)
			obj->ChangeProperty(propertytype,para,count);
	}
	if (modeltype==ͼԪ)
	{
		CPSymbol* obj=(CPSymbol*)CBasicModel::find(id, subid, this);
		if (obj!=nullptr)
			obj->ChangeProperty(modeltype,propertytype,para,count);
	}
	if (modeltype==�Զ���ģ��)
	{
		CPCustom* obj=(CPCustom*)CBasicModel::find(id, subid, this);
		if (obj!=nullptr)
			obj->ChangeProperty(modeltype,propertytype,para,count);
	}
	if (modeltype==���� || modeltype==����)
	{
		CPline* obj=(CPline*)CBasicModel::find(id, subid, this);
		if (obj!=nullptr)
			obj->ChangeProperty(modeltype, propertytype,para,count);
	}
	if (modeltype==����)
	{
		CPArea* obj=(CPArea*)CBasicModel::find(id, subid, this);
		if (obj!=nullptr)
			obj->ChangeProperty(modeltype, propertytype,para,count, para2, count2);
	}
	else if (modeltype==��ֵͼ)
	{
		auto iter=models.find(id);
		if (iter!=models.end())
		{
			((CPPanel*)iter->second)->ChangeProperty(propertytype,para,count);
		}
	}
	else if (modeltype==��ͼ)
	{
		if (propertytype==��ַ)
		{
			wstring s((WCHAR*)para);
			CMapTile::mapIP=s;
		}
        else if (propertytype==·��)
		{
			wstring s((WCHAR*)para);
			CMapTile::mapPath=s;
		}
        else if (propertytype==·��2)
		{
			wstring s((WCHAR*)para);
			CMapTile::mapPath2=s;
		}
		else if (propertytype=����)
		{
			EMapType oldtype= earthpara.mapType;
			memcpy(&earthpara,(STRUCT_EarthPara*)para,sizeof(earthpara));

			m_pd3dDevice->SetRenderState(D3DRS_ZENABLE,earthpara.isDepthStencil); //zh��Ȳ���
			this->para.m_fSurfaceSettingsChanged=true;
		}
	}
	else if (modeltype==����)
	{
		CPText* obj=(CPText*)CBasicModel::find(id, subid, this);
		if (obj!=nullptr)
			obj->ChangeProperty(propertytype,para,count);
            
	}

}



//------------------------------------------------
//ʰȡģ�ͣ�����id
//------------------------------------------------
int
CTriangleRenderer::Pick_Model(POINT ptCursor)
{
	int result=0;
	for (auto iter=models.begin();iter!=models.end();iter++)
	{
		if (iter->second->isReceivePick)
		{
			result=iter->second->Pick_Model(m_pd3dDevice, ptCursor);
			if (result!=0)
				return result;
		}
	}

	return result;
}

//------------------------------------------------
//ʰȡģ��,  �޶�flag������id
//------------------------------------------------
int
CTriangleRenderer::Pick_Model(POINT ptCursor,int flagid)
{
	int result=0;
	for (auto iter=models.begin();iter!=models.end();iter++)
	{
		if (iter->second->isReceivePick && iter->second->pickFlagId==flagid)
		{
			result=iter->second->Pick_Model(m_pd3dDevice, ptCursor);
			if (result!=0)
				return result;
		}
	}

	return result;
}

