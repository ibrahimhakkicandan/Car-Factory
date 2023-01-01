#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for sleep function
#include <stdbool.h>





bool max(int count, int count1);

// global variables
pthread_mutex_t mutex;  // mutex to protect shared variables
pthread_mutex_t mutex_cars;


sem_t* sem_chassis;  // semaphore to synchronize chassis placement
sem_t* sem_engine;  // semaphore to synchronize component installation
sem_t* sem_seat;  // semaphore to synchronize component installation
sem_t* sem_tire;  // semaphore to synchronize component installation
sem_t* sem_painting;
sem_t* sem_top_cover;  // semaphore to synchronize top cover installation

int carCount=0;
int chassis_count = 0;  // number of chassis available
int painting_count = 0;  // number of paintings available
int tire_count = 0;  // number of tires available
int seat_count = 0;  // number of seats available
int engine_count = 0;  // number of engines available
int top_cover_count = 0;  // number of top covers available
int max_chassis_count;  // number of chassis available
int max_painting_count;  // number of paintings available
int max_tire_count;  // number of tires available
int max_seat_count;  // number of seats available
int max_engine_count;  // number of engines available
int max_top_cover_count;  // number of top covers available
int maxCarPerDay = 0;
int* type_a_count;
int* type_b_count;
int* type_c_count;
int* type_d_count;


int maxValue(int a, int b){
    if(a>b) return a;
    else return b;
}
int minValue(int a,int b, int c, int d, int e, int f){
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    if (d < min) min = d;
    if (e < min) min = e;
    if (f < min) min = f;
    return min;
}


void* type_a_technician(void* arg) {

    // Type A technician adds tires and paints the car
    int id = *((int*) arg);  // technician ID

    while (tire_count<max_tire_count || painting_count<max_painting_count) {
        sem_wait(sem_engine);  // wait for chassis to be available

            pthread_mutex_lock(&mutex);  // lock the mutex

                if(tire_count<max_tire_count) {
                    tire_count++;  // decrement tire count
                    type_a_count[id - 1]++;
                    printf("tire %d \n",tire_count);
                    sem_post(sem_tire);  // signal that a component is available
                }
            pthread_mutex_unlock(&mutex);  // unlock the mutex


        sem_wait(sem_top_cover);// lock the mutex

        if(painting_count<max_painting_count){
            pthread_mutex_lock(&mutex);  // lock the mutex
            painting_count++;  // decrement tire count
            type_a_count[id - 1]++;
            printf("painting %d \n",painting_count);
            sem_post(sem_painting);  // signal that a component is available
            carCount++;
            pthread_mutex_unlock(&mutex);  // unlock the mutex

        }


        // paint the car
      //  printf("Type A - %d %d painting %d\n", id, cars_assembled[0] + 1, cars_assembled[0] + 1);






    }

    return NULL;
}



void* type_b_technician(void* arg) {

    // Type B technician places the chassis on the belt
    int id = *((int*) arg);  // technician ID

    while (chassis_count < max_chassis_count) {
        pthread_mutex_lock(&mutex);  // lock the mutex
        if(chassis_count < max_chassis_count){
            chassis_count++;  // increment chassis count
            printf("chassy %d \n",chassis_count);
            type_b_count[id-1]++;
        }
        sem_post(sem_chassis );
        pthread_mutex_unlock(&mutex);  // unlock the mutex


    }
    sem_post(sem_chassis);
    sem_post(sem_chassis);
    sem_post(sem_chassis);
    sem_post(sem_chassis);
    sem_post(sem_chassis);

    return NULL;
}

void* type_c_technician(void* arg) {

    // Type C technician adds the seats
    int id = *((int*) arg);  // technician ID

    while (seat_count < max_seat_count) {

        sem_wait(sem_tire);  // wait for chassis to be available

        pthread_mutex_lock(&mutex);  // unlock the mutex
            if(seat_count < max_seat_count) {
                seat_count++;  // decrement seat count
                type_c_count[id - 1]++;
                printf("seat %d \n",seat_count);
                sem_post(sem_seat);  // signal that a component is available
            }
        pthread_mutex_unlock(&mutex);  // unlock the mutex
       // printf("seat %d\n",seat_count);



    }

    return NULL;
}

void* type_d_technician(void* arg) {

    // Type D technician adds the engine and top cover
    int id = *((int*) arg);  // technician ID


    while (engine_count<max_engine_count || top_cover_count<max_top_cover_count) {

       // if(engine_count<max_engine_count){
            sem_wait(sem_chassis);

            pthread_mutex_lock(&mutex);  // lock the mutex
                if(engine_count<max_engine_count) {
                    engine_count++;  // decrement seat count
                   type_d_count[id - 1]++;
                    printf("engine %d \n",engine_count);
                    sem_post(sem_engine);  // signal that a component is available
                }
            pthread_mutex_unlock(&mutex);  // unlock the mutex


        //}



        sem_wait(sem_seat)  ;



            pthread_mutex_lock(&mutex);  // lock the mutex
                if(top_cover_count<max_top_cover_count) {
                    top_cover_count++;  // decrement seat count
                    type_d_count[id - 1]++;
                    printf("top cover %d \n",top_cover_count);
                    sem_post(sem_top_cover);  // signal that a component is available
                }
            pthread_mutex_unlock(&mutex);  // unlock the mutex



       // printf("Type D - %d %d cover %d\n", id, cars_assembled[0] + 1, cars_assembled[0] + 1);

    }

    return NULL;
}

int main() {


    int total_cars_produced = 0;

    pthread_mutex_init(&mutex, NULL);
   // pthread_mutex_init(&mutex_cars, NULL);


    sem_chassis = sem_open("sem_chassis",O_CREAT,0777,0);

    sem_engine = sem_open("sem_engine", O_CREAT, 0777, 0);
    sem_tire = sem_open("sem_tire", O_CREAT, 0777, 0);
    sem_seat = sem_open("sem_seat", O_CREAT, 0777, 0);
    sem_painting = sem_open("sem_painting", O_CREAT, 0777, 0);
    sem_top_cover = sem_open("sem_top_cover", O_CREAT, 0777, 0);



    // read input file
    FILE *input_file = fopen("/Users/candan/ClionProjects/FactoryCar/input.txt", "r");
    if (input_file == NULL) {
        fprintf(stderr, "Error opening input file.\n");
        return 1;
    }
    int x, y, z, t, D = 0;
    fscanf(input_file, "%d %d %d %d %d", &x, &y, &z, &t, &D);
    fscanf(input_file, "%d %d %d %d %d %d", &max_chassis_count, &max_painting_count, &max_tire_count, &max_seat_count, &max_engine_count,
           &max_top_cover_count);

    maxCarPerDay = minValue(max_top_cover_count,max_painting_count,max_tire_count,max_engine_count,max_chassis_count,max_seat_count);

    fclose(input_file);
    type_a_count = malloc(x * sizeof(int));
    for (int i = 0; i < x; i++) {
        type_a_count[i] = 0;
    }
    type_b_count = malloc(y * sizeof(int));
    for (int i = 0; i < y; i++) {
        type_b_count[i] = 0;
    }
    type_c_count = malloc(z * sizeof(int));
    for (int i = 0; i < z; i++) {
        type_b_count[i] = 0;
    }
    type_d_count = malloc(t * sizeof(int));
    for (int i = 0; i < t; i++) {
        type_d_count[i] = 0;
    }
    int* cars_produced = malloc(D * sizeof(int));
    for (int i = 0; i < D; i++) {
        cars_produced[i] = 0;
    }
    // create technician threads
    pthread_t type_a_threads[x];
    pthread_t type_b_threads[y];
    pthread_t type_c_threads[z];
    pthread_t type_d_threads[t];

    int type_a_ids[x];
    int type_b_ids[y];
    int type_c_ids[z];
    int type_d_ids[t];

    FILE *output_file = fopen("/Users/candan/ClionProjects/FactoryCar/output.txt", "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error opening output file.\n");
        return 1;
    }


    //for(int i = 0; i<D; i++){
        for (int i = 0; i < x; i++) {
            type_a_ids[i] = i + 1;
            pthread_create(&type_a_threads[i], NULL, type_a_technician, &type_a_ids[i]);

        }
        for (int i = 0; i < y; i++) {
            type_b_ids[i] = i + 1;

            pthread_create(&type_b_threads[i], NULL, type_b_technician, &type_b_ids[i]);

        }

        for (int i = 0; i < z; i++) {
            type_c_ids[i] = i + 1;

            pthread_create(&type_c_threads[i], NULL, type_c_technician, &type_c_ids[i]);

        }
        for (int i = 0; i < t; i++) {
            type_d_ids[i] = i + 1;

            pthread_create(&type_d_threads[i], NULL, type_d_technician, &type_d_ids[i]);

        }



        // write output file

        // wait for technician threads to complete
sleep(3);
    printf(" total car :%d \n",carCount);
    printf(" total chsasy :%d \n", chassis_count);  // number of chassis available
    printf(" total paint :%d \n", painting_count );  // number of paintings available
    printf(" total tire :%d \n",tire_count );  // number of tires available
    printf(" total seat :%d \n", seat_count);  // number of seats available
    printf(" total cengine :%d \n",  engine_count);  // number of engines available
    printf(" total topcover :%d \n",  top_cover_count );
            for (int i = 0; i < y; i++) {
                pthread_join(type_b_threads[i], NULL);
                printf("serkan \n");
            }
            for (int i = 0; i < t; i++) {
                printf("cndn \n");
                pthread_join(type_d_threads[i], NULL);
                printf("atila \n");
            }
            for (int i = 0; i < x; i++) {
                printf("cndn \n");
                pthread_join(type_a_threads[i], NULL);
                printf("ilhan \n");
            }
            for (int i = 0; i < z; i++) {
                printf("tağan \n");
                pthread_join(type_c_threads[i], NULL);
                printf("kç \n");
            }



    //}



        for (int i = 0; i < D; i++) {
            fprintf(output_file, "Day %d: %d cars produced\n", i + 1, cars_produced[i]);
        }
        fprintf(output_file, "Total cars produced: %d\n", total_cars_produced);
        fprintf(output_file, "Type A technicians:\n");
        for (int i = 0; i < x; i++) {
            fprintf(output_file, "  Technician %d: %d operations\n", i + 1, type_a_count[i]);
        }
        fprintf(output_file, "Type B technicians:\n");
        for (int i = 0; i < y; i++) {

            fprintf(output_file, "  Technician %d: %d operations\n", i + 1, type_b_count[i]);
        }
        fprintf(output_file, "Type C technicians:\n");
        for (int i = 0; i < z; i++) {
            fprintf(output_file, "  Technician %d: %d operations\n", i + 1, type_c_count[i]);
        }
        fprintf(output_file, "Type D technicians:\n");
        for (int i = 0; i < t; i++) {
            fprintf(output_file, "  Technician %d: %d operations\n", i + 1, type_d_count[i]);
        }
        fclose(output_file);

    // clean up
    pthread_mutex_destroy(&mutex);

    sem_close(&sem_chassis);

    sem_close(&sem_top_cover);
    sem_close(&sem_engine);
    sem_close(&sem_tire);
    sem_close(&sem_seat);
    printf("abc\n");

    return 0;
}