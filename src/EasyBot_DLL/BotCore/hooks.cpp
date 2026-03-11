#include "hooks.h"
#include "BuildConfig.h"
#include "Creature.h"
#include "CustomFunctions.h"
#include "pattern_scan.h"
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>

namespace {
    std::mutex g_logMutex;

    struct LuaBindDescriptor {
        uintptr_t metadata;
        uintptr_t function;
        uintptr_t singleton;
    };

    std::string getRuntimeLogPath() {
        char tempPath[MAX_PATH] = {0};
        DWORD len = GetTempPathA(MAX_PATH, tempPath);
        if (len == 0 || len >= MAX_PATH) {
            return "easybot_runtime.log";
        }
        std::string path(tempPath);
        if (!path.empty() && path.back() != '\\' && path.back() != '/') {
            path.push_back('\\');
        }
        path += "easybot_runtime.log";
        return path;
    }

    void appendHookLog(const std::string& line) {
        std::lock_guard<std::mutex> lock(g_logMutex);
        std::ofstream out(getRuntimeLogPath(), std::ios::app);
        if (!out.is_open()) return;
        out << line << std::endl;
    }

    std::string hexPtr(uintptr_t value) {
        std::ostringstream oss;
        oss << "0x" << std::hex << std::uppercase << value;
        return oss.str();
    }

    bool isInMainModule(uintptr_t value) {
        if (!value) return false;
        MODULEINFO moduleInfo = {0};
        HMODULE hModule = GetModuleHandle(NULL);
        if (!hModule) return false;
        if (!GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo))) {
            return false;
        }
        const auto base = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
        const auto end = base + moduleInfo.SizeOfImage;
        return value >= base && value < end;
    }

    bool tryReadSingletonDescriptor(uintptr_t rawDescriptorPtr, uintptr_t& functionOut, uintptr_t& singletonOut) {
        if (!rawDescriptorPtr) return false;
        __try {
            const auto* descriptor = reinterpret_cast<const LuaBindDescriptor*>(rawDescriptorPtr);
            if (!isInMainModule(descriptor->function) || !descriptor->singleton) {
                return false;
            }
            functionOut = descriptor->function;
            singletonOut = descriptor->singleton;
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    bool tryReadClassDescriptor(uintptr_t rawDescriptorPtr, uintptr_t& functionOut) {
        if (!rawDescriptorPtr) return false;
        __try {
            const auto* descriptor = reinterpret_cast<const LuaBindDescriptor*>(rawDescriptorPtr);
            if (!isInMainModule(descriptor->function)) {
                return false;
            }
            functionOut = descriptor->function;
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    bool shouldUseSingletonDescriptor(const std::string& global, const std::string& field) {
        return
            (global == "g_map" && field == "findPath") ||
            (global == "g_game" && field == "getLocalPlayer") ||
            (global == "g_game" && field == "autoWalk");
    }

    bool shouldUseClassDescriptor(const std::string& owner, const std::string& field) {
        return owner == "LocalPlayer" &&
            (field == "autoWalk" || field == "stopAutoWalk" || field == "isAutoWalking");
    }
}


//static std::ofstream g_log("GameAddress.txt",  std::ios::trunc);
void __stdcall hooked_bindSingletonFunction(uintptr_t a1, uintptr_t a2, uintptr_t a3) {
    CONTEXT ctx;
    RtlCaptureContext(&ctx);
    uintptr_t ebp = ctx.Ebp;
    uintptr_t tmp = 0;
    auto global = *reinterpret_cast<std::string*>(a1);
    auto field = *reinterpret_cast<std::string*>(a2);
    if (global[1] != '_') {
        if (!shouldUseClassDescriptor(global, field) || !tryReadClassDescriptor(a3, tmp)) {
            tmp = *reinterpret_cast<uintptr_t*>(ebp + classFunctionOffset);
        }
        ClassMemberFunctions[std::string(global) + "." + std::string(field)]  = tmp;
        if (global == "LocalPlayer" &&
            (field == "autoWalk" || field == "stopAutoWalk" || field == "isAutoWalking")) {
            appendHookLog("[bind-class] " + global + "." + field +
                " fn=" + hexPtr(tmp) +
                " desc=" + hexPtr(a3) +
                " ebp=" + hexPtr(ebp));
        }
        /*
        g_log << "[Class Member Function] class: "<<  global << " function: " << field << " function_address: " << std::hex << tmp << std::endl;
        g_log.flush();
        */
    } else {
        uintptr_t second_tmp = 0;
        if (!shouldUseSingletonDescriptor(global, field) || !tryReadSingletonDescriptor(a3, tmp, second_tmp)) {
            tmp = *reinterpret_cast<uintptr_t*>(ebp + singletonFunctionOffset);
            second_tmp = *reinterpret_cast<uintptr_t*>(ebp + singletonFunctionOffset + 0x04);
        }
        SingletonFunctions[std::string(global) + "." + std::string(field)]  = {tmp, second_tmp};
        if ((global == "g_game" && (field == "walk" || field == "autoWalk" || field == "look" || field == "getLocalPlayer")) ||
            (global == "g_map" && field == "findPath")) {
            appendHookLog("[bind] " + global + "." + field +
                " fn=" + hexPtr(tmp) +
                " this=" + hexPtr(second_tmp) +
                " desc=" + hexPtr(a3) +
                " ebp=" + hexPtr(ebp) +
                " off_fn=0x" + hexPtr(singletonFunctionOffset).substr(2) +
                " off_this=0x" + hexPtr(singletonFunctionOffset + 0x04).substr(2));
        }
        /*
        g_log << "[Singleton Function] class: " << global << " function: " << field <<
        " function_address: " << std::hex << tmp<< " second_param: " << std::hex << second_tmp << std::endl;
        g_log.flush();
        */
    }
    original_bindSingletonFunction(a1,a2,a3);
}

void __stdcall hooked_callLuaField(uintptr_t* a1) {
    original_callLuaField(a1);
}

inline uint32_t itemId;

#pragma optimize( "", off )
void __stdcall hooked_callGlobalField(uintptr_t **a1, uintptr_t **a2) {
    CONTEXT ctx;
    RtlCaptureContext(&ctx);
    uintptr_t ebp = ctx.Ebp;
    auto global = *reinterpret_cast<std::string*>(a1);
    auto field = *reinterpret_cast<std::string*>(a2);
    if (global == "g_game") {
        if (field == "onTextMessage") {
            uintptr_t onTextMessage_address = ebp + onTextMessageOffset;
            auto ptr_messageText = g_custom->getMessagePtr(onTextMessage_address);
            auto message_address = reinterpret_cast<std::string*>(ptr_messageText);
            if (message_address->find("You see") != std::string::npos)
            {
                *message_address = "ID: " + std::to_string(itemId) + "\n" + *reinterpret_cast<std::string*>(ptr_messageText);
            }
        } else if (field == "onTalk") {
            auto args = reinterpret_cast<StackArgs*>(ebp + onTalkOffset);
            g_custom->onTalk(
                *args->name,
                reinterpret_cast<uint16_t>(args->level),
                *args->mode,
                *args->text,
                *args->channelId,
                *args->pos
            );
        }
    }
    original_callGlobalField(a1, a2);
}

#pragma optimize( "", on )

int hooked_MainLoop(int a1) {
    g_dispatcher->executeEvent();
    auto result = original_mainLoop(a1);
    return result;
}

typedef uint32_t(gameCall* GetId)(
    uintptr_t RCX,
    void *RDX
    );


void __stdcall hooked_Look(const uintptr_t& thing, const bool isBattleList) {
    look_original(&thing, isBattleList);
    auto function = reinterpret_cast<GetId>(ClassMemberFunctions["Item.getId"]);
    void* pMysteryPtr = nullptr;
    itemId = function(thing, &pMysteryPtr);

}
