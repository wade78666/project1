#include<stdio.h>
vec3D c;//the closest point between ball and rectangle on rectangle.
const double R_hoop = 10;
const double R_ball = 5;
const double r_hoop = 1;
const int hoopControlTimeDiv = 75;
bool collision_control = false;
bool Gravity = false;
bool computerjump = true;
vec3D hoopPosition = { -145, 71, 0, 1 };
vec3D vUp = { 0, 1, 0 }, vZ = { 0, 0, 1 }, vX = { 1, 0, 0 }, vDOWN = { 0 , -1 , 0 };
struct obstacle {
    vec3D Pos;
    double l;
    double w;
    double h;
};
enum Owner {
    NO = 0, CPU = 1, CTN = 2, CT = 3, CIN = 4, PL = 5, PTN = 6, PT = 7, PIN = 8, FLYC = 9, FLYP = 10, GP = 11, GC = 12
};
// NO : ball isn't moving // CPU : ball is on ai // CTN : ball will be shot by ai // CT : ball has be shot by ai // CIN : ball is scored by ai
// PL : ball is on player // PTN : ball will be shot by player // PT : ball has be shot by player // PIN : ball is scored by player // FLY : ball is moving
obstacle Check_Enter_Rec, Backboard;
struct moveState {
    bool FORWARD = false;
    bool BACKWARD = false;
    bool LEFT = false;
    bool RIGHT = false;
    bool UP = false;
    bool DOWN = false;
    bool JUMP = false;
};
struct Player {
    object OBJ, Hands;
    int alphar;
    vec3D Pos;
    vec3D Vel;
    vec3D LookDir;
    double VelLength;
    double r;
    vec3D AngleRad;
    moveState Move;
    int Color;
} com, player;
struct circle {
    vec3D Pos;
    vec3D Vel;
    double r;
    Owner owner;
}ball, jball, audienceOption;
double distanceObstacle(double x1, double x2, double y1, double y2, double z1, double z2) {
    double deltaX = x2 - x1;
    double deltaY = y2 - y1;
    double deltaZ = z2 - z1;
    return deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;
}
void CollisionResulthoop() {
    double fOfHoop = pow((R_hoop - sqrt(pow((ball.Pos.x - hoopPosition.x), 2) + pow(ball.Pos.z - hoopPosition.z, 2))), 2) + pow(ball.Pos.y - hoopPosition.y, 2) - (R_ball + r_hoop);
    if (fOfHoop < 35 && collision_control == false) {
        collision_control = true;
        vec3D vOfBall = { ball.Vel.x, ball.Vel.y, ball.Vel.z, 1 };
        vec3D vOfNormal, vOfReflect;
        vec3D tmp = { ball.Pos.x - hoopPosition.x, ball.Pos.y - hoopPosition.y, ball.Pos.z - hoopPosition.z, 1 };
        vOfNormal.x = -2 * tmp.x * (R_hoop - sqrt(tmp.x * tmp.x + tmp.z * tmp.z)) / sqrt(tmp.x * tmp.x + tmp.z * tmp.z);
        vOfNormal.z = -2 * tmp.z * (R_hoop - sqrt(tmp.x * tmp.x + tmp.z * tmp.z)) / sqrt(tmp.x * tmp.x + tmp.z * tmp.z);
        vOfNormal.y = 2 * tmp.y;
        vOfNormal = Vector_Normalise(vOfNormal);
        vOfReflect.x = vOfBall.x - 2 * (Vector_DotProduct(vOfBall, vOfNormal)) * vOfNormal.x;
        vOfReflect.y = vOfBall.y - 2 * (Vector_DotProduct(vOfBall, vOfNormal)) * vOfNormal.y;
        vOfReflect.z = vOfBall.z - 2 * (Vector_DotProduct(vOfBall, vOfNormal)) * vOfNormal.z;
        ball.Vel.x = vOfReflect.x;
        ball.Vel.y = vOfReflect.y;
        ball.Vel.z = vOfReflect.z;
        if (ball.owner == PT)
            ball.owner = FLYP;
        else if (ball.owner == CT)
            ball.owner = FLYC;
    }
}

bool checkCollision(circle& a, obstacle& b) {
    //x direction
    if (a.Pos.x < b.Pos.x) {
        c.x = b.Pos.x;
    }
    else if (a.Pos.x > b.Pos.x + b.l) {
        c.x = b.Pos.x + b.l;
    }
    else {
        c.x = a.Pos.x;
    }
    // y direction
    if (a.Pos.y < b.Pos.y) {
        c.y = b.Pos.y;
    }
    else if (a.Pos.y > b.Pos.y + b.h) {
        c.y = b.Pos.y + b.h;
    }
    else {
        c.y = a.Pos.y;
    }

    // z direction
    if (a.Pos.z < b.Pos.z) {
        c.z = b.Pos.z;
    }
    else if (a.Pos.z > b.Pos.z + b.w) {
        c.z = b.Pos.z + b.w;
    }
    else {
        c.z = a.Pos.z;
    }
    if (distanceObstacle(a.Pos.x, c.x, a.Pos.y, c.y, a.Pos.z, c.z) < a.r * a.r) {
        return true;
    }
    //or return false.
    return false;
}

bool checkenter(circle& a, obstacle& b) {
    circle temp = a;
    if (ball.Vel.y < 0) {
        for (double i = 0; i > ball.Vel.y; i -= 0.01) {
            temp.Pos.y -= 0.01;
            if (checkCollision(temp, b)) {
                return true;
            }
        }
    }
    return false;
}
bool collisionballandplayer(circle& a, Player& p) {
    if (Point_Distance_XZ(a.Pos, p.Pos) < 10 && (a.Pos.y < 30)) {
        return true;
    }
    return false;
}

void Moveball() {
    if ((collisionballandplayer(ball, player) || collisionballandplayer(ball, com)) && computerjump) {
        vec3D t;
        if (collisionballandplayer(ball, player))
            t = Vector_Sub(player.Pos, ball.Pos);
        else
            t = Vector_Sub(com.Pos, ball.Pos);
        if (!ball.Vel.x && !ball.Vel.z) {
            t = Vector_Mul(t, 0.2);
            ball.Pos = Vector_Sub(ball.Pos, t);
            ball.Pos.y = 5;
        }
        else {
            computerjump = false;
            ball.Vel.x = -Vector_Length_XZ(ball.Vel) * t.x * 0.018;
            ball.Vel.z = -Vector_Length_XZ(ball.Vel) * t.z * 0.018;
        }
    }
    if (ball.owner == PT || ball.owner == CT || ball.owner == PIN || ball.owner == FLYP || ball.owner == FLYC || ball.owner == GP || ball.owner == GC || ball.owner == CIN) {
        if (ball.Vel.x > 0) {
            for (double i = 0; i < ball.Vel.x; i += 0.01) {
                ball.Pos.x += 0.01;
                CollisionResulthoop();
            }
        }
        else if (ball.Vel.x < 0) {
            for (double i = 0; i > ball.Vel.x; i -= 0.01) {
                ball.Pos.x -= 0.01;
                CollisionResulthoop();
            }
        }
        ball.Pos.z += ball.Vel.z;
        ball.Pos.y += ball.Vel.y;
    }
}
void Collision_boundary() {
    if (ball.Pos.y - ball.r <= 0 && ball.Vel.y < 0) {
        ball.Vel.y *= -0.9;
        ball.Vel.x *= 0.9;
        ball.Vel.z *= 0.9;
        if (ball.owner == PT)
            ball.owner = GP;
        else if (ball.owner == CT)
            ball.owner = GC;
        if (Vector_Length(ball.Vel) <= 2.0) {// if the velocity of ball smaller than a constant then it stop.
            ball.Vel.x = 0;
            ball.Vel.y = 0;
            ball.Vel.z = 0;
            Gravity = false;
            if (ball.owner != PL && ball.owner != CPU)
                ball.owner = NO;
        }
    }
    //when ball hit the right wall
    if (ball.Pos.x < -152 && ball.Vel.x < 0) {
        //printf("hit -x axis\n");
        ball.Vel.y *= 0.9;
        ball.Vel.x *= -0.9;
        ball.Vel.z *= 0.9;
        if (ball.owner == PT)
            ball.owner = FLYP;
        else if (ball.owner == CT)
            ball.owner = FLYC;
    }
    //when ball hit the left wall.
    else if (ball.Pos.x > 200 && ball.Vel.x > 0) {
        //printf("hit +x axis\n");
        ball.Vel.y *= 0.9;
        ball.Vel.x *= -0.9;
        ball.Vel.z *= 0.9;
        if (ball.owner == PT)
            ball.owner = FLYP;
        else if (ball.owner == CT)
            ball.owner = FLYC;
    }
    else if (ball.Pos.z < -150 && ball.Vel.z < 0) {
        //printf("hit -z axis\n");
        ball.Vel.y *= 0.9;
        ball.Vel.x *= 0.9;
        ball.Vel.z *= -0.9;
        if (ball.owner == PT)
            ball.owner = FLYP;
        else if (ball.owner == CT)
            ball.owner = FLYC;
    }
    //when ball hit the left wall.
    else if (ball.Pos.z > 150 && ball.Vel.z > 0) {
        //printf("hit +z axis\n");
        ball.Vel.y *= 0.95;
        ball.Vel.x *= 0.95;
        ball.Vel.z *= -0.85;
        if (ball.owner == PT)
            ball.owner = FLYP;
        else if (ball.owner == CT)
            ball.owner = FLYC;
    }
}

bool checkColPlayer(circle& a, Player& p) {
    if (Point_Distance_XZ(a.Pos, p.Pos) < p.r && (a.Pos.y < 35 + a.r + p.Pos.y)) {
        return true;
    }
    return false;
}
bool Check_Dis(vec3D& a, vec3D& b, double dis) {
    if (Point_Distance_XZ(a, b) < dis) {
        return true;
    }
    return false;
}
bool Grab_ball(circle& b, Player& a) {
    circle temp;
    temp.Pos.x = a.Pos.x + a.LookDir.x * a.r;
    temp.Pos.y = a.Pos.y + 20 + a.LookDir.y * a.r;
    temp.Pos.z = a.Pos.z + a.LookDir.z * a.r;
    temp.r = 5;
    if (Point_Distance(temp.Pos, b.Pos) < temp.r + b.r) {
        return true;
    }
    return false;
}
