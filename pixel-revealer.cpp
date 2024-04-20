#include <iostream>
#include <Windows.h>
#include <ShObjIdl_core.h>
#include <ctime>
#include <random>

using namespace std;

int main() {
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        cerr << "Error initializing COM library: " << hex << hr << endl;
        return -1;
    }

    IFileOpenDialog* pFileOpen;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if (FAILED(hr)) {
        cerr << "Error creating file open dialog: " << hex << hr << endl;
        CoUninitialize();
        return -1;
    }

    FILEOPENDIALOGOPTIONS options;
    hr = pFileOpen->GetOptions(&options);
    if (FAILED(hr)) {
        cerr << "Error getting file dialog options: " << hex << hr << endl;
        pFileOpen->Release();
        CoUninitialize();
        return -1;
    }

    pFileOpen->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST);

    hr = pFileOpen->Show(NULL);
    if (FAILED(hr)) {
        cerr << "Error showing file open dialog: " << hex << hr << endl;
        pFileOpen->Release();
        CoUninitialize();
        return -1;
    }

    IShellItem* pItem;
    hr = pFileOpen->GetResult(&pItem);
    if (FAILED(hr)) {
        cerr << "Error getting file dialog result: " << hex << hr << endl;
        pFileOpen->Release();
        CoUninitialize();
        return -1;
    }

    PWSTR filePath;
    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
    if (FAILED(hr)) {
        cerr << "Error getting file path: " << hex << hr << endl;
        pItem->Release();
        pFileOpen->Release();
        CoUninitialize();
        return -1;
    }

    pItem->Release();
    pFileOpen->Release();
    CoUninitialize();

    wchar_t* wideFilePath = filePath;
    char filePathBuffer[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, wideFilePath, -1, filePathBuffer, MAX_PATH, NULL, NULL);

    string imagePath = filePathBuffer;

    int pixelCount;
    cout << "Enter the number of pixels to reveal: ";
    cin >> pixelCount;

    srand(static_cast<unsigned int>(time(nullptr)));

    HDC hdc = GetDC(NULL);
    HBITMAP hBitmap = (HBITMAP)LoadImageW(NULL, wideFilePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hBitmap == NULL) {
        cout << "Error loading image." << endl;
        return -1;
    }

    BITMAP bitmap;
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);

    int width = bitmap.bmWidth;
    int height = bitmap.bmHeight;

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    for (int count = 0; count < pixelCount; ++count) {
        int randomX = rand() % width;
        int randomY = rand() % height;

        COLORREF color = GetPixel(hdcMem, randomX, randomY);
        SetPixel(hdc, randomX, randomY, RGB(0, 0, 0));

        Sleep(1); // Optional delay for visualization purposes
        InvalidateRect(NULL, NULL, TRUE); // Update the screen

        cout << "Pixel (" << randomX << ", " << randomY << ") Revealed!" << endl;
    }

    SelectObject(hdcMem, hOldBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdc);
    DeleteObject(hBitmap);

    return 0;
}
