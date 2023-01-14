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

    auto& s = MyService::Get();
    Services::CommandService cs;

    {
        MyEntity e;

        s.sendMessage(MyServiceMessage::Hello);
        s.sendMessage(MyServiceMessage::World);

        Messages::GetMessageBus().sendMessages();
    }

    MyEntity e;

    DWORD comThreadId;
    HANDLE comThread = CreateThread(0, 0, runCommand, &cs, 0, &comThreadId);

    for (int i = 0; i < 10; i++) {
        Sleep(160);
    }

    WaitForSingleObject(comThread, INFINITE);
    CloseHandle(comThread);

    return 0;
}
