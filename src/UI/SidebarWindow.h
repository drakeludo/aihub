#pragma once

class ChatController;

class SidebarWindow {
public:
    explicit SidebarWindow(ChatController* controller);
    void render();
    
private:
    ChatController* controller_;
    int selectedChat_ = -1;
};
