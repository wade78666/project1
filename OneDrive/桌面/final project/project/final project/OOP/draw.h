//
//  draw.h
//  OOP
//
//  Created by ¦¿±á on 2021/6/24.
//
#include<algorithm>
//Screen dimension constants
const int WIDTH = 1200;
const int HEIGHT = 700;
int mouseX = WIDTH / 2, mouseY = HEIGHT / 2;
Player Camera;
int Yaw = 0, Pitch = 0;
mat4x4 matProj = Matrix_MakeProjection(90, (double)HEIGHT / WIDTH, 0.1, 1000);
#ifndef draw_h
#define draw_h

int Triangle_ClipAgainstPlane(vec3D plane_p, vec3D plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2) {
    plane_n = Vector_Normalise(plane_n);

    vec3D* inside_points[3];  int InsidePointCount = 0;
    vec3D* outside_points[3]; int OutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    double d0 = Vector_DotProduct(plane_n, in_tri.p[0]) - Vector_DotProduct(plane_n, plane_p);
    double d1 = Vector_DotProduct(plane_n, in_tri.p[1]) - Vector_DotProduct(plane_n, plane_p);
    double d2 = Vector_DotProduct(plane_n, in_tri.p[2]) - Vector_DotProduct(plane_n, plane_p);

    if (d0 >= 0)
        inside_points[InsidePointCount++] = &in_tri.p[0];
    else
        outside_points[OutsidePointCount++] = &in_tri.p[0];
    if (d1 >= 0)
        inside_points[InsidePointCount++] = &in_tri.p[1];
    else
        outside_points[OutsidePointCount++] = &in_tri.p[1];
    if (d2 >= 0)
        inside_points[InsidePointCount++] = &in_tri.p[2];
    else
        outside_points[OutsidePointCount++] = &in_tri.p[2];

    // Now classify triangle points, and break the input triangle into
    // smaller output triangles if required. There are four possible
    // outcomes...

    if (InsidePointCount == 0) {
        // All points lie on the outside of plane, so clip whole triangle

        return 0; // No returned triangles are valid
    }

    if (InsidePointCount == 3) {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simply pass through
        out_tri1 = in_tri;

        return 1; // Just the one returned original triangle is valid
    }

    if (InsidePointCount == 1 && OutsidePointCount == 2) {
        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        out_tri1.r = in_tri.r;
        out_tri1.g = in_tri.g;
        out_tri1.b = in_tri.b;
        out_tri1.a = in_tri.a;
        // The inside point is valid, so keep that...
        out_tri1.p[0] = *inside_points[0];

        // but the two new points are at the locations where the
        // original sides of the triangle (lines) intersect with the plane
        out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

        return 1; // Return the newly formed single triangle
    }

    if (InsidePointCount == 2 && OutsidePointCount == 1) {
        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped triangle becomes a "quad".

        // Copy appearance info to new triangles
        out_tri1.r = in_tri.r;
        out_tri1.g = in_tri.g;
        out_tri1.b = in_tri.b;
        out_tri1.a = in_tri.a;
        out_tri2.r = in_tri.r;
        out_tri2.g = in_tri.g;
        out_tri2.b = in_tri.b;
        out_tri2.a = in_tri.a;
        // The first triangle consists of the two inside points and a new
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.p[0] = *inside_points[0];
        out_tri1.p[1] = *inside_points[1];
        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the
        // triangle and the plane, and the newly created point above
        out_tri2.p[0] = *inside_points[1];
        out_tri2.p[1] = out_tri1.p[2];
        out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

        return 2; // Return two newly formed triangles which form a quad
    }
    return 0;
}

vector <triangle> calculateObject(object o, double x, double y, double z, double AngleRadX, double AngleRadY, double AngleRadZ) {
    int i;
    int I;
    //Set up Rotation Matrix
    mat4x4 matRotZ, matRotX, matRotY;
    matRotZ = Matrix_MakeRotationZ(AngleRadZ);
    matRotX = Matrix_MakeRotationX(AngleRadX);
    matRotY = Matrix_MakeRotationY(AngleRadY);

    //Set up the translation Matrix
    //This one for moving the object
    mat4x4 matTrans;
    matTrans = Matrix_MakeTranslation(x, y, z);

    //Multply all the matrices, after that, we can use matWorld do everything above
    mat4x4 matWorld;
    matWorld = Matrix_MakeIdentity();
    matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX);
    matWorld = Matrix_MultiplyMatrix(matWorld, matRotY);
    matWorld = Matrix_MultiplyMatrix(matWorld, matTrans);

    vec3D Up = { 0, 1, 0 };
    vec3D Target = { 0, 0, 1 };
    mat4x4 TMP1 = Matrix_MakeRotationY(DtR(Yaw));
    mat4x4 TMP2 = Matrix_MakeRotationX(DtR(Pitch));
    mat4x4 matCameraRot = Matrix_MultiplyMatrix(TMP2, TMP1);
    Camera.LookDir = player.LookDir = Matrix_MultiplyVector(matCameraRot, Target);
    Target = Vector_Add(Camera.Pos, Camera.LookDir);
    mat4x4 matCamera = Matrix_PointAt(Camera.Pos, Target, Up);

    //Make view martix from camera
    mat4x4 matView = Matrix_QuickInverse(matCamera);

    //This vector keeps the datas of the surfaces which we should show up in the screen.
    vector<triangle> vecTrianglesToRaster;
    for (I = 0; I < o.tris.size(); I++) {
        triangle tri = o.tris[I];
        triangle triProjected, triTransformed, triViewed;

        for (i = 0; i < 3; i++) {
            triTransformed.p[i] = Matrix_MultiplyVector(matWorld, tri.p[i]);
        }

        //Calculate normal
        vec3D normal, line1, line2;
        //Get lines either side of triangle
        line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
        line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

        //Take cross product of lines to get normal to triangle surface
        normal = Vector_CrossProduct(line1, line2);

        //Normalise the normal
        normal = Vector_Normalise(normal);
        //Get Ray from triangle to camera
        vec3D CameraRay = Vector_Sub(triTransformed.p[0], Camera.Pos);
        if (Vector_DotProduct(CameraRay, normal) < 0) {
            //Illumination
            vec3D light_direction = { 0.5, 1, -1 };// to change the light direction
            light_direction = Vector_Normalise(light_direction);
            triViewed.r = tri.r * max(0.2, Vector_DotProduct(light_direction, normal));;
            triViewed.g = tri.g * max(0.2, Vector_DotProduct(light_direction, normal));;
            triViewed.b = tri.b * max(0.2, Vector_DotProduct(light_direction, normal));;
            triViewed.a = tri.a;

            for (i = 0; i < 3; i++) {
                //Convert World Space --> View Space
                triViewed.p[i] = Matrix_MultiplyVector(matView, triTransformed.p[i]);
            }

            int ClippedTriangles = 0;
            triangle clipped[2];
            ClippedTriangles = Triangle_ClipAgainstPlane({ 0, 0, 0.1 }, { 0, 0, 1 }, triViewed, clipped[0], clipped[1]);

            for (int n = 0; n < ClippedTriangles; n++) {

                for (i = 0; i < 3; i++) {
                    //Project from 3D to 2D
                    triProjected.p[i] = Matrix_MultiplyVector(matProj, clipped[n].p[i]);
                    triProjected.r = clipped[n].r;
                    triProjected.g = clipped[n].g;
                    triProjected.b = clipped[n].b;
                    triProjected.a = clipped[n].a;
                    triProjected.p[i] = Vector_Div(triProjected.p[i], triProjected.p[i].w);
                }
                for (i = 0; i < 3; i++) {
                    //Our Screen center is not (0, 0)
                    //So we shouble offset verts into visible normalised space
                    //printf("%lf %lf %lf\n", triProjected.p[i].x, triProjected.p[i].y, triProjected.p[i].z);
                    triProjected.p[i].y *= -1;
                    triProjected.p[i].x *= -1;
                    vec3D OffsetView = { 1,1,0 };
                    triProjected.p[i] = Vector_Add(triProjected.p[i], OffsetView);

                    triProjected.p[i].x *= 0.5 * WIDTH;
                    triProjected.p[i].y *= 0.5 * HEIGHT;
                }
                // Store triangle
                vecTrianglesToRaster.push_back(triProjected);
            }
        }
    }
    return vecTrianglesToRaster;
}


void drawBuffer(SDL_Renderer* renderer, vector <triangle>& tri) {
    sort(tri.begin(), tri.end(), [](triangle& t1, triangle& t2)
        {
            double z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0;
            double z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0;
            return z1 > z2;
        });
    int I, J;
    for (I = 0; I < tri.size(); I++) {
        triangle triToRaster = tri[I];
        triangle clipped[2];
        vector<triangle> vectorTriangles;
        vectorTriangles.push_back(triToRaster);
        int newTriangles = 1;
        for (int p = 0; p < 4; p++) {
            int trisToAdd = 0;
            while (newTriangles > 0) {
                triangle tmp = vectorTriangles.front();
                vectorTriangles.erase(vectorTriangles.begin());
                newTriangles--;

                switch (p) {
                case 0:
                    trisToAdd = Triangle_ClipAgainstPlane({ 0, 0, 0 }, { 0, 1, 0 }, tmp, clipped[0], clipped[1]);
                    break;
                case 1:
                    trisToAdd = Triangle_ClipAgainstPlane({ 0, (double)HEIGHT - 1, 0 }, { 0, -1, 0 }, tmp, clipped[0], clipped[1]);
                    break;
                case 2:
                    trisToAdd = Triangle_ClipAgainstPlane({ 0, 0, 0 }, { 1, 0, 0 }, tmp, clipped[0], clipped[1]);
                    break;
                case 3:
                    trisToAdd = Triangle_ClipAgainstPlane({ (double)WIDTH - 1, 0, 0 }, { -1, 0, 0 }, tmp, clipped[0], clipped[1]);
                    break;
                }
                for (int w = 0; w < trisToAdd; w++)
                    vectorTriangles.push_back(clipped[w]);
            }
            newTriangles = vectorTriangles.size();
        }
        for (J = 0; J < vectorTriangles.size(); J++) {
            triangle tri = vectorTriangles[J];
            //filledTrigonRGBA(renderer, tri.p[0].x, tri.p[0].y, tri.p[1].x, tri.p[1].y, tri.p[2].x, tri.p[2].y, tri.r, tri.g, tri.b, tri.a);
            trigonRGBA(renderer, tri.p[0].x, tri.p[0].y, tri.p[1].x, tri.p[1].y, tri.p[2].x, tri.p[2].y, 0, tri.b, 0, 0xFF);
        }
    }
    tri.clear();
}

void loadToBuffer(vector<triangle>& everything, object& o, double x, double y, double z, int COLOR, int ALPHAR, double AngleRadX, double AngleRadY, double AngleRadZ) {
    int RED, GREEN, BLUE, temp_COLOR = COLOR;
    BLUE = temp_COLOR % 256;
    temp_COLOR /= 256;
    GREEN = temp_COLOR % 256;
    temp_COLOR /= 256;
    RED = temp_COLOR % 256;
    vector <triangle> tmp;
    for (int i = 0; i < o.tris.size(); i++) {
        o.tris[i].r = RED;
        o.tris[i].g = GREEN;
        o.tris[i].b = BLUE;
        o.tris[i].a = ALPHAR;
    }
    tmp = calculateObject(o, x, y, z, AngleRadX, AngleRadY, AngleRadZ);
    everything.insert(everything.end(), tmp.begin(), tmp.end());
}

#endif /* draw_h */
