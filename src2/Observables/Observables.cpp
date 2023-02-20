#include "Observables.h"

namespace Observables {
Messages::MessageBus& GetMessageBus() {
    static Messages::MessageBus MESSAGE_BUS;
    return MESSAGE_BUS;
}
}  // namespace Observables
