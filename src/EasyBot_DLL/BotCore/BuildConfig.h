#ifndef BUILDCONFIG_H
#define BUILDCONFIG_H
#include <Windows.h>

#ifdef _WIN64
    #define gameCall __fastcall
#else
    #define gameCall __thiscall
#endif

// Patterns
static const BYTE* bindSingletonFunction_x86_PATTERN = reinterpret_cast<const BYTE*>("\x55\x8b\xec\x8b\x45\x00\x83\x78\x00\x00\x00\x00\x8b\x00\x50\x68\x00\x00\x00\x00\xff\x35\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x83\xc4");
static LPCSTR bindSingletonFunction_x86_MASK = "xxxxx?xx????xxxx????xx????x????xx";

static const BYTE* mainLoop_x86_PATTERN = reinterpret_cast<const BYTE*>("\x8b\x54\x24\x00\x8b\x00\x24\x00\x53\x56");
static LPCSTR mainLoop_x86_MASK = "xxx?x?x?xx";


// Enum for building
enum {
    Miracle = 1,
    Realera,
    Dbwots,
    Outcast,
    Thirus,
    Exordion,
    Yurevo,
    Dbl,
    Treasura
};
#define BUILD_MIRACLE 1
#define BUILD_REALERA 2
#define BUILD_DBWOTS  3
#define BUILD_OUTCAST 4
#define BUILD_THIRUS  5
#define BUILD_EXORDION  6
#define BUILD_YUREVO 7
#define BUILD_DBL 8
#define BUILD_TREASURA 9



#define BuildOption BUILD_DBWOTS
#if BuildOption == BUILD_MIRACLE
    static const BYTE* callGlobalField_PATTERN = reinterpret_cast<const BYTE*>("\x55\x8b\xec\x8b\x45\x00\x83\x78\x00\x00\x00\x00\x8b\x00\x50\x68\x00\x00\x00\x00\xff\x35\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x6a");
    static LPCSTR callGlobalField_MASK = "xxxxx?xx????xxxx????xx????x?????";
    #define classFunctionOffset 0x50
    #define singletonFunctionOffset 0x10
    #define onTextMessageOffset 0x14
    #define onTalkOffset 0x10
    #define lightHackOffset 0xAC
#elif BuildOption == BUILD_REALERA
    static const BYTE* callGlobalField_PATTERN = reinterpret_cast<const BYTE*>("\x55\x8b\xec\x8b\x45\x00\x83\x78\x00\x00\x00\x00\x8b\x00\x50\x68\x00\x00\x00\x00\xff\x35\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x6a");
    static LPCSTR callGlobalField_MASK = "xxxxx?xx????xxxx????xx????x?????";
    #define classFunctionOffset 0xC
    #define singletonFunctionOffset 0x10
    #define onTextMessageOffset 0x14
    #define onTalkOffset 0x10
    #define lightHackOffset 0xAC
#elif BuildOption == BUILD_DBWOTS
    static const BYTE* callGlobalField_PATTERN = reinterpret_cast<const BYTE*>("\x55\x8b\xec\x8b\x45\x00\x83\x78\x00\x00\x00\x00\x8b\x00\x50\x68\x00\x00\x00\x00\xff\x35\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x6a");
    static LPCSTR callGlobalField_MASK = "xxxxx?xx????xxxx????xx????x?????";
    #define classFunctionOffset 0x50
    #define singletonFunctionOffset 0x10
    #define onTextMessageOffset 0x14
    #define onTalkOffset 0x10
    #define lightHackOffset 0xAC
    // DBW client RVAs confirmed from live client binding scan.
    #define dbw_g_game_singleton_rva 0x47CF60
    #define dbw_g_map_singleton_rva 0x47D080
    #define dbw_g_game_getLocalPlayer_rva 0x029530
    #define dbw_g_game_autoWalk_rva 0x22C990
    #define dbw_g_game_walk_rva 0x22D1A0
    #define dbw_g_game_look_rva 0x22BE00
    #define dbw_g_map_findPath_rva 0x256950
    #define dbw_localPlayer_isAutoWalking_rva 0x0343D0
    #define dbw_localPlayer_autoWalk_rva 0x23D070
    #define dbw_localPlayer_stopAutoWalk_rva 0x23E230
#elif BuildOption == BUILD_EXORDION
    static const BYTE* callGlobalField_PATTERN = reinterpret_cast<const BYTE*>("\x55\x8b\xec\x8b\x45\x00\x83\x78\x00\x00\x00\x00\x8b\x00\x50\x68\x00\x00\x00\x00\xff\x35\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x6a");
    static LPCSTR callGlobalField_MASK = "xxxxx?xx????xxxx????xx????x?????";
    #define classFunctionOffset 0x58
    #define singletonFunctionOffset 0x10
    #define onTextMessageOffset 0x14
    #define onTalkOffset 0x10
    #define lightHackOffset 0xB0
#elif BuildOption == BUILD_YUREVO
    static const BYTE* callGlobalField_PATTERN = reinterpret_cast<const BYTE*>("\x55\x8b\xec\x8b\x45\x00\x83\x78\x00\x00\x00\x00\x8b\x00\x50\x68\x00\x00\x00\x00\xff\x35\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x6a");
    static LPCSTR callGlobalField_MASK = "xxxxx?xx????xxxx????xx????x?????";
    #define classFunctionOffset 0x50
    #define singletonFunctionOffset 0x10
    #define onTextMessageOffset 0x14
    #define onTalkOffset 0x10
    #define lightHackOffset 0xAC
#elif BuildOption == BUILD_TREASURA
    static const BYTE* callGlobalField_PATTERN = reinterpret_cast<const BYTE*>("\x55\x8b\xec\x8b\x45\x00\x83\x78\x00\x00\x00\x00\x8b\x00\x50\x68\x00\x00\x00\x00\xff\x35\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x6a");
    static LPCSTR callGlobalField_MASK = "xxxxx?xx????xxxx????xx????x?????";
    #define classFunctionOffset 0x50
    #define singletonFunctionOffset 0x10
    #define onTextMessageOffset 0x14
    #define onTalkOffset 0x10
    #define lightHackOffset 0xAC
#elif BuildOption == BUILD_DBL
    static const BYTE* callGlobalField_PATTERN = reinterpret_cast<const BYTE*>("\x55\x8b\xec\x8b\x45\x00\x83\x78\x00\x00\x00\x00\x8b\x00\x50\x68\x00\x00\x00\x00\xff\x35\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x6a");
    static LPCSTR callGlobalField_MASK = "xxxxx?xx????xxxx????xx????x?????";
    #define classFunctionOffset 0x50
    #define singletonFunctionOffset 0x10
    #define onTextMessageOffset 0x14
    #define onTalkOffset 0x10
    #define lightHackOffset 0xAC
#endif






#endif //BUILDCONFIG_H
