#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>  

#define MAX_PATCHES 10000
#define MAX_POINTS 50000
#define EPSILON 1e-6
#define M_PI 3.14159265358979323846 

typedef struct {
    double x, y, z;
} Point;

typedef struct {
    int indices[16];
} Patch;

int is_almost_equal(double a, double b) {
    return fabs(a - b) < EPSILON;
}

int are_points_equal(Point a, Point b) {
    return is_almost_equal(a.x, b.x) && is_almost_equal(a.y, b.y) && is_almost_equal(a.z, b.z);
}

Point spherical_to_cartesian(double alpha, double beta, double radius) {
    Point p;
    p.x = radius * cos(beta) * sin(alpha);
    p.y = radius * sin(beta);
    p.z = radius * cos(beta) * cos(alpha);
    return p;
}

Point add_random_variation(Point p, double base_radius, double max_deviation) {
    double length = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    double unit_x = p.x / length;
    double unit_y = p.y / length;
    double unit_z = p.z / length;

    double new_radius = base_radius + (rand() / (double)RAND_MAX) * 2 * max_deviation - max_deviation;

    Point modified_point;
    modified_point.x = unit_x * new_radius;
    modified_point.y = unit_y * new_radius;
    modified_point.z = unit_z * new_radius;
    return modified_point;
}

int find_or_insert_point(Point p, Point* points, int* num_points) {
    for (int i = 0; i < *num_points; ++i) {
        if (are_points_equal(points[i], p)) {
            return i;
        }
    }
    points[*num_points] = p;
    return (*num_points)++;
}

void generate_comet_geometry(double alpha, double beta, double delta_alpha, double delta_beta, double radius, double max_deviation, Point* points, int* num_points, Patch* patches, int* num_patches) {
    Point vertices[16];
    int idx = 0;

    // geração de pontos (sem divisão em hemisférios)
    for (int i = 0; i < 4; ++i) {
        double a = alpha + i * delta_alpha;
        for (int j = 0; j < 4; ++j) {
            double b = beta + j * delta_beta;
            Point p = spherical_to_cartesian(a, b, radius);
            if ((i == 1 || i == 2) && (j == 1 || j == 2)) {
                p = add_random_variation(p, radius, max_deviation);
            }
            vertices[idx++] = p;
        }
    }

    // criar o patch superior (Hemisfério Norte) com orientação anti-horária
    Patch patch_upper;
    idx = 0;
    for (int i = 0; i < 4; ++i) { 
        for (int j = 0; j < 4; ++j) {
            // inverter a ordem dos índices (i1, i2, i3 -> i3, i2, i1)
            patch_upper.indices[idx++] = find_or_insert_point(vertices[i * 4 + (3 - j)], points, num_points); // inverter a ordem das colunas
        }
    }

    patches[(*num_patches)++] = patch_upper;

    
    Patch patch_lower;
    idx = 0;
    for (int i = 0; i < 4; ++i) {
        double a = alpha + i * delta_alpha;
        for (int j = 0; j < 4; ++j) {
            double b = -(beta + j * delta_beta); 
            Point p = spherical_to_cartesian(a, b, radius);
            if ((i == 1 || i == 2) && (j == 1 || j == 2)) {
                p = add_random_variation(p, radius, max_deviation);
            }
            vertices[idx++] = p;
        }
    }


    Patch patch_lower_corrected;
    idx = 0;
    for (int i = 0; i < 4; ++i) { 
        for (int j = 0; j < 4; ++j) { 
            // Inverter a ordem dos índices (i1, i2, i3 -> i3, i2, i1)
            patch_lower_corrected.indices[idx++] = find_or_insert_point(vertices[(3 - i) * 4 + (3 - j)], points, num_points); // inverter a ordem das linhas e das colunas
        }
    }

    patches[(*num_patches)++] = patch_lower_corrected;
}



void write_patch_file(const char* filename, Patch* patches, int num_patches, Point* points, int num_points) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return;
    }

    fprintf(file, "%d\n", num_patches);
    for (int i = 0; i < num_patches; ++i) {
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
    printf("Ficheiro '%s' gerado com sucesso!\n", filename);
}

int main() {

    int seed = (int)time(NULL); 
    double radius = 1.0; 
    double max_deviation = 0.2;  // irregularidade
    int slices = 10; 
    int stacks = 5;  
    char* filename = "comet.patch"; 

    srand(seed);  

    double delta_alpha = 2 * M_PI / slices;
    double delta_beta = M_PI / 2.0 / stacks;

    Point points[MAX_POINTS];
    Patch patches[MAX_PATCHES];
    int num_points = 0;
    int num_patches = 0;

    for (int i = 0; i < slices; ++i) {
        for (int j = 0; j < stacks; ++j) {
            double alpha = i * delta_alpha;
            double beta = j * delta_beta;
            generate_comet_geometry(alpha, beta, delta_alpha / 3.0, delta_beta / 3.0, radius, max_deviation, points, &num_points, patches, &num_patches);
        }
    }

    write_patch_file(filename, patches, num_patches, points, num_points);

    return 0;
}