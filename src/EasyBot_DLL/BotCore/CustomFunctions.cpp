#include "CustomFunctions.h"


CustomFunctions* CustomFunctions::instance{nullptr};
std::mutex CustomFunctions::mutex;

CustomFunctions* CustomFunctions::getInstance() {
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr) {
        instance = new CustomFunctions();
    }
    return instance;
}

void CustomFunctions::onTalk(std::string name, uint16_t level, Otc::MessageMode mode, std::string text,
    uint16_t channelId, const Position &pos) {
    std::lock_guard<std::mutex> lock(dataMutex);
    if (messages.size() >= MAX_MESSAGES) {
        messages.erase(messages.begin());
    }
    MessageStruct record = {std::move(name), level, mode, std::move(text), channelId, pos};
    messages.push_back(std::move(record));
}


std::vector<MessageStruct> CustomFunctions::getMessages(int messageNumber) {
    std::lock_guard<std::mutex> lock(dataMutex);
    auto count = static_cast<size_t>(messageNumber);
    size_t actual_size = messages.size();
    size_t start_index;
    if (count >= actual_size) {
        start_index = 0;
    } else {
        start_index = actual_size - count;
    }
    return std::vector<MessageStruct>(
        messages.begin() + start_index,
        messages.end()
    );
}

void CustomFunctions::clearMessages() {
    std::lock_guard<std::mutex> lock(dataMutex);
    messages.clear();
}

void CustomFunctions::dropMessages(int messageNumber) {
    std::lock_guard<std::mutex> lock(dataMutex);
    auto count = static_cast<size_t>(messageNumber);
    size_t current_size = messages.size();
    if (count >= current_size) {
        messages.clear();
    } else {
        messages.resize(current_size - count);
    }
}


uintptr_t* CustomFunctions::getMessagePtr(uintptr_t message_address) {
    return *reinterpret_cast<uintptr_t**>(message_address);
}

void CustomFunctions::pushCorpseEvent(uint64_t creature, std::string name, const Position& pos, uint64_t timestamp) {
    std::lock_guard<std::mutex> lock(dataMutex);
    if (!corpseEvents.empty()) {
        const auto& last = corpseEvents.back();
        if (last.creature == creature &&
            last.pos.x == pos.x &&
            last.pos.y == pos.y &&
            last.pos.z == pos.z) {
            return;
        }
    }

    if (corpseEvents.size() >= MAX_CORPSE_EVENTS) {
        corpseEvents.pop_front();
    }
    corpseEvents.push_back(CorpseEventStruct{creature, std::move(name), pos, timestamp});
}

bool CustomFunctions::popCorpseEvent(CorpseEventStruct* corpseEvent) {
    if (!corpseEvent) {
        return false;
    }

    std::lock_guard<std::mutex> lock(dataMutex);
    if (corpseEvents.empty()) {
        return false;
    }

    *corpseEvent = corpseEvents.front();
    corpseEvents.pop_front();
    return true;
}




