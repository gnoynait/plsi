#include <stdio.h>
#include <vector>
#include <utility>

using std::vector;
using std::make_pair;
using std::pair;

int d = 0, w = 0;
vector<vector<pair<int, int>>> docs;
int z = 0;

double *p_dz, *p_wz, *p_z;
double *p_dz_new, *p_wz_new, *p_z_new;

double *nominator_p_dz, *denominator_p_dz;
double *nominator_p_wz, *denominator_p_wz;
double *nominator_p_z;

template <class T>
void swap(T& a, T& b) {
    T c = a;
    a = b;
    b = c;
}

void read() {
    FILE* file = fopen("docs", "r");
    vector<pair<int, int>> doc;
    while (!feof(file)) {
        int word, count;
        char c;
        if (fscanf(file, "%d:%d%c", &word, &count, &c) <= 0) break;
        doc.push_back(make_pair(word, count));
        if (word > w) w = word;
        if (c != ' ') {
            // end of line
            docs.push_back(doc);
            doc.clear();
        }
        // printf("%d %d %d\n", word, count, c);
    }
    d = docs.size();
    w ++;
    z = 10;
    printf("%d docs read, %d words, %d topics\n", d, w, z);
    fclose(file);
}

void init() {
    p_dz = new double[d * z];
    p_wz = new double[w * z];
    p_z = new double[z];

    for (int i = 0; i < z; i++) {
        p_z[i] = 1.0 / z;

        for (int j = 0; j < d; j ++) {
            p_dz[i * z + j] = 1.0 / d;
        }

        for (int j = 0; j < w; j ++) {
            p_wz[i * z + j] = 1.0 / w;
        }
    }

    p_dz_new = new double[d * z];
    p_wz_new = new double[w * z];
    p_z_new = new double[z];

    nominator_p_dz = new double[d * z];
    denominator_p_dz = new double[z];
    nominator_p_wz = new double[d * z];
    denominator_p_wz = new double[z];
    nominator_p_z = new double[z];
}

void finish() {
    delete[] p_dz;
    delete[] p_wz;
    delete[] p_z;
    delete[] p_dz_new;
    delete[] p_wz_new;
    delete[] p_z_new;
    delete[] nominator_p_dz;
    delete[] denominator_p_dz;
    delete[] nominator_p_wz;
    delete[] denominator_p_wz;
    delete[] nominator_p_z;
}

double likelihood() {
    return 0;
}

void update_p_dz() {
    for (int di = 0; di < d; di ++) {
        for (auto& word_tf : docs[di]) {
            int wi = word_tf.first,
                tf = word_tf.second;
            double denominator = 0;
            double* nominator = new double[z];
            for (int zi = 0; zi < z; zi ++) {
                nominator[zi] = p_dz[zi * z + di] * p_wz[zi * z + wi] * p_z[zi];
                denominator += nominator[zi];
            }
            for (int zi = 0; zi < z; zi ++) {
                double p_z_condition_d_w = nominator[zi] / denominator;
                nominator_p_dz[zi * z + di] += tf * p_z_condition_d_w;
                denominator_p_dz[zi] += tf * p_z_condition_d_w;
            }
            delete[] nominator;
        }
    }

    for (int di = 0; di < d; di ++) {
        for (int zi = 0; zi < z; zi ++) {
            p_dz_new[zi * z + d] = nominator_p_dz[zi * z + di] / denominator_p_dz[zi];
        }
    }
}

void update_p_wz() {
    for (int di = 0; di < d; di ++) {
        for (auto& word_tf : docs[di]) {
            int wi = word_tf.first,
                tf = word_tf.second;
            double denominator = 0;
            double* nominator = new double[z];
            for (int zi = 0; zi < z; zi ++) {
                nominator[zi] = p_dz[zi * z + di] * p_wz[zi * z + wi] * p_z[zi];
                denominator += nominator[zi];
            }
            for (int zi = 0; zi < z; zi ++) {
                double p_z_condition_d_w = nominator[zi] / denominator;
                nominator_p_wz[zi * z + di] += tf * p_z_condition_d_w;
                denominator_p_wz[zi] += tf * p_z_condition_d_w;
            }
            delete[] nominator;
        }
    }

    for (int wi = 0; wi < w; wi ++) {
        for (int zi = 0; zi < z; zi ++) {
            p_wz_new[zi * z + d] = nominator_p_wz[zi * z + wi] / denominator_p_wz[zi];
        }
    }
}

void update_p_z() {
    double denominator_p_z = 0;
    for (int di = 0; di < d; di ++) {
        for (auto& word_tf : docs[di]) {
            int wi = word_tf.first,
                tf = word_tf.second;
            double denominator = 0;
            double* nominator = new double[z];
            for (int zi = 0; zi < z; zi ++) {
                nominator[zi] = p_dz[zi * z + di] * p_wz[zi * z + wi] * p_z[zi];
                denominator += nominator[zi];
            }
            for (int zi = 0; zi < z; zi ++) {
                double p_z_condition_d_w = nominator[zi] / denominator;
                nominator_p_z[zi] += tf * p_z_condition_d_w;
            }
            denominator_p_z += tf;
            delete[] nominator;
        }
    }

    for (int zi = 0; zi < z; zi ++) {
        p_z_new[zi] = nominator_p_z[zi] / denominator_p_z;
    }
}

void update() {
    update_p_dz();
    update_p_wz();
    update_p_z();
    swap(p_wz, p_wz_new);
    swap(p_dz, p_dz_new);
    swap(p_z, p_z_new);
}

void train() {
    for (int i = 0; i < 100; i++) {
        update();
    }
}

void output() {
    FILE* file_z = fopen("z", "w");
    for (int zi = 0; zi < z; zi ++) {
        fprintf(file_z, "%f\n", p_z[zi]);
    }
    fclose(file_z);

    FILE* file_wz = fopen("wz", "w");
    for (int zi = 0; zi < z; zi ++) {
        for (int wi = 0; wi < w; wi ++) {
            if (wi > 0) fprintf(file_wz, " ");
            fprintf(file_wz, "%f", p_wz[zi * z + wi]);
        }
        fprintf(file_wz, "\n");
    }
    fclose(file_wz);

    FILE* file_dz = fopen("dz", "w");
    for (int zi = 0; zi < z; zi ++) {
        for (int di = 0; di < w; di ++) {
            if (di > 0) fprintf(file_dz, " ");
            fprintf(file_dz, "%f", p_dz[zi * z + di]);
        }
        fprintf(file_dz, "\n");
    }
    fclose(file_dz);
}

int main() {
    fprintf(stderr, "loading...\n");
    read();

    fprintf(stderr, "initing...\n");
    init();

    fprintf(stderr, "training...\n");
    train();

    fprintf(stderr, "outputing...\n");
    output();

    fprintf(stderr, "finishing...\n");
    finish();

    return 0;
}
