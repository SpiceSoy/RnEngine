﻿#include "WindowsEntry.h"
#include "Core/Engine/RnEngine.h"
#include "Platform/Windows/WindowsCommons.h"
#include "Platform/Windows/WindowsPlatformContext.h"
#include <iostream>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                           // 현재 인스턴스입니다.
WCHAR     szTitle[ MAX_LOADSTRING ];       // 제목 표시줄 텍스트입니다.
WCHAR     szWindowClass[ MAX_LOADSTRING ]; // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM MyRegisterClass( HINSTANCE hInstance );

BOOL InitInstance( HINSTANCE, int );

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

INT_PTR CALLBACK About( HWND, UINT, WPARAM, LPARAM );

int APIENTRY wWinMain( _In_ HINSTANCE     hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPWSTR        lpCmdLine,
					   _In_ int           nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	// 전역 문자열을 초기화합니다.
	LoadStringW( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
	LoadStringW( hInstance, IDC_RNENGINE, szWindowClass, MAX_LOADSTRING );
	MyRegisterClass( hInstance );

	auto platformContext = new Rn::WindowsPlatformContext();
	if ( !platformContext ) return FALSE;

	Rn::RnEngine& engine = Rn::RnEngine::GetInstance();

	std::vector< std::wstring_view > arguments;
	wchar_t**                        argv = __wargv;

	for ( auto i = 0; i < __argc; ++i )
	{
		arguments.emplace_back( argv[ i ] );
	}

	bool result = engine.Initialize( platformContext, arguments );
	if ( !result ) return FALSE;

	// 애플리케이션 초기화를 수행합니다:
	if ( !engine.IsConsoleMode() && !InitInstance( hInstance, nCmdShow ) )
	{
		return FALSE;
	}

	if ( engine.IsConsoleMode() || engine.IsShowDebugConsole() )
	{
		if ( !AllocConsole() )
		{
			MessageBox( nullptr, L"The console window was not created", nullptr, MB_ICONEXCLAMATION );
		}

		FILE* console = nullptr;

		freopen_s( &console, "CONIN$", "r", stdin );
		freopen_s( &console, "CONOUT$", "w", stderr );
		freopen_s( &console, "CONOUT$", "w", stdout );

		printf( "CONSOLE MODE : DEBUG CONSOLE OPEN\n" );

		std::cout.clear();
	}

	if ( engine.IsConsoleMode() )
	{
		while ( !engine.IsTerminated() )
		{
			engine.Tick();
		}

		engine.OnClose();

		return 0;
	}
	HACCEL hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_RNENGINE ) );

	MSG msg{};

	while ( msg.message != WM_QUIT )
	{
		if ( ::PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
		{
			if ( !::TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) )
			{
				TranslateMessage( &msg );
				::DispatchMessage( &msg );
			}
		}
		else
		{
			engine.Tick();
		}
	}

	engine.OnClose();

	return static_cast< int >(msg.wParam);
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof( WNDCLASSEX );

	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_RNENGINE ) );
	wcex.hCursor       = LoadCursor( nullptr, IDC_ARROW );
	wcex.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
	// wcex.lpszMenuName  = MAKEINTRESOURCEW( IDC_RNENGINE );
	wcex.lpszMenuName  = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm       = LoadIcon( wcex.hInstance, MAKEINTRESOURCE( IDI_SMALL ) );

	return RegisterClassExW( &wcex );
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	HWND hWnd = CreateWindowW( szWindowClass,
							   szTitle,
							   WS_OVERLAPPEDWINDOW,
							   CW_USEDEFAULT,
							   0,
							   CW_USEDEFAULT,
							   0,
							   nullptr,
							   nullptr,
							   hInstance,
							   nullptr );

	if ( !hWnd )
	{
		return FALSE;
	}

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_COMMAND:
		{
			int wmId = LOWORD( wParam );
			// 메뉴 선택을 구문 분석합니다:
			switch ( wmId )
			{
			case IDM_ABOUT:
				DialogBox( hInst, MAKEINTRESOURCE( IDD_ABOUTBOX ), hWnd, About );
				break;
			case IDM_EXIT:
				DestroyWindow( hWnd );
				break;
			default:
				return DefWindowProc( hWnd, message, wParam, lParam );
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC         hdc = BeginPaint( hWnd, &ps );
			// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
			EndPaint( hWnd, &ps );
		}
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( lParam );
	switch ( message )
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if ( LOWORD( wParam ) == IDOK || LOWORD( wParam ) == IDCANCEL )
		{
			EndDialog( hDlg, LOWORD( wParam ) );
			return TRUE;
		}
		break;
	}
	return FALSE;
}
