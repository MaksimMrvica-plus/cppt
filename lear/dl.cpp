#include <windows.h>
#include <iostream>

// 定义 GetMonitorInfoA 函数的原型
typedef BOOL (WINAPI *GetMonitorInfoAProc)(HMONITOR, LPMONITORINFO);

int main() {
    // 加载 kernel32.dll
    std::string path = R"(C:\Windows\System32\user32.dll)";
    HMODULE hKernel32 = LoadLibraryA(path.c_str());
    if (!hKernel32) {
        std::cerr << "Failed to load kernel32.dll" << std::endl;
        return -1;
    }

    // 获取 GetMonitorInfoA 函数的地址
    GetMonitorInfoAProc GetMonitorInfoA = (GetMonitorInfoAProc)GetProcAddress(hKernel32, "GetMonitorInfoA");
    if (!GetMonitorInfoA) {
        std::cerr << "Failed to get address of GetMonitorInfoA" << std::endl;
        FreeLibrary(hKernel32);
        return -1;
    }

    // 使用 GetMonitorInfoA 函数
    MONITORINFO mi = {sizeof(MONITORINFO)};
    HMONITOR hMonitor = MonitorFromWind
    ow(GetConsoleWindow(), MONITOR_DEFAULTTONEAREST);
    if (GetMonitorInfoA(hMonitor, &mi)) {
        std::cout << "Monitor Info:" << std::endl;
        std::cout << "Monitor Name: " << "SUMSANG" << std::endl;
        std::cout << "Monitor Rect: " << mi.rcMonitor.left << ", " << mi.rcMonitor.top << ", " 
                  << mi.rcMonitor.right << ", " << mi.rcMonitor.bottom << std::endl;
        std::cout << "Work Rect: " << mi.rcWork.left << ", " << mi.rcWork.top << ", " 
                  << mi.rcWork.right << ", " << mi.rcWork.bottom << std::endl;
    } else {
        std::cerr << "Failed to get monitor info" << std::endl;
    }

    // 释放 kernel32.dll
    FreeLibrary(hKernel32);

    return 0;
}