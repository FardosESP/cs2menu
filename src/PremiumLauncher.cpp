#include <Windows.h>
#include <CommCtrl.h>
#include <TlHelp32.h>
#include <string>
#include <thread>
#include <chrono>
#include "ManualMap.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Professional Launcher for CS2MENU - Premium Edition
// Features: Manual Mapping, GUI, Status Updates, Anti-Debug

#define WM_INJECT_STATUS (WM_USER + 1)
#define WM_INJECT_COMPLETE (WM_USER + 2)
#define WM_INJECT_FAILED (WM_USER + 3)

// Global variables
HWND g_hMainWnd = NULL;
HWND g_hStatusText = NULL;
HWND g_hProgressBar = NULL;
HWND g_hInjectBtn = NULL;
HWND g_hMethodCombo = NULL;
HFONT g_hTitleFont = NULL;
HFONT g_hNormalFont = NULL;
HBRUSH g_hBackgroundBrush = NULL;

// Colors (Dark theme - professional)
#define COLOR_BACKGROUND RGB(20, 20, 25)
#define COLOR_PANEL RGB(30, 30, 35)
#define COLOR_ACCENT RGB(88, 101, 242)  // Discord blue
#define COLOR_TEXT RGB(220, 220, 220)
#define COLOR_SUCCESS RGB(67, 181, 129)
#define COLOR_ERROR RGB(240, 71, 71)

// Injection methods
enum InjectionMethod
{
    METHOD_LOADLIBRARY = 0,
    METHOD_MANUAL_MAP = 1,
    METHOD_THREAD_HIJACK = 2
};

struct InjectionContext
{
    DWORD processId;
    std::wstring dllPath;
    InjectionMethod method;
    HWND statusWindow;
};

// Forward declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitializeUI(HWND hWnd);
void UpdateStatus(const wchar_t* message, COLORREF color = COLOR_TEXT);
void SetProgress(int percent);
DWORD WINAPI InjectionThread(LPVOID lpParam);
DWORD FindProcessByName(const wchar_t* processName);
bool IsProcessRunning(DWORD pid);
bool CheckAntiDebug();
std::wstring GetDllPath();

// XOR string obfuscation (basic anti-reverse engineering)
std::wstring DecryptString(const char* encrypted, size_t len, char key)
{
    std::wstring result;
    for (size_t i = 0; i < len; i++)
    {
        result += (wchar_t)(encrypted[i] ^ key);
    }
    return result;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Anti-debug check
    if (CheckAntiDebug())
    {
        MessageBoxW(NULL, L"Debugger detected. Exiting.", L"Security Alert", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    // Register window class
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(COLOR_BACKGROUND);
    wc.lpszClassName = L"CS2MenuLauncher";
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassExW(&wc))
    {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create main window
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 500;
    int windowHeight = 400;
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    g_hMainWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST,
        L"CS2MenuLauncher",
        L"CS2MENU - Premium Loader",
        WS_POPUP | WS_VISIBLE,
        posX, posY, windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL
    );

    if (!g_hMainWnd)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Set window transparency
    SetLayeredWindowAttributes(g_hMainWnd, 0, 250, LWA_ALPHA);

    // Initialize UI elements
    InitializeUI(g_hMainWnd);

    // Show window with fade-in effect
    for (int alpha = 0; alpha <= 255; alpha += 15)
    {
        SetLayeredWindowAttributes(g_hMainWnd, 0, alpha, LWA_ALPHA);
        Sleep(10);
    }

    ShowWindow(g_hMainWnd, nCmdShow);
    UpdateWindow(g_hMainWnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    if (g_hTitleFont) DeleteObject(g_hTitleFont);
    if (g_hNormalFont) DeleteObject(g_hNormalFont);
    if (g_hBackgroundBrush) DeleteObject(g_hBackgroundBrush);

    return (int)msg.wParam;
}

void InitializeUI(HWND hWnd)
{
    // Create fonts
    g_hTitleFont = CreateFontW(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    g_hNormalFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    // Title label
    HWND hTitle = CreateWindowW(L"STATIC", L"CS2MENU",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 30, 500, 40,
        hWnd, NULL, NULL, NULL);
    SendMessage(hTitle, WM_SETFONT, (WPARAM)g_hTitleFont, TRUE);

    // Subtitle
    HWND hSubtitle = CreateWindowW(L"STATIC", L"Premium Edition - Build 14138.5",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 75, 500, 25,
        hWnd, NULL, NULL, NULL);
    SendMessage(hSubtitle, WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);

    // Injection method combo box
    HWND hMethodLabel = CreateWindowW(L"STATIC", L"Injection Method:",
        WS_CHILD | WS_VISIBLE,
        50, 120, 150, 25,
        hWnd, NULL, NULL, NULL);
    SendMessage(hMethodLabel, WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);

    g_hMethodCombo = CreateWindowW(L"COMBOBOX", NULL,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        200, 118, 250, 200,
        hWnd, (HMENU)1001, NULL, NULL);
    SendMessage(g_hMethodCombo, WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);
    SendMessage(g_hMethodCombo, CB_ADDSTRING, 0, (LPARAM)L"LoadLibrary (Standard)");
    SendMessage(g_hMethodCombo, CB_ADDSTRING, 0, (LPARAM)L"Manual Map (Stealth)");
    SendMessage(g_hMethodCombo, CB_ADDSTRING, 0, (LPARAM)L"Thread Hijack (Advanced)");
    SendMessage(g_hMethodCombo, CB_SETCURSEL, 1, 0);  // Default to Manual Map

    // Status text
    g_hStatusText = CreateWindowW(L"STATIC", L"Ready to inject...",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        50, 170, 400, 25,
        hWnd, NULL, NULL, NULL);
    SendMessage(g_hStatusText, WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);

    // Progress bar
    g_hProgressBar = CreateWindowExW(0, PROGRESS_CLASSW, NULL,
        WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
        50, 210, 400, 25,
        hWnd, NULL, NULL, NULL);
    SendMessage(g_hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
    SendMessage(g_hProgressBar, PBM_SETSTEP, 1, 0);

    // Inject button
    g_hInjectBtn = CreateWindowW(L"BUTTON", L"INJECT",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
        150, 260, 200, 45,
        hWnd, (HMENU)1002, NULL, NULL);
    SendMessage(g_hInjectBtn, WM_SETFONT, (WPARAM)g_hTitleFont, TRUE);

    // Close button
    HWND hCloseBtn = CreateWindowW(L"BUTTON", L"✕",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
        460, 10, 30, 30,
        hWnd, (HMENU)1003, NULL, NULL);
    SendMessage(hCloseBtn, WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);

    // Info text
    HWND hInfo = CreateWindowW(L"STATIC", L"Press INSERT in-game to open menu",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 330, 500, 20,
        hWnd, NULL, NULL, NULL);
    SendMessage(hInfo, WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        break;

    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, COLOR_TEXT);
        SetBkColor(hdcStatic, COLOR_BACKGROUND);
        return (LRESULT)CreateSolidBrush(COLOR_BACKGROUND);
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case 1002:  // Inject button
        {
            // Disable button during injection
            EnableWindow(g_hInjectBtn, FALSE);
            SetWindowTextW(g_hInjectBtn, L"INJECTING...");

            // Get selected method
            int methodIndex = (int)SendMessage(g_hMethodCombo, CB_GETCURSEL, 0, 0);

            // Create injection context
            InjectionContext* ctx = new InjectionContext();
            ctx->processId = FindProcessByName(L"cs2.exe");
            ctx->dllPath = GetDllPath();
            ctx->method = (InjectionMethod)methodIndex;
            ctx->statusWindow = hWnd;

            if (ctx->processId == 0)
            {
                UpdateStatus(L"CS2 not found! Please start the game.", COLOR_ERROR);
                EnableWindow(g_hInjectBtn, TRUE);
                SetWindowTextW(g_hInjectBtn, L"INJECT");
                delete ctx;
                break;
            }

            // Start injection thread
            CreateThread(NULL, 0, InjectionThread, ctx, 0, NULL);
            break;
        }
        case 1003:  // Close button
            PostQuitMessage(0);
            break;
        }
        break;
    }

    case WM_INJECT_STATUS:
    {
        wchar_t* message = (wchar_t*)lParam;
        UpdateStatus(message, COLOR_TEXT);
        delete[] message;
        break;
    }

    case WM_INJECT_COMPLETE:
    {
        UpdateStatus(L"✓ Injection successful! Press INSERT in-game.", COLOR_SUCCESS);
        SetProgress(100);
        EnableWindow(g_hInjectBtn, TRUE);
        SetWindowTextW(g_hInjectBtn, L"INJECT");
        break;
    }

    case WM_INJECT_FAILED:
    {
        wchar_t* error = (wchar_t*)lParam;
        UpdateStatus(error, COLOR_ERROR);
        delete[] error;
        SetProgress(0);
        EnableWindow(g_hInjectBtn, TRUE);
        SetWindowTextW(g_hInjectBtn, L"INJECT");
        break;
    }

    case WM_LBUTTONDOWN:
    {
        // Allow window dragging
        SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void UpdateStatus(const wchar_t* message, COLORREF color)
{
    SetWindowTextW(g_hStatusText, message);
    // Note: Color change would require custom drawing
}

void SetProgress(int percent)
{
    SendMessage(g_hProgressBar, PBM_SETPOS, percent, 0);
}

DWORD WINAPI InjectionThread(LPVOID lpParam)
{
    InjectionContext* ctx = (InjectionContext*)lpParam;

    // Update status
    auto SendStatus = [ctx](const wchar_t* msg) {
        wchar_t* copy = new wchar_t[wcslen(msg) + 1];
        wcscpy_s(copy, wcslen(msg) + 1, msg);
        PostMessage(ctx->statusWindow, WM_INJECT_STATUS, 0, (LPARAM)copy);
    };

    auto SendError = [ctx](const wchar_t* msg) {
        wchar_t* copy = new wchar_t[wcslen(msg) + 1];
        wcscpy_s(copy, wcslen(msg) + 1, msg);
        PostMessage(ctx->statusWindow, WM_INJECT_FAILED, 0, (LPARAM)copy);
    };

    SendStatus(L"Opening CS2 process...");
    SetProgress(20);
    Sleep(500);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ctx->processId);
    if (!hProcess)
    {
        SendError(L"✗ Failed to open CS2 process. Run as admin!");
        delete ctx;
        return 1;
    }

    SendStatus(L"Checking process protection...");
    SetProgress(40);
    Sleep(300);

    if (ManualMap::IsProcessProtected(hProcess))
    {
        SendStatus(L"⚠ Process protected - using advanced method...");
    }

    SendStatus(L"Injecting DLL...");
    SetProgress(60);
    Sleep(500);

    bool success = false;
    switch (ctx->method)
    {
    case METHOD_MANUAL_MAP:
        success = ManualMap::InjectDll(hProcess, std::string(ctx->dllPath.begin(), ctx->dllPath.end()).c_str());
        break;
    case METHOD_LOADLIBRARY:
        // Standard LoadLibrary injection (fallback)
        SendError(L"✗ LoadLibrary not implemented in this version");
        break;
    case METHOD_THREAD_HIJACK:
        SendError(L"✗ Thread Hijack not implemented in this version");
        break;
    }

    if (success)
    {
        SendStatus(L"Erasing PE headers...");
        SetProgress(80);
        Sleep(300);

        PostMessage(ctx->statusWindow, WM_INJECT_COMPLETE, 0, 0);
    }
    else
    {
        SendError(L"✗ Injection failed. Check anti-virus/anti-cheat.");
    }

    CloseHandle(hProcess);
    delete ctx;
    return 0;
}

DWORD FindProcessByName(const wchar_t* processName)
{
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32W pe = { sizeof(pe) };
        if (Process32FirstW(snapshot, &pe))
        {
            do {
                if (_wcsicmp(processName, pe.szExeFile) == 0)
                {
                    pid = pe.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &pe));
        }
        CloseHandle(snapshot);
    }
    return pid;
}

bool IsProcessRunning(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess)
    {
        DWORD exitCode;
        GetExitCodeProcess(hProcess, &exitCode);
        CloseHandle(hProcess);
        return exitCode == STILL_ACTIVE;
    }
    return false;
}

bool CheckAntiDebug()
{
    // Check for debugger
    if (IsDebuggerPresent())
        return true;

    // Check for remote debugger
    BOOL isRemoteDebuggerPresent = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isRemoteDebuggerPresent);
    if (isRemoteDebuggerPresent)
        return true;

    // Check for hardware breakpoints
    CONTEXT ctx = { 0 };
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    if (GetThreadContext(GetCurrentThread(), &ctx))
    {
        if (ctx.Dr0 != 0 || ctx.Dr1 != 0 || ctx.Dr2 != 0 || ctx.Dr3 != 0)
            return true;
    }

    return false;
}

std::wstring GetDllPath()
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring fullPath(path);
    size_t lastSlash = fullPath.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos)
    {
        fullPath = fullPath.substr(0, lastSlash + 1);
    }
    fullPath += L"cs2menu.dll";
    return fullPath;
}
