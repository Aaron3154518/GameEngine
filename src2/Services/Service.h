#ifndef SERVICE_H
#define SERVICE_H

#include <Entities/UUID.h>
#include <Messages/MessageBus.h>
#include <Messages/Messager.h>

#include <vector>

namespace Services {
class Service : public Messages::Messager {
   public:
    void subscribe(const Entities::UUID& eId);
    void unsubscribe(const Entities::UUID& eId);

   protected:
    virtual void onSubscribe(const Entities::UUID& eId);
    virtual void onUnsubscribe(const Entities::UUID& eId);

    virtual void service_init();

    std::vector<Entities::UUID> mEntities;

   private:
    void init() final;
};
}  // namespace Services

#endif
