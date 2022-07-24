#include "Templates.h"

// Unsubscriber
void Unsubscriber::unsubscribe() { *mSubscribed = false; }

Unsubscriber::operator bool() const { return *mSubscribed; }