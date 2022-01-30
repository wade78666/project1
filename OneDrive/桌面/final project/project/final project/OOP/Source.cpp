#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include "triangle.h"
#include "Calculation.h"
#include "collision.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "draw.h"
#include "menu.h"
using namespace std;
const int SOLID = 100, SHADED = 101, BLENDED = 102;
int score1 = 0, score2 = 0, CK = 3;
bool stop = false;
vec3D Dk, Vk, shootPosition, tmpPosition;;
int testshoot = 0, score = 0;
int musicplay = 1;
int TIME = 0, timeDiv = hoopControlTimeDiv, TR = 0, Control_Enter_TIME = 0, Control_JUMP_TIME = 0, Control_Music_TIME = -1, ControlStealTime = 0, WinTime = 0;
Mix_Chunk* tone = NULL;
Mix_Music* backgroundMusic = NULL;
int dribble = 0;
bool touch = false, jud = true, timeup = true, fixMouse = false, Win = false;
int viewState = 0;//0 for first perspective, 1 for third perspective, 2 no limitation
int initSDL();
void closeSDL();
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
Uint32 showObject(Uint32 interval, void* param);
int MODE = 0;
struct TextData
{
    SDL_Texture* texture;
    int width;
    int height;
};
enum MouseState
{
    NONE = 0, leftdown = 1, leftup = 2, rightdown = 3
};
MouseState mouseState;
int initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    window = SDL_CreateWindow("OOP SDL Tutorial", 50, 50, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 2;
    }
    if (TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 4;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 8, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 5;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 6;
    }
    return 0;
}
void closeSDL()
{
    Mix_FreeMusic(backgroundMusic);
    Mix_FreeChunk(tone);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}
void mouseHandleEvent(SDL_Event* e, MouseState* mouseState, int* x, int* y)
{
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP || e->type == SDL_MOUSEWHEEL)
    {
        SDL_GetMouseState(x, y);
        SDL_SetRelativeMouseMode(fixMouse ? SDL_TRUE : SDL_FALSE);
        switch (e->type) {
        case SDL_MOUSEBUTTONDOWN:
            if (e->button.button == SDL_BUTTON_LEFT) {
                *mouseState = leftdown;
                if (ball.owner == PL && viewState != 0)
                    ball.owner = PTN;
            }
            if (e->button.button == SDL_BUTTON_RIGHT) {
                *mouseState = rightdown;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (e->button.button == SDL_BUTTON_LEFT) {
                *mouseState = leftup;
                if (ball.owner == PTN)
                    ball.owner = PT;
            }
            break;
        case SDL_MOUSEMOTION:
            double mouseSentivitity = 400;
            Yaw += (double)e->motion.xrel / mouseSentivitity * 180;
            Pitch += 12 / 7. * (double)e->motion.yrel / mouseSentivitity * 75;
            if (Pitch > 75)
                Pitch = 75;
            if (Pitch < -75)
                Pitch = -75;
            break;
        }
    }
}

TextData loadTextTexture(const char* str, const char* fontPath, int fontSize, Uint8 fr, Uint8 fg, Uint8 fb, int textType, Uint8 br, Uint8 bg, Uint8 bb)
{
    TTF_Font* ttfFont = NULL;
    ttfFont = TTF_OpenFont(fontPath, fontSize);
    if (ttfFont == NULL)
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    SDL_Color textFgColor = { fr, fg, fb }, textBgColor = { br, bg, bb };
    SDL_Surface* textSurface = NULL;
    if (textType == SOLID)
        textSurface = TTF_RenderText_Solid(ttfFont, str, textFgColor);
    else if (textType == SHADED)
        textSurface = TTF_RenderText_Shaded(ttfFont, str, textFgColor, textBgColor);
    else if (textType == BLENDED)
        textSurface = TTF_RenderText_Blended(ttfFont, str, textFgColor);
    TTF_CloseFont(ttfFont);

    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return { NULL };
    }
    else
    {
        TextData text;
        text.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (text.texture == NULL)
        {
            printf("SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
        }
        text.width = textSurface->w;
        text.height = textSurface->h;
        SDL_FreeSurface(textSurface);
        return text;
    }
}

int textRender(SDL_Renderer* renderer, TextData text, int posX, int posY, int cx, int cy, double angle, SDL_RendererFlip flip, int alpha)
{
    SDL_Rect r;
    r.x = posX;
    r.y = posY;
    r.w = text.width;
    r.h = text.height;
    if (SDL_SetTextureBlendMode(text.texture, SDL_BLENDMODE_BLEND) == -1)
    {
        printf("SDL_SetTextureBlendMode failed: %s\n", SDL_GetError());
        return -1;
    }
    if (SDL_SetTextureAlphaMod(text.texture, alpha) == -1)
    {
        printf("SDL_SetTextureAlphaMod failed: %s\n", SDL_GetError());
        return -1;
    }
    SDL_Point center = { cx, cy };
    SDL_RenderCopyEx(renderer, text.texture, NULL, &r, angle, &center, flip);
    return 1;
}

Uint32 moveCloud(Uint32 interval, void* Param) {
    if (choose.CLOUDS == 1) {
        vec3D* c = (vec3D*)Param;
        int i;
        for (i = 0; i < 9; i++) {
            c[i].x++;
            c[i].z++;
            if (c[i].z >= 1500)
                c[i].z = -1500;
            if (c[i].x >= 1500)
                c[i].x = -1500;
        }
    }
    return interval;
}

bool judgetouchball() {
    if (Grab_ball(ball, player) && !ControlStealTime && ball.owner != PL && ball.owner != PT && ball.owner != PTN && ball.owner != CIN && ball.owner != PIN) {
        ControlStealTime = 1000;
        touch = true;
        return true;
    }
    return false;
}

void Judgeenter() {
    if (ball.owner != PIN && ball.owner != CIN && checkenter(jball, Check_Enter_Rec) && (ball.Vel.y < 0)) {
        Mix_PlayChannel(-1, tone, 0);
        //printf("enter ball\n");
        if (ball.owner == PT || ball.owner == FLYP)
        {
            if (choose.GameMode == 1)
                score1++;
            ball.owner = PIN;
        }
        else if (ball.owner == CT || ball.owner == FLYC)
        {
            if (choose.GameMode == 1)
            {
                shootPosition.y = 0;
                score2++;
                printf("shoot=%d enter=%d\n", testshoot, score2);
                ball.owner = CIN;
            }
        }
    }
}

//timercallback fuctions
Uint32 move_ball(Uint32 interval, void* Param) {
    if (MODE == 2) {
        if (ball.owner == PTN && viewState != 0)
        {
            if (TIME <= 0 && timeup == false)
            {
                timeup = true;
            }
            if (TIME >= 1000 && timeup == true)
            {
                timeup = false;
            }
            if (timeup == true)
            {
                TIME += 10;
            }
            else
            {
                TIME -= 10;
            }
        }
        ball.r = 5;
        if (ball.owner == PTN && viewState == 0)
        {
            TR += interval;
        }
        if (collision_control == true) {
            timeDiv -= 15;
            if (timeDiv == 0) {
                timeDiv = hoopControlTimeDiv;
                collision_control = false;
            }
        }
        Moveball();
        Collision_boundary();
        Judgeenter();
        if (Gravity == true) {
            ball.Vel.y -= interval * 0.01;
        }
        if ((judgetouchball() || touch) && ball.owner != PL && ball.owner != PT && ball.owner != PTN && ball.owner != CIN && ball.owner != PIN) {
            //if jud==false the velocity of ball won't influence by our forward;
            jud = true;
            touch = true;
            ball.owner = PL;
        }
        if (ball.owner == PL)
        {
            ball.Pos.x = player.Pos.x + player.LookDir.x / Vector_Length(player.LookDir) * 25;
            ball.Pos.y = player.Pos.y + player.LookDir.y / Vector_Length(player.LookDir) * 25;
            ball.Pos.z = player.Pos.z + player.LookDir.z / Vector_Length(player.LookDir) * 25;
            switch (dribble)
            {
            case 0:
                ball.Pos.y += 20;
                dribble++;
                break;
            case 1:
                ball.Pos.y += 15;
                dribble++;
                break;
            case 2:
                ball.Pos.y += 10;
                dribble++;
                break;
            case 3:
                ball.Pos.y += 5;
                dribble++;
                break;
            case 4:
                dribble++;
                break;
            case 5:
                ball.Pos.y += 5;
                dribble++;
                break;
            case 6:
                ball.Pos.y += 10;
                dribble++;
                break;
            case 7:
                ball.Pos.y += 15;
                dribble = 0;
                break;
            }
        }
        else if (ball.owner == PTN)
        {
            ball.Pos.x = player.Pos.x + player.LookDir.x / Vector_Length(player.LookDir) * 25;
            ball.Pos.y = player.Pos.y + player.LookDir.y / Vector_Length(player.LookDir) * 25 + 20;
            ball.Pos.z = player.Pos.z + player.LookDir.z / Vector_Length(player.LookDir) * 25;
        }
    }
    return interval;
}


Uint32 movePlayer(Uint32 interval, void* Param) {
    if (MODE != 1) {
        vec3D Forward;
        vec3D Up = { 0, 1, 0 };
        vec3D Right;
        if (viewState != 2) {
            Forward = Vector_Normalise_XZ(player.LookDir);
            Forward = Vector_Mul(Forward, player.VelLength);
            Forward.y = 0;
        }
        else {
            double velOfGodMode = 3;
            Forward = Vector_Mul(player.LookDir, velOfGodMode);
            Up = Vector_Mul(Up, velOfGodMode);
        }
        Right = Vector_CrossProduct(Forward, Up);
        //Basic Move
        if (player.Move.FORWARD && !stop)
            player.Pos = Vector_Add(player.Pos, Forward);
        if (player.Move.BACKWARD && !stop)
            player.Pos = Vector_Sub(player.Pos, Forward);
        if (player.Move.RIGHT && !stop)
            player.Pos = Vector_Add(player.Pos, Right);
        if (player.Move.LEFT && !stop)
            player.Pos = Vector_Sub(player.Pos, Right);
        //God Mode Move
        if (viewState == 2) {
            if (player.Move.UP)
                player.Pos = Vector_Add(player.Pos, Up);
            if (player.Move.DOWN)
                player.Pos = Vector_Sub(player.Pos, Up);
        }
        else {
            //Jump
            if (player.Move.JUMP && player.Pos.y == 3) {
                player.Vel.y = 4;
            }
            if (player.Pos.y >= 3) {
                player.Move.JUMP = false;
                player.Vel.y -= 0.01 * interval;
                player.Pos.y += player.Vel.y;
            }
            if (player.Pos.y < 3) {
                player.Pos.y = 3;
                player.Vel.y = 0;
            }
        }
        player.AngleRad.x = DtR(Pitch);
        player.AngleRad.y = DtR(Yaw);
        //Offset Camera behind of player
        if (viewState == 0) {
            Camera.Pos.x = player.Pos.x - player.LookDir.x / Vector_Length(player.LookDir) * 50;
            Camera.Pos.y = player.Pos.y - player.LookDir.y / Vector_Length(player.LookDir) * 50 + 20;
            Camera.Pos.z = player.Pos.z - player.LookDir.z / Vector_Length(player.LookDir) * 50;
            if (Camera.Pos.y < 3)
                Camera.Pos.y = 3;
            player.alphar = 0xAF;
        }
        else {
            //Move Camera to eyes position of player
            Camera.Pos = player.Pos;
            Camera.Pos.y += 30;
            player.alphar = 0xFF;
        }
    }
    return interval;
}


vector <triangle> drawparabola(circle& ball, object& balltrace) {
    vector <triangle> tri;
    circle route;
    Vector_Assign(route.Pos, ball.Pos);
    route.Vel.x = TIME * 0.02 / player.VelLength * player.LookDir.x;
    route.Vel.y = TIME * 0.03 / player.VelLength * player.LookDir.y;
    route.Vel.z = TIME * 0.02 / player.VelLength * player.LookDir.z;
    for (int i = 1; i < 15; i += 3) {
        vector <triangle> tmp;
        route.Pos.x += route.Vel.x * i;
        route.Pos.y += route.Vel.y * i;
        route.Pos.z += route.Vel.z * i;
        tmp = calculateObject(balltrace, route.Pos.x, route.Pos.y, route.Pos.z, 0, 0, 0);
        route.Vel.y -= 0.02 * 15 * 3;
        tri.insert(tri.end(), tmp.begin(), tmp.end());
    }
    return tri;
}
Uint32 Sec(Uint32 interval, void* Param) {
    if (stop && player.Pos.y <= 3 && CK > 0)
    {
        CK--;
    }
    if (CK == 0)
    {
        CK = 3;
        stop = false;
    }
    return interval;
}

Uint32 Control_Time(Uint32 interval, void* Param) {
    if (MODE == 2) {
        if (ControlStealTime > 0) {
            ControlStealTime -= 50;
        }
        if (ball.owner == CIN || ball.owner == PIN) {
            Control_Enter_TIME += 1;
        }
        if (Control_Enter_TIME == 20) {
            Control_Enter_TIME = 0;
            if (ball.owner == PIN)
                ball.owner = GP;
            else if (ball.owner == CIN)
                ball.owner = GC;
        }
        if (computerjump == false) {
            Control_JUMP_TIME += 1;
        }
        if (Control_JUMP_TIME == 30) {
            Control_JUMP_TIME = 0;
            computerjump = true;
        }
        if (Control_Music_TIME > 0) {
            Control_Music_TIME -= 1;
        }
        if (score1 == 2 || score2 == 10)
        {
            WinTime += 1;
        }
        if (WinTime == 40)
        {
            score1 = 0;
            score2 = 0;
            WinTime = 0;
            Win = true;
        }

    }
    return interval;
}
Uint32 OWNER(Uint32 interval, void* Param) {
    if (MODE == 2) {
        printf(" owner = %d\n", ball.owner);
    }
    return interval;
}
Uint32 collisionPlayer(Uint32 interval, void* Param) {
    if (MODE == 2 && choose.GameMode == 1) {
        if (viewState != 2) {
            if (Check_Dis(com.Pos, player.Pos, (com.r + player.r) / 2.)) {
                vec3D t1 = Vector_Sub(player.Pos, com.Pos);
                t1 = Vector_Mul(t1, 0.2);
                com.Pos = Vector_Sub(com.Pos, t1);
                player.Pos = Vector_Add(player.Pos, t1);
                if (com.Pos.y < 3)
                    com.Pos.y = 3;
                if (player.Pos.y < 3)
                    com.Pos.y = 3;
            }
        }
    }
    return interval;
}
Uint32 move_ai(Uint32 interval, void* Param) {
    if (MODE == 2 && choose.GameMode != 2) {
        double theta = (atan2(ball.Pos.z - com.Pos.z, ball.Pos.x - com.Pos.x) / M_PI * 180);
        double distance = Point_Distance_XZ(ball.Pos, com.Pos);
        //compute the vel of computer depending on the distance between of the ball
        com.Vel.x = (distance + 20) * cos(theta / 180 * M_PI) * com.VelLength;
        com.Vel.z = (distance + 20) * sin(theta / 180 * M_PI) * com.VelLength;
        //detect if AI got the ball
        if (com.Pos.y >= 3) {
            com.Vel.y -= 0.01 * interval;
            com.Pos.y += com.Vel.y;
        }
        if (com.Pos.y < 3) {
            com.Pos.y = 3;
            com.Vel.y = 0;
        }
        if (!stop)
        {
            if (Grab_ball(ball, com) && !ControlStealTime && ball.owner != CPU && ball.owner != CT) {
                ControlStealTime = 1000;
                ball.Pos.x = com.Pos.x;
                ball.Pos.y = com.Pos.y + 30;
                ball.Pos.z = com.Pos.z;
                ball.owner = CPU;
                touch = false;
                shootPosition.y = 0;
                TR = 0;
            }
            //decide what to do depending on the state of ball
            if (ball.owner == PL) {//AI approach to ball
                if (Check_Dis(com.Pos, player.Pos, (com.r + player.r) / 2.)) {
                    vec3D t = Vector_Normalise_XZ(player.LookDir);
                    vec3D compare = Vector_CrossProduct(player.LookDir, t);
                    if (compare.y < 0) {
                        vec3D Right = Vector_CrossProduct(t, vUp);
                        com.Vel.x += Right.x * +t.x * 4;
                        com.Vel.z += Right.z * +t.z * 4;
                    }
                    else {
                        vec3D Left = Vector_CrossProduct(t, vDOWN);
                        com.Vel.x += Left.x + t.x * 4;
                        com.Vel.z += Left.z + t.z * 4;
                    }
                }
            }
            else if (ball.owner == PTN) {//AI jump
                if (Check_Dis(com.Pos, player.Pos, com.r + player.r + 20) && computerjump) {
                    if (com.Pos.y == 3) {
                        com.Vel.y = 5;
                    }
                    if (com.Pos.y >= 3) {
                        com.Vel.y -= 0.01 * interval;
                        com.Pos.y += com.Vel.y;
                    }
                    if (com.Pos.y < 3) {
                        com.Pos.y = 3;
                        com.Vel.y = 0;
                        computerjump = false;
                    }
                    if (checkColPlayer(ball, com)) {
                        touch = false;
                        ball.owner = CPU;
                        TR = 0;
                    }
                }
            }
            else if (ball.owner == CPU) {//AI got the ball, find a position to shoot
                if (shootPosition.y == 0) { //if the shoot position hasn't been decided
                    shootPosition.x = rand() % 130 - 130;
                    shootPosition.y = 3;
                    shootPosition.z = rand() % 280 - 140;
                }
                distance = Point_Distance_XZ(shootPosition, com.Pos);
                theta = (atan2(shootPosition.z - com.Pos.z, shootPosition.x - com.Pos.x) / M_PI * 180);
                com.Vel.x = (distance + 15) * cos(theta / 180 * M_PI) * com.VelLength;
                com.Vel.z = (distance + 15) * sin(theta / 180 * M_PI) * com.VelLength;
                //coumpute the position of ball, it should be on the hands
                vec3D forward = Vector_Mul(com.LookDir, com.r);
                ball.Pos = Vector_Add(com.Pos, forward);
                switch (dribble)
                {
                case 0:
                    ball.Pos.y += 20;
                    dribble++;
                    break;
                case 1:
                    ball.Pos.y += 15;
                    dribble++;
                    break;
                case 2:
                    ball.Pos.y += 10;
                    dribble++;
                    break;
                case 3:
                    ball.Pos.y += 5;
                    dribble++;
                    break;
                case 4:
                    dribble++;
                    break;
                case 5:
                    ball.Pos.y += 5;
                    dribble++;
                    break;
                case 6:
                    ball.Pos.y += 10;
                    dribble++;
                    break;
                case 7:
                    ball.Pos.y += 15;
                    dribble = 0;
                    break;
                }

                if (Point_Distance_XZ(shootPosition, com.Pos) <= 5.0)
                    ball.owner = CTN;
            }
            //move AI depending on above result
            com.Pos.x += com.Vel.x;
            com.Pos.z += com.Vel.z;

            //AI Shoot and Calculate AI Look Dir
            vec3D eyePos = com.Pos;
            eyePos.y += 20;
            if (ball.owner == CTN) {
                testshoot++;
                int a = rand() % 1001;
                vec3D Vel, Dis;
                if (a % 2) {//蝛箏?
                    Dis.x = -145 - ball.Pos.x;
                    Dis.y = 69 - ball.Pos.y;
                    Dis.z = 0 - ball.Pos.z;
                }
                else {//??
                    Dis.x = -160 - ball.Pos.x;
                    Dis.y = 69 - ball.Pos.y;
                    Dis.z = 0 - ball.Pos.z;
                }
                Vel = CalVel(Dis);

                if (choose.DIF == 1)
                {
                    Vel.x *= ((a / 10000.) + 0.95);
                    Vel.z *= ((a / 10000.) + 0.95);
                }

                Vector_Assign(ball.Vel, Vel);
                ball.owner = CT;
                Gravity = true;
                com.LookDir = ball.Vel;
            }
            else if (ball.owner != CPU) {
                com.LookDir = Vector_Sub(ball.Pos, eyePos);
                if (!Check_Dis(ball.Pos, com.Pos, 25))
                    com.LookDir.y = 0;
            }
            else {
                if (Point_Distance_XZ(shootPosition, com.Pos) > 40)//if AI is far away from shoot position, it should look at the shoot position.
                    com.LookDir = Vector_Sub(shootPosition, com.Pos);
                else//if it is close enough, look at the hoop and get ready to shoot
                    com.LookDir = Vector_Sub(hoopPosition, com.Pos);
            }
            com.LookDir = Vector_Normalise(com.LookDir);
            com.AngleRad.x = acos(Vector_DotProduct(com.LookDir, vUp)) - M_PI / 2;
            if (com.LookDir.x < 0)
                com.AngleRad.y = acos(Vector_DotProduct(com.LookDir, vZ));
            else
                com.AngleRad.y = -acos(Vector_DotProduct(com.LookDir, vZ));
        }
    }
    return interval;
}
Uint32 audienceControl(Uint32 interval, void* Param) {
    if (audienceOption.Pos.y >= 3) {
        audienceOption.Vel.y -= 0.01 * interval;
        audienceOption.Pos.y += audienceOption.Vel.y;
    }
    if (audienceOption.Pos.y < 3) {
        audienceOption.Pos.y = 3;
        audienceOption.Vel.y = 0;
    }
    return interval;
}

void entermusic(vec3D& p) {
    vec3D t = { 0,0,200 };
    if (Point_Distance_XZ(p, t) < 30 && Control_Music_TIME <= 0) {
        //printf("in\n");
        Control_Music_TIME = 100;
        musicplay *= -1;
        if (Mix_PlayingMusic() == 0) {
            Mix_PlayMusic(backgroundMusic, -1);
        }
        else {
            if (Mix_PausedMusic() == 1) {
                Mix_ResumeMusic();
            }
            else {
                Mix_PauseMusic();
            }
        }
    }
}

void define() {
    //define all the position coordinate
    Backboard.Pos = { -160, 68, -25 };
    Backboard.l = 2;
    Backboard.h = 34;
    Backboard.w = 50;
    Check_Enter_Rec.Pos = { -153, 67, -8 };
    Check_Enter_Rec.l = 16;
    Check_Enter_Rec.h = 3;
    Check_Enter_Rec.w = 16;
    //ball
    ball.Pos = { 100, 10, 0 };
    jball.r = 0.5;
    //Computer
    com.Pos = { 100, 1123, 150 };
    com.r = 20;
    com.Color = 0xAF923F;
    //player
    player.Pos = { 0, 3, -120 };
    player.r = 20;
    player.VelLength = 1;
    //Camera
    Camera.Pos = { 150, 40, 0 };
    //Other
    ball.owner = NO;
    shootPosition.y = 0;
    audienceOption.Pos.y = 3;
}

void restart() {
    if (choose.GameMode == 1) {
        if (ball.owner == PIN) {
            com.Pos = { 100, 4, 0 };
            player.Pos = { 20, 4, 0 };
            Camera.LookDir = player.LookDir = { 1, 0, 0 };
            ball.owner = CPU;
            vec3D forward = Vector_Mul(com.LookDir, com.r);
            ball.Pos = Vector_Add(com.Pos, forward);
        }
        else {
            com.Pos = { 20, 4, 0 };
            player.Pos = { 100, 4, 0 };
            ball.owner = PL;
        }
    }
}
int main(int argc, char* args[])
{

    if (initSDL())
    {
        printf("Failed to initialize SDL!\n");
        return -1;
    }
    define();
    vec3D cloud[9];
    //define cloud Pos
    for (int i = 0; i < 9; i++) {
        cloud[i].x = -1000 + 1000 * (i % 3);
        cloud[i].y = 60 * (i % 3) + 200;
        cloud[i].z = -1000 + 1000 * (i / 3);
    }
    char POINT1[3] = "0", POINT2[3] = "0", SECOND[3] = "60", CLOCK[2] = "3";
    SDL_TimerID timerSEC = SDL_AddTimer(1000, Sec, NULL);
    SDL_TimerID timerID_ball = SDL_AddTimer(30, move_ball, &ball);
    SDL_TimerID judgeenter = SDL_AddTimer(50, Control_Time, &ball);
    SDL_TimerID moveai = SDL_AddTimer(30, move_ai, &com);
    SDL_TimerID timerMovePlayer = SDL_AddTimer(30, movePlayer, NULL);
    //SDL_TimerID owner = SDL_AddTimer(50, OWNER, NULL);
    SDL_TimerID timerCollisionPlayer = SDL_AddTimer(30, collisionPlayer, NULL);
    SDL_TimerID timerCloud = SDL_AddTimer(50, moveCloud, &cloud);
    SDL_TimerID timerAudience = SDL_AddTimer(50, audienceControl, NULL);

    object ball3d, balltrace, hoop, ground, court_out, ai, rack, bang, hoop_line, cloud_object, audience;
    object T_1, T_2, T_3, T_Y, T_N, T_Audiences, T_Easy, T_Normal, T_Hard, T_Mode, T_R, T_G, T_B;
    object T_Clouds, T_Color, T_Go, menuground, pillars;
    object Music_play, Music_stop;

    TextData win = loadTextTexture("You Win", "../fonts/A.ttf", 50, 0, 0, 0, BLENDED, NULL, NULL, NULL);
    TextData lose = loadTextTexture("You Lose", "../fonts/A.ttf", 50, 0, 0, 0, BLENDED, NULL, NULL, NULL);
    TextData point1 = loadTextTexture(POINT1, "../fonts/A.ttf", 20, 0, 0, 0, BLENDED, NULL, NULL, NULL);
    TextData point2 = loadTextTexture(POINT2, "../fonts/A.ttf", 20, 0, 0, 0, BLENDED, NULL, NULL, NULL);
    TextData Player = loadTextTexture("PLAYER", "../fonts/A.ttf", 20, 0, 0, 0, BLENDED, NULL, NULL, NULL);
    TextData computer = loadTextTexture("COMPUTER", "../fonts/A.ttf", 20, 0, 0, 0, BLENDED, NULL, NULL, NULL);
    TextData clock = loadTextTexture(CLOCK, "../fonts/A.ttf", 40, 0, 0, 0, BLENDED, NULL, NULL, NULL);
    char tone_Path[2][100] = { "../audio/c11.wav","../audio/BackgroundMusic.mp3" };
    tone = Mix_LoadWAV(tone_Path[0]);
    backgroundMusic = Mix_LoadMUS(tone_Path[1]);
    srand(time(NULL));
    if (tone == NULL) {
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
        printf("cannot open file");
    }
    if (backgroundMusic == NULL) {
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
        printf("cannot open file");
    }
    //menu world
    loadObject(T_3, "text_3.obj");
    loadObject(T_2, "text_2.obj");
    loadObject(T_1, "text_1.obj");
    loadObject(T_Audiences, "text_Audiences.obj");
    loadObject(T_B, "text_B.obj");
    loadObject(T_N, "text_N.obj");
    loadObject(T_Y, "text_Y.obj");
    loadObject(T_R, "text_R.obj");
    loadObject(T_G, "text_G.obj");
    loadObject(T_Clouds, "text_Clouds.obj");
    loadObject(T_Color, "text_color.obj");
    loadObject(menuground, "menuground.obj");
    loadObject(T_Easy, "text_Easy.obj");
    loadObject(T_Normal, "text_Normal.obj");
    loadObject(T_Hard, "text_Hard.obj");
    loadObject(T_Mode, "text_Mode.obj");
    loadObject(pillars, "pillar.obj");
    loadObject(audience, "audience.obj");

    loadObject(cloud_object, "clouds.obj");
    loadObject(hoop, "hoop.obj");
    loadObject(rack, "rack_10.obj");
    loadObject(ground, "court.obj");
    loadObject(hoop_line, "hoop_line.obj");
    loadObject(bang, "bang.obj");
    loadObject(balltrace, "balltrace.obj");
    loadObject(ball3d, "ball.obj");
    loadObject(ai, "player1.obj");
    loadObject(player.OBJ, "player1.obj");
    loadObject(player.Hands, "handsonplane.obj");
    loadObject(court_out, "court_out.obj");
    loadObject(Music_play, "Music_logo_play.obj");
    loadObject(Music_stop, "Music_logo_stop.obj");
    SDL_Event e;

    bool quit = false;

    //While application is running
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            switch (MODE) {
            case 0:
                if (e.type == SDL_KEYDOWN) {
                    vec3D tmp;
                    switch (e.key.keysym.sym) {
                    case SDLK_w:
                        player.Move.FORWARD = true;
                        break;
                    case SDLK_d:
                        player.Move.RIGHT = true;
                        break;
                    case SDLK_s:
                        player.Move.BACKWARD = true;
                        break;
                    case SDLK_a:
                        player.Move.LEFT = true;
                        break;
                    case SDLK_SPACE:
                        if (viewState != 2)
                            player.Move.JUMP = true;
                        else
                            player.Move.UP = true;
                        break;
                    case SDLK_x:
                        player.Move.DOWN = true;
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_v:
                        viewState = !viewState;
                        break;
                    case SDLK_g:
                        viewState = 2;
                        break;
                    case SDLK_f:
                        fixMouse = !fixMouse;
                        break;
                    case SDLK_p:
                        player.Pos = tmpPosition;
                        player.VelLength = 2;
                        MODE = 2;
                        break;
                    }
                }
                else if (e.type == SDL_KEYUP) {
                    switch (e.key.keysym.sym) {
                    case SDLK_w:
                        player.Move.FORWARD = false;
                        break;
                    case SDLK_d:
                        player.Move.RIGHT = false;
                        break;
                    case SDLK_s:
                        player.Move.BACKWARD = false;
                        break;
                    case SDLK_a:
                        player.Move.LEFT = false;
                        break;
                    case SDLK_SPACE:
                        player.Move.UP = false;
                        break;
                    case SDLK_x:
                        player.Move.DOWN = false;
                        break;
                    }
                }
                mouseHandleEvent(&e, &mouseState, &mouseX, &mouseY);
                break;
            case 1:

                break;
            case 2:
                if (e.type == SDL_KEYDOWN) {
                    vec3D tmp;
                    switch (e.key.keysym.sym) {
                    case SDLK_w:
                        player.Move.FORWARD = true;
                        break;
                    case SDLK_d:
                        player.Move.RIGHT = true;
                        break;
                    case SDLK_s:
                        player.Move.BACKWARD = true;
                        break;
                    case SDLK_a:
                        player.Move.LEFT = true;
                        break;
                    case SDLK_SPACE:
                        if (viewState != 2)
                            player.Move.JUMP = true;
                        else
                            player.Move.UP = true;
                        break;
                    case SDLK_x:
                        player.Move.DOWN = true;
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_v:
                        viewState = !viewState;
                        break;
                    case SDLK_g:
                        viewState = 2;
                        break;
                    case SDLK_r:
                        if (ball.owner == PL && viewState == 0)
                            ball.owner = PTN;
                        break;
                    case SDLK_q:
                        tmp = Vector_Mul(player.LookDir, 15);
                        ball.Pos = Vector_Add(player.Pos, tmp);
                        ball.Pos.y += 20;
                        jud = true;
                        ball.owner = PL;
                        break;
                    case SDLK_f:
                        fixMouse = !fixMouse;
                        break;
                    case SDLK_p:
                        tmpPosition = player.Pos;
                        player.Pos = { 0, 3, -120 };
                        MODE = 0;
                        player.VelLength = 1;
                        break;
                    }
                }
                else if (e.type == SDL_KEYUP) {
                    switch (e.key.keysym.sym) {
                    case SDLK_w:
                        player.Move.FORWARD = false;
                        break;
                    case SDLK_d:
                        player.Move.RIGHT = false;
                        break;
                    case SDLK_s:
                        player.Move.BACKWARD = false;
                        break;
                    case SDLK_a:
                        player.Move.LEFT = false;
                        break;
                    case SDLK_SPACE:
                        player.Move.UP = false;
                        break;
                    case SDLK_x:
                        player.Move.DOWN = false;
                        break;
                    case SDLK_r:
                        if (ball.owner == PTN && viewState == 0)
                        {
                            Dk.x = -145 - ball.Pos.x;
                            Dk.y = 69 - ball.Pos.y;
                            Dk.z = 0 - ball.Pos.z;
                            Vk = CalVel(Dk, TR);
                            Vector_Assign(ball.Vel, Vk);
                            ball.owner = PT;
                            Gravity = true;
                            touch = false;
                            TR = 0;
                        }
                        break;
                    }
                }
                mouseHandleEvent(&e, &mouseState, &mouseX, &mouseY);
                switch (mouseState)
                {
                case NONE:
                    break;
                case leftdown:
                    break;
                case leftup:
                    //the fuction of "jud" to avoid velocity of ball when moving the Forward
                    //initialize the velocity
                    if (ball.owner == PT && jud == true && viewState != 0) {
                        jud = false;
                        Gravity = true;
                        touch = false;
                        ball.owner = PT;
                        ball.Vel.x = TIME * 0.02 / player.VelLength * player.LookDir.x;
                        ball.Vel.y = TIME * 0.03 / player.VelLength * player.LookDir.y;
                        ball.Vel.z = TIME * 0.02 / player.VelLength * player.LookDir.z;
                        TIME = 0;
                    }
                    break;
                }
                break;//End of Case 2
            }// end of switch(mode)
        }// end of while (SDL_PollEvent(&e) != 0)
        if (MODE == 0) {
            SDL_SetRenderDrawColor(renderer, 0x87, 0xce, 0xeb, 0xFF);
            SDL_RenderClear(renderer);
            vector <triangle>  objectBuffer;

            loadToBuffer(objectBuffer, menuground, 0, 0, 0, 0x5F8CBB, 0xFF, 0, 0, 0);
            drawBuffer(renderer, objectBuffer);

            loadToBuffer(objectBuffer, pillars, 0, 0, 0, 0xFFFFFF, 0xFF, 0, 0, 0);
            if (player.Pos.z >= -110 && player.Pos.z <= -70)
            {
                loadToBuffer(objectBuffer, T_Easy, 40, 45, -80, choose.DIF == 1 ? 0xFF0000 : 0xFFFFFF, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_Normal, 5, 45, -80, choose.DIF == 2 ? 0xFF0000 : 0xFFFFFF, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_Hard, -33, 45, -80, choose.DIF == 3 ? 0xFF0000 : 0xFFFFFF, 0xFF, 0, 0, 0);
                if (Inbox(20, -90, 40, 20, player.Pos.x, player.Pos.z)) {
                    choose.DIF = 1;
                    com.VelLength = 0.0075;
                }
                else if (Inbox(-20, -90, 40, 20, player.Pos.x, player.Pos.z)) {
                    choose.DIF = 2;
                    com.VelLength = 0.015;
                }
                else if (Inbox(-60, -90, 40, 20, player.Pos.x, player.Pos.z)) {
                    choose.DIF = 3;
                    com.VelLength = 0.03;
                }
            }
            else if (player.Pos.z >= -70 && player.Pos.z <= -30)
            {
                loadToBuffer(objectBuffer, T_Mode, 3.5, 50, -40, 0xFFFFFF, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_1, 30, 45, -40, choose.GameMode == 1 ? 0xFF0000 : 0xFFFFFF, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_2, 0, 45, -40, choose.GameMode == 2 ? 0xFF0000 : 0xFFFFFF, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_3, -30, 45, -40, choose.GameMode == 3 ? 0xFF0000 : 0xFFFFFF, 0xFF, 0, 0, 0);
                if (Inbox(20, -50, 40, 20, player.Pos.x, player.Pos.z)) {
                    choose.GameMode = 1;
                }
                else if (Inbox(-20, -50, 40, 20, player.Pos.x, player.Pos.z)) {
                    choose.GameMode = 2;
                }
                else if (Inbox(-60, -50, 40, 20, player.Pos.x, player.Pos.z)) {
                    choose.GameMode = 3;
                }
            }
            else if (player.Pos.z >= -30 && player.Pos.z <= 10)
            {
                loadToBuffer(objectBuffer, T_Clouds, 4.5, 50, 0, 0xFFFFFF, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_Y, 25, 45, 0, choose.CLOUDS == 1 ? 0xFF0000 : 0xFFFFFF0, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_N, -25, 45, 0, choose.CLOUDS == 2 ? 0xFF0000 : 0xFFFFFF0, 0xFF, 0, 0, 0);
                if (Inbox(0, -10, 60, 20, player.Pos.x, player.Pos.z)) {
                    choose.CLOUDS = 1;
                }
                else if (Inbox(-60, -10, 60, 20, player.Pos.x, player.Pos.z)) {
                    choose.CLOUDS = 2;
                }
            }
            else if (player.Pos.z >= 10 && player.Pos.z <= 40)
            {
                loadToBuffer(objectBuffer, T_Audiences, 6.5, 50, 40, 0xFFFFFF, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_Y, 25, 45, 40, choose.AUDIENCE == 1 ? 0xFF0000 : 0xFFFFFF, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_N, -25, 45, 40, choose.AUDIENCE == 2 ? 0xFF0000 : 0xFFFFFF, 0xFF, 0, 0, 0);
                if (Inbox(0, 30, 60, 20, player.Pos.x, player.Pos.z)) {
                    choose.AUDIENCE = 1;
                }
                else if (Inbox(-60, 30, 60, 20, player.Pos.x, player.Pos.z)) {
                    choose.AUDIENCE = 2;
                }
            }
            else if (player.Pos.z >= 50 && player.Pos.z <= 90)
            {
                loadToBuffer(objectBuffer, T_Color, 3, 50, 80, 0xFFFFFF, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_R, 30, 45, 80, 0xFF0000, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_G, 0, 45, 80, 0x00FF00, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, T_B, -30, 45, 80, 0x0000FF, 0xFF, 0, 0, 0);
                if (Inbox(20, 70, 40, 10, player.Pos.x, player.Pos.z)) {
                    choose.C_PLAYER = 1;
                    player.Color += 0x020000;
                    if (player.Color / 65536 == 0xFE)
                        player.Color -= 0xFF0000;
                }
                else if (Inbox(-20, 70, 40, 10, player.Pos.x, player.Pos.z)) {
                    choose.C_PLAYER = 2;
                    player.Color += 0x000200;
                    if (player.Color / 256 % 256 == 0xFE)
                        player.Color -= 0x00FF00;
                }
                else if (Inbox(-60, 70, 40, 10, player.Pos.x, player.Pos.z)) {
                    choose.C_PLAYER = 3;
                    player.Color += 0x000002;
                    if (player.Color % 256 == 0xFE)
                        player.Color -= 0x0000FF;
                }

            }
            else if (player.Pos.z >= 120 && player.Pos.z <= 130)
            {
                MODE = 2;
                player.Pos = { 100, 1123, -150 };
                player.VelLength = 2;
                com.Pos = { 100, 1123 , 150 };
                ball.Pos = { 100 ,5 , 0 };
                if (choose.GameMode == 1)
                    stop = true;
                //頝喳?隞
            }

            loadToBuffer(objectBuffer, T_Go, -2, 45, 120, 0xFFFFFF, 0xFF, 0, 0, 0);
            if (choose.CLOUDS == 1) {
                for (int i = 0; i < 9; i++)
                    loadToBuffer(objectBuffer, cloud_object, cloud[i].x, cloud[i].y, cloud[i].z, 0xFFFFFF, 0x7F, 0, 0, 0);
            }
            if (choose.AUDIENCE == 1) {
                for (int i = 0; i < 5; i++) {
                    loadToBuffer(objectBuffer, audience, 65, 3, 40 + 15 * i, player.Color, 0xFF, 0, DtR(-90), 0);
                    loadToBuffer(objectBuffer, player.Hands, 65, 15, 40 + 15 * i, player.Color, 0xFF, DtR(225), DtR(-90), 0);
                    loadToBuffer(objectBuffer, audience, -65, 3, 40 + 15 * i, player.Color, 0xFF, 0, DtR(90), 0);
                    loadToBuffer(objectBuffer, player.Hands, -65, 15, 40 + 15 * i, player.Color, 0xFF, DtR(-135), DtR(90), 0);
                }
            }
            loadToBuffer(objectBuffer, player.OBJ, player.Pos.x, player.Pos.y, player.Pos.z, player.Color, player.alphar, 0, 0, 0);
            loadToBuffer(objectBuffer, player.Hands, player.Pos.x, player.Pos.y + 20, player.Pos.z, player.Color, player.alphar, player.AngleRad.x, player.AngleRad.y, 0);
            drawBuffer(renderer, objectBuffer);

            SDL_RenderPresent(renderer);
        }
        else if (MODE == 1) {

        }//end of if(mode ==1)
        else {
            if (ball.owner == PIN || ball.owner == CIN) {
                audienceOption.Vel.y = 4;
                restart();
                if (choose.GameMode == 1 && score1 != 2 && score2 != 10)
                    stop = true;
            }

            if (stop && com.Pos.y <= 3)
            {
                SDL_DestroyTexture(clock.texture);
                sprintf(CLOCK, "%d", CK);
                clock = loadTextTexture(CLOCK, "../fonts/A.ttf", 40, 255, 255, 255, BLENDED, NULL, NULL, NULL);
            }

            Vector_Assign(jball.Pos, ball.Pos);
            double distance;
            distance = sqrt((ball.Pos.x - com.Pos.x) * (ball.Pos.x - com.Pos.x) + (ball.Pos.z - com.Pos.z) * (ball.Pos.z - com.Pos.z));
            if (choose.GameMode == 1)
            {
                SDL_DestroyTexture(point1.texture);
                sprintf(POINT1, "%d", score1);
                point1 = loadTextTexture(POINT1, "../fonts/A.ttf", 20, 0, 0, 0, BLENDED, NULL, NULL, NULL);
                SDL_DestroyTexture(point2.texture);
                sprintf(POINT2, "%d", score2);
                point2 = loadTextTexture(POINT2, "../fonts/A.ttf", 20, 0, 0, 0, BLENDED, NULL, NULL, NULL);
            }
            SDL_SetRenderDrawColor(renderer, 0x87, 0xce, 0xeb, 0xFF);
            SDL_RenderClear(renderer);
            vector <triangle>  objectBuffer;

            loadToBuffer(objectBuffer, court_out, 0, 0, 0, 0x5F8CBB, 0xFF, 0, 0, 0);
            drawBuffer(renderer, objectBuffer);
            loadToBuffer(objectBuffer, ground, 0, 0, 0, 0x4c6044, 0xFF, 0, 0, 0);
            drawBuffer(renderer, objectBuffer);
            if (choose.CLOUDS == 1) {
                for (int i = 0; i < 9; i++)
                    loadToBuffer(objectBuffer, cloud_object, cloud[i].x, cloud[i].y, cloud[i].z, 0xFFFFFF, 0x7F, 0, 0, 0);
            }
            drawBuffer(renderer, objectBuffer);

            loadToBuffer(objectBuffer, rack, 0, 0, 0, 0x000000, 0xFF, 0, 0, 0);
            drawBuffer(renderer, objectBuffer);

            loadToBuffer(objectBuffer, bang, 0, 0, 0, 0xFFFFFF, 0xFF, 0, 0, 0);
            drawBuffer(renderer, objectBuffer);

            loadToBuffer(objectBuffer, hoop_line, 0, 0, 0, 0xFF0000, 0xFF, 0, 0, 0);
            drawBuffer(renderer, objectBuffer);
            loadToBuffer(objectBuffer, hoop, 0, 0, 0, 0xFFFFFF, 0xFF, 0, 0, 0);
            if (choose.GameMode != 2) {
                loadToBuffer(objectBuffer, ai, com.Pos.x, com.Pos.y, com.Pos.z, com.Color, 0xFF, 0, 0, 0);
                loadToBuffer(objectBuffer, player.Hands, com.Pos.x, com.Pos.y + 20, com.Pos.z, com.Color, 0xFF, com.AngleRad.x, com.AngleRad.y, 0);
            }
            loadToBuffer(objectBuffer, ball3d, ball.Pos.x, ball.Pos.y, ball.Pos.z, 0xFFB5B5, ball.owner == PTN ? 0x5F : 0xFF, 0, 0, 0);
            loadToBuffer(objectBuffer, player.OBJ, player.Pos.x, player.Pos.y, player.Pos.z, player.Color, player.alphar, 0, 0, 0);
            loadToBuffer(objectBuffer, player.Hands, player.Pos.x, player.Pos.y + 20, player.Pos.z, player.Color, player.alphar, player.AngleRad.x, player.AngleRad.y, 0);

            if (musicplay == -1) {
                loadToBuffer(objectBuffer, Music_stop, 0, 0, 200, 0x000000, 0xFF, 0, 0, 0);
                entermusic(player.Pos);
            }
            else if (musicplay == 1) {
                loadToBuffer(objectBuffer, Music_play, 0, 0, 200, 0x000000, 0xFF, 0, 0, 0);
                entermusic(player.Pos);
            }
            if (choose.AUDIENCE == 1) {
                for (int i = 0; i < 7; i++) {
                    loadToBuffer(objectBuffer, audience, -130 + 50 * i, audienceOption.Pos.y, -175, player.Color, 0xFF, 0, 0, 0);
                    loadToBuffer(objectBuffer, player.Hands, -130 + 50 * i, 12 + audienceOption.Pos.y, -175, player.Color, 0xFF, DtR(225), DtR(180), 0);
                    loadToBuffer(objectBuffer, audience, -130 + 50 * i, audienceOption.Pos.y, 175, com.Color, 0xFF, 0, 0, 0);
                    loadToBuffer(objectBuffer, player.Hands, -130 + 50 * i, 12 + audienceOption.Pos.y, 175, com.Color, 0xFF, DtR(225), 0, 0);
                }
            }
            if (ball.owner == PTN && viewState != 0) {
                vector <triangle> tmp_triangle;
                tmp_triangle = drawparabola(ball, balltrace);
                objectBuffer.insert(objectBuffer.end(), tmp_triangle.begin(), tmp_triangle.end());
            }
            drawBuffer(renderer, objectBuffer);
            if (choose.GameMode == 1)
            {
                textRender(renderer, Player, 950, 30, NULL, NULL, NULL, SDL_FLIP_NONE, 255);
                textRender(renderer, computer, 950, 50, NULL, NULL, NULL, SDL_FLIP_NONE, 255);
                textRender(renderer, point1, 1150, 30, NULL, NULL, NULL, SDL_FLIP_NONE, 255);
                textRender(renderer, point2, 1150, 50, NULL, NULL, NULL, SDL_FLIP_NONE, 255);
            }
            if (stop && com.Pos.y <= 3)
                textRender(renderer, clock, 600, 100, NULL, NULL, NULL, SDL_FLIP_NONE, 255);
            if (score1 == 2)
                textRender(renderer, win, 600, 100, NULL, NULL, NULL, SDL_FLIP_NONE, 255);
            if (score2 == 10)
                textRender(renderer, lose, 600, 100, NULL, NULL, NULL, SDL_FLIP_NONE, 255);
            if (Win)
            {
                player.Pos = { 0, 3, -120 };
                MODE = 0;
                player.VelLength = 1;
                Win = false;
            }
            boxColor(renderer, 1100, 500, 1110, 620, 0xCCFFAABB);
            if (TR <= 2400)
                boxColor(renderer, 1100, (TR <= 1200) ? (620 - TR / 10) : (500 + (TR - 1200) / 10), 1110, 620, 0xFF00FF00);
            SDL_RenderPresent(renderer);
        }//end of else if (MODE == 2)
    }//end of  while (!quit)
    SDL_DestroyTexture(Player.texture);
    SDL_DestroyTexture(computer.texture);
    SDL_DestroyTexture(point1.texture);
    SDL_DestroyTexture(point2.texture);

    closeSDL();
    return 0;
}