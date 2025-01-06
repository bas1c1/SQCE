#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

/* PSEUDO RANDOM NUMBERS GENERATOR */

#define A 1664525
#define C 1013904223
#define M 4294967296

unsigned int seed = 1;

unsigned int rand2() {
    seed = (A * seed + C) % M;
    return seed;
}

/* PSEUDO RANDOM NUMBERS GENERATOR */

#define QBIT_DEF QuantumBit qb;
#define GET_QBIT_BY_INDEX qb = qbits[q_ind];
#define SET_QBIT_BY_INDEX qbits[q_ind] = qb;

#define BIT_DEF DefaultBit db;
#define GET_BIT_BY_INDEX db = dbits[d_ind];
#define SET_BIT_BY_INDEX dbits[d_ind] = db;

typedef struct {
    double complex state[2];
} QuantumBit;

typedef double DefaultBit;

DefaultBit dbits[1000];
QuantumBit qbits[1000];
int qpointer = 0;
int dpointer = 0;

void init_qbit() {
    QuantumBit qb;
    qb.state[0] = 1;
    qb.state[1] = 0;
    qbits[qpointer++] = qb;
}

void init_bit() {
    DefaultBit db;
    db = 0.0;
    dbits[dpointer++] = db;
}

void measure(int q_ind, int d_ind) {
    QBIT_DEF
    BIT_DEF

    GET_QBIT_BY_INDEX
    double p0 = creal(qb.state[0]) * creal(qb.state[0]) + cimag(qb.state[0]) * cimag(qb.state[0]);
    
    if (rand2() % 1000 <= p0 * 1000) {
        db = 0;
        qb.state[0] = 1.0;
        qb.state[1] = 0.0;
    } else {
        db = 1;
        qb.state[0] = 0.0;
        qb.state[1] = 1.0;
    }
    SET_BIT_BY_INDEX
    SET_QBIT_BY_INDEX
}

void op_gate_y(int q_ind) {
    QBIT_DEF

    GET_QBIT_BY_INDEX
    double complex tmp = 0 + I * qb.state[0];
    qb.state[0] = -I * qb.state[1];
    qb.state[1] = tmp;
    SET_QBIT_BY_INDEX
}

void op_gate_z(int q_ind) {
    QBIT_DEF

    GET_QBIT_BY_INDEX
    qb.state[1] = -qb.state[1];
    SET_QBIT_BY_INDEX
}

void op_hadamard(int q_ind) {
    QBIT_DEF

    GET_QBIT_BY_INDEX
    double complex a = qb.state[0]; 
    double complex b = qb.state[1];
    qb.state[0] = (a + b) / sqrt(2.0);
    qb.state[1] = (a - b) / sqrt(2.0);
    SET_QBIT_BY_INDEX
}

void op_not(int q_ind) {
    QBIT_DEF

    GET_QBIT_BY_INDEX
    double complex tmp = qb.state[0];
    qb.state[0] = qb.state[1];
    qb.state[1] = tmp;
    SET_QBIT_BY_INDEX
}

void op_cnot(int q_ind, int target_q_ind) {
    QBIT_DEF
    BIT_DEF
    int d_ind = 0;

    GET_QBIT_BY_INDEX

    measure(q_ind, 0);
    GET_BIT_BY_INDEX
    int measure1 = db;
    if (measure1 == 1) {
        op_not(target_q_ind);
    }
}

void entagle_pair(int q_ind, int q_ind_b) {
    QBIT_DEF

    GET_QBIT_BY_INDEX
    qb.state[0] = 1.0 / sqrt(2.0);
    qb.state[1] = 0.0;
    SET_QBIT_BY_INDEX

    q_ind = q_ind_b;

    GET_QBIT_BY_INDEX
    qb.state[0] = 0.0;
    qb.state[1] = 1.0 / sqrt(2.0);
    SET_QBIT_BY_INDEX

    op_cnot(q_ind, q_ind_b);
}

int main() {
    seed = time(0);

    QBIT_DEF
    BIT_DEF
    int q_ind = 0;
    int d_ind = 0;

    init_bit();
    init_qbit();

    //Создание исходного кубита в произвольном состоянии
    op_not(q_ind);
    GET_QBIT_BY_INDEX
    printf("Initialized: |0>: %f + %fi, |1>: %f + %fi\n", creal(qb.state[0]), cimag(qb.state[0]), creal(qb.state[1]), cimag(qb.state[1]));

    //Создание кубитов Алисы и Боба в состоянии |0>
    init_qbit();//Кубит Алисы, q_ind = 1
    init_qbit();//Кубит Боба, q_ind = 2

    //Применение гейта CNOT к исходному кубиту и к кубиту Алисы
    op_cnot(0, 1);
    //Применение гейта Адамара к исходному кубиту
    op_hadamard(0);

    //Измерение исходного кубита и кубита Алисы
    measure(0, 0);
    GET_BIT_BY_INDEX
    int measure1 = db;
    measure(1, 0);
    GET_BIT_BY_INDEX
    int measure2 = db;

    //Применение коррекций к кубиту Боба
    if (measure1 == 1) {
        op_gate_z(2);
    }
    if (measure2 == 1) {
        op_not(2);
    }

    //Вывод телепортированного кубита
    q_ind = 2;
    GET_QBIT_BY_INDEX
    printf("Teleported: |0>: %f + %fi, |1>: %f + %fi\n", creal(qb.state[0]), cimag(qb.state[0]), creal(qb.state[1]), cimag(qb.state[1]));

    /*GET_QBIT_BY_INDEX
    printf("[%lf ; %lf]\n", creal(qb.state[0]), creal(qb.state[1]));

    op_not(q_ind);
    op_cnot(q_ind, q_ind+1);
    GET_QBIT_BY_INDEX
    printf("[%lf ; %lf]\n", creal(qb.state[0]), creal(qb.state[1]));

    q_ind = q_ind + 1;
    GET_QBIT_BY_INDEX
    printf("[%lf ; %lf]\n", creal(qb.state[0]), creal(qb.state[1]));

    int c0 = 0;
    int c1 = 0;

    for (int i = 0; i < 1000; i++) {
        op_hadamard(q_ind);
        GET_QBIT_BY_INDEX
        //printf("H[%lf ; %lf]\n", creal(qb.state[0]), creal(qb.state[1]));

        measure(0, 0);
        GET_BIT_BY_INDEX
        //printf("\n%lf\n", db);
        if (db > 0) {
            c1++;
        } else {
            c0++;
        }
    }

    printf("\n0: %d\n1: %d\n", c0, c1);*/


    return 0;
}