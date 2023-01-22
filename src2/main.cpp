#define SDL_MAIN_HANDLED

#include <Framework/EventSystem/EventSystem.h>
#include <Framework/RenderSystem/RenderSystem.h>
#include <Test.h>
#include <Windows.h>

#include <iostream>

struct ThreadData {
    Services::CommandService* cs;
    std::queue<Messages::MessagePtr> msgs;
};

DWORD WINAPI runCommand(LPVOID param) {
    ThreadData* data = (ThreadData*)param;
    while (data->cs->checkInput(data->msgs)) {
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
    auto& s = GameObjects::Get<MyService>();
    Entities::UUID id;

    // Test unsubscribe
    {
        auto e = GameObjects::New<MyEntity>();
        id = e->id();

        mb.queueMessage<MyMessage>(MyServiceMessage::Hello);
        mb.queueMessage<MyMessage>(MyServiceMessage::World);

        e.reset();
    }

    auto e = GameObjects::New<MyEntity>();

    // Test entity targetting
    mb.queueMessage<MyMessage>(MyServiceMessage::Hello, {id});
    mb.queueMessage<MyMessage>(MyServiceMessage::World, {e->id()});

    ThreadData data{&cs, {}};

    DWORD comThreadId;
    HANDLE comThread = CreateThread(0, 0, runCommand, &data, 0, &comThreadId);

    while (true) {
        EventSystem::update();

        if (EventSystem::get().quit) {
            break;
        }

        // mb.queueMessage<RenderServiceMessage>(RenderService::Code::Render);
        while (!data.msgs.empty()) {
            mb.queueMessage(std::move(data.msgs.front()));
            data.msgs.pop();
        }

        RenderSystem::enforceFPS(60);
    }

    CloseHandle(comThread);

    RenderSystem::clean();

    return 0;
}
