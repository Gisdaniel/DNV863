#include "StdAfx.h"
#include "MapTile.h"
#include "d3dx9.h"
#include "xnamath.h"
#include <cmath>

#include <sstream>

#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#define MAXBLOCKSIZE 50000

#pragma comment( lib, "Wininet.lib" )

wstring CMapTile::mapIP=L"http://localhost:8080/img.aspx";
wstring CMapTile::mapPath=L"";
wstring CMapTile::mapPath2=L"";


CMapTile::CMapTile(void)
{
	pHigh=nullptr;
	Terrain_pd3dVB=nullptr;
	Terrain_pd3dIB=nullptr;
}


CMapTile::~CMapTile(void)
{
	m_pd3dVB->Release();
	m_pd3dIB->Release();
	Terrain_pd3dVB->Release();
	Terrain_pd3dIB->Release();
	
}


HRESULT 
CMapTile::Create(IDirect3DDevice9 *zm_pd3dDevice, CBasicModel **ppModel, int id, int zlayer, int idxx,int idxy, bool isShowTerrain, int terrainSpan, void* pHigh,CTriangleRenderer* tri)
{
    HRESULT hr = S_OK;

    CMapTile *pModel = new CMapTile();
    IFCOOM(pModel);
	pModel->myTri=tri;

	pModel->id=id;
	pModel->layer=zlayer;
	pModel->idxX=idxx;
	pModel->idxY=idxy;
	pModel->QuadKey=CHelper::ToQuadKey(zlayer,idxx,idxy);

	pModel->operate=0;
	pModel->status=0;

	pModel->isShowTerrain=isShowTerrain;
	if (isShowTerrain)
	{
		pModel->terrainSpan=terrainSpan;
		float* hs=(float*)pHigh;
		pModel->pHigh=new float[(terrainSpan+1)*(terrainSpan+1)];
		memcpy(pModel->pHigh,hs,sizeof(hs[0])*(terrainSpan+1)*(terrainSpan+1)); //����
	}
    

    *ppModel = pModel;
    pModel = NULL;

	

Cleanup:
    delete pModel;

    return hr;
}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CMapTile::Init
//
//  Synopsis:
//      Renders the rotating triangle
//
//------------------------------------------------------------------------------
HRESULT 
CMapTile::Init()
{
    HRESULT hr = S_OK;


	curMapType=myTri->earthpara.mapType;
	GetTexture(myTri->m_pd3dDevice);

	curIsShowOverlay=myTri->earthpara.isShowOverlay;
	if (curIsShowOverlay)
		GetOverlayTexture(myTri->m_pd3dDevice);

	hr=buildPlaneMesh();  //��ʼ����ƽ��mesh, ����mesh��̬���ݲ����仯ʹ��setterrain����������

	if (this->isShowTerrain)
		buildTerrainMesh(pHigh);

	status=1;
Cleanup:

    return hr;
}

HRESULT
CMapTile::buildPlaneMesh() //����ƽ��mesh
{
  HRESULT hr = S_OK;

	//=====================================================
	// ������Ƭ
	//=====================================================
	//===���ɶ���

	CUSTOMVERTEX_T* vertices(nullptr);
	WORD* idxes(nullptr);

	if (myTri->earthpara.SceneMode==����)
	{
		const float Radius =myTri->para.Radius;// 6378.137f;

		int Div =static_cast<int>(32.0 / (layer + 1));
		Div= Div < 1 ? 1 : Div;

		int ycount = pow(2.0f, layer);
		float angle = XM_2PI/ycount;
		float tileLength		 = angle;//(float)(MathHelper.TwoPi / Math.Pow(2, layer)); //2*pi/power(2,layer)  �ٶ��뾶Ϊ1, ƽ���ͼ����ϵ�¸ò㻮�ֿ�ĳ��Ϳ��
		float xStart = idxX * angle - XM_PI;//  ָ�������ſ����ʼ����
		float yStart = atan(exp(XM_PI - idxY * tileLength)) * 2 - XM_PIDIV2; //atan(exp(PI-idxy*2*pi/power(2,layer)))*2-pi/2  ָ�������ſ����ʼγ��
		float yEnd = atan(exp(XM_PI - (idxY + 1) * tileLength)) * 2 - XM_PIDIV2; //ָ�������ſ�Ľ���γ��
		double angleDiv = angle / Div;

		float ys =(float)(Radius * sin(yStart));
		float ye =(float)(Radius * sin(yEnd));

		vertices=new CUSTOMVERTEX_T[(Div + 1) * (Div + 1)];

		int idx = 0;
		for (int yi = 0; yi <= Div; yi++)
		{
			float yangle = atan(exp(XM_PI - (idxY + (float)yi / Div) * tileLength)) * 2 - XM_PI / 2;
			float ynew = Radius * sin(yangle);
			for (int ti = 0; ti <= Div; ti++)
			{
				double t = -angleDiv * ti - xStart + XM_PI;
				double r = sqrt(Radius * Radius - ynew * ynew);
				double x = r * cos(t);
				double z = r * sin(t);

				vertices[idx].position=D3DXVECTOR3(x,ynew,z);
				vertices[idx].normal=D3DXVECTOR3(x,ynew,z);
				D3DXVec3Normalize(&vertices[idx].normal,&vertices[idx].normal);

				D3DXMATRIXA16 matrix;
				D3DXMatrixScaling(&matrix,1.0f / Div, 1.0f / (ye - ys), 1.0f);
				D3DXVECTOR3 pin((float)ti, (float)(ynew - ys), 1);
				D3DXVECTOR3 pout;
				D3DXVec3TransformCoord(&pout,&pin,&matrix);

				vertices[idx].u=pout.x;
				vertices[idx].v=pout.y>1?1:pout.y;

				idx++;
			}
		}
		//����
		idxes=new WORD[Div * Div * 6];

		idx = 0;
		for (int yi = 0; yi < Div; yi++)
		{
			for (int ti = 0; ti < Div; ti++)
			{
				int x0 = ti;
				int x1 = (ti + 1);
				int y0 = yi * (Div + 1);
				int y1 = (yi + 1) * (Div + 1);

				idxes[idx] = x0 + y0;
				idxes[idx+1] = x0 + y1;
				idxes[idx+2] = x1 + y0;
				idxes[idx+3] = x1 + y0;
				idxes[idx+4] = x0 + y1;
				idxes[idx+5] = x1 + y1;
				idx += 6;

			}
		}
		NumVertices=(Div + 1) * (Div + 1);
		PrimCount=Div*Div*2;
	}
	else  //�ֲ�ƽ��mesh
	{
		float xStart,xEnd,yStart,yEnd;
		if ((myTri->earthpara.tileReadMode==������Ƭ���� || myTri->earthpara.tileReadMode==�Զ���Web��Ƭ) && myTri->earthpara.InputCoordinate==WGS84��������)
		{
			int ycount = pow(2.0f, layer);
			float angle = XM_2PI/ycount;
			float tileLength		 = angle;//(float)(MathHelper.TwoPi / Math.Pow(2, layer)); //2*pi/power(2,layer)  �ٶ��뾶Ϊ1, ƽ���ͼ����ϵ�¸ò㻮�ֿ�ĳ��Ϳ��
			xStart = (idxX * angle - XM_PI)/D3DX_PI*180;//  ָ�������ſ����ʼ���ȣ��Ƕ�
			xEnd = ((idxX+1) * angle - XM_PI)/D3DX_PI*180;//  ָ�������ſ����ʼ���ȣ��Ƕ�
			yStart = (atan(exp(XM_PI - idxY * tileLength)) * 2 - XM_PIDIV2)/D3DX_PI*180; //atan(exp(PI-idxy*2*pi/power(2,layer)))*2-pi/2  ָ�������ſ����ʼγ��
			yEnd = (atan(exp(XM_PI - (idxY + 1) * tileLength)) * 2 - XM_PIDIV2)/D3DX_PI*180; //ָ�������ſ�Ľ���γ��
		}
		else
		{
			 int count = pow(2.0f, layer);
			 float jdLength = (myTri->earthpara.EndLocation.Longitude-myTri->earthpara.StartLocation.Longitude) / count;
			 float wdLength = (myTri->earthpara.EndLocation.Latitude - myTri->earthpara.StartLocation.Latitude) / count;
			 xStart = myTri->earthpara.StartLocation.Longitude + idxX * jdLength;
			 yStart = myTri->earthpara.EndLocation.Latitude - idxY * wdLength;   //γ�ȷ�������Ƭ�����෴
			 xEnd=xStart+jdLength;
			 yEnd=yStart-wdLength;
		}

		vertices=new CUSTOMVERTEX_T[4];
		vertices[0].position=D3DXVECTOR3((xStart-myTri->earthpara.StartLocation.Longitude)*myTri->earthpara.UnitLoneLen,(yStart-myTri->earthpara.StartLocation.Latitude)*myTri->earthpara.UnitLatLen,0);
		vertices[1].position=D3DXVECTOR3((xEnd-myTri->earthpara.StartLocation.Longitude)*myTri->earthpara.UnitLoneLen,(yStart-myTri->earthpara.StartLocation.Latitude)*myTri->earthpara.UnitLatLen,0);
		vertices[2].position=D3DXVECTOR3((xStart-myTri->earthpara.StartLocation.Longitude)*myTri->earthpara.UnitLoneLen,(yEnd-myTri->earthpara.StartLocation.Latitude)*myTri->earthpara.UnitLatLen,0);
		vertices[3].position=D3DXVECTOR3((xEnd-myTri->earthpara.StartLocation.Longitude)*myTri->earthpara.UnitLoneLen,(yEnd-myTri->earthpara.StartLocation.Latitude)*myTri->earthpara.UnitLatLen,0);
		vertices[0].normal=	vertices[1].normal=	vertices[2].normal=	vertices[3].normal=D3DXVECTOR3(0,0,1);

		vertices[0].u=0;vertices[0].v=0;
		vertices[1].u=1;vertices[1].v=0;
		vertices[2].u=0;vertices[2].v=1;
		vertices[3].u=1;vertices[3].v=1;
		

		//����
		idxes=new WORD[6];
		idxes[0]=0;
		idxes[1]=2;
		idxes[2]=1;
		idxes[3]=1;
		idxes[4]=2;
		idxes[5]=3;

		NumVertices=4;
		PrimCount=2;
	}


	//===== ���㻺��	
	IFC(myTri->m_pd3dDevice->CreateVertexBuffer(NumVertices*sizeof(CUSTOMVERTEX_T), 0, D3DFVF_CUSTOMVERTEX_T, D3DPOOL_DEFAULT, &m_pd3dVB, NULL)); 
	void *pVertices;
	IFC(m_pd3dVB->Lock(0, NumVertices*sizeof(CUSTOMVERTEX_T), &pVertices, 0));
	memcpy(pVertices, vertices, NumVertices*sizeof(CUSTOMVERTEX_T));
	m_pd3dVB->Unlock();
	//===== ��������
	IFC(myTri->m_pd3dDevice->CreateIndexBuffer(PrimCount*3*sizeof(WORD), 0, D3DFMT_INDEX16 , D3DPOOL_DEFAULT, &m_pd3dIB, NULL)); 
	void *pIndexes;
	IFC(m_pd3dIB->Lock(0, PrimCount*3*sizeof(WORD), &pIndexes, 0));
	memcpy(pIndexes, idxes, PrimCount*3*sizeof(WORD));
	m_pd3dIB->Unlock();


	status=1;
Cleanup:
	delete vertices;
	delete idxes;

    return hr;
}

HRESULT
CMapTile::buildTerrainMesh(void* pHigh)   //��������mesh
{
	  HRESULT hr = S_OK;

	//=====================================================
	// ������Ƭ
	//=====================================================
	//===���ɶ���
  	float* pHeights=(float*)pHigh;

	CUSTOMVERTEX_T* vertices(nullptr);
	WORD* idxes(nullptr);

	if (myTri->earthpara.SceneMode==����)
	{
		const float Radius =myTri->para.Radius;// 6378.137f;

		int Div =terrainSpan;

		int ycount = pow(2.0f, layer);
		float angle = XM_2PI/ycount;
		float tileLength		 = angle;//(float)(MathHelper.TwoPi / Math.Pow(2, layer)); //2*pi/power(2,layer)  �ٶ��뾶Ϊ1, ƽ���ͼ����ϵ�¸ò㻮�ֿ�ĳ��Ϳ��
		float xStart = idxX * angle - XM_PI;//  ָ�������ſ����ʼ����
		float yStart = atan(exp(XM_PI - idxY * tileLength)) * 2 - XM_PIDIV2; //atan(exp(PI-idxy*2*pi/power(2,layer)))*2-pi/2  ָ�������ſ����ʼγ��
		float yEnd = atan(exp(XM_PI - (idxY + 1) * tileLength)) * 2 - XM_PIDIV2; //ָ�������ſ�Ľ���γ��
		double angleDiv = angle / Div;

		float ys =(float)(Radius * sin(yStart));
		float ye =(float)(Radius * sin(yEnd));

		vertices=new CUSTOMVERTEX_T[(Div + 1) * (Div + 1)];

		int idx = 0;
		for (int yi = 0; yi <= Div; yi++)
		{
			float yangle = atan(exp(XM_PI - (idxY + (float)yi / Div) * tileLength)) * 2 - XM_PI / 2;
			float ynew = Radius * sin(yangle);
			for (int ti = 0; ti <= Div; ti++)
			{
				double t = -angleDiv * ti - xStart + XM_PI;
				double r = sqrt(Radius * Radius - ynew * ynew)+ *(pHeights+yi*(terrainSpan+1)+ti);  //���Ӹ߶�
				double x = r * cos(t);
				double z = r * sin(t);

				vertices[idx].position=D3DXVECTOR3(x,ynew,z);
				vertices[idx].normal=D3DXVECTOR3(x,ynew,z);
				D3DXVec3Normalize(&vertices[idx].normal,&vertices[idx].normal);
				D3DXMATRIXA16 matrix;
				D3DXMatrixScaling(&matrix,1.0f / Div, 1.0f / (ye - ys), 1.0f);
				D3DXVECTOR3 pin((float)ti, (float)(ynew - ys), 1);
				D3DXVECTOR3 pout;
				D3DXVec3TransformCoord(&pout,&pin,&matrix);

				vertices[idx].u=pout.x;
				vertices[idx].v=pout.y>1?1:pout.y;

				idx++;
			}
		}
		//����
		idxes=new WORD[Div * Div * 6];

		idx = 0;
		for (int yi = 0; yi < Div; yi++)
		{
			for (int ti = 0; ti < Div; ti++)
			{
				int x0 = ti;
				int x1 = (ti + 1);
				int y0 = yi * (Div + 1);
				int y1 = (yi + 1) * (Div + 1);

				idxes[idx] = x0 + y0;
				idxes[idx+1] = x0 + y1;
				idxes[idx+2] = x1 + y0;
				idxes[idx+3] = x1 + y0;
				idxes[idx+4] = x0 + y1;
				idxes[idx+5] = x1 + y1;
				idx += 6;

			}
		}
		TerrainNumVertices=(Div + 1) * (Div + 1);
		TerrainPrimCount=Div*Div*2;
	}
	else  //�ֲ�ƽ��mesh
	{
		float xStart,xEnd,yStart,yEnd;
		if ((myTri->earthpara.tileReadMode==������Ƭ���� || myTri->earthpara.tileReadMode==�Զ���Web��Ƭ) && myTri->earthpara.InputCoordinate==WGS84��������)
		{
			int ycount = pow(2.0f, layer);
			float angle = XM_2PI/ycount;
			float tileLength		 = angle;//(float)(MathHelper.TwoPi / Math.Pow(2, layer)); //2*pi/power(2,layer)  �ٶ��뾶Ϊ1, ƽ���ͼ����ϵ�¸ò㻮�ֿ�ĳ��Ϳ��
			xStart = (idxX * angle - XM_PI)/D3DX_PI*180;//  ָ�������ſ����ʼ���ȣ��Ƕ�
			xEnd = ((idxX+1) * angle - XM_PI)/D3DX_PI*180;//  ָ�������ſ����ʼ���ȣ��Ƕ�
			yStart = (atan(exp(XM_PI - idxY * tileLength)) * 2 - XM_PIDIV2)/D3DX_PI*180; //atan(exp(PI-idxy*2*pi/power(2,layer)))*2-pi/2  ָ�������ſ����ʼγ��
			yEnd = (atan(exp(XM_PI - (idxY + 1) * tileLength)) * 2 - XM_PIDIV2)/D3DX_PI*180; //ָ�������ſ�Ľ���γ��
		}
		else
		{
			 int count = pow(2.0f, layer);
			 float jdLength = (myTri->earthpara.EndLocation.Longitude-myTri->earthpara.StartLocation.Longitude) / count;
			 float wdLength = (myTri->earthpara.EndLocation.Latitude - myTri->earthpara.StartLocation.Latitude) / count;
			 xStart = myTri->earthpara.StartLocation.Longitude + idxX * jdLength;
			 yStart = myTri->earthpara.EndLocation.Latitude - idxY * wdLength;   //γ�ȷ�������Ƭ�����෴
			 xEnd=xStart+jdLength;
			 yEnd=yStart-wdLength;
		}

		
		int Div =terrainSpan;
		float jdspan=(xEnd-xStart)/Div;
		float wdspan=(yEnd-yStart)/Div;

		vertices=new CUSTOMVERTEX_T[(Div+1)*(Div+1)];
		for(int yi=0; yi<=Div; yi++)
		{
			for(int xi=0; xi<=Div; xi++)
			{
				vertices[yi*(Div+1)+xi].position=D3DXVECTOR3((xStart+xi*jdspan-myTri->earthpara.StartLocation.Longitude)*myTri->earthpara.UnitLoneLen,(yStart+yi*wdspan-myTri->earthpara.StartLocation.Latitude)*myTri->earthpara.UnitLatLen,*(pHeights+yi*(terrainSpan+1)+xi));
				vertices[yi*(Div+1)+xi].normal=D3DXVECTOR3(0,0,1);
				vertices[yi*(Div+1)+xi].u=(float)xi/Div;
				vertices[yi*(Div+1)+xi].v=(float)yi/Div;
			}
		}

		//vertices=new CUSTOMVERTEX_T[4];
		//vertices[0].position=D3DXVECTOR3((xStart-myTri->earthpara.StartLocation.Longitude)*myTri->earthpara.UnitLoneLen,(yStart-myTri->earthpara.StartLocation.Latitude)*myTri->earthpara.UnitLatLen,0);
		//vertices[1].position=D3DXVECTOR3((xEnd-myTri->earthpara.StartLocation.Longitude)*myTri->earthpara.UnitLoneLen,(yStart-myTri->earthpara.StartLocation.Latitude)*myTri->earthpara.UnitLatLen,0);
		//vertices[2].position=D3DXVECTOR3((xStart-myTri->earthpara.StartLocation.Longitude)*myTri->earthpara.UnitLoneLen,(yEnd-myTri->earthpara.StartLocation.Latitude)*myTri->earthpara.UnitLatLen,0);
		//vertices[3].position=D3DXVECTOR3((xEnd-myTri->earthpara.StartLocation.Longitude)*myTri->earthpara.UnitLoneLen,(yEnd-myTri->earthpara.StartLocation.Latitude)*myTri->earthpara.UnitLatLen,0);
		//vertices[0].normal=	vertices[1].normal=	vertices[2].normal=	vertices[3].normal=D3DXVECTOR3(0,0,1);

		//vertices[0].u=0;vertices[0].v=0;
		//vertices[1].u=1;vertices[1].v=0;
		//vertices[2].u=0;vertices[2].v=1;
		//vertices[3].u=1;vertices[3].v=1;
		

		//����
		
		idxes=new WORD[Div*Div*6];
		int idx = 0;
		for (int yi = 0; yi < Div; yi++)
		{
			for (int ti = 0; ti < Div; ti++)
			{
				int x0 = ti;
				int x1 = (ti + 1);
				int y0 = yi * (Div + 1);
				int y1 = (yi + 1) * (Div + 1);

				idxes[idx] = x0 + y0;
				idxes[idx+1] = x0 + y1;
				idxes[idx+2] = x1 + y0;
				idxes[idx+3] = x1 + y0;
				idxes[idx+4] = x0 + y1;
				idxes[idx+5] = x1 + y1;
				idx += 6;

			}
		}
		//idxes[0]=0;
		//idxes[1]=2;
		//idxes[2]=1;
		//idxes[3]=1;
		//idxes[4]=2;
		//idxes[5]=3;

		TerrainNumVertices=(Div+1)*(Div+1);
		TerrainPrimCount=Div*Div*2;
	}


	//===== ���㻺��	
	IFC(myTri->m_pd3dDevice->CreateVertexBuffer(TerrainNumVertices*sizeof(CUSTOMVERTEX_T), 0, D3DFVF_CUSTOMVERTEX_T, D3DPOOL_DEFAULT, &Terrain_pd3dVB, NULL)); 
	void *pVertices;
	IFC(Terrain_pd3dVB->Lock(0, TerrainNumVertices*sizeof(CUSTOMVERTEX_T), &pVertices, 0));
	memcpy(pVertices, vertices, TerrainNumVertices*sizeof(CUSTOMVERTEX_T));
	Terrain_pd3dVB->Unlock();
	//===== ��������
	IFC(myTri->m_pd3dDevice->CreateIndexBuffer(TerrainPrimCount*3*sizeof(WORD), 0, D3DFMT_INDEX16 , D3DPOOL_DEFAULT, &Terrain_pd3dIB, NULL)); 
	void *pIndexes;
	IFC(Terrain_pd3dIB->Lock(0, TerrainPrimCount*3*sizeof(WORD), &pIndexes, 0));
	memcpy(pIndexes, idxes, TerrainPrimCount*3*sizeof(WORD));
	Terrain_pd3dIB->Unlock();


	status=1;

	if (this->pHigh!=nullptr)
		delete this->pHigh;
Cleanup:
	delete vertices;
	delete idxes;

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
CMapTile::Render(IDirect3DDevice9 *m_pd3dDevice)
{
	HRESULT hr = S_OK;

	IFC(m_pd3dDevice->SetTransform(D3DTS_WORLD, &world));




	CheckTexture();
	IFC(m_pd3dDevice->SetTexture(0,g_pTexture));

	if (curIsShowOverlay && myTri->earthpara.isShowOverlay)
		IFC(m_pd3dDevice->SetTexture(1,g_pTexture2));


	if (isShowTerrain && Terrain_pd3dIB!=nullptr &&Terrain_pd3dVB!=nullptr)  //��ʾ����
	{
		IFC(m_pd3dDevice->SetStreamSource(0, Terrain_pd3dVB, 0, sizeof(CUSTOMVERTEX_T)));
		IFC(m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX_T));
		IFC(m_pd3dDevice->SetIndices(Terrain_pd3dIB));  //������Ƭ��
		IFC(m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,TerrainNumVertices,0,TerrainPrimCount));   //������Ƭ��
	}
	else
	{
		IFC(m_pd3dDevice->SetStreamSource(0, m_pd3dVB, 0, sizeof(CUSTOMVERTEX_T)));
		IFC(m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX_T));
		IFC(m_pd3dDevice->SetIndices(m_pd3dIB));  //������Ƭ��
		IFC(m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,NumVertices,0,PrimCount));   //������Ƭ��
	}

	IFC(m_pd3dDevice->SetTexture(0,nullptr));
	if (curIsShowOverlay && myTri->earthpara.isShowOverlay)
		IFC(m_pd3dDevice->SetTexture(1,nullptr));


	//IFC(m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1));   //ɫ��ʾ����
	//IFC(m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));   //����ʾ����


Cleanup:
    return hr;
}


int Replace(std::wstring& strContent, std::wstring& strReplace, std::wstring & strDest)  
{  
  
    while (true)  
    {  
        size_t pos = strContent.find(strReplace);  
        if (pos != std::wstring::npos)  
        {  
            WCHAR pBuf[1]={L'\0'};  
            strContent.replace(pos, strReplace.length(), pBuf, 0);  
            strContent.insert(pos, strDest);  
        }  
        else  
        {  
            break;  
        }  
    }  
    return 0;  
}  


HRESULT 
CMapTile::GetTexture(IDirect3DDevice9 *m_pd3dDevice)
{

	HRESULT hr = S_OK;
	wstring surl;//=CMapTile::mapIP; //IP��Path

	if (myTri->earthpara.tileReadMode==�Զ����ļ���Ƭ)  //�ļ���ȡ��ʽ
	{
		//surl=L"f:\\2.jpg";
		surl=CMapTile::mapPath; 
		wostringstream oss;  
		//oss<<layer<<L"\\"<<idxX<<L"\\"<<idxY<<L".jpg";
		int newlayer=layer;//+myTri->earthpara.tileFileOffsetLI;
		int newidxx=idxX;//+myTri->earthpara.tileFileOffsetXI*pow(2.0,layer+myTri->earthpara.tileFileOffsetLI);
		int newidxy=idxY;//+myTri->earthpara.tileFileOffsetYI*pow(2.0,layer+myTri->earthpara.tileFileOffsetLI);
		
		oss<<newlayer<<L"\\"<<newidxy<<L"\\"<<newlayer<<"_"<<newidxy<<"_"<<newidxx<<L".png";

		surl+=oss.str();
		if (myTri->earthpara.isCover)  // �ص�ģʽ��·����Ƭ����overlay
		{
			if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, surl.c_str(), &g_pTexture2 ) ) )
			{
				//MessageBox(NULL, L"��������ʧ��", L"Texture.exe", MB_OK);
				hr= E_FAIL;
			}
		}
		else
		{
			if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, surl.c_str(), &g_pTexture ) ) )
			{
				//MessageBox(NULL, L"��������ʧ��", L"Texture.exe", MB_OK);
				hr= E_FAIL;
			}
		}
	}
	else if (myTri->earthpara.tileReadMode==�Զ���Web��Ƭ)
	{
		surl=CMapTile::mapPath; 
		int newlayer=layer;//+myTri->earthpara.tileFileOffsetLI;
		int newidxx=idxX;//+myTri->earthpara.tileFileOffsetXI*pow(2.0,layer+myTri->earthpara.tileFileOffsetLI);
		int newidxy=idxY;//+myTri->earthpara.tileFileOffsetYI*pow(2.0,layer+myTri->earthpara.tileFileOffsetLI);
		
		wostringstream oss;
		wstring strorg=L"{layer}";
		oss<<newlayer;
		wstring strdes=oss.str();
		Replace(surl,strorg,strdes);
		
		strorg=L"{x}";
		oss.str(L""); 
		oss.clear();
		oss<<newidxx;
		strdes=oss.str();
		Replace(surl,strorg,strdes);

		strorg=L"{y}";
		oss.str(L""); 
		oss.clear();
		oss<<newidxy;
		strdes=oss.str();
		Replace(surl,strorg,strdes);


		LPCWSTR Url=surl.c_str();//L"http://localhost:8080/img.aspx?quadkey=00&imgmode=googlemaps/satellite&imgtype=.jpg";


		HINTERNET hSession = InternetOpen(L"RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (hSession != NULL)
		{
			HINTERNET handle2 = InternetOpenUrl(hSession, Url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
			if (handle2 != NULL)
			{

				byte Temp[MAXBLOCKSIZE];
				ULONG Number = 1;
				//FILE *stream;
				//if( (stream = fopen( "F:\\new.jpg", "wb" )) != NULL )//����ֻ�Ǹ����ԣ����д�˸������ļ�·��
				{
					//while (Number > 0)
					{
						InternetReadFile(handle2, Temp, MAXBLOCKSIZE - 1, &Number);
						//fwrite(Temp, sizeof (char), Number , stream);
					}
					//fclose( stream );
					if (g_pTexture!=NULL)
						g_pTexture->Release();
					D3DXCreateTextureFromFileInMemory(m_pd3dDevice,Temp,Number,&g_pTexture);
					//D3DXCreateTextureFromFileInMemoryEx(m_pd3dDevice,Temp,Number,0,0,0,0,D3DFMT_DXT1,D3DPOOL_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,NULL,NULL,&g_pTexture);
				}

				InternetCloseHandle(handle2);
				handle2 = NULL;
			}

			InternetCloseHandle(hSession);
			hSession = NULL;

		}
	}
	else if (myTri->earthpara.tileReadMode==������Ƭ���� || myTri->earthpara.isCover)  //ȱʡ��������Ƭweb��ȡ��ʽ
	{
		surl=CMapTile::mapIP;
		//surl+=L"/img.aspx?quadkey=";
		surl+=L"?quadkey=";
		surl+=QuadKey;
		if (myTri->earthpara.mapType==��·)
			surl+=L"&imgmode=googlemaps/roadmap&imgtype=.png";
		else if (myTri->earthpara.mapType==����)
			surl+=L"&imgmode=googlemaps/terrain&imgtype=.jpg";
		else
			surl+=L"&imgmode=googlemaps/satellite&imgtype=.jpg";

		//wstring surl=L"http://localhost:8080/img.aspx?quadkey=";
		//surl+=QuadKey;
		//surl+=L"&imgmode=googlemaps/satellite&imgtype=.jpg";

		//surl=L"http://localhost:8080/nofind.jpg"; //�����÷���ͼ

		LPCWSTR Url=surl.c_str();//L"http://localhost:8080/img.aspx?quadkey=00&imgmode=googlemaps/satellite&imgtype=.jpg";


		HINTERNET hSession = InternetOpen(L"RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (hSession != NULL)
		{
			HINTERNET handle2 = InternetOpenUrl(hSession, Url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
			if (handle2 != NULL)
			{

				byte Temp[MAXBLOCKSIZE];
				ULONG Number = 1;
				//FILE *stream;
				//if( (stream = fopen( "F:\\new.jpg", "wb" )) != NULL )//����ֻ�Ǹ����ԣ����д�˸������ļ�·��
				{
					//while (Number > 0)
					{
						InternetReadFile(handle2, Temp, MAXBLOCKSIZE - 1, &Number);
						//fwrite(Temp, sizeof (char), Number , stream);
					}
					//fclose( stream );
					if (g_pTexture!=NULL)
						g_pTexture->Release();
					D3DXCreateTextureFromFileInMemory(m_pd3dDevice,Temp,Number,&g_pTexture);
					//D3DXCreateTextureFromFileInMemoryEx(m_pd3dDevice,Temp,Number,0,0,0,0,D3DFMT_DXT1,D3DPOOL_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,NULL,NULL,&g_pTexture);
				}

				InternetCloseHandle(handle2);
				handle2 = NULL;
			}

			InternetCloseHandle(hSession);
			hSession = NULL;

		}

	}

	

Cleanup:
    return hr;
}

HRESULT 
CMapTile::GetOverlayTexture(IDirect3DDevice9 *m_pd3dDevice)
{

	HRESULT hr = S_OK;
	if (myTri->earthpara.tileReadMode==�Զ����ļ���Ƭ) return hr;  //��Ϊ�ص���ʽ�����ٶ�ȡoverlay

	if (myTri->earthpara.tileReadMode==�Զ����ļ���Ƭ && CMapTile::mapPath2!=L"")  //�ļ���ȡ��ʽ, �ݲ�֧��overlay
	{
		wstring surl=CMapTile::mapPath2;
		wostringstream oss;  
		oss<<layer<<L"\\"<<idxX<<L"\\"<<idxY<<L".png";
		surl+=oss.str();

		if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, surl.c_str(), &g_pTexture2 ) ) )
		{
			return E_FAIL;
		}
	}
	else  //ȱʡ��������Ƭweb��ȡ��ʽ
	{
		wstring surl=CMapTile::mapIP;
		surl+=L"/img.aspx?quadkey=";
		surl+=QuadKey;
		surl+=L"&imgmode=googlemaps/overlay&imgtype=.png";

		LPCWSTR Url=surl.c_str();//L"http://localhost:8080/img.aspx?quadkey=00&imgmode=googlemaps/overlay&imgtype=.png";


		HINTERNET hSession = InternetOpen(L"RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (hSession != NULL)
		{
			HINTERNET handle2 = InternetOpenUrl(hSession, Url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
			if (handle2 != NULL)
			{

				byte Temp[MAXBLOCKSIZE];
				ULONG Number = 1;
				//FILE *stream;
				//if( (stream = fopen( "F:\\new.jpg", "wb" )) != NULL )//����ֻ�Ǹ����ԣ����д�˸������ļ�·��
				{
					//while (Number > 0)
					{
						InternetReadFile(handle2, Temp, MAXBLOCKSIZE - 1, &Number);
						//fwrite(Temp, sizeof (char), Number , stream);
					}
					//fclose( stream );

					D3DXCreateTextureFromFileInMemory(m_pd3dDevice,Temp,Number,&g_pTexture2);
				}

				InternetCloseHandle(handle2);
				handle2 = NULL;
			}

			InternetCloseHandle(hSession);
			hSession = NULL;
		}
	}


Cleanup:
	return hr;
}

void
CMapTile::CheckTexture()
{
	if (myTri->earthpara.mapType!=curMapType)
	{

		GetTexture(myTri->m_pd3dDevice);
		curMapType=myTri->earthpara.mapType;
	}
	if (myTri->earthpara.isShowOverlay && !curIsShowOverlay)
	{
		GetOverlayTexture(myTri->m_pd3dDevice);
		curIsShowOverlay=myTri->earthpara.isShowOverlay;
	}
}

void
CMapTile::SetEffect(IDirect3DDevice9 *m_pd3dDevice,CTriangleRenderer* myTri)
{
	//�رչ���
    //m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, false );

	//�������������ʽ
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	//m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	
	//��
	//m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, 0xffffffff);	//���������ɫ
	//static float fogStart   = 1;      //��������ʼλ��
	//static float fogEnd     = 200;     //����������λ��
	//static float fogDensity = 0.01f;   //���Ũ��
	//m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, true);        //������
	//m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR); 
	//m_pd3dDevice->SetRenderState(D3DRS_FOGSTART,  *(DWORD*)&fogStart);			//������������ʼλ�úͽ���λ��
	//m_pd3dDevice->SetRenderState(D3DRS_FOGEND,    *(DWORD*)&fogEnd);


	//����������
	if (myTri->earthpara.isShowOverlay)
	{
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,0);

		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);


		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_BLENDTEXTUREALPHA);

		//��������1������ʽ
		m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

		//���ð�͸���
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	}
	else
	{
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	}


}

void 
CMapTile::SetTerrain(bool IsShowTerrain, int TerrainSpan, void* PHigh)
{
 	this->isShowTerrain=IsShowTerrain;

	if (isShowTerrain && terrainSpan!=TerrainSpan)  //ֻ�ڿ���ʾ���� �� ��Ƭ���仯ʱ���ؽ�����mesh
	{
		this->terrainSpan=TerrainSpan;
		buildTerrainMesh(PHigh);
	}

}