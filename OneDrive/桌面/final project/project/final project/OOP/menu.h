//
//  menu.h
//  OOP
//
//  Created by 江晨 on 2021/7/1.
//

#ifndef menu_h
#define menu_h
struct Setting
{
    int DIF;//0 = easy //1 = normal //2 = hard
    int GameMode;// 1 = 計分// 2 = 計時
    int CLOUDS;
    int AUDIENCE;
    int C_PLAYER;
    int C_COM;
}choose;

bool Inbox(int x, int y, int w, int h, int posx, int posy) {
    if (posx > x && posx<x + w && posy>y && posy < y + h) {
        return true;
    }
    return false;
}


#endif /* menu_h */