//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dinit.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates how to initialize Direct3D, how to use the book's framework
//       functions, and how to clear the screen to black.  Note that the Direct3D
//       initialization code is in the d3dUtility.h/.cpp files.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

ID3DXMesh* Objects[2] = { 0,0 };
D3DXMATRIX World[2];
D3DXMATRIX originWorld;
D3DMATERIAL9 Mtrl[2];

struct Vertex
{
	float x, y, z;
	Vertex(float a, float b, float c) {
		x = a; y = b; z = c;
	}
};


IDirect3DVertexBuffer9* VB = 0;
IDirect3DIndexBuffer9* IB = 0;

const int width = 800;
const int height = 600;

//
// Framework Functions
//

bool Setup()
{	
	D3DXCreateSphere(Device, 0.25f, 20, 20, &Objects[0], 0);
	D3DXCreateSphere(Device, 0.25f, 20, 20, &Objects[1], 0);

	D3DXMatrixTranslation(&World[0], 1.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&World[1],-1.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&originWorld, 0.0f, 0.0f, 0.0f);


	Mtrl[0] = d3d::YELLOW_MTRL;
	Mtrl[1] = d3d::YELLOW_MTRL;

	Device->CreateVertexBuffer(8 * sizeof(Vertex), D3DUSAGE_WRITEONLY, 
		D3DFVF_XYZ, D3DPOOL_MANAGED, &VB, 0);
	Vertex* vertices;
	VB->Lock(0, 0, (void**)&vertices, 0);
	vertices[0] = Vertex(3.0f, 0.0f, 0.5f);
	vertices[1] = Vertex(3.0f, 0.0f, -0.5f);
	vertices[2] = Vertex(0.0f, -sqrtf(3.0f), 0.5f);
	vertices[3] = Vertex(0.0f, -sqrtf(3.0f),-0.5f);
	vertices[4] = Vertex(-3.0f, 0.0f, 0.5f);
	vertices[5] = Vertex(-3.0f, 0.0f,-0.5f);
	vertices[6] = Vertex(0.0f, sqrtf(3.0f), 0.5f);
	vertices[7] = Vertex(0.0f, sqrtf(3.0f), -0.5f);
	VB->Unlock();

	Device->CreateIndexBuffer(24 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
		D3DPOOL_MANAGED, &IB, 0);
	WORD* indices = 0;
	IB->Lock(0, 0, (void**)&indices, 0);
	indices[0] = 0; indices[1] = 1;
	indices[2] = 0; indices[3] = 2;
	indices[4] = 0; indices[5] = 6;
	indices[6] = 1; indices[7] = 3;
	indices[8] = 1; indices[9] = 7;
	indices[10] = 2; indices[11] = 3;
	indices[12] = 2; indices[13] = 4;
	indices[14] = 3; indices[15] = 5;
	indices[16] = 4; indices[17] = 5;
	indices[18] = 4; indices[19] = 6;
	indices[20] = 5; indices[21] = 7;
	indices[22] = 6; indices[23] = 7;
	IB->Unlock();


	D3DXVECTOR3 dir(1.0f, -0.0f, 0.25f);
	D3DXCOLOR c = d3d::WHITE;
	D3DLIGHT9 dirLight= d3d::InitDirectionalLight(&dir, &c);

	Device->SetLight(0, &dirLight);
	Device->LightEnable(0, true);

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f, // 45 - degree
		(float)width / (float)height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;


}

void Cleanup()
{
	for (int i = 0; i < 2; i++)
		d3d::Release<ID3DXMesh*>(Objects[i]);
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
}

bool Display(float timeDelta)
{
	if( Device ) // Only use Device methods if we have a valid device.
	{
		// 
		// Update the scene: update camera position.
		//

		static float angle = (3.0f * D3DX_PI) / 2.0f;
		static float height = 5.0f;

		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			angle -= 0.5f * timeDelta;

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			angle += 0.5f * timeDelta;

		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			height += 5.0f * timeDelta;

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			height -= 5.0f * timeDelta;

		D3DXVECTOR3 position(cosf(angle) * 7.0f, height, sinf(angle) * 7.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);

		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Draw the scene:
		//
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		for (int i = 0; i < 2; i++)
		{
			// set material and world matrix for ith object, then render
			// the ith object.
			Device->SetMaterial(&Mtrl[i]);
			Device->SetTransform(D3DTS_WORLD, &World[i]);
			Objects[i]->DrawSubset(0);
		}

		Device->SetTransform(D3DTS_WORLD, &originWorld);

		Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
		Device->SetIndices(IB);
		Device->SetFVF(D3DFVF_XYZ);

		Device->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 8, 0, 12);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;

}


//
// WndProc
//

LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	HMENU hMenu;


	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);

		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		width, height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, L"InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, L"Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}