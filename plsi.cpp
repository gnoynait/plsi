#include <cstdio>
#include <cstdlib>
#include <vector>
#include <utility>
#include <cmath>
#include <cassert>
#include <cstring>
#include <random>
#include <chrono>

#define tol 1e-3

using std::vector;
using std::make_pair;
using std::pair;

int d = 0, w = 0;
vector<vector<pair<int, int>>> docs;
int z = 0;
int iteration_max;
int iteration;

double *p_dz, *p_wz, *p_z;

void read() {
    FILE* file = fopen("docs_train", "r");
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

    const char *z_str = getenv("Z");
    if (z_str == NULL) z_str = "10";
    z = atoi(z_str);

    const char *iteration_max_str = getenv("IMAX");
    if (iteration_max_str == NULL) iteration_max_str = "1000";
    iteration_max = atoi(iteration_max_str);

    printf("%d docs read, %d words, %d topics\n", d, w, z);
    fclose(file);
}

void init() {
    p_dz = new double[d * z];
    p_wz = new double[w * z];
    p_z = new double[z];

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    for (int zi = 0; zi < z; zi ++) {
        p_z[zi] = distribution(generator);

        for (int di = 0; di < d; di ++) {
            p_dz[di * z + zi] = distribution(generator);
        }

        for (int wi = 0; wi < w; wi ++) {
            p_wz[wi * z + zi] = distribution(generator);
        }
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
            fprintf(file_wz, "%f", p_wz[wi * z + zi]);
        }
        fprintf(file_wz, "\n");
    }
    fclose(file_wz);

    FILE* file_dz = fopen("dz", "w");
    for (int zi = 0; zi < z; zi ++) {
        for (int di = 0; di < w; di ++) {
            if (di > 0) fprintf(file_dz, " ");
            fprintf(file_dz, "%f", p_dz[di * z + zi]);
        }
        fprintf(file_dz, "\n");
    }
    fclose(file_dz);
}

void finish() {
    delete[] p_dz;
    delete[] p_wz;
    delete[] p_z;
}

void debug() {
    printf("===== iteration: %d =====\n", iteration);
    //for (int i = 0; i < z; i ++) printf("%f\n", p_z[i]);

    double r = 0;
    for (int i = 0; i < w * z; i ++) {
        r += p_wz[i];
    }
    printf("w | z: %g %d\n", r, z);

    r = 0;
    for (int i = 0; i < d * z; i ++) {
        r += p_dz[i];
    }
    printf("d | z: %g %d\n", r, z);

    r = 0;
    for (int i = 0; i < z; i ++) {
        r += p_z[i];
    }
    printf("z: %g 1\n", r);
}

double likelihood() {
    double r = 0;
    for (int di = 0; di < d; di ++) {
        for (auto& word_tf : docs[di]) {
            int wi = word_tf.first,
                tf = word_tf.second;
            double p_dw = 0;
            for (int zi = 0; zi < z; zi ++) {
                p_dw += p_z[zi] * p_wz[wi * z + zi] * p_dz[di * z + zi];
            }
            r += tf * log(p_dw);
        }
    }
    return r;
}

void update() {

    double *nominator_p_dz = new double[d * z];
    double *denominator_p_dz = new double[z];
    memset(nominator_p_dz, 0, sizeof(double) * d * z);
    memset(denominator_p_dz, 0, sizeof(double) * z);

    double *nominator_p_wz = new double[w * z];
    double *denominator_p_wz = new double[z];
    memset(nominator_p_wz, 0, sizeof(double) * w * z);
    memset(denominator_p_wz, 0, sizeof(double) * z);

    double *nominator_p_z = new double[z];
    double denominator_p_z = 0;
    memset(nominator_p_z, 0, sizeof(double) * z);

    for (int di = 0; di < d; di ++) {
        for (auto& word_tf : docs[di]) {
            int wi = word_tf.first,
                tf = word_tf.second;
            double denominator = 0;
            double* nominator = new double[z];

            for (int zi = 0; zi < z; zi ++) {
                nominator[zi] = p_dz[di * z + zi] * p_wz[wi * z + zi] * p_z[zi];
                assert(nominator[zi] >= 0);
                denominator += nominator[zi];
            }
            assert(denominator >= 0);

            for (int zi = 0; zi < z; zi ++) {
                double p_z_condition_d_w = nominator[zi] / denominator;

                nominator_p_dz[di * z + zi] += tf * p_z_condition_d_w;
                denominator_p_dz[zi] += tf * p_z_condition_d_w;

                nominator_p_wz[wi * z + zi] += tf * p_z_condition_d_w;
                denominator_p_wz[zi] += tf * p_z_condition_d_w;

                nominator_p_z[zi] += tf * p_z_condition_d_w;
            }
            denominator_p_z += tf;

            delete[] nominator;
        }
    }

    for (int di = 0; di < d; di ++) {
        for (int zi = 0; zi < z; zi ++) {
            p_dz[di * z + zi] = nominator_p_dz[di * z + zi] / denominator_p_dz[zi];
            assert(p_dz[di * z + zi] <= 1);
            assert(p_dz[di * z + zi] >= 0);
        }
    }

    for (int wi = 0; wi < w; wi ++) {
        for (int zi = 0; zi < z; zi ++) {
            p_wz[wi * z + zi] = nominator_p_wz[wi * z + zi] / denominator_p_wz[zi];
            assert(p_wz[wi * z + zi] >= 0);
            assert(p_wz[wi * z + zi] <= 1);
        }
    }

    for (int zi = 0; zi < z; zi ++) {
        p_z[zi] = nominator_p_z[zi] / denominator_p_z;
        assert(p_z[zi] >= 0);
        assert(p_z[zi] <= 1);
    }

    delete[] nominator_p_dz;
    delete[] denominator_p_dz;

    delete[] nominator_p_wz;
    delete[] denominator_p_wz;

    delete[] nominator_p_z;
}

void train() {
    double last_likelihood = -1;
    for (iteration = 0; iteration < iteration_max; iteration++) {
        update();
        // debug();
        double now_likelihood = likelihood();
        fprintf(stdout, "iteration: %d, log-likelihood: %f\n", iteration, now_likelihood);
        if (fabs(now_likelihood - last_likelihood) < tol) break;
        last_likelihood = now_likelihood;
        if (iteration % 100 == 0) {
            output();
        }
    }
}

int main() {
    fprintf(stdout, "loading...\n");
    read();

    fprintf(stdout, "initing...\n");
    init();

    fprintf(stdout, "training...\n");
    train();

    fprintf(stdout, "outputing...\n");
    output();

    fprintf(stdout, "finishing...\n");
    finish();

    return 0;
}
