#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_PATCHES 10000
#define MAX_POINTS 50000
#define EPSILON 1e-6

typedef struct {
    double x, y, z;
} Point;

typedef struct {
    int indices[16];
} Patch;

int nearly_equal(double a, double b) {
    return fabs(a - b) < EPSILON;
}

int point_equals(Point a, Point b) {
    return nearly_equal(a.x, b.x) && nearly_equal(a.y, b.y) && nearly_equal(a.z, b.z);
}

Point spherical_point(double alpha, double beta, double r) {
    Point p;
    p.x = r * cos(beta) * sin(alpha);
    p.y = r * sin(beta);
    p.z = r * cos(beta) * cos(alpha);
    return p;
}

Point add_irregularity(Point p, double base_radius, double max_height) {
    double norm = sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
    double dx = p.x / norm;
    double dy = p.y / norm;
    double dz = p.z / norm;
    double r = base_radius + ((double)rand() / RAND_MAX) * 2 * max_height - max_height;

    Point irregular;
    irregular.x = dx * r;
    irregular.y = dy * r;
    irregular.z = dz * r;
    return irregular;
}

Point rotate_point_y(Point p, double angle) {
    Point r;
    r.x = cos(angle) * p.x + sin(angle) * p.z;
    r.y = p.y;
    r.z = -sin(angle) * p.x + cos(angle) * p.z;
    return r;
}

Point transform_bottom(Point p) {
    Point t;
    t.x = p.z;
    t.y = -p.y;
    t.z = p.x;
    return t;
}

int find_or_add_point(Point p, Point* points, int* num_points) {
    for (int i = 0; i < *num_points; ++i) {
        if (point_equals(points[i], p)) {
            return i;
        }
    }
    points[*num_points] = p;
    return (*num_points)++;
}

void generate_patch(double alpha, double beta, double da, double db, double radius, double max_height, Point* points, int* num_points, Patch* patches, int* patch_count) {
    Point top[16];
    int idx = 0;
    for (int i = 0; i < 4; i++) {
        double a = alpha + i * da;
        for (int j = 0; j < 4; j++) {
            double b = beta + j * db;
            Point p = spherical_point(a, b, radius);
            if ((i == 1 || i == 2) && (j == 1 || j == 2)) {
                p = add_irregularity(p, radius, max_height);
            }
            top[idx++] = p;
        }
    }

    Patch top_patch;
    for (int i = 0; i < 16; i++) {
        top_patch.indices[i] = find_or_add_point(top[i], points, num_points);
    }
    patches[(*patch_count)++] = top_patch;

    // bottom patch (rotated)
    Patch bottom_patch;
    for (int i = 0; i < 16; i++) {
        Point p = transform_bottom(top[i]);
        p = rotate_point_y(p, da * 1.5);
        bottom_patch.indices[i] = find_or_add_point(p, points, num_points);
    }
    patches[(*patch_count)++] = bottom_patch;
}

int main(int argc, char* argv[]) {
    if (argc < 7) {
        printf("Uso: %s <seed> <raio> <altura_max> <slices> <stacks> <ficheiro_saida>\n", argv[0]);
        return 1;
    }

    int seed = atoi(argv[1]);
    double radius = atof(argv[2]);
    double max_height = atof(argv[3]);
    int slices = atoi(argv[4]);
    int stacks = atoi(argv[5]);
    char* filename = argv[6];

    srand(seed);

    double da = 2 * M_PI / slices / 3.0;
    double db = (M_PI / 2.0) / stacks / 3.0;

    Point points[MAX_POINTS];
    Patch patches[MAX_PATCHES];
    int num_points = 0;
    int patch_count = 0;

    for (int i = 0; i < slices; ++i) {
        for (int j = 0; j < stacks; ++j) {
            double alpha = i * 3 * da;
            double beta = j * 3 * db;
            generate_patch(alpha, beta, da, db, radius, max_height, points, &num_points, patches, &patch_count);
        }
    }

    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao abrir ficheiro");
        return 1;
    }

    fprintf(file, "%d\n", patch_count);
    for (int i = 0; i < patch_count; ++i) {
        for (int j = 0; j < 16; ++j) {
            fprintf(file, "%d", patches[i].indices[j]);
            if (j < 15) fprintf(file, ", ");
        }
        fprintf(file, "\n");
    }

    fprintf(file, "%d\n", num_points);
    for (int i = 0; i < num_points; ++i) {
        fprintf(file, "%.7f, %.7f, %.7f\n", points[i].x, points[i].y, points[i].z);
    }

    fclose(file);
    printf("Ficheiro '%s' criado com sucesso!\n", filename);
    return 0;
}
