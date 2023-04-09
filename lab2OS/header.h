#define MAX 20
void ReadMatA();
void ReadMatB();
void by_matrix();
void by_row();
void by_element();
void Write_to_file(int method);
void* multi_first_method();
void* multi_second_method(void* r);
void *multi_third_method(void *arg);
void clear_Matrix_C();

typedef struct {
    int row;
    int col;
}element_Location;

int ROWA, COLA;
int ROWB, COLB;
float matA[MAX][MAX];
float matB[MAX][MAX];
float matC[MAX][MAX];

char first_input_file[20];
char second_input_file[20];
char first_method_out_file[20];
char second_method_out_file[20];
char third_method_out_file[20];
