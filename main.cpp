#include<windows.h>
#include<iostream>
#include<stdio.h>
#include <tchar.h>
#include <shlobj.h>
#include <vector>
#define BUFSIZE MAX_PATH
#define READ_BUTTON 1
#define CREATE_BUTTON 2
#define DELETE_BUTTON 3
#define GENERATE_BUTTON 4
#define CD_BUTTON 5
//#include <strsafe.h>
//#pragma comment(lib, "User32.lib")

using namespace std;


HANDLE hFile;
LRESULT CALLBACK WindowProcedure(HWND,UINT,WPARAM,LPARAM);

void AddControls(HWND);

HWND input;
HWND hOut;
HWND hEdit;

void criarArquivoTmp(HWND hWnd)
{
    TCHAR Buffer[BUFSIZE];
    TCHAR szBuffer[100];
    GetCurrentDirectory(BUFSIZE, Buffer);
    TCHAR* nomeArq = "ARQUIVOTMP.txt";

    SendMessage((HWND) input,(UINT) WM_GETTEXT, (WPARAM) 10, (LPARAM) &szBuffer);
    //cin >> nomeArq; nao vai funcionar
    strcat(Buffer, "\\");
    strcat(Buffer, nomeArq);

    hFile = CreateFile(Buffer,
                       GENERIC_ALL,
                       FILE_SHARE_READ,
                       NULL,
                       CREATE_NEW,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);


    if( hFile == INVALID_HANDLE_VALUE)
    {
        cout<<"createFile failed & error no =" << GetLastError() <<endl;
    }
    else
    {
        strcat(Buffer," Criado com sucesso!");
        MessageBox(NULL,Buffer,"Criação de arquivo",MB_OK);
        cout<<"Arquivo criado com sucesso!\n"<< endl;
    }

}

void deletarArquivo(HWND hWnd)
{
    OPENFILENAME ofn;

    char file_name[100];

    ZeroMemory(&ofn,sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = "All files\0*.*\0Source Files\0*.CPP\0Text Files\0*.TXT\0";
    ofn.nFilterIndex = 1;

    GetOpenFileName(&ofn);

    bool rm = DeleteFile(ofn.lpstrFile);
    strcat(ofn.lpstrFile," foi deletado com sucesso!");

  if(rm == 0)
    MessageBox(NULL,TEXT("FALHA AO DELETAR ARQUIVO"),"",MB_OK);
  else
    MessageBox(NULL,ofn.lpstrFile,"",MB_OK);


}

void deletarArquivosTmp(TCHAR* arquivosTmp)
{
    bool rm = DeleteFile(arquivosTmp);
    strcat(arquivosTmp," foi deletado com sucesso!");
}




void currentDirec()
{
   TCHAR Buffer[BUFSIZE];
   DWORD dwRet;

   dwRet = GetCurrentDirectory(BUFSIZE, Buffer);
   strcat(Buffer, " virou seu novo diretorio!");

   MessageBox(NULL,Buffer,"Diretorios",MB_OK);
}


void listarArquivos()
{

    TCHAR Buffer[BUFSIZE];
    GetCurrentDirectory(BUFSIZE, Buffer);

    cout << "LISTAR ARQUIVOS:\n";


   WIN32_FIND_DATA ffd;
   LARGE_INTEGER filesize;
   TCHAR szDir[MAX_PATH];
   TCHAR szDir2[MAX_PATH];
   size_t length_of_arg;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   DWORD dwError=0;

   strcpy(szDir, Buffer);
   strcat(szDir, TEXT("\\*"));


   hFind = FindFirstFile(szDir, &ffd);
   do
   {
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
          char file[MAX_PATH];
          strcat(szDir, ffd.cFileName);
          strcat(szDir, TEXT(" <DIR> \r\n"));
          //SetWindowText(hOut, ffd.cFileName);
         printf(TEXT("  %s   <DIR>\r\n"), ffd.cFileName);
      }
      else
      {
         strcat(szDir, ffd.cFileName);
         strcat(szDir, TEXT(" \r\n"));
         filesize.LowPart = ffd.nFileSizeLow;
         filesize.HighPart = ffd.nFileSizeHigh;
         //SetWindowText(hOut, ffd.cFileName);
         printf(TEXT("  %s   %ld bytes\r\n"), ffd.cFileName, filesize.QuadPart);
      }
   }
   while (FindNextFile(hFind, &ffd) != 0);
   SetWindowText(hOut, szDir);
   FindClose(hFind);
}

void mostrar(char *path)
{
    FILE *file;
    file = fopen(path,"rb");
    fseek(file,0,SEEK_END);
    int _size = ftell(file);
    rewind(file);
    char *data = new char(_size+1);
    fread(data,_size,1,file);
    data[_size] = '\0';

    cout << data;
    SetWindowText(hOut,data);
    MessageBox(NULL,data,"Leitura",MB_OK);
}

void lerArquivo()
{

    OPENFILENAME ofn;
    char file_name[100];

    ZeroMemory(&ofn,sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = "All files\0*.*\0Source Files\0*.CPP\0Text Files\0*.TXT\0";
    ofn.nFilterIndex = 1;

    GetOpenFileName(&ofn);
    mostrar(ofn.lpstrFile);
}


void mudarDiretorio()
{
  wchar_t filepath[MAX_PATH];

	TCHAR szDir[MAX_PATH];
    BROWSEINFO bInfo;
    bInfo.hwndOwner = NULL;
    bInfo.pidlRoot = NULL;
    bInfo.pszDisplayName = szDir;
    bInfo.lpszTitle = "Please, select a folder";
    bInfo.ulFlags = 0 ;
    bInfo.lpfn = NULL;
    bInfo.lParam = 0;
    bInfo.iImage = -1;

    LPITEMIDLIST lpItem = SHBrowseForFolder( &bInfo);
    if( lpItem != NULL )
    {
        SHGetPathFromIDList(lpItem, szDir );
        cout << szDir;
    }

    SetCurrentDirectory(szDir);
    listarArquivos();
    currentDirec();
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    WNDCLASSW wc  = {0};

    wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"myWindowClass";
    wc.lpfnWndProc = WindowProcedure;

    if(!RegisterClassW(&wc))
        return -1;

    CreateWindowW(L"myWindowClass",L"My Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE,100,100,500,500,NULL,NULL,NULL,NULL);

    MSG msg = {0};

    while( GetMessage(&msg,NULL,NULL,NULL) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}




LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch(msg)
    {
        case WM_COMMAND:
            switch(wp)
            {
                case GENERATE_BUTTON:
                    break;
                case READ_BUTTON:
                    lerArquivo();
                    break;
                case CREATE_BUTTON:
                    criarArquivoTmp(hWnd);
                    break;
                case DELETE_BUTTON:
                    deletarArquivo(hWnd);
                    break;
                case CD_BUTTON:
                    mudarDiretorio();
                    break;
            }

        case WM_CREATE:
            AddControls(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProcW(hWnd,msg,wp,lp);
        }
}





void AddControls(HWND hWnd)
{

    CreateWindowW(L"Button",L"Ler Arquivo", WS_VISIBLE | WS_CHILD, 100, 140, 110, 38, hWnd, (HMENU) READ_BUTTON, NULL, NULL);
    CreateWindowW(L"Button",L"Deletar Arquivo", WS_VISIBLE | WS_CHILD, 100, 80, 110, 38, hWnd, (HMENU) DELETE_BUTTON, NULL, NULL);
    CreateWindowW(L"Button",L"Mudar Diretorio", WS_VISIBLE | WS_CHILD, 290, 140, 110, 38, hWnd, (HMENU) CD_BUTTON, NULL, NULL);
    CreateWindowW(L"Button",L"Criar Arquivo", WS_VISIBLE | WS_CHILD, 290, 80, 110, 38, hWnd, (HMENU)CREATE_BUTTON, NULL, NULL);
    hOut = CreateWindowW(       L"EDIT", L"",
                                WS_VISIBLE | WS_CHILD |
                                ES_MULTILINE | WS_BORDER | ES_AUTOVSCROLL,
                                100, 200, 300, 200,
                                hWnd,
                                NULL,
                                NULL,
                                NULL);
}



