#define SDL_MAIN_HANDLED

#include <Framework/EventSystem/EventSystem.h>
#include <Framework/EventSystem/Services.h>
#include <Framework/RenderSystem/RenderSystem.h>
#include <Framework/RenderSystem/Services.h>
#include <Services/CommandService.h>
#include <Test.h>
#include <Windows.h>

#include <iostream>

CRITICAL_SECTION msgQueue;

struct ThreadData {
    Services::CommandService* cs;
    std::queue<Messages::MessagePtr> msgs;
};

DWORD WINAPI runCommand(LPVOID param) {
    ThreadData* data = (ThreadData*)param;
    while (data->cs->checkInput(data->msgs, &msgQueue)) {
    }
    return 0;
}

int main(int argc, char* argv[]) {
    // Enables colored text
    system("echo");

    RenderSystem::Options opts{500, 500};
    RenderSystem::init(opts);

    auto& cs = GameObjects::Get<Services::CommandService>();

    auto& mb = Messages::GetMessageBus();
    Entities::UUID id;

    // Test unsubscribe
    {
        auto e = GameObjects::New<MyEntity>();
        id = e->id();

        mb.sendMessage(MyService::Message(MyService::Hello));
        mb.sendMessage(MyService::Message(MyService::World));

        e.reset();
    }

    auto e = GameObjects::New<MyEntity>();

    // Test entity targetting
    mb.sendMessage(MyService::Message(MyService::Hello, {id}));
    mb.sendMessage(MyService::Message(MyService::World, {e->id()}));

    // Create CLI thread
    if (!InitializeCriticalSectionAndSpinCount(&msgQueue, 0x00000400)) {
        return 0;
    }

    ThreadData data{&cs, {}};

    DWORD comThreadId;
    HANDLE comThread = CreateThread(0, 0, runCommand, &data, 0, &comThreadId);

    auto csMsgId = Messages::ID<Services::CommandService::Message>();

    uint32_t sum = 0, cnt = 0;
    while (true) {
        auto t1 = SDL_GetTicks();
        EventSystem::update();

        if (EventSystem::get().quit) {
            break;
        }

        bool quit = false;
        EnterCriticalSection(&msgQueue);
        while (!data.msgs.empty()) {
            auto& msg = *data.msgs.front();
            if (msg.id() == csMsgId &&
                msg.code == Services::CommandService::Quit) {
                quit = true;
                break;
            }
            mb.sendMessage(msg);
            data.msgs.pop();
        }
        LeaveCriticalSection(&msgQueue);
        if (quit) {
            break;
        }

        mb.sendMessage(UpdateService::Message(UpdateService::Update,
                                              EventSystem::get().dt));

        mb.sendMessage(RenderService::Message(RenderService::Render));

        sum += SDL_GetTicks() - t1;
        ++cnt;

        RenderSystem::enforceFPS(60);
    }

    CloseHandle(comThread);
    DeleteCriticalSection(&msgQueue);

    RenderSystem::clean();

    std::cerr << "Average Frame: " << (sum / cnt) << "ms" << std::endl;

    return 0;
}
