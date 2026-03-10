#include "Map.h"

Map* Map::instance{nullptr};
std::mutex Map::mutex;


Map* Map::getInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new Map();
    }
    return instance;
}

TilePtr Map::getTile(Position tilePos) {
    typedef TilePtr*(gameCall* GetTile)(
        uintptr_t a1,
        const Position *a2
        );
    auto function = reinterpret_cast<GetTile>(SingletonFunctions["g_map.getTile"].first);
    return g_dispatcher->scheduleEventEx([function, tilePos]() {
        return *function(SingletonFunctions["g_map.getTile"].second, &tilePos);
    });
}

std::vector<CreaturePtr> Map::getSpectators(const Position centerPos, bool multiFloor) {
    typedef void(gameCall* GetSpectators)(
        uintptr_t RCX,
        std::vector<CreaturePtr> *RDX,
        const Position *R8,
        bool R9
        );
    auto function = reinterpret_cast<GetSpectators>(SingletonFunctions["g_map.getSpectators"].first);
    return g_dispatcher->scheduleEventEx([function, centerPos, multiFloor]() {
        std::vector<CreaturePtr> result;
        function(SingletonFunctions["g_map.getSpectators"].second, &result, &centerPos, multiFloor);
        return result;
    });
}

std::vector<Otc::Direction> Map::findPath(const Position startPos, const Position goalPos, int maxComplexity, int flags) {
    typedef uintptr_t*(gameCall* FindPath)(
        uintptr_t RCX,
        std::tuple<std::vector<Otc::Direction>, Otc::PathFindResult> *result,
        const Position *R8,
        const Position *R9,
        int maxComplexity,
        int flags
        );
    auto function = reinterpret_cast<FindPath>(SingletonFunctions["g_map.findPath"].first);
    return g_dispatcher->scheduleEventEx([function, startPos, goalPos, maxComplexity, flags]() {
        std::tuple<std::vector<Otc::Direction>, Otc::PathFindResult> pMysteryPtr;
        auto ret = function(SingletonFunctions["g_map.findPath"].second, &pMysteryPtr, &startPos,  &goalPos, maxComplexity, flags);
        if (std::get<1>(pMysteryPtr) == Otc::PathFindResultOk)
            return std::get<0>(pMysteryPtr);
        return std::vector<Otc::Direction> {};
    });
}

bool Map::isSightClear(const Position& fromPos, const Position& toPos) {
    typedef bool(gameCall* IsSightClear)(
        uintptr_t RCX,
        void *RDX,
        const Position *fromPos,
        const Position *toPos
        );
    auto function = reinterpret_cast<IsSightClear>(SingletonFunctions["g_map.isSightClear"].first);
    return g_dispatcher->scheduleEventEx([function, fromPos, toPos]() {
        void* pMysteryPtr = nullptr;
        return function(SingletonFunctions["g_map.isSightClear"].second, &pMysteryPtr, &fromPos, &toPos);
    });
}






