#pragma once

class SidebarWindow {
public:
    SidebarWindow();
    void render();
    
private:
    int selectedChat_{-1};
};
