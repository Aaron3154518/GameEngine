#ifndef SERVICE_H
#define SERVICE_H

#include <Entities/UUID.h>
#include <Messages/MessageBus.h>
#include <Messages/Messager.h>

#include <vector>

namespace Services {
class Service : public Messages::Messager {
   public:
    virtual ~Service() = default;

    void subscribe(const Entities::UUID& eId);
    void unsubscribe(const Entities::UUID& eId);

   protected:
    std::vector<Entities::UUID> mEntities;

   private:
    void init() final;
    virtual void service_init();
};
}  // namespace Services

#endif
