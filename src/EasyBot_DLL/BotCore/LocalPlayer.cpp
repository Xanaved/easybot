#include "LocalPlayer.h"
#include <fstream>
#include <mutex>
#include <sstream>

namespace {
    std::mutex g_localPlayerLogMutex;

    std::string localPlayerHex(uintptr_t value) {
        std::ostringstream oss;
        oss << "0x" << std::hex << std::uppercase << value;
        return oss.str();
    }

    std::string localPlayerPos(const Position& pos) {
        std::ostringstream oss;
        oss << "(" << pos.x << "," << pos.y << "," << pos.z << ")";
        return oss.str();
    }

    void appendLocalPlayerLog(const std::string& line) {
        std::lock_guard<std::mutex> lock(g_localPlayerLogMutex);
        char tempPath[MAX_PATH] = {0};
        DWORD len = GetTempPathA(MAX_PATH, tempPath);
        std::string path = (len > 0 && len < MAX_PATH) ? std::string(tempPath) : std::string();
        if (!path.empty() && path.back() != '\\' && path.back() != '/') {
            path.push_back('\\');
        }
        path += "easybot_runtime.log";
        std::ofstream out(path, std::ios::app);
        if (!out.is_open()) return;
        out << line << std::endl;
    }
}

LocalPlayer* LocalPlayer::instance{nullptr};
std::mutex LocalPlayer::mutex;


LocalPlayer* LocalPlayer::getInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new LocalPlayer();
    }
    return instance;
}

Otc::PlayerStates LocalPlayer::getStates(LocalPlayerPtr localPlayer) {
    if (!localPlayer) return Otc::IconNone;
    typedef Otc::PlayerStates(gameCall* GetStates)(
        uintptr_t RCX
        );
    auto function = reinterpret_cast<GetStates>(ClassMemberFunctions["LocalPlayer.getStates"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer]() {
        return function(localPlayer);
    });
}

double LocalPlayer::getHealth(LocalPlayerPtr localPlayer) {
    if (!localPlayer) return 0;
    typedef double(gameCall* GetHealth)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetHealth>(ClassMemberFunctions["LocalPlayer.getHealth"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer]() {
        void* pMysteryPtr = nullptr;
        return function(localPlayer, &pMysteryPtr);
    });
}

double LocalPlayer::getMaxHealth(LocalPlayerPtr localPlayer) {
    if (!localPlayer) return 0;
    typedef double(gameCall* GetMaxHealth)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetMaxHealth>(ClassMemberFunctions["LocalPlayer.getMaxHealth"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer]() {
        void* pMysteryPtr = nullptr;
        return function(localPlayer, &pMysteryPtr);
    });
}

double LocalPlayer::getFreeCapacity(LocalPlayerPtr localPlayer) {
    if (!localPlayer) return 0;
    typedef double(gameCall* GetFreeCapacity)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetFreeCapacity>(ClassMemberFunctions["LocalPlayer.getFreeCapacity"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer]() {
        void* pMysteryPtr = nullptr;
        return function(localPlayer, &pMysteryPtr);
    });
}

uint16_t LocalPlayer::getLevel(LocalPlayerPtr localPlayer) {
    if (!localPlayer) return 0;
    typedef uint16_t(gameCall* GetLevel)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetLevel>(ClassMemberFunctions["LocalPlayer.getLevel"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer]() {
        void* pMysteryPtr = nullptr;
        return function(localPlayer, &pMysteryPtr);
    });
}

double LocalPlayer::getMana(LocalPlayerPtr localPlayer) {
    if (!localPlayer) return 0;
    typedef double(gameCall* GetMana)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetMana>(ClassMemberFunctions["LocalPlayer.getMana"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer]() {
        void* pMysteryPtr = nullptr;
        return function(localPlayer, &pMysteryPtr);
    });
}

double LocalPlayer::getMaxMana(LocalPlayerPtr localPlayer) {
    if (!localPlayer) return 0;
    typedef double(gameCall* GetMaxMana)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetMaxMana>(ClassMemberFunctions["LocalPlayer.getMaxMana"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer]() {
        void* pMysteryPtr = nullptr;
        return function(localPlayer, &pMysteryPtr);
    });
}

uint8_t LocalPlayer::getSoul(LocalPlayerPtr localPlayer) {
    if (!localPlayer) return 0;
    typedef uint8_t(gameCall* GetSoul)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetSoul>(ClassMemberFunctions["LocalPlayer.getSoul"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer]() {
        void* pMysteryPtr = nullptr;
        return function(localPlayer, &pMysteryPtr);
    });
}

uint16_t LocalPlayer::getStamina(LocalPlayerPtr localPlayer) {
    if (!localPlayer) return 0;
    typedef uint16_t(gameCall* GetStamina)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetStamina>(ClassMemberFunctions["LocalPlayer.getStamina"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer]() {
        void* pMysteryPtr = nullptr;
        return function(localPlayer, &pMysteryPtr);
    });
}

ItemPtr LocalPlayer::getInventoryItem(LocalPlayerPtr localPlayer, Otc::InventorySlot inventorySlot) {
    if (!localPlayer) return 0;
    typedef void(gameCall* GetInventoryItem)(
        uintptr_t RCX,
        ItemPtr *RDX,
        Otc::InventorySlot inventorySlot
        );
    auto function = reinterpret_cast<GetInventoryItem>(ClassMemberFunctions["LocalPlayer.getInventoryItem"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer, inventorySlot]() {
        ItemPtr result;
        function(localPlayer, &result, inventorySlot);
        return result;
    });
}

int LocalPlayer::getInventoryCount(LocalPlayerPtr localPlayer, uint16_t itemId, int tier) {
    if (!localPlayer) return 0;
    typedef int(gameCall* GetInventoryCount)(
        uintptr_t RCX,
        void *RDX,
        uint16_t itemId,
        int tier
        );
    auto function = reinterpret_cast<GetInventoryCount>(ClassMemberFunctions["LocalPlayer.getInventoryCount"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer, itemId, tier]() {
        void* pMysteryPtr = nullptr;
        return function(localPlayer, &pMysteryPtr, itemId, tier);
    });
}

bool LocalPlayer::hasSight(LocalPlayerPtr localPlayer, const Position &pos) {
    if (!localPlayer) return 0;
    typedef bool(gameCall* HasSight)(
        uintptr_t RCX,
        void *RDX,
        const Position *pos
        );
    auto function = reinterpret_cast<HasSight>(ClassMemberFunctions["LocalPlayer.hasSight"]);
    return g_dispatcher->scheduleEventEx([function, localPlayer, pos]() {
        void* pMysteryPtr = nullptr;
        return function(localPlayer, &pMysteryPtr, &pos);
    });
}

bool LocalPlayer::isAutoWalking(LocalPlayerPtr localPlayer) {
    if (!localPlayer) return 0;
    const auto it = ClassMemberFunctions.find("LocalPlayer.isAutoWalking");
    if (it == ClassMemberFunctions.end() || !it->second) return 0;
    typedef bool(gameCall* IsAutoWalking)(
        uintptr_t RCX
        );
    auto function = reinterpret_cast<IsAutoWalking>(it->second);
    return g_dispatcher->scheduleEventEx([function, localPlayer]() {
        const auto result = function(localPlayer);
        appendLocalPlayerLog("[call] LocalPlayer.isAutoWalking fn=" +
            localPlayerHex(reinterpret_cast<uintptr_t>(function)) +
            " this=" + localPlayerHex(localPlayer) +
            " result=" + (result ? "true" : "false"));
        return result;
    });
}

void LocalPlayer::stopAutoWalk(LocalPlayerPtr localPlayer) {
    if (!localPlayer) return;
    const auto it = ClassMemberFunctions.find("LocalPlayer.stopAutoWalk");
    if (it == ClassMemberFunctions.end() || !it->second) return;
    typedef void(gameCall* StopAutoWalk)(
        uintptr_t RCX
        );
    auto function = reinterpret_cast<StopAutoWalk>(it->second);
    return g_dispatcher->scheduleEventEx([function, localPlayer]() {
        appendLocalPlayerLog("[call] LocalPlayer.stopAutoWalk fn=" +
            localPlayerHex(reinterpret_cast<uintptr_t>(function)) +
            " this=" + localPlayerHex(localPlayer));
        return function(localPlayer);
    });
}

bool LocalPlayer::autoWalk(LocalPlayerPtr localPlayer, const Position &destination, bool retry) {
    if (!localPlayer) return 0;
    const auto it = ClassMemberFunctions.find("LocalPlayer.autoWalk");
    if (it == ClassMemberFunctions.end() || !it->second) return 0;
    typedef bool(gameCall* AutoWalk)(
        uintptr_t RCX,
        const Position *destination
        );
    auto function = reinterpret_cast<AutoWalk>(it->second);
    return g_dispatcher->scheduleEventEx([function, localPlayer, destination, retry]() {
        appendLocalPlayerLog("[call] LocalPlayer.autoWalk.enter fn=" +
            localPlayerHex(reinterpret_cast<uintptr_t>(function)) +
            " this=" + localPlayerHex(localPlayer) +
            " destination=" + localPlayerPos(destination) +
            " retry=" + (retry ? "true" : "false"));
        const auto result = function(localPlayer, &destination);
        appendLocalPlayerLog("[call] LocalPlayer.autoWalk.leave result=" + std::string(result ? "true" : "false"));
        return result;
    });
}

void LocalPlayer::setLightHack(LocalPlayerPtr localPlayer, uint16_t lightLevel) {
    if (!localPlayer) return;
    g_dispatcher->scheduleEventEx([localPlayer, lightLevel]() {
        auto lightPtr = reinterpret_cast<uint16_t*>(localPlayer + lightHackOffset);
        if (*lightPtr != lightLevel) *lightPtr = lightLevel;
    });
}
