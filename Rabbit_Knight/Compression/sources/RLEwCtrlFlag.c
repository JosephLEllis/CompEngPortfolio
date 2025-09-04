#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define IDC_INPUT   101
#define IDC_OUTPUT  102
#define IDC_BUTTON  103
#define IDC_CLEAR   104
#define IDC_CHECKBOX 105
#define IDC_REVERSE 106 

#define MAX_ROWS 256 
#define MAX_COLS 256

#include "../resources/resources.h"

const char g_szClassName[] = "myWindowClass";

// Parse DB $xx,$yy,$zz style into bytes
int parseInput(const char* text, unsigned char* bytes, int maxBytes) {
    int count = 0;
    const char* p = text;

    while (*p && count < maxBytes) {
        if (*p == '$') {
            p++;
            unsigned int val;
            if (sscanf(p, "%x", &val) == 1) {
                bytes[count++] = (unsigned char)val;
            }
        }
        p++;
    }
    return count;
}

void RLECtrl(const unsigned char *input, int input_len, unsigned char *output, int *out_len, BOOL rev) {
    int in, out; 

    if(rev){
        in = input_len - 1, out = 0;

    } else {
        in = 0, out = 0;
    }

    if(rev){
         while (in >= 0) {
        int run_len = 1;

        // Repeat run (scanning backwards)
        while (in - run_len >= 0 && input[in] == input[in - run_len] && run_len < 128) {
            run_len++;
        }

        if (run_len >= 2) {
            // Encode repeat run
            output[out++] = 0x80 | (run_len - 1);
            output[out++] = input[in];
            in -= run_len;
        } else {
            // Literal run
            int lit_end = in;
            int lit_len = 1;
            in--;

            while (lit_len < 128 && in >= 0) {
                if (in - 1 >= 0 && input[in] == input[in - 1]) break;
                lit_len++;
                in--;
            }

            output[out++] = lit_len - 1;
            // Copy literals in 
            for (int j = 0; j < lit_len; ++j) {
                output[out++] = input[lit_end - j];
            }
        }
    }

    } else {

        while (in < input_len) {
        int run_len = 1;

        // Repeat run
        while (in + run_len < input_len && input[in] == input[in + run_len] && run_len < 128) {
            run_len++;
        }

        if (run_len >= 2) {
            // Encode repeat run
            output[out++] = 0x80 | (run_len - 1);
            output[out++] = input[in];
            in += run_len;
        } else {
            // Literal run
            int lit_start = in;
            int lit_len = 1;
            in++;

            while (lit_len < 128 && in < input_len) {
                if (in + 1 < input_len && input[in] == input[in + 1]) break;
                lit_len++;
                in++;
            }

            output[out++] = lit_len - 1;
            for (int j = 0; j < lit_len; ++j) {
                output[out++] = input[lit_start + j];
            }
        }
    }

    }

    *out_len = out;
}

// Continuous RLE with max run length of 255
int RLE(const unsigned char* data, int len, unsigned char* out, int maxOut) {
    int i = 0, outPos = 0;
    while (i < len && outPos + 2 <= maxOut) {
        unsigned char val = data[i];
        int run = 1;
        while (i + run < len && data[i + run] == val && run < 255) {
            run++;
        }
        out[outPos++] = (unsigned char)run;
        out[outPos++] = val;
        i += run;
    }
    return outPos;
}

// Output one DB line with all runs
void formatRLE(const unsigned char* data, int len, char* outText, int maxLen) {
    char buffer[32];
    outText[0] = '\0';

    int bytesOnLine = 0;
    for (int i = 0; i < len; i += 2) {
        // Start a new DB line if first entry or after 10 bytes
        if (bytesOnLine == 0) {
            if (strlen(outText) > 0) {  // not the first line
                strncat(outText, "\r\n", maxLen - strlen(outText) - 1);
            }
            strncat(outText, "DB ", maxLen - strlen(outText) - 1);
        }

        snprintf(buffer, sizeof(buffer), "$%02X, $%02X", data[i], data[i+1]);
        strncat(outText, buffer, maxLen - strlen(outText) - 1);

        bytesOnLine += 2;
        if (i + 2 < len) {
            strncat(outText, ", ", maxLen - strlen(outText));
        }

        // Reset after 10 bytes (5 pairs)
        if (bytesOnLine >= 10) {
            bytesOnLine = 0;
        }
    }
}

// Append text safely
static void appendText(char* dest, const char* src, int maxLen) {
    strncat(dest, src, maxLen - strlen(dest) - 1);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
                10, 10, 300, 100, hwnd, (HMENU)IDC_INPUT, GetModuleHandle(NULL), NULL);

            CreateWindow("BUTTON", "Process",
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                10, 120, 100, 30, hwnd, (HMENU)IDC_BUTTON, GetModuleHandle(NULL), NULL);

            CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY,
                10, 160, 300, 100, hwnd, (HMENU)IDC_OUTPUT, GetModuleHandle(NULL), NULL);

            CreateWindow("BUTTON", "Clear",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                120, 120, 100, 30, hwnd, (HMENU)IDC_CLEAR, GetModuleHandle(NULL), NULL);

            CreateWindow(
            "BUTTON", 
            "Use RLE with Ctrl Flag", 
            WS_CHILD | WS_VISIBLE | BS_CHECKBOX | WS_TABSTOP,
            10, 270, 200, 20, 
            hwnd, (HMENU)IDC_CHECKBOX, 
            GetModuleHandle(NULL), NULL);

            CreateWindow(
            "BUTTON", 
            "Reverse Ctrl Flag output", 
            WS_CHILD | WS_VISIBLE | BS_CHECKBOX | WS_TABSTOP,
            10, 250, 200, 20, 
            hwnd, (HMENU)IDC_REVERSE, 
            GetModuleHandle(NULL), NULL); 
            break;

        case WM_COMMAND:

           switch(LOWORD(wParam)){
               
               case IDC_BUTTON: {
                
                BOOL isChecked = IsDlgButtonChecked(hwnd, IDC_CHECKBOX);
                BOOL useReverse = IsDlgButtonChecked(hwnd, IDC_REVERSE); 
                BOOL useAlternate = IsDlgButtonChecked(hwnd, IDC_CHECKBOX) == BST_CHECKED;

                char input[4096];
                char outputText[4096];
                unsigned char bytes[2048];
                unsigned char rleData[2048];

                int rleCount = 0; 

                GetWindowText(GetDlgItem(hwnd, IDC_INPUT), input, sizeof(input));

                int byteCount = parseInput(input, bytes, sizeof(bytes));

                if (useAlternate) {
                   
                    // R:E with Ctrl flag 
                   RLECtrl(bytes, byteCount, rleData, &rleCount, useReverse);
                } else {
                   
                   // Standard RLE compression 
                   rleCount = RLE(bytes, byteCount, rleData, sizeof(rleData));
                }

                formatRLE(rleData, rleCount, outputText, sizeof(outputText));

                // Append statistics
                char stats[256];
                double ratio = byteCount > 0 ? ((double)(byteCount - rleCount) / (double)byteCount) * 100.0 : 0.0;
                
                snprintf(stats, sizeof(stats), "\r\n\r\nOriginal size: %d bytes\r\nCompressed size: %d bytes\r\nCompression efficiency: %.2f%%\r\nisChecked: %s", 
                byteCount, rleCount, ratio, isChecked ? "true" : "false");
                
                strncat(outputText, stats, sizeof(outputText) - strlen(outputText) - 1);

                SetWindowText(GetDlgItem(hwnd, IDC_OUTPUT), outputText);
               }
               break; 

            case ID_HELP_GO: 
               WinHelp(hwnd, "HLPFiles/Test.hlp", HELP_CONTENTS, 0);
               break; 

            case IDC_CHECKBOX:
                if (IsDlgButtonChecked(hwnd, IDC_CHECKBOX) == BST_CHECKED){
                   CheckDlgButton(hwnd, IDC_CHECKBOX, BST_UNCHECKED);
                } else {
                   CheckDlgButton(hwnd, IDC_CHECKBOX, BST_CHECKED);
                }
                break;

            case IDC_REVERSE:
                if (IsDlgButtonChecked(hwnd, IDC_REVERSE) == BST_CHECKED) {
                   CheckDlgButton(hwnd, IDC_REVERSE, BST_UNCHECKED);
                } else {
                   CheckDlgButton(hwnd, IDC_REVERSE, BST_CHECKED);
                }
                break; 

            case ID_FILE_EXIT: 
                PostQuitMessage(0);
                break;
            }

            break; 

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon  = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
    wc.hIconSm  = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, 0);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MYMENU);
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "RLE Compression Tool",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 400,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}