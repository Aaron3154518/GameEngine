#include "Observable.h"

void Unsubscriber::unsubscribe() {
    *mSubscribed = false;
}

Unsubscriber::operator bool() const {
    return *mSubscribed;
}