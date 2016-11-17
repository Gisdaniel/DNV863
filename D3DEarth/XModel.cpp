#include "StdAfx.h"
#include "XModel.h"


CXModel::CXModel(void)
{
	g_pMesh          = NULL;  //����ģ�Ͷ���
	g_pMeshMaterials = NULL;  //����ģ�Ͳ���
	g_pMeshTextures  = NULL;  //����ģ������
	g_dwNumMaterials = 0L;    //����ģ�Ͳ�������

	//mesh=NULL;
	g_pTexture=NULL;
}


CXModel::~CXModel(void)
{
}


//-----------------------------------------------------------------------------
// Desc: ��������ͼ�Σ���custom����
//-----------------------------------------------------------------------------
void CXModel::InitCustom(void* pvertices,void* pnormal, int vcount, void* pindex, int icount, void* puv, int uvcount, void* ptexture)
{
	this->isCustom=true;

	g_dwNumMaterials=1;
	//===== ����
	g_pMeshMaterials =NULL;// new D3DMATERIAL9[1];
	//ZeroMemory( &g_pMeshMaterials[0], sizeof(D3DMATERIAL9) );
	//

	//g_pMeshMaterials[0].Diffuse.r = g_pMeshMaterials[0].Ambient.r = 0.0f;
 //   g_pMeshMaterials[0].Diffuse.g = g_pMeshMaterials[0].Ambient.g = 0.0f;
 //   g_pMeshMaterials[0].Diffuse.b = g_pMeshMaterials[0].Ambient.b = 0.0f;
 //   g_pMeshMaterials[0].Diffuse.a = g_pMeshMaterials[0].Ambient.a = 255.0f;
	
	//===== ����
	if (ptexture!=nullptr)
	{
		g_pMeshTextures  = new LPDIRECT3DTEXTURE9[1];
        wstring file((WCHAR*)ptexture);
		D3DXCreateTextureFromFile(CGlobal::pCurRender->m_pd3dDevice,file.c_str(),&g_pMeshTextures[0]);
	}
	else
	{
		g_pMeshTextures  = new LPDIRECT3DTEXTURE9[1];
		g_pMeshTextures[0]=nullptr;
	}

	//===== mesh

	
	D3DXVECTOR3* pv=(D3DXVECTOR3*)pvertices;
	D3DXVECTOR3* pn=(D3DXVECTOR3*)pnormal;
	D3DXVECTOR2* uv;
	if (puv!=nullptr)
		uv=(D3DXVECTOR2*)puv;

	CUSTOMVERTEX_T* vertices(nullptr);
	vertices=new CUSTOMVERTEX_T[vcount];

	for (int i=0; i<vcount; i++)
	{
		vertices[i].position=D3DXVECTOR3(pv[i].x,pv[i].y,pv[i].z);
		vertices[i].normal=D3DXVECTOR3(pn[i].x,pn[i].y,pn[i].z);
		D3DXVec3Normalize(&vertices[i].normal,&vertices[i].normal);
		if (puv!=nullptr)
		{
			vertices[i].u=uv[i].x;
			vertices[i].v=uv[i].y;
		}
	}

	WORD* pi=(WORD*)pindex;
	WORD* idxes(nullptr);
	idxes=new WORD[icount];
	memcpy(idxes,pi,sizeof(pi[0])*icount); //�������� 

	int NumVertices=vcount; //������
	int PrimCount=icount/3; //������


	//==== ����mesh
	if (g_pMesh!=NULL)
		g_pMesh->Release();

	D3DXCreateMeshFVF(PrimCount,NumVertices,D3DXMESH_DYNAMIC, D3DFVF_CUSTOMVERTEX_T, CGlobal::pCurRender->m_pd3dDevice, &g_pMesh);
	
	CUSTOMVERTEX_T* v = 0;
	g_pMesh->LockVertexBuffer(0, (void**)&v);
	for (int i=0;i<NumVertices;i++)
		v[i]=vertices[i];
	g_pMesh->UnlockVertexBuffer();

 
	WORD* idx = 0;
	g_pMesh->LockIndexBuffer(0, (void**)&idx);
	for (int i=0;i<icount;i++)
	{
		idx[i]=idxes[i];
	}
	g_pMesh->UnlockIndexBuffer();
	//�Ż�
	vector<DWORD> adjacencyBuffer(g_pMesh->GetNumFaces() * 3);
	g_pMesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);
	g_pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_COMPACT|D3DXMESHOPT_VERTEXCACHE, &adjacencyBuffer[0],0, 0, 0);




	Cleanup:
	delete vertices;
	delete idxes;



//	//===== ����
//	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
//	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
//    mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
//    mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
//    mtrl.Diffuse.a = mtrl.Ambient.a =255.0f;	
//	//===== ����
//        wstring file((WCHAR*)ptexture);
//		D3DXCreateTextureFromFile(CGlobal::pCurRender->m_pd3dDevice,file.c_str(),&g_pTexture);
//	//========mesh
//
//D3DXVECTOR3* pv=(D3DXVECTOR3*)pvertices;
//	D3DXVECTOR3* pn=(D3DXVECTOR3*)pnormal;
//	D3DXVECTOR2* uv;
//	if (puv!=nullptr)
//		uv=(D3DXVECTOR2*)puv;
//
//	CUSTOMVERTEX_T* vertices(nullptr);
//	vertices=new CUSTOMVERTEX_T[vcount];
//
//	for (int i=0; i<vcount; i++)
//	{
//		vertices[i].position=D3DXVECTOR3(pv[i].x,-pv[i].y,pv[i].z);
//		vertices[i].normal=D3DXVECTOR3(pn[i].x,pn[i].y,pn[i].z);
//		if (puv!=nullptr)
//		{
//			vertices[i].u=uv[i].x;
//			vertices[i].v=uv[i].y;
//		}
//	}
//
//	WORD* pi=(WORD*)pindex;
//	WORD* idxes(nullptr);
//	idxes=new WORD[icount];
//	memcpy(idxes,pi,sizeof(pi[0])*icount); //�������� 
//
//	int NumVertices=vcount; //������
//	int PrimCount=icount/3; //������
//
//
//	// ����mesh
//	if (mesh!=NULL)
//		mesh->Release();
//
//	D3DXCreateMeshFVF(PrimCount,NumVertices,D3DXMESH_DYNAMIC, D3DFVF_CUSTOMVERTEX_T,CGlobal::pCurRender->m_pd3dDevice, &mesh);
//	
//	CUSTOMVERTEX_T* v = 0;
//	mesh->LockVertexBuffer(0, (void**)&v);
//	for (int i=0;i<NumVertices;i++)
//		v[i]=vertices[i];
//	mesh->UnlockVertexBuffer();
//
// 
//	WORD* idx = 0;
//	mesh->LockIndexBuffer(0, (void**)&idx);
//	for (int i=0;i<icount;i++)
//	{
//		idx[i]=idxes[i];
//	}
//	mesh->UnlockIndexBuffer();
//	//�Ż�
//	vector<DWORD> adjacencyBuffer(mesh->GetNumFaces() * 3);
//	mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);
//	mesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_COMPACT|D3DXMESHOPT_VERTEXCACHE, &adjacencyBuffer[0],0, 0, 0);
//
//
//
//
//	Cleanup:
//	delete vertices;
//	delete idxes;

}


//-----------------------------------------------------------------------------
// Desc: ��������ͼ�Σ���.x�ļ�
//-----------------------------------------------------------------------------
HRESULT
CXModel::InitGeometry()
{
	this->isCustom=false;
    LPD3DXBUFFER pD3DXMtrlBuffer;  //�洢����ģ�Ͳ��ʵĻ���������

    //�Ӵ����ļ���������ģ��

    if( FAILED( D3DXLoadMeshFromX( xfile.c_str(), D3DXMESH_DYNAMIC, 
                                   CGlobal::pCurRender->m_pd3dDevice, NULL, 
                                   &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, 
                                   &g_pMesh ) ) )
    {
        MessageBox(NULL, L"δ��װ��ģ���ļ�", L"Mesh", MB_OK);
        return E_FAIL;
    }

    //������ģ������ȡ�������Ժ������ļ��� 
    D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
    g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];

    if( g_pMeshMaterials == NULL )
        return E_OUTOFMEMORY;

    g_pMeshTextures  = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];
    if( g_pMeshTextures == NULL )
        return E_OUTOFMEMORY;

	//�����ȡ����ģ�Ͳ������Ժ������ļ���
    for( DWORD i=0; i<g_dwNumMaterials; i++ )
    {
        //��������
        g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
		//����ģ�Ͳ��ϵĻ����ⷴ��ϵ��, ��Ϊģ�Ͳ��ϱ���û�����û����ⷴ��ϵ��
        g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

        g_pMeshTextures[i] = NULL;
        if( d3dxMaterials[i].pTextureFilename != NULL && 
            strlen(d3dxMaterials[i].pTextureFilename) > 0 )
        {
			//��ȡ�����ļ���
			WCHAR filename[256];
			RemovePathFromFileName(d3dxMaterials[i].pTextureFilename, filename);
			
            //��������
            if( FAILED( D3DXCreateTextureFromFile( CGlobal::pCurRender->m_pd3dDevice, filename, &g_pMeshTextures[i] ) ) )
            {
                MessageBox(NULL, L"Could not find texture file", L"Mesh", MB_OK);
            }
        }
    }

	//�ͷ��ڼ���ģ���ļ�ʱ�����ı���ģ�Ͳ��ʺ��������ݵĻ���������
    pD3DXMtrlBuffer->Release();  

    return S_OK;
}

//-----------------------------------------------------------------------------
// Desc: �Ӿ���·������ȡ�����ļ���
//-----------------------------------------------------------------------------
void
CXModel::RemovePathFromFileName(LPSTR fullPath, LPWSTR fileName)
{
	//�Ƚ�fullPath�����ͱ任ΪLPWSTR
	WCHAR wszBuf[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, fullPath, -1, wszBuf, MAX_PATH );
	wszBuf[MAX_PATH-1] = L'\0';

	WCHAR* wszFullPath = wszBuf;

	//�Ӿ���·������ȡ�ļ��� , zhhע����������Ŀ¼��ʱ��Ϊ���ڹ�����ע�����������
	LPWSTR pch=wcsrchr(wszFullPath,'\\');
	//if (pch)
	//	lstrcpy(fileName, ++pch);
	//else
		lstrcpy(fileName, wszFullPath);
}

VOID
CXModel::Cleanup()
{
	//�ͷ�����ģ�Ͳ���
    if( g_pMeshMaterials != NULL ) 
        delete[] g_pMeshMaterials;

	//�ͷ�����ģ������
    if( g_pMeshTextures )
    {
        for( DWORD i = 0; i < g_dwNumMaterials; i++ )
        {
            if( g_pMeshTextures[i] )
                g_pMeshTextures[i]->Release();
        }
        delete[] g_pMeshTextures;
    }

	//�ͷ�����ģ�Ͷ���
    if( g_pMesh != NULL )
        g_pMesh->Release();

}

//-----------------------------------------------------------------------------
// Desc: ��Ⱦ����
//-----------------------------------------------------------------------------
VOID
CXModel::Render()
{

	//if (this->isCustom)
	//{
	//	CGlobal::pCurRender->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	//	CGlobal::pCurRender->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x80ffffff);
	//	CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	//	CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	//	CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	//	CGlobal::pCurRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//	//if (drawMode==��ɫģʽ)
	//	//{
	//	//	IFC(m_pd3dDevice->SetTexture(0,nullptr));
	//	//	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,mtrl.Diffuse.a==1); //zh�������, ��Ϊ��͸������Ϊfalse
	//	//	myTri->m_pd3dDevice->SetMaterial( &mtrl );
	//	//}
	//	//else if (drawMode==����ģʽ)
	//	//{
	//	CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,true);
	//	CGlobal::pCurRender->m_pd3dDevice->SetTexture(0,g_pTexture);
	//	//}
	//	//else if (drawMode==�߿�ģʽ)
	//	//{
	//	//	IFC(m_pd3dDevice->SetTexture(0,nullptr));
	//	//	myTri->m_pd3dDevice->SetMaterial( &mtrl );
	//	//	m_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);

	//	//}

	//	mesh->DrawSubset(0);

		//CGlobal::pCurRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	//	CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

	//}
	//else  // x�ļ�ģ�͵���ʾ
	{


		CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,true);

		CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true);
		//CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x00000000);
		//CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);


		CGlobal::pCurRender->m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);

		CGlobal::pCurRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );





		//	CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_NEVER);
		//CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
		//CGlobal::pCurRender->m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
		//CGlobal::pCurRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
		//CGlobal::pCurRender->m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);


	//		 //���ò���
 //   D3DMATERIAL9 mtrl;
 //   ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
 //   mtrl.Diffuse.r = mtrl.Ambient.r = 0;
 //   mtrl.Diffuse.g = mtrl.Ambient.g = 0;
 //   mtrl.Diffuse.b = mtrl.Ambient.b = 1;
 //   mtrl.Diffuse.a = mtrl.Ambient.a = 0.8;

	//mtrl.Specular.r=mtrl.Specular.g=mtrl.Specular.b=0.00025;
	//mtrl.Specular.a=1;
	//mtrl.Power=5;
	//CGlobal::pCurRender->m_pd3dDevice->SetMaterial( &mtrl );

	// //���õƹ�
 //   D3DXVECTOR3 vecDir;
 //   D3DLIGHT9 light;
 //   ZeroMemory( &light, sizeof(D3DLIGHT9) );
 //   light.Type       = D3DLIGHT_DIRECTIONAL;
 //   light.Diffuse.r  =1;
 //   light.Diffuse.g  = 1;
 //   light.Diffuse.b  = 1;
	//light.Ambient.r=0;
	//light.Ambient.g=0;
	//light.Ambient.b=0;
	//light.Specular.r=light.Specular.g=light.Specular.b=0.0001;
	//vecDir = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	//////vecDir = D3DXVECTOR3(-0.96580493 ,0.011636866 ,0.25900859);
 //   D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
 //   //light.Range       = 1000.0f;
 //   CGlobal::pCurRender->m_pd3dDevice->SetLight( 0, &light );
 //   CGlobal::pCurRender->m_pd3dDevice->LightEnable( 0, true );
	//CGlobal::pCurRender->m_pd3dDevice->LightEnable( 1, false );
	//CGlobal::pCurRender->m_pd3dDevice->LightEnable( 2, false );
	//CGlobal::pCurRender->m_pd3dDevice->LightEnable( 3, false );
	//CGlobal::pCurRender->m_pd3dDevice->LightEnable( 4, false );
	//CGlobal::pCurRender->m_pd3dDevice->LightEnable( 5, false );
    //CGlobal::pCurRender->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );
	//CGlobal::pCurRender->m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, true );
 //   //���û�����
 //   CGlobal::pCurRender->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xFF101010);






		//�����Ⱦ����ģ��
		for( DWORD i=0; i<g_dwNumMaterials; i++ )
		{
			//���ò��ʺ�����
			if (g_pMeshMaterials!=NULL)
				CGlobal::pCurRender->m_pd3dDevice->SetMaterial( &g_pMeshMaterials[i] );
			CGlobal::pCurRender->m_pd3dDevice->SetTexture( 0, g_pMeshTextures[i] );
			//��Ⱦģ��
			g_pMesh->DrawSubset( i );


		}
			CGlobal::pCurRender->m_pd3dDevice->SetTexture(0,nullptr);


		CGlobal::pCurRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

	}






	





}