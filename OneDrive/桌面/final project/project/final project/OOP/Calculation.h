//
//  Calculation.h
//  OOP
//
//  Created by ??謒?on 2021/5/21.
#ifndef Calculation_h
#define Calculation_h
#define max(a,b) (a>b?a:b)
double DtR(int n) {
    return (n / 180.) * M_PI;
}
struct mat4x4 {
    double m[4][4] = { 0 };
};
vec3D Matrix_MultiplyVector(mat4x4& m, vec3D& i) {
    vec3D v;
    v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
    v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
    return v;
}
mat4x4 Matrix_MultiplyMatrix(mat4x4& m1, mat4x4& m2) {
    mat4x4 matrix;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
    return matrix;
}
mat4x4 Matrix_MakeIdentity() {
    mat4x4 matrix;
    matrix.m[0][0] = 1;
    matrix.m[1][1] = 1;
    matrix.m[2][2] = 1;
    matrix.m[3][3] = 1;
    return matrix;
}

mat4x4 Matrix_MakeRotationX(double AngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = 1;
    matrix.m[1][1] = cos(AngleRad);
    matrix.m[1][2] = sin(AngleRad);
    matrix.m[2][1] = -sin(AngleRad);
    matrix.m[2][2] = cos(AngleRad);
    matrix.m[3][3] = 1;
    return matrix;
}

mat4x4 Matrix_MakeRotationY(double AngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = cos(AngleRad);
    matrix.m[0][2] = sin(AngleRad);
    matrix.m[2][0] = -sin(AngleRad);
    matrix.m[1][1] = 1;
    matrix.m[2][2] = cos(AngleRad);
    matrix.m[3][3] = 1;
    return matrix;
}

mat4x4 Matrix_MakeRotationZ(double AngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = cos(AngleRad);
    matrix.m[0][1] = sin(AngleRad);
    matrix.m[1][0] = -sin(AngleRad);
    matrix.m[1][1] = cos(AngleRad);
    matrix.m[2][2] = 1;
    matrix.m[3][3] = 1;
    return matrix;
}

mat4x4 Matrix_MakeTranslation(double x, double y, double z) {
    mat4x4 matrix;
    matrix.m[0][0] = 1;
    matrix.m[1][1] = 1;
    matrix.m[2][2] = 1;
    matrix.m[3][3] = 1;
    matrix.m[3][0] = x;
    matrix.m[3][1] = y;
    matrix.m[3][2] = z;
    return matrix;
}

mat4x4 Matrix_MakeProjection(double FovDegrees, double AspectRatio, double Near, double Far) {
    double FovRad = 1.0 / tan(FovDegrees * 0.5 / 180 * M_PI);
    mat4x4 matrix;
    matrix.m[0][0] = AspectRatio * FovRad;
    matrix.m[1][1] = FovRad;
    matrix.m[2][2] = Far / (Far - Near);
    matrix.m[3][2] = (-Far * Near) / (Far - Near);
    matrix.m[2][3] = 1;
    matrix.m[3][3] = 0;
    return matrix;
}

vec3D Vector_Add(vec3D& v1, vec3D& v2) {
    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

vec3D Vector_Sub(vec3D& v1, vec3D& v2) {
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

vec3D Vector_Mul(vec3D& v1, double k) {
    return { v1.x * k, v1.y * k, v1.z * k };
}

vec3D Vector_Div(vec3D& v1, double k) {
    return { v1.x / k, v1.y / k, v1.z / k };
}

void Vector_Assign(vec3D& v1, const vec3D& v2) {
    v1.x = v2.x;
    v1.y = v2.y;
    v1.z = v2.z;
}
double Vector_DotProduct(vec3D& v1, vec3D& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

double Vector_Length(vec3D& v) {
    return sqrt(Vector_DotProduct(v, v));
}

double Vector_Length_XZ(vec3D& v) {
    return sqrt(v.x * v.x + v.z * v.z);
}
double Point_Distance(vec3D& v1, vec3D& v2) {
    return sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y) + (v1.z - v2.z) * (v1.z - v2.z));
}

double Point_Distance_XZ(vec3D& v1, vec3D& v2) {
    return sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.z - v2.z) * (v1.z - v2.z));
}
vec3D Vector_Normalise(vec3D& v)
{
    double l = Vector_Length(v);
    return { v.x / l, v.y / l, v.z / l };
}

vec3D Vector_Normalise_XZ(vec3D& v)
{
    double l = Vector_Length_XZ(v);
    return { v.x / l, v.y / l, v.z / l };
}
vec3D Vector_CrossProduct(vec3D& v1, vec3D& v2)
{
    vec3D v;
    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;
    return v;
}

mat4x4 Matrix_PointAt(vec3D& pos, vec3D& target, vec3D& up) {
    //Calculate new forward direction
    vec3D newForward = Vector_Sub(target, pos);
    newForward = Vector_Normalise(newForward);

    //Calculate new Up direction
    vec3D a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
    vec3D newUp = Vector_Sub(up, a);
    newUp = Vector_Normalise(newUp);

    //New Right direction is easy, its just cross product
    vec3D newRight = Vector_CrossProduct(newUp, newForward);

    //Construct Dimensioning and Translation Matrix
    mat4x4 matrix;
    matrix.m[0][0] = newRight.x;  matrix.m[0][1] = newRight.y;  matrix.m[0][2] = newRight.z;  matrix.m[0][3] = 0;
    matrix.m[1][0] = newUp.x;     matrix.m[1][1] = newUp.y;     matrix.m[1][2] = newUp.z;     matrix.m[1][3] = 0;
    matrix.m[2][0] = newForward.x; matrix.m[2][1] = newForward.y; matrix.m[2][2] = newForward.z; matrix.m[2][3] = 0;
    matrix.m[3][0] = pos.x;     matrix.m[3][1] = pos.y;     matrix.m[3][2] = pos.z;       matrix.m[3][3] = 1;
    return matrix;
}

mat4x4 Matrix_QuickInverse(mat4x4& m) {
    // Only for Rotation/Translation Matrices
    mat4x4 matrix;
    matrix.m[0][0] = m.m[0][0];     matrix.m[0][1] = m.m[1][0];     matrix.m[0][2] = m.m[2][0];     matrix.m[0][3] = 0;
    matrix.m[1][0] = m.m[0][1];     matrix.m[1][1] = m.m[1][1];     matrix.m[1][2] = m.m[2][1];     matrix.m[1][3] = 0;
    matrix.m[2][0] = m.m[0][2];     matrix.m[2][1] = m.m[1][2];     matrix.m[2][2] = m.m[2][2];     matrix.m[2][3] = 0;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1;
    return matrix;
}


vec3D Vector_IntersectPlane(vec3D& plane_p, vec3D& plane_n, vec3D& lineStart, vec3D& lineEnd) {
    plane_n = Vector_Normalise(plane_n);
    double plane_d = -Vector_DotProduct(plane_n, plane_p);
    double ad = Vector_DotProduct(lineStart, plane_n);
    double bd = Vector_DotProduct(lineEnd, plane_n);
    double t = (-plane_d - ad) / (bd - ad);
    vec3D lineStartToEnd = Vector_Sub(lineEnd, lineStart);
    vec3D lineToIntersect = Vector_Mul(lineStartToEnd, t);
    return Vector_Add(lineStart, lineToIntersect);
}

vec3D CalVel(vec3D D)
{
    vec3D v;
    v.x = D.x / 30.;
    v.z = D.z / 30.;
    v.y = (D.y + 135) / 30.;
    return v;
}
vec3D CalVel(vec3D D, int T)
{
    vec3D v;
    double t;
    t = T / 60000.;
    t += 0.98;
    v.x = t * D.x / 50.;
    v.z = t * D.z / 50.;
    v.y = t * (D.y + 375) / 50.;
    return v;
}
#endif /* Calculation_h */
