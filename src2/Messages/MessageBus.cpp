#include "MessageBus.h"

namespace Messages {
// MessageBus
void MessageBus::sendMessage(const Message<>& msg) {
    // Subscribed to this sender and code
    sendMessage(msg, subscribers[msg.id()][msg.code]);
    // Subscribed to this sender
    sendMessage(msg, subscribers[msg.id()][NO_CODE]);
    // Subscribed to all messages
    sendMessage(msg, subscribers[ID<Message<>>()][NO_CODE]);
}

void MessageBus::sendMessage(const Message<>& msg, CallbackVector& targets) {
    auto& target = msg.opts.target;
    bool ignoreTarget = target == Entities::NullId();
    targets.idxs.push_back(0);
    size_t idx = targets.idxs.size() - 1;
    while (targets.idxs.at(idx) < (int)targets.vec.size()) {
        auto& sub = targets.vec.at(targets.idxs.at(idx));
        if (ignoreTarget || sub.eId == target) {
            sub.func(msg);
        }
        targets.idxs.at(idx)++;
    }
    targets.idxs.pop_back();
}

MessageHandle MessageBus::subscribe(
    const std::function<void(const Message<>&)>& callback,
    const Entities::UUID& eId, const std::type_index& mId, EnumT msgCode) {
    auto& cnt = entity_counts[eId];
    subscribers[mId][msgCode].vec.push_back({eId, cnt, callback});
    return MessageHandle{eId, cnt++, mId, msgCode};
}

void MessageBus::unsubscribe(MessageHandle handle) {
    auto& list = subscribers[handle.mId][handle.code];
    for (auto it = list.vec.begin(); it != list.vec.end();) {
        if (it->eId == handle.eId && it->eNum == handle.eNum) {
            int pos = it - list.vec.begin();
            for (auto& idx : list.idxs) {
                if (pos <= idx && idx != 0) {
                    idx--;
                }
            }
            it = list.vec.erase(it);
        } else {
            ++it;
        }
    }
}

MessageBus& GetMessageBus() {
    static MessageBus MSG_BUS;
    return MSG_BUS;
}
}  // namespace Messages
