#include "../../proto_functions_server.h"
#include "pattern_scan.h"
#include "MinHook.h"
#include "hooks.h"
#include "Game.h"
#include "LocalPlayer.h"
#include "Thing.h"
#include "Container.h"
#include "Item.h"
#include <fstream>
#include <iomanip>
#include <sstream>

namespace {
    void appendLoaderLog(const std::string& line) {
        CreateDirectoryA("C:\\temp", nullptr);
        std::ofstream out("C:\\temp\\easybot_runtime.log", std::ios::app);
        if (!out.is_open()) return;
        out << line << std::endl;
    }

    std::string hexPtr(uintptr_t value) {
        std::ostringstream oss;
        oss << "0x" << std::hex << std::uppercase << value;
        return oss.str();
    }
}


DWORD WINAPI EasyBot(HMODULE hModule) {
    if (MH_Initialize() != MH_OK) {
        appendLoaderLog("[loader] MH_Initialize failed");
        return 0;
    }

    uintptr_t bindSingletonFunction_func = FindPattern(bindSingletonFunction_x86_PATTERN, bindSingletonFunction_x86_MASK);
    uintptr_t callGlobalField_func = FindPattern(callGlobalField_PATTERN, callGlobalField_MASK);
    uintptr_t mainLoop_func = FindPattern(mainLoop_x86_PATTERN, mainLoop_x86_MASK);

    appendLoaderLog("[loader] bindSingletonFunction=" + hexPtr(bindSingletonFunction_func));
    appendLoaderLog("[loader] callGlobalField=" + hexPtr(callGlobalField_func));
    appendLoaderLog("[loader] mainLoop=" + hexPtr(mainLoop_func));

    if (bindSingletonFunction_func) {
        MH_CreateHook(reinterpret_cast<LPVOID>(bindSingletonFunction_func), &hooked_bindSingletonFunction,
                      reinterpret_cast<LPVOID*>(&original_bindSingletonFunction));
    } else {
        appendLoaderLog("[loader] bindSingletonFunction pattern not found");
    }
    if (callGlobalField_func) {
        MH_CreateHook(reinterpret_cast<LPVOID>(callGlobalField_func), &hooked_callGlobalField,
                      reinterpret_cast<LPVOID*>(&original_callGlobalField));
    } else {
        appendLoaderLog("[loader] callGlobalField pattern not found");
    }
    if (mainLoop_func) {
        MH_CreateHook(reinterpret_cast<LPVOID>(mainLoop_func), &hooked_MainLoop,
                      reinterpret_cast<LPVOID*>(&original_mainLoop));
    } else {
        appendLoaderLog("[loader] mainLoop pattern not found");
    }
    /*
    FILE *f;
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "Singleton " << std::hex <<bindSingletonFunction_func << std::endl;
    std::cout << "Call global " << std::hex <<callGlobalField_func << std::endl;
    std::cout << "Main Loop " << std::hex << mainLoop_func << std::endl;
    */
    MH_EnableHook(MH_ALL_HOOKS);
    const auto startTick = GetTickCount();
    while (true) {
        auto it = SingletonFunctions.find("g_game.look");
        if (it != SingletonFunctions.end() && it->second.first) break;
        if (GetTickCount() - startTick > 20000) {
            appendLoaderLog("[loader] timeout waiting for g_game.look");
            break;
        }
        Sleep(10);
    }
    auto itLook = SingletonFunctions.find("g_game.look");
    if (itLook != SingletonFunctions.end() && itLook->second.first) {
        appendLoaderLog("[loader] g_game.look fn=" + hexPtr(itLook->second.first) + " this=" + hexPtr(itLook->second.second));
        appendLoaderLog("[loader] look hook disabled for compatibility");
    } else {
        appendLoaderLog("[loader] g_game.look not available");
    }
    MessageBoxA(NULL, "Bot is running - enjoy", "EasyBot", MB_OK | MB_TOPMOST);
    RunServer();
    return 0;
}


struct BotLoader {
    BotLoader() {
        CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(EasyBot), NULL, 0, NULL);
    }
};
static BotLoader loader;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}
