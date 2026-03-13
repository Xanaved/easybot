#include "Creature.h"
#include <algorithm>
#include <cstdlib>
#include "Game.h"
#include "LocalPlayer.h"
#include "Map.h"
#include "Thing.h"


Creature* Creature::instance{nullptr};
std::mutex Creature::mutex;

Creature* Creature::getInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new Creature();
    }
    return instance;
}

std::string Creature::getName(CreaturePtr creature) {
    if (!creature) return "";
    typedef void(gameCall* GetName)(
        uintptr_t RCX,
        std::string *RDX
        );
    auto function = reinterpret_cast<GetName>(ClassMemberFunctions["Creature.getName"]);
    if (!function) return "";
    return g_dispatcher->scheduleEventEx([function, creature]() {
        std::string result;
        function(creature, &result);
        return result;
    });
}

uint8_t Creature::getManaPercent(CreaturePtr creature) {
    if (!creature) return 0;
    typedef uint8_t(gameCall* GetManaPercent)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetManaPercent>(ClassMemberFunctions["Creature.getManaPercent"]);
    if (!function) {
        if (!g_thing->isLocalPlayer(creature)) return 0;
        const auto localPlayer = static_cast<LocalPlayerPtr>(creature);
        const auto maxMana = g_localPlayer->getMaxMana(localPlayer);
        if (maxMana <= 0.0) return 0;
        const auto mana = g_localPlayer->getMana(localPlayer);
        const auto percent = static_cast<int>((mana / maxMana) * 100.0);
        return static_cast<uint8_t>(std::clamp(percent, 0, 100));
    }
    return g_dispatcher->scheduleEventEx([function, creature]() {
        void* pMysteryPtr = nullptr;
        return function(creature, &pMysteryPtr);
    });
}

uint8_t Creature::getHealthPercent(CreaturePtr creature) {
    if (!creature) return 0;
    typedef uint8_t(gameCall* GetHealthPercent)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetHealthPercent>(ClassMemberFunctions["Creature.getHealthPercent"]);
    if (!function) return 0;
    return g_dispatcher->scheduleEventEx([function, creature]() {
        void* pMysteryPtr = nullptr;
        return function(creature, &pMysteryPtr);
    });
}

Otc::PlayerSkulls Creature::getSkull(CreaturePtr creature) {
    if (!creature) return {};
    typedef Otc::PlayerSkulls(gameCall* GetSkull)(
        uintptr_t RCX
        );
    auto function = reinterpret_cast<GetSkull>(ClassMemberFunctions["Creature.getSkull"]);
    if (!function) return {};
    return g_dispatcher->scheduleEventEx([function, creature]() {
            return function(creature);
    });
}

Otc::Direction Creature::getDirection(CreaturePtr creature) {
    if (!creature) return {};
    typedef Otc::Direction(gameCall* GetDirection)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetDirection>(ClassMemberFunctions["Creature.getDirection"]);
    if (!function) return {};
    return g_dispatcher->scheduleEventEx([function, creature]() {
            void* pMysteryPtr = nullptr;
            return function(creature, &pMysteryPtr);
    });
}

bool Creature::isDead(CreaturePtr creature) {
    if (!creature) return 0;
    typedef bool(gameCall* IsDead)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<IsDead>(ClassMemberFunctions["Creature.isDead"]);
    if (!function) return 0;
    return g_dispatcher->scheduleEventEx([function, creature]() {
            void* pMysteryPtr = nullptr;
            return function(creature, &pMysteryPtr);
    });
}

bool Creature::isWalking(CreaturePtr creature) {
    if (!creature) return 0;
    typedef bool(gameCall* IsWalking)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<IsWalking>(ClassMemberFunctions["Creature.isWalking"]);
    if (!function) return 0;
    return g_dispatcher->scheduleEventEx([function, creature]() {
            void* pMysteryPtr = nullptr;
            return function(creature, &pMysteryPtr);
    });
}

bool Creature::canBeSeen(CreaturePtr creature) {
    if (!creature) return 0;
    typedef bool(gameCall* CanBeSeen)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<CanBeSeen>(ClassMemberFunctions["Creature.canBeSeen"]);
    if (!function) return 0;
    return g_dispatcher->scheduleEventEx([function, creature]() {
            void* pMysteryPtr = nullptr;
            return function(creature, &pMysteryPtr);
    });
}

bool Creature::isCovered(CreaturePtr creature) {
    if (!creature) return 0;
    typedef bool(gameCall* IsCovered)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<IsCovered>(ClassMemberFunctions["Creature.isCovered"]);
    if (!function) return 0;
    return g_dispatcher->scheduleEventEx([function, creature]() {
            void* pMysteryPtr = nullptr;
            return function(creature, &pMysteryPtr);
    });
}

bool Creature::canShoot(CreaturePtr creature, int distance) {
    if (!creature) return 0;
    typedef bool(gameCall* CanShoot)(
        uintptr_t RCX,
        void *RDX,
        int distance
        );
    auto function = reinterpret_cast<CanShoot>(ClassMemberFunctions["Creature.canShoot"]);
    if (!function) {
        const auto localPlayer = g_game->getLocalPlayer();
        if (!localPlayer) return true;
        const auto fromPos = g_thing->getPosition(localPlayer);
        const auto toPos = g_thing->getPosition(creature);
        if (fromPos.z != toPos.z) return false;
        const auto deltaX = std::abs(fromPos.x - toPos.x);
        const auto deltaY = std::abs(fromPos.y - toPos.y);
        if ((std::max)(deltaX, deltaY) > distance) return false;
        if (ClassMemberFunctions["LocalPlayer.hasSight"]) {
            return g_localPlayer->hasSight(localPlayer, toPos);
        }
        if (SingletonFunctions["g_map.isSightClear"].first && SingletonFunctions["g_map.isSightClear"].second) {
            return g_map->isSightClear(fromPos, toPos);
        }
        return true;
    }
    return g_dispatcher->scheduleEventEx([function, creature, distance]() {
            void* pMysteryPtr = nullptr;
            return function(creature, &pMysteryPtr, distance);
    });
}
