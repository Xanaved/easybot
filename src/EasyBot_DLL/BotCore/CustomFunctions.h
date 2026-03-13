#ifndef CUSTOM_FUNCTIONS_H
#define CUSTOM_FUNCTIONS_H
#define g_custom CustomFunctions::getInstance()
#include "../../const.h"
#include "EventDispatcher.h"
#include <deque>

struct StackArgs {
    std::string* name;
    uintptr_t* level;
    Otc::MessageMode* mode;
    std::string* text;
    uint16_t* channelId;
    Position* pos;
};

struct MessageStruct {
    std::string name;
    uint16_t level;
    Otc::MessageMode mode;
    std::string text;
    uint16_t channelId;
    Position pos;
};

struct ChannelStruct
{
    uint16_t channelId;
    std::string channelName;
};

struct CorpseEventStruct {
    uint64_t creature;
    std::string name;
    Position pos;
    uint64_t timestamp;
};

class CustomFunctions{
    static CustomFunctions* instance;
    static std::mutex mutex;
    std::mutex dataMutex;
    std::vector<MessageStruct> messages;
    std::vector<ChannelStruct> channels;
    std::deque<CorpseEventStruct> corpseEvents;

    const size_t MAX_MESSAGES = 100;
    const size_t MAX_CORPSE_EVENTS = 64;
protected:
    CustomFunctions()=default;
    ~CustomFunctions()= default;
public:
    CustomFunctions(CustomFunctions const&) = delete;
    void operator=(const CustomFunctions&) = delete;
    static CustomFunctions* getInstance();


    void onTalk(std::string name, uint16_t level, Otc::MessageMode mode, std::string text, uint16_t channelId, const Position& pos);

    std::vector<MessageStruct> getMessages(int messageNumber);
    void clearMessages();
    void dropMessages(int messageNumber);
    uintptr_t* getMessagePtr(uintptr_t message_address);
    void pushCorpseEvent(uint64_t creature, std::string name, const Position& pos, uint64_t timestamp);
    bool popCorpseEvent(CorpseEventStruct* corpseEvent);
};






#endif //CUSTOM_FUNCTIONS_H
