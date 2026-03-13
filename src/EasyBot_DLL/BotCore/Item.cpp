#include "Item.h"
#include <algorithm>
#include <cctype>

namespace {
    std::string itemMetaSource(Item* self, ItemPtr item) {
        auto tooltip = self->getTooltip(item);
        if (!tooltip.empty()) return tooltip;
        auto description = self->getDescription(item);
        if (!description.empty()) return description;
        auto text = self->getText(item);
        if (!text.empty()) return text;
        return self->getName(item);
    }

    uint32_t parseTrailingNumber(const std::string& source, const std::string& key) {
        if (source.empty()) return 0;
        auto lower = source;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        const auto keyPos = lower.find(key);
        if (keyPos == std::string::npos) return 0;
        size_t pos = keyPos + key.size();
        while (pos < lower.size() && !std::isdigit(static_cast<unsigned char>(lower[pos]))) {
            ++pos;
        }
        if (pos >= lower.size()) return 0;
        uint32_t value = 0;
        while (pos < lower.size() && std::isdigit(static_cast<unsigned char>(lower[pos]))) {
            value = value * 10 + static_cast<uint32_t>(lower[pos] - '0');
            ++pos;
        }
        return value;
    }
}


Item* Item::instance{nullptr};
std::mutex Item::mutex;


Item* Item::getInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new Item();
    }
    return instance;
}

std::string Item::getName(ItemPtr item) {
    if (!item) return "";
    typedef void(gameCall* GetName)(
        uintptr_t RCX,
        std::string *RDX
        );
    auto function = reinterpret_cast<GetName>(ClassMemberFunctions["Item.getName"]);
    if (!function) return "";
    return g_dispatcher->scheduleEventEx([function, item]() {
        std::string result;
        function(item, &result);
        return result;
    });
}

std::string Item::getDescription(ItemPtr item) {
    if (!item) return "";
    typedef void(gameCall* GetDescription)(
        uintptr_t RCX,
        std::string *RDX
        );
    auto function = reinterpret_cast<GetDescription>(ClassMemberFunctions["Item.getDescription"]);
    if (!function) return "";
    return g_dispatcher->scheduleEventEx([function, item]() {
        std::string result;
        function(item, &result);
        return result;
    });
}

int Item::getCount(ItemPtr item) {
    if (!item) return 0;
    typedef int(gameCall* GetCount)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetCount>(ClassMemberFunctions["Item.getCount"]);
    if (!function) return 0;
    return g_dispatcher->scheduleEventEx([function, item]() {
        void* pMysteryPtr = nullptr;
        return function(item, &pMysteryPtr);
    });
}

int Item::getSubType(ItemPtr item) {
    if (!item) return 0;
    typedef int(gameCall* GetSubType)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetSubType>(ClassMemberFunctions["Item.getSubType"]);
    if (!function) return 0;
    return g_dispatcher->scheduleEventEx([function, item]() {
        void* pMysteryPtr = nullptr;
        return function(item, &pMysteryPtr);
    });
}

uint32_t Item::getId(ItemPtr item) {
    if (!item) return 0;
    typedef uint32_t(gameCall* GetId)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetId>(ClassMemberFunctions["Item.getId"]);
    if (!function) return 0;
    return g_dispatcher->scheduleEventEx([function, item]() {
        void* pMysteryPtr = nullptr;
        return function(item, &pMysteryPtr);
    });
}

std::string Item::getTooltip(ItemPtr item) {
    if (!item) return "";
    typedef void(gameCall* GetTooltip)(
        uintptr_t RCX,
        std::string *RDX
        );
    auto function = reinterpret_cast<GetTooltip>(ClassMemberFunctions["Item.getTooltip"]);
    if (!function) {
        auto description = getDescription(item);
        if (!description.empty()) return description;
        auto text = getText(item);
        if (!text.empty()) return text;
        return getName(item);
    }
    return g_dispatcher->scheduleEventEx([function, item]() {
        std::string result;
        function(item, &result);
        return result;
    });
}

uint32_t Item::getDurationTime(ItemPtr item) {
    if (!item) return 0;
    typedef uint32_t(gameCall* GetDurationTime)(
        uintptr_t RCX,
        void *RDX
        );
    auto function = reinterpret_cast<GetDurationTime>(ClassMemberFunctions["Item.getDurationTime"]);
    if (!function) {
        const auto parsed = parseTrailingNumber(itemMetaSource(this, item), "duration");
        if (parsed) return parsed;
        const auto subType = getSubType(item);
        return subType > 0 ? static_cast<uint32_t>(subType) : 0;
    }
    return g_dispatcher->scheduleEventEx([function, item]() {
        void* pMysteryPtr = nullptr;
        return function(item, &pMysteryPtr);
    });
}

uint8_t Item::getTier(ItemPtr item) {
    if (!item) return 0;
    typedef void(gameCall* GetTier)(
        uintptr_t RCX,
        uint8_t *RDX
        );
    auto function = reinterpret_cast<GetTier>(ClassMemberFunctions["Item.getTier"]);
    if (!function) {
        const auto parsed = parseTrailingNumber(itemMetaSource(this, item), "tier");
        return parsed <= 255 ? static_cast<uint8_t>(parsed) : 0;
    }
    return g_dispatcher->scheduleEventEx([function, item]() {
        uint8_t result = 0;
        function(item, &result);
        return result;
    });
}

std::string Item::getText(ItemPtr item) {
    if (!item) return "";
    typedef void(gameCall* GetText)(
        uintptr_t RCX,
        std::string *RDX
        );
    auto function = reinterpret_cast<GetText>(ClassMemberFunctions["Item.getText"]);
    if (!function) return "";
    return g_dispatcher->scheduleEventEx([function, item]() {
        std::string result;
        function(item, &result);
        return result;
    });
}
