#include "../head/Win.h"
#include "cassert"
#include "cstdio"

window_t* window = NULL;

static LRESULT CALLBACK msg_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)	//�����¼���ÿ֡����
{
	switch (msg)
	{
	case WM_CLOSE:
		window->is_close = true;
		break;
	case WM_KEYDOWN:
		window->keys[wParam & 511] = 1;
		break;
	case WM_KEYUP:
		window->keys[wParam & 511] = 0;
		break;
	case WM_LBUTTONDOWN:
		window->mouse_info.orbit_pos = get_mouse_pos();	//�������ʱÿ֡��ȡ���λ�ã�����������ƽǶ�
		window->buttons[0] = 1; break;
	case WM_LBUTTONUP:
		window->buttons[0] = 0;
		break;
	case WM_RBUTTONDOWN:
		window->mouse_info.fv_pos = get_mouse_pos();	//�Ҽ�����ʱÿ֡��ȡ���λ�ã���������ռ�λ��
		window->buttons[1] = 1;
		break;
	case WM_RBUTTONUP:
		window->buttons[1] = 0;
		break;
	case WM_MOUSEWHEEL:									//���ܹ�����ת�Ƕ�
		window->mouse_info.wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		break;

	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

static void register_window_class()							//ע�ᴰ�ں���
{
	ATOM atom;												//��ʼ���ṹ��
	WNDCLASS wc;											//
	wc.style = CS_BYTEALIGNCLIENT;							//���ڷ��
	wc.lpfnWndProc = (WNDPROC)msg_callback;					//�ص�����
	wc.cbClsExtra = 0;										//�����ڴ�����β����һ�����ռ䣬��������Ϊ0
	wc.cbWndExtra = 0;										//�����ڴ���ʵ��β����һ�����ռ䣬��������Ϊ0
	wc.hInstance = GetModuleHandle(NULL);					//��ǰʵ�����
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);				//������ͼ��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);				//�����ʽ
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	//������ʽ
	wc.lpszMenuName = NULL;									//�˵�
	wc.lpszClassName = ("Sparrow");							//�ô����������,vs19������Ҫ����Ŀ-��Ŀ������-�߼�-�ַ���-ʹ�ö��ֽ��ַ���

	atom = RegisterClass(&wc);								//����ע�ᴰ�ڶ���
	assert(atom != 0);
}

static void init_bm_header(BITMAPINFOHEADER& bi, int width, int height)	//��ʼ��λͼͷ��ʽ
{
	memset(&bi, 0, sizeof(BITMAPINFOHEADER));
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;									//���ϵ���
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = width * height * 4;					//λͼ�ߴ��С
}

int window_init(int width, int height, const char* title)
{
	window = (window_t*)malloc(sizeof(window_t));			//���봰�ڸ�ʽ
	memset(window, 0, sizeof(window_t));					//��ʼ�����ڸ�ʽΪ0
	window->is_close = false;								

	RECT rect = { 0, 0, width, height };					//������ʾ���η�Χ ��������
	int wx, wy, sx, sy;										
	LPVOID ptr;												//LPVOID��û�����͵�ָ�룬��������ָ��
	HDC hDC;												//�豸���
	BITMAPINFOHEADER bi;									//λͼͷ��ʽ

	register_window_class();								//����ע�ắ��������ע�ᴰ�ڶ���
	
	window->h_window = CreateWindow(("Sparrow"), title,			//�������ڣ�������ָ��Ĵ�����������ʾ�Ĵ��ڱ��⣬���ڷ�񣬳�ʼλ�úͿ�ߣ�
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,//�����ھ�����˵������ģ��ʵ�����������ָ��
		0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
	assert(window->h_window != NULL);
	
	init_bm_header(bi, width, height);						//��ʼ��λͼͷ��ʽ
	
	hDC = GetDC(window->h_window);							//�Ӵ��ھ������ȡ�豸�����ľ��
	window->mem_dc = CreateCompatibleDC(hDC);				//����һ�����豸���ݵģ��ڴ��豸����mem_dc���������
	ReleaseDC(window->h_window, hDC);						//�ͷ��豸����
	
	//�����豸�޹�λͼ�����CreateDIBSection�������豸���������λͼͷ��ʽ�����ԣ�ָ��λͼλ���ݵ�ָ�룬�ļ�ӳ����������ļ�ӳ��ƫ��
	window->bm_dib = CreateDIBSection(window->mem_dc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &ptr, 0, 0); 
	assert(window->bm_dib != NULL);

	window->bm_old = (HBITMAP)SelectObject(window->mem_dc, window->bm_dib);//���´�����λͼ���д��mem_dc�����滻ͬ���͵ľ�λͼ
	window->window_fb = (unsigned char*)ptr;

	window->width = width;
	window->height = height;

	//GetWindowLong���봰�ھ������ȡGWL_STYLE�����ڷ����Ϣ���ο���https://blog.csdn.net/hnhyhongmingjiang/article/details/2154410
	//AdjustWindowRect�����ݿͻ�����rect��С�������ͻ����ڴ�С��ע����ʾ���γߴ�ʹ��ڳߴ粻ͬ��
	AdjustWindowRect(&rect, GetWindowLong(window->h_window, GWL_STYLE), 0);	
	wx = rect.right - rect.left;				// ��������ڿ�Ⱥ͸߶�
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2; // GetSystemMetrics(SM_CXSCREEN)��ȡ��Ļ�Ŀ�ȣ�SM_CYSCREEN��ȡ��Ļ�ĸ߶ȵ�
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2; // �������������λ��
	if (sy < 0) sy = 0;
	//���ô��ڳߴ磬���������ھ������ʶ˳����߽硢�б߽硢��ȡ��߶ȡ����Ա�־���ο���https://baike.baidu.com/item/SetWindowPos/6376849?fr=aladdin#2
	SetWindowPos(window->h_window, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(window->h_window);		//����ǰ̨����
	ShowWindow(window->h_window, SW_NORMAL);	//��ʾ����

	msg_dispatch();								//��Ϣ����

	memset(window->window_fb, 0, width * height * 4);	//��ʼ��λͼ�ڴ�����
	memset(window->keys, 0, sizeof(char) * 512);		//��ʼ�������ڴ�����
	return 0;
}

int window_destroy()
{
	if (window->mem_dc)
	{
		if (window->bm_old)
		{
			SelectObject(window->mem_dc, window->bm_old); // д��ԭ����bitmap�������ͷ�DC��
			window->bm_old = NULL;
		}
		DeleteDC(window->mem_dc);
		window->mem_dc = NULL;
	}
	if (window->bm_dib)
	{
		DeleteObject(window->bm_dib);
		window->bm_dib = NULL;
	}
	if (window->h_window)
	{
		CloseWindow(window->h_window);
		window->h_window = NULL;
	}

	free(window);
	return 0;
}

void msg_dispatch()
{
	MSG msg;
	while (1)
	{
		// Peek��������Get��������PM_NOREMOVE��ʾ�������Ϣ������������������Get����
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break; //û��Ϣ���ȷ������Ϣ����Get
		if (!GetMessage(&msg, NULL, 0, 0)) break;

		TranslateMessage(&msg);	 //ת����Ϣ ���ⰴ��->�ַ�
		DispatchMessage(&msg); //������Ϣ���ص�
	}
}

static void window_display()
{
	LOGFONT logfont; //�ı��������
	ZeroMemory(&logfont, sizeof(LOGFONT));
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfHeight = 20; //��������Ĵ�С
	HFONT hFont = CreateFontIndirect(&logfont);

	HDC hDC = GetDC(window->h_window);
	//Ŀ����е����Ͻ�(x,y), ��ȣ��߶ȣ�������ָ��
	SelectObject(window->mem_dc, hFont);
	SetTextColor(window->mem_dc, RGB(190, 190, 190));
	SetBkColor(window->mem_dc, RGB(80, 80, 80));
	TextOut(window->mem_dc, 20, 20, "control:hold left buttion to rotate, right button to pan", strlen("Control:hold left buttion to rotate, right button to pan"));

	// �Ѽ�����DC�����ݴ���������DC��
	BitBlt(hDC, 0, 0, window->width, window->height, window->mem_dc, 0, 0, SRCCOPY);
	ReleaseDC(window->h_window, hDC);
}

void window_draw(unsigned char* framebuffer)	//����ͼ�񣬽�֡�������ݸ��Ƶ�λͼ��Ӧλ��
{
	for (int i = 0; i < window->height; i++)
	{
		for (int j = 0; j < window->width; j++)
		{
			int index = (i * window->width + j) * 4;
			window->window_fb[index] = framebuffer[index];
			window->window_fb[index + 1] = framebuffer[index + 1];
			window->window_fb[index + 2] = framebuffer[index + 2];
		}
	}
	window_display();
}

vec2 get_mouse_pos()
{
	POINT point;							//�������
	GetCursorPos(&point);
	ScreenToClient(window->h_window, &point); // ����Ļ�ռ�ת�����ڿռ�
	return vec2((float)point.x, (float)point.y);
}

static double get_native_time(void) {	
	static double period = -1;
	LARGE_INTEGER counter;
	if (period < 0) {
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);	//��ȡʱ��Ƶ��
		period = 1 / (double)frequency.QuadPart;
	}
	QueryPerformanceCounter(&counter);
	return counter.QuadPart * period;
}

float platform_get_time(void) {			//��ȡƽ̨����ʱ��
	static double initial = -1;
	if (initial < 0) {
		initial = get_native_time();
	}
	return (float)(get_native_time() - initial);
}
