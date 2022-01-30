//
//  triangle.h
//  OOP
//
//  Created by 瘙 on 2021/5/20.
//
using namespace std;
#ifndef triangle_h
#define triangle_h
struct vec3D {
    double x = 0;
    double y = 0;
    double z = 0;
    double w = 1;
};
struct triangle {
    vec3D p[3];
    int r;
    int g;
    int b;
    int a = 0xFF;
};
struct object {
    vector<triangle> tris;
};
void loadObject(object& o, const char* Filename) {
    FILE* fp;
    if ((fp = fopen(Filename, "r")) == NULL) {
        printf("Can not open File %s!\n", Filename);
        exit(1);
    }
    vector<vec3D> verts;
    char line[100];
    rewind(fp);
    while (fgets(line, 100, fp)) {
        char tmp;
        if (line[0] == 'v') {
            vec3D v;
            sscanf(line, "%c %lf %lf %lf", &tmp, &v.x, &v.y, &v.z);
            verts.push_back(v);
        }
        else if (line[0] == 'f') {
            double f[3];
            sscanf(line, "%c %lf %lf %lf", &tmp, &f[0], &f[1], &f[2]);

            o.tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
        }
    }
    fclose(fp);
}
#endif /* triangle_h */
