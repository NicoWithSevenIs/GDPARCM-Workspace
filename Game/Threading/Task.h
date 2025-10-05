#pragma once

#include <thread>
#include <functional>
class Task {
    

    public:
        inline static std::thread* Spawn(std::function<void()> callback, bool detach_on_create = true) {
            auto t = new std::thread(callback);
            if(detach_on_create)
                t->detach();
            return t;
        }

};
