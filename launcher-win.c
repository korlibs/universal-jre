// tcc -luser32 -Wl,-subsystem=gui launcher-win.c

// resourcehacker -open ResourceHacker.exe -save savedicon.ico -action extract -mask ICONGROUP,MAINICON, -log CONSOLE
// resourcehacker -open launcher-win.exe -save launcher-win.exe -action addskip -res savedicon.ico  -mask ICONGROUP,MAINICON,
#define _WIN64 1
#include <stdio.h>
#include <stdlib.h>
#include <winapi/windows.h>
#include <winapi/winuser.h>
//#include <unistd.h>

int strlen_w(WCHAR *ptr, int maxLen) {
    for (int n = 0; n < maxLen; n++) {
        if (ptr[n] == 0) return n;
    }
    return maxLen;
}

WCHAR *strrchr_w(WCHAR *ptr, int c) {
    int maxLen = strlen_w(ptr, MAX_PATH);
    //printf("maxLen: %d\n", maxLen);
    for (int n = maxLen - 1; n >= 0; n--) {
        if (ptr[n] == c) {
            //printf("n: %d\n", n);
            return &ptr[n];
        }
    }
    return NULL;
}

int main() {
    WCHAR path[MAX_PATH] = {0};
    GetModuleFileNameW(NULL, path, MAX_PATH);
    //printf("%ls\n", path);
    WCHAR *ptr = strrchr_w(path, '\\');
    //printf("%ls\n", ptr);
    if (ptr) {
        *ptr = 0;
    }
    //printf("%ls\n", path);
    //printf("%d\n", SetCurrentDirectoryW(path));
    //system("jre\\bin\\javaw.exe -jar app.jar");

    STARTUPINFO info={sizeof(info)};
    PROCESS_INFORMATION processInfo;
    WCHAR *commandLine = L"jre\\bin\\javaw.exe -jar app.jar";
    if (CreateProcessW(
        NULL,
        commandLine,
        NULL, NULL, TRUE, 0, NULL, path, &info, &processInfo
    )) {
        WaitForSingleObject(processInfo.hProcess, INFINITE);
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    } else {
        MessageBoxW(NULL, commandLine, L"Error opening process", 0);
    }

    return 0;
}