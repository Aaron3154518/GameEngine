#define SDL_MAIN_HANDLED

#include <Framework/RenderSystem/RenderSystem.h>
#include <Test.h>
#include <Windows.h>

#include <iostream>

DWORD WINAPI runCommand(LPVOID param) {
    Services::CommandService& cs = *(Services::CommandService*)param;
    while (cs.checkInput()) {
        Messages::GetMessageBus().sendMessages();
        Sleep(16);
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

        Messages::GetMessageBus().sendMessages();

        e.reset();
    }

    auto e = GameObjects::New<MyEntity>();

    // Test entity targetting
    mb.queueMessage<MyMessage>(MyServiceMessage::Hello, {id});
    mb.queueMessage<MyMessage>(MyServiceMessage::World, {e->id()});

    Messages::GetMessageBus().sendMessages();

    DWORD comThreadId;
    HANDLE comThread = CreateThread(0, 0, runCommand, &cs, 0, &comThreadId);

    for (int i = 0; i < 10; i++) {
        Sleep(160);
    }

    WaitForSingleObject(comThread, INFINITE);
    CloseHandle(comThread);

    RenderSystem::clean();

    return 0;
}
