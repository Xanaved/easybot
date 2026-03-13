#include "Game.h"
#include <fstream>
#include <mutex>
#include <sstream>

namespace {
    std::mutex g_gameLogMutex;

    std::string gameHex(uintptr_t value) {
        std::ostringstream oss;
        oss << "0x" << std::hex << std::uppercase << value;
        return oss.str();
    }

    void appendGameLog(const std::string& line) {
        std::lock_guard<std::mutex> lock(g_gameLogMutex);
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


Game* Game::instance{nullptr};
std::mutex Game::mutex;


Game* Game::getInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new Game();
    }
    return instance;
}

void Game::safeLogout() {
    typedef void(gameCall* SafeLogout)(
    uintptr_t RCX
    );
    auto function = reinterpret_cast<SafeLogout>(SingletonFunctions["g_game.safeLogout"].first);
    return g_dispatcher->scheduleEventEx([function]() {
        function(SingletonFunctions["g_game.safeLogout"].second);
    });
}

void Game::walk(Otc::Direction direction)
{
    const auto it = SingletonFunctions.find("g_game.walk");
    if (it == SingletonFunctions.end() || !it->second.first || !it->second.second) return;
    typedef bool(gameCall* Walk)(
        uintptr_t ThisPtr,
        Otc::Direction direction
        );
    auto function = reinterpret_cast<Walk>(it->second.first);
    const auto gamePtr = it->second.second;
    return g_dispatcher->scheduleEventEx([function, gamePtr, direction]() {
        function(gamePtr, direction);
    });
}

void Game::autoWalk(const std::vector<Otc::Direction>& dirs, const Position &startPos) {
    const auto it = SingletonFunctions.find("g_game.autoWalk");
    if (it == SingletonFunctions.end() || !it->second.first || !it->second.second) return;
    typedef void(gameCall* AutoWalk)(
        uintptr_t ThisPtr,
        std::vector<Otc::Direction> dirs
        );
    auto function = reinterpret_cast<AutoWalk>(it->second.first);
    const auto gamePtr = it->second.second;
    return g_dispatcher->scheduleEventEx([function, gamePtr, dirs, startPos]() {
        (void)startPos;
        function(gamePtr, dirs);
    });
}

void Game::turn(Otc::Direction direction)
{
    typedef void(gameCall* Turn)(
        uintptr_t RCX,
        Otc::Direction RDX
        );
    auto function = reinterpret_cast<Turn>(SingletonFunctions["g_game.turn"].first);
    return g_dispatcher->scheduleEventEx([function, direction]() {
        function(SingletonFunctions["g_game.turn"].second, direction);
    });
}

void Game::stop() {
    typedef void(gameCall* Stop)(
        uintptr_t RCX
        );
    auto function = reinterpret_cast<Stop>(SingletonFunctions["g_game.stop"].first);
    return g_dispatcher->scheduleEventEx([function]() {
        function(SingletonFunctions["g_game.stop"].second);
    });
}


void Game::move(const ThingPtr &thing, const Position& toPos, int count) {
    if (!thing) return;
    typedef void(gameCall* Move)(
        uintptr_t RCX,
        const ThingPtr *RDX,
        const Position *R8,
        int count
        );
    auto function = reinterpret_cast<Move>(SingletonFunctions["g_game.move"].first);
    return g_dispatcher->scheduleEventEx([function, thing, toPos, count]() {
        function(SingletonFunctions["g_game.move"].second, &thing, &toPos, count);
    });
}

void Game::moveToParentContainer(const ThingPtr& thing, const int count) {
    if (!thing) return;
    typedef void(gameCall* MoveToParentContainer)(
        uintptr_t RCX,
        const ThingPtr *RDX,
        const int count
        );
    auto function = reinterpret_cast<MoveToParentContainer>(SingletonFunctions["g_game.moveToParentContainer"].first);
    return g_dispatcher->scheduleEventEx([function, thing, count]() {
        function(SingletonFunctions["g_game.moveToParentContainer"].second, &thing, count);
    });
}

void Game::use(const ThingPtr &thing) {
    if (!thing) return;
    typedef void(gameCall* Use)(
        uintptr_t RCX,
        const ThingPtr *RDX
        );
    auto function = reinterpret_cast<Use>(SingletonFunctions["g_game.use"].first);
    return g_dispatcher->scheduleEventEx([function, thing]() {
        function(SingletonFunctions["g_game.use"].second, &thing);
    });
}

void Game::useWith(const ItemPtr &item, const ThingPtr &toThing) {
    if (!item || !toThing) return;
    typedef void(gameCall* UseWith)(
        uintptr_t RCX,
        const ItemPtr *RDX,
        const ThingPtr *R8
        );
    auto function = reinterpret_cast<UseWith>(SingletonFunctions["g_game.useWith"].first);
    return g_dispatcher->scheduleEventEx([function, item, toThing]() {
        function(SingletonFunctions["g_game.useWith"].second, &item, &toThing);
    });
}

void Game::useInventoryItem(const uint16_t itemId) {
    if (!itemId) return;
    typedef void(gameCall* UseInventoryItem)(
        uintptr_t RCX,
        const uint16_t itemId
    );
    auto function = reinterpret_cast<UseInventoryItem>(SingletonFunctions["g_game.useInventoryItem"].first);
    return g_dispatcher->scheduleEventEx([function, itemId]() {
        function(SingletonFunctions["g_game.useInventoryItem"].second, itemId);
    });
}

void Game::useInventoryItemWith(const uint16_t itemId, const ThingPtr &toThing) {
    if (!toThing) return;
    typedef void(gameCall* UseInventoryItemWith)(
        uintptr_t second_param,
        const uint16_t itemId,
        const ThingPtr *toThing
    );
    auto function = reinterpret_cast<UseInventoryItemWith>(SingletonFunctions["g_game.useInventoryItemWith"].first);
    return g_dispatcher->scheduleEventEx([function, itemId, toThing]() {
        function(SingletonFunctions["g_game.useInventoryItemWith"].second, itemId, &toThing);
    });
}

ItemPtr Game::findItemInContainers(uint32_t itemId, int subType, uint8_t tier) {
    typedef void(gameCall* FindItemInContainers)(
        uintptr_t RCX,
        ItemPtr* RDX,
        uint32_t R8,
        int R9,
        uint8_t tier
        );
    auto function = reinterpret_cast<FindItemInContainers>(SingletonFunctions["g_game.findItemInContainers"].first);
    return g_dispatcher->scheduleEventEx([function, itemId, subType, tier]() {
        ItemPtr result;
        function(SingletonFunctions["g_game.findItemInContainers"].second, &result, itemId, subType, tier);
        return result;
    });
}

int Game::open(const ItemPtr &item, const ContainerPtr &previousContainer) {
    if (!item) return 0;
    typedef int(gameCall* Open)(
        uintptr_t RCX,
        const ItemPtr *RDX,
        const ContainerPtr *R8
        );
    auto function = reinterpret_cast<Open>(SingletonFunctions["g_game.open"].first);
    return g_dispatcher->scheduleEventEx([function, item, previousContainer]() {
            auto ret = function(SingletonFunctions["g_game.open"].second, &item, &previousContainer);
            return ret;
    });
}

void Game::openParent(const ContainerPtr &container) {
    if (!container) return;
    typedef void(gameCall* OpenParent)(
        uintptr_t RCX,
        const ContainerPtr *RDX
        );
    auto function = reinterpret_cast<OpenParent>(SingletonFunctions["g_game.openParent"].first);
    return g_dispatcher->scheduleEventEx([function, container]() {
        function(SingletonFunctions["g_game.openParent"].second, &container);
    });
}

void Game::close(const ContainerPtr &container) {
    if (!container) return;
    typedef void(gameCall* Close)(
        uintptr_t RCX,
        const ContainerPtr *RDX
        );
    auto function = reinterpret_cast<Close>(SingletonFunctions["g_game.close"].first);
    return g_dispatcher->scheduleEventEx([function, container]() {
        function(SingletonFunctions["g_game.close"].second, &container);
    });
}

void Game::refreshContainer(const ContainerPtr &container) {
    if (!container) return;
    typedef void(gameCall* RefreshContainer)(
        uintptr_t RCX,
        const ContainerPtr *RDX
        );
    auto function = reinterpret_cast<RefreshContainer>(SingletonFunctions["g_game.refreshContainer"].first);
    return g_dispatcher->scheduleEventEx([function, container]() {
        function(SingletonFunctions["g_game.refreshContainer"].second, &container);
    });
}

void Game::attack(const CreaturePtr &creature, bool cancel = false) {
    if (!creature) return;
    typedef void(gameCall* Attack)(
        uintptr_t RCX,
        CreaturePtr RDX,
        bool R8
        );
    auto function = reinterpret_cast<Attack>(SingletonFunctions["g_game.attack"].first);
    return g_dispatcher->scheduleEventEx([function, creature, cancel]() {
        function(SingletonFunctions["g_game.attack"].second, creature, cancel);
    });
}

void Game::cancelAttack() {
    typedef void(gameCall* CancelAttack)(
        uintptr_t RCX
        );
    auto function = reinterpret_cast<CancelAttack>(SingletonFunctions["g_game.cancelAttack"].first);
    return g_dispatcher->scheduleEventEx([function]() {
        function(SingletonFunctions["g_game.cancelAttack"].second);
    });
}

void Game::follow(const CreaturePtr& creature) {
    if (!creature) return;
    typedef void(gameCall* Follow)(
        uintptr_t RCX,
        const CreaturePtr* RDX
        );
    auto function = reinterpret_cast<Follow>(SingletonFunctions["g_game.follow"].first);
    return g_dispatcher->scheduleEventEx([function, creature]() {
        function(SingletonFunctions["g_game.follow"].second, &creature);
    });

}

void Game::cancelAttackAndFollow() {
    typedef void(gameCall* CancelAttackAndFollow)(
        uintptr_t RCX
    );
    auto function = reinterpret_cast<CancelAttackAndFollow>(SingletonFunctions["g_game.cancelAttackAndFollow"].first);
    return g_dispatcher->scheduleEventEx([function]() {
        function(SingletonFunctions["g_game.cancelAttackAndFollow"].second);
    });
}


void Game::talk(const std::string& message) {
    typedef void(gameCall *Talk)(
        uintptr_t RCX,
        const std::string& message
    );
    auto function = reinterpret_cast<Talk>(SingletonFunctions["g_game.talk"].first);
    return g_dispatcher->scheduleEventEx([function, message]() {
        function(SingletonFunctions["g_game.talk"].second, message);
    });
}


void Game::talkChannel(const Otc::MessageMode mode, const uint16_t channelId, const std::string& message) {
    typedef void(gameCall *TalkChannel)(
        uintptr_t RCX,
        const Otc::MessageMode RDX,
        const uint16_t channelId,
        const std::string& message
        );
    auto function = reinterpret_cast<TalkChannel>(SingletonFunctions["g_game.talkChannel"].first);
    return g_dispatcher->scheduleEventEx([function, mode, channelId, message]() {
            function(SingletonFunctions["g_game.talkChannel"].second, mode, channelId, message);
    });
}

void Game::talkPrivate(Otc::MessageMode msgMode,std::string receiver,std::string message) {
    typedef void(gameCall *TalkPrivate)(
        uintptr_t first_param,
        Otc::MessageMode msgMode,
        std::string *receiver,
        std::string *message
    );
    auto function = reinterpret_cast<TalkPrivate>(SingletonFunctions["g_game.talkPrivate"].first);
    g_dispatcher->scheduleEventEx([function, receiver, message]() mutable {
        Otc::MessageMode m = Otc::MessageMode::MessagePrivateTo;
        function(SingletonFunctions["g_game.talkPrivate"].second, m, &receiver, &message);
    });
}



void Game::openPrivateChannel(const std::string& receiver) {
    typedef void(gameCall *OpenPrivateChannel)(
        uintptr_t RCX,
        const std::string& receiver
    );
    auto function = reinterpret_cast<OpenPrivateChannel>(SingletonFunctions["g_game.openPrivateChannel"].first);
    return g_dispatcher->scheduleEventEx([function, receiver]() {
            function(SingletonFunctions["g_game.openPrivateChannel"].second, receiver);
    });
}

Otc::FightModes Game::getFightMode() {
    typedef Otc::FightModes(gameCall* GetFightMode)(
        uintptr_t RCX
        );
    auto function = reinterpret_cast<GetFightMode>(SingletonFunctions["g_game.getFightMode"].first);
    return g_dispatcher->scheduleEventEx([function]() {
            auto ret = function(SingletonFunctions["g_game.getFightMode"].second);
            return ret;
    });
}

Otc::ChaseModes Game::getChaseMode() {
    typedef Otc::ChaseModes(gameCall* GetChaseMode)(
        uintptr_t RCX
        );
    auto function = reinterpret_cast<GetChaseMode>(SingletonFunctions["g_game.getChaseMode"].first);
    return g_dispatcher->scheduleEventEx([function]() {
            auto ret = function(SingletonFunctions["g_game.getChaseMode"].second);
            return ret;
    });
}

void Game::setChaseMode(Otc::ChaseModes mode) {
    typedef void(gameCall *SetChaseMode)(
        uintptr_t RCX,
        Otc::ChaseModes mode
        );
    auto function = reinterpret_cast<SetChaseMode>(SingletonFunctions["g_game.setChaseMode"].first);
    return g_dispatcher->scheduleEventEx([function, mode]() {
            function(SingletonFunctions["g_game.setChaseMode"].second, mode);
    });
}

void Game::setFightMode(Otc::FightModes mode) {
    typedef void(gameCall *SetFightMode)(
        uintptr_t RCX,
        Otc::FightModes mode
        );
    auto function = reinterpret_cast<SetFightMode>(SingletonFunctions["g_game.setFightMode"].first);
    return g_dispatcher->scheduleEventEx([function, mode]() {
            function(SingletonFunctions["g_game.setFightMode"].second, mode);
    });
}

void Game::buyItem(const ItemPtr& item, const uint16_t amount, const bool ignoreCapacity, const bool buyWithBackpack) {
    typedef void(gameCall *BuyItem)(
        uintptr_t RCX,
        const ItemPtr *item,
        uint16_t amount,
        bool ignoreCapacity,
        bool buyWithBackpack
        );
    auto function = reinterpret_cast<BuyItem>(SingletonFunctions["g_game.buyItem"].first);
    return g_dispatcher->scheduleEventEx([function, item, amount, ignoreCapacity, buyWithBackpack]() {
            function(SingletonFunctions["g_game.buyItem"].second, &item, amount, ignoreCapacity, buyWithBackpack);
    });
}

void Game::sellItem(const ItemPtr& item, const uint16_t amount, const bool ignoreEquipped) {
    typedef void(gameCall *SellItem)(
        uintptr_t RCX,
        const ItemPtr *item,
        uint16_t amount,
        bool ignoreEquipped
        );
    auto function = reinterpret_cast<SellItem>(SingletonFunctions["g_game.sellItem"].first);
    return g_dispatcher->scheduleEventEx([function, item, amount, ignoreEquipped]() {
            function(SingletonFunctions["g_game.sellItem"].second, &item, amount, ignoreEquipped);
    });
}

void Game::equipItem(const ItemPtr &item) {
    if (!item) return;
    typedef void(gameCall* EquipItem)(
        uintptr_t RCX,
        const ItemPtr *item
        );
    auto function = reinterpret_cast<EquipItem>(SingletonFunctions["g_game.equipItem"].first);
    return g_dispatcher->scheduleEventEx([function, item]() {
        function(SingletonFunctions["g_game.equipItem"].second, &item);
    });
}

void Game::equipItemId(uint16_t itemId, uint8_t tier) {
        typedef void(gameCall* EquipItemId)(
        uintptr_t RCX,
        uint16_t itemId,
        uint8_t tier
        );
    auto function = reinterpret_cast<EquipItemId>(SingletonFunctions["g_game.equipItemId"].first);
    const auto gamePtr = SingletonFunctions["g_game.equipItemId"].second;
    if (!function || !gamePtr) {
        const auto item = findItemInContainers(itemId, -1, tier);
        if (item) {
            equipItem(item);
        }
        return;
    }
    return g_dispatcher->scheduleEventEx([function, itemId, tier]() {
        function(SingletonFunctions["g_game.equipItemId"].second, itemId, tier);
    });
}

void Game::mount(bool mountStatus) {
    typedef void(gameCall* Mount)(
        uintptr_t RCX,
        bool mountStatus
        );
    auto function = reinterpret_cast<Mount>(SingletonFunctions["g_game.mount"].first);
    return g_dispatcher->scheduleEventEx([function, mountStatus]() {
        function(SingletonFunctions["g_game.mount"].second, mountStatus);
    });
}

void Game::changeMapAwareRange(const uint8_t xrange, const uint8_t yrange) {
    typedef void(gameCall* ChangeMapAwareRange)(
        uintptr_t RCX,
        uint8_t xrange,
        uint8_t yrange
    );
    auto function = reinterpret_cast<ChangeMapAwareRange>(SingletonFunctions["g_game.changeMapAwareRange"].first);
    return g_dispatcher->scheduleEventEx([function, xrange, yrange]() {
        function(SingletonFunctions["g_game.changeMapAwareRange"].second, xrange, yrange);
    });
}

bool Game::canPerformGameAction() {
    typedef bool(gameCall* CanPerformGameAction)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<CanPerformGameAction>(SingletonFunctions["g_game.canPerformGameAction"].first);
    return g_dispatcher->scheduleEventEx([function]() {
            void* pMysteryPtr = nullptr; // 8 bytes is enough for bool/ignored param
            auto ret = function(SingletonFunctions["g_game.canPerformGameAction"].second, &pMysteryPtr);
            return ret;
    });
}

bool Game::isOnline() {
    typedef bool(gameCall* IsOnline)(
       uintptr_t RCX,
       void *RDX
       );
    auto function = reinterpret_cast<IsOnline>(SingletonFunctions["g_game.isOnline"].first);
    if (!function || !SingletonFunctions["g_game.isOnline"].second) return false;
    return g_dispatcher->scheduleEventEx([function]() {
            void* pMysteryPtr = nullptr;
            auto ret = function(SingletonFunctions["g_game.isOnline"].second, &pMysteryPtr);
            return ret;
    });
}

bool Game::isAttacking() {
    typedef bool(gameCall* IsAttacking)(
       uintptr_t RCX,
       void *RDX
       );
    auto function = reinterpret_cast<IsAttacking>(SingletonFunctions["g_game.isAttacking"].first);
    if (!function || !SingletonFunctions["g_game.isAttacking"].second) return false;
    return g_dispatcher->scheduleEventEx([function]() {
            void* pMysteryPtr = nullptr;
            auto ret = function(SingletonFunctions["g_game.isAttacking"].second, &pMysteryPtr);
            return ret;
    });
}

bool Game::isFollowing() {
    typedef bool(gameCall* IsFollowing)(
       uintptr_t RCX,
       void *RDX
       );
    auto function = reinterpret_cast<IsFollowing>(SingletonFunctions["g_game.isFollowing"].first);
    if (!function || !SingletonFunctions["g_game.isFollowing"].second) return false;
    return g_dispatcher->scheduleEventEx([function]() {
            void* pMysteryPtr = nullptr;
            auto ret = function(SingletonFunctions["g_game.isFollowing"].second, &pMysteryPtr);
            return ret;
    });
}


ContainerPtr Game::getContainer(int index) {
    typedef void(gameCall* GetContainer)(
        uintptr_t RCX,
        ContainerPtr *RDX,
        int idx
        );
    auto function = reinterpret_cast<GetContainer>(SingletonFunctions["g_game.getContainer"].first);
    return g_dispatcher->scheduleEventEx([function, index]() {
            ContainerPtr result;
            function(SingletonFunctions["g_game.getContainer"].second, &result, index);
            return result;
    });
}

std::vector<ContainerPtr> Game::getContainers() {
    typedef void(gameCall* GetContainers)(
        uintptr_t RCX,
        std::map<int, uintptr_t> *RDX
    );
    auto function = reinterpret_cast<GetContainers>(SingletonFunctions["g_game.getContainers"].first);
    return g_dispatcher->scheduleEventEx([function]() {
        std::map<int, uintptr_t> result;
        function(SingletonFunctions["g_game.getContainers"].second, &result);
        std::vector<ContainerPtr> containers;
        for (auto items : result){
            containers.push_back(ContainerPtr(items.second));
        }
        return containers;
    });
}

CreaturePtr Game::getAttackingCreature() {
    typedef void(gameCall* GetAttackingCreature)(
        uintptr_t RCX,
        CreaturePtr *RDX
        );
    auto function = reinterpret_cast<GetAttackingCreature>(SingletonFunctions["g_game.getAttackingCreature"].first);
    return g_dispatcher->scheduleEventEx([function]() {
            CreaturePtr result;
            function(SingletonFunctions["g_game.getAttackingCreature"].second, &result);
            return result;
    });
}

CreaturePtr Game::getFollowingCreature() {
    typedef void(gameCall* GetFollowingCreature)(
        uintptr_t RCX,
        CreaturePtr *RDX
        );
    auto function = reinterpret_cast<GetFollowingCreature>(SingletonFunctions["g_game.getFollowingCreature"].first);
    return g_dispatcher->scheduleEventEx([function]() {
            CreaturePtr result;
            function(SingletonFunctions["g_game.getFollowingCreature"].second, &result);
            return result;
    });
}

LocalPlayerPtr Game::getLocalPlayer() {
    typedef void(gameCall* GetLocalPlayer)(
        uintptr_t RCX,
        LocalPlayerPtr *RDX
        );
    auto function = reinterpret_cast<GetLocalPlayer>(SingletonFunctions["g_game.getLocalPlayer"].first);
    return g_dispatcher->scheduleEventEx([function]() {
        LocalPlayerPtr result;
        function(SingletonFunctions["g_game.getLocalPlayer"].second, &result);
        appendGameLog("[call] g_game.getLocalPlayer fn=" +
            gameHex(SingletonFunctions["g_game.getLocalPlayer"].first) +
            " this=" + gameHex(SingletonFunctions["g_game.getLocalPlayer"].second) +
            " result=" + gameHex(result));
        return result;
    });
}

int Game::getClientVersion() {
    typedef int(gameCall* GetClientVersion)(
        uintptr_t RCX
        );
    auto function = reinterpret_cast<GetClientVersion>(SingletonFunctions["g_game.getClientVersion"].first);
    return g_dispatcher->scheduleEventEx([function]() {
        return function(SingletonFunctions["g_game.getClientVersion"].second);
    });
}

std::string Game::getCharacterName() {
    typedef void(gameCall* GetCharacterName)(
        uintptr_t RCX,
        std::string *RDX
        );
    auto function = reinterpret_cast<GetCharacterName>(SingletonFunctions["g_game.getCharacterName"].first);
    return g_dispatcher->scheduleEventEx([function]() {
        std::string result;
        function(SingletonFunctions["g_game.getCharacterName"].second, &result);
        return result;
    });
}













