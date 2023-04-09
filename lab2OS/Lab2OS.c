#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "header.h"



void main(int argc, char *argv[]) {
    
    char extension[20] = ".txt";
    if (argc == 4){
        //input files names
        strcpy(first_input_file, argv[1]);
        strcat(first_input_file, extension);
        strcpy(second_input_file, argv[2]);
        strcat(second_input_file, extension);

        //output files names
        strcpy(extension, "_per_matrix.txt");
        strcpy(first_method_out_file, argv[3]);
        strcat(first_method_out_file, extension);

        strcpy(extension, "_per_row.txt");
        strcpy(second_method_out_file, argv[3]);
        strcat(second_method_out_file, extension);

        strcpy(extension, "_per_element.txt");
        strcpy(third_method_out_file, argv[3]);
        strcat(third_method_out_file, extension);
        
    }else{
        //input files names
        strcpy(extension, "a.txt");
        strcat(first_input_file, extension);
        strcpy(extension, "b.txt");
        strcat(second_input_file, extension);

        //output files names
        strcpy(extension, "c_per_matrix.txt");
        strcat(first_method_out_file, extension);

        strcpy(extension, "c_per_row.txt");
        strcat(second_method_out_file, extension);

        strcpy(extension, "c_per_element.txt");
        strcat(third_method_out_file, extension);
        
    }
    //read Matrix A from file
    ReadMatA();
    //read matrix B from file
    ReadMatB();

    //compute multiplication using A thread per out matrix
    clear_Matrix_C();
    by_matrix();
    //clear out matrix (C)
    clear_Matrix_C();
    //compute multiplication using A thread per row
    by_row();
    clear_Matrix_C();
    //compute multiplication using A thread per element
    by_element();
    clear_Matrix_C();
}

void clear_Matrix_C(){
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 20; j++){
            matC[i][j] = 0.0;
        }
    }
}


void by_matrix(){
    //create thread for matrix (first methos)
    pthread_t First_method_thread;
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time

    if (pthread_create(&First_method_thread, NULL, multi_first_method, NULL) != 0){
        printf("Failed creation thread");
        exit(-1);
    }
    // joining and waiting for thread complete
    pthread_join(First_method_thread, NULL);

    gettimeofday(&stop, NULL); //end checking time

    int secondsTaken = (stop.tv_sec - start.tv_sec);
    int millisecondTaken = (stop.tv_usec - start.tv_usec);

    printf(">>>>>>>> First Method\n");
    printf(">>>>>>>> Number Of Threads: %d\n", 1);
    printf(">>>>>>>> Seconds taken %u\n", secondsTaken);
    printf(">>>>>>>> Microseconds taken: %u\n\n", millisecondTaken);
    Write_to_file(1);
}

void by_row(){

    //create threads for second method (A thread per row)
    struct timeval stop, start;
    pthread_t threads[ROWA];

    gettimeofday(&start, NULL); //start checking time

    for (int i = 0; i < ROWA; i++) {
        int *arg = malloc(sizeof(*arg));
        if ( arg == NULL ) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }
        *arg = i;
        if (pthread_create(&threads[i], NULL, multi_second_method, arg) != 0){
            printf("Failed creation thread");
            exit(-1);
        }
    }
    // joining and waiting for threads to complete
    for (int i = 0; i < ROWA; i++)
        pthread_join(threads[i], NULL);
    
    gettimeofday(&stop, NULL); //end checking time
    int secondsTaken = (stop.tv_sec - start.tv_sec);
    int millisecondTaken = (stop.tv_usec - start.tv_usec);

    printf(">>>>>>>> Second Method\n");
    printf(">>>>>>>> Number Of Threads: %d\n", ROWA);
    printf(">>>>>>>> Seconds taken %u\n", secondsTaken);
    printf(">>>>>>>> Microseconds taken: %u\n\n", millisecondTaken);

    Write_to_file(2);

}

void by_element(){
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    //create threads for third method (A thread per element)
    pthread_t threads_Per_element[ROWA * COLB];
    for (int i = 0; i < (ROWA * COLB); i++) {
        element_Location *loc = malloc(sizeof *loc);
        int j = COLB;
        for (int f = 0; f < ROWA; f++){
            if (i < j){
                loc->row = f;
                break;
            }
            j += COLB;
        }
        loc->col = i % COLB;
        if (pthread_create(&threads_Per_element[i], NULL, multi_third_method,(void *) loc) != 0){
            printf("Failed creation thread");
            exit(-1);
        }
    }

    // joining and waiting for threads to complete
    for (int i = 0; i < (ROWA * COLB); i++)
        pthread_join(threads_Per_element[i], NULL);

    gettimeofday(&stop, NULL); //end checking time
    int secondsTaken = (stop.tv_sec - start.tv_sec);
    int millisecondTaken = (stop.tv_usec - start.tv_usec);

    printf(">>>>>>>> Third Method\n");
    printf(">>>>>>>> Number Of Threads: %d\n", ROWA * COLB);
    printf(">>>>>>>> Seconds taken %u\n", secondsTaken);
    printf(">>>>>>>> Microseconds taken: %u\n\n", millisecondTaken);

    Write_to_file(3);
}


void Write_to_file(int method){
    int i,j;
    FILE *f1;
    if (method == 1){
        f1=fopen(first_method_out_file,"w");
        fprintf(f1,"%s\n", "Method: A thread per matrix");
    }else if (method == 2){
        f1=fopen(second_method_out_file,"w");
        fprintf(f1,"%s\n", "Method: A thread per row");
    }else if (method == 3){
        f1=fopen(third_method_out_file,"w");
        fprintf(f1,"%s\n", "Method: A thread per element");
    }
    fprintf(f1,"row=%d col=%d\n", ROWA, COLB);
    for (i=0; i < ROWA;i++)
    {
        for (j=0; j < COLB;j++)
        {
            fprintf(f1,"%5.1f ", matC[i][j]);
        }
        fprintf(f1,"\n");
    }
    fclose(f1);
}

void* multi_first_method()
{   
    for (int i = 0; i < ROWA; ++i) {
      for (int j = 0; j < COLB; ++j) {
        
         for (int k = 0; k < COLA; ++k) {
            matC[i][j] += matA[i][k] * matB[k][j];
         }
      }
   }
   return NULL;
}

void* multi_second_method(void* r)
{
    
    int row = *((int *) r);
    for (int j = 0; j < COLB; j++)
      for (int k = 0; k < COLA; k++)
        matC[row][j] += matA[row][k] * matB[k][j];

    return NULL;
}
 
void *multi_third_method(void *arg){
    
    element_Location thread_arg = *(element_Location*) arg;
    for (int i = 0; i < ROWB; i++){
        matC[thread_arg.row][thread_arg.col] += matA[thread_arg.row][i] * matB[i][thread_arg.col];
    }

    free(arg);
    return NULL;
}


void ReadMatA(){
    FILE *file;
    file =fopen(first_input_file,"r");
    int i,j;
    char temp[20];
    //read first line to get size of MATRIX
    fscanf(file, "%[^\n]%*c",temp);
    char *row = strtok(temp, " ");
    char *col = strtok(NULL, "");
    strtok(row, "=");
    row = strtok(NULL, "");
    strtok(col, "=");
    col = strtok(NULL, "");
    ROWA = atoi(row);
    COLA = atoi(col);
    //read remaining lines to get elemnts of Matrix
    for(i=0 ;i<ROWA; i++){
        for(j=0; j<COLA; j++){
            fscanf(file, "%f%*[^' '||'\n']%*c",&matA[i][j]);
		printf("%f ", matA[i][j]);
        }
	printf("\n");
    }
}

void ReadMatB(){
    printf("\n");
    FILE *file;
    file = fopen(second_input_file,"r");
    int i,j;
    char temp[20];
    //read first line to get size of MATRIX
    fscanf(file, "%[^\n]%*c",temp);
    char *row = strtok(temp, " ");
    char *col = strtok(NULL, "");
    strtok(row, "=");
    row = strtok(NULL, "");
    strtok(col, "=");
    col = strtok(NULL, "");
    ROWB = atoi(row);
    COLB = atoi(col);
    //read remaining lines to get elemnts of Matrix
    for(i=0 ;i<ROWB; i++){
        for(j=0; j<COLB; j++){
            fscanf(file, "%f%*[^' '||'\n']%*c",&matB[i][j]);
        	printf("%f ", matB[i][j]);
        }
	printf("\n");
    }
    
}

