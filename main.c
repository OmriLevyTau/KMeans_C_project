#include <stdio.h>
#include <string.h>
# include <stdlib.h>
# include <math.h>
# include <setjmp.h>
#include <unistd.h>
# define epsilon 0.001
jmp_buf savebuf;

int countLines(char* filePath);
int countCols(char* filePath);
void printMatrix(double** mat, int rows, int cols);
double** buildMatrix(int rows, int cols);
double** createMatrix(int rows, int cols, char* filePath);
double* sub_vectors(const double *A, const double *B, int n);
double* add_vectors(const double *A, const double *B, int n);
double squared_dot_product(const double *A, const double *B, int n);
FILE* write_output(char* output_filename, int rows, int cols,double** centroids);
double** K_means(int K, int max_iter, char* input_filename, char* output_filename);
int validate_input_args(int argc, char* argv[]);
void kmean_test(int K, int max_iter, char* input_path, char* output_path);



int countLines(char* filePath){
    /*
     * input: file Name
     * output: number of lines in file
     */

    FILE *fp =  fopen(filePath,"r");;
    char c;
    int counter=0;

    if (fp==NULL){
        longjmp(savebuf,1);
    }
    for (c= getc(fp); c!=EOF; c= getc(fp)){
        if (c=='\n'){
            counter+=1;
        }
    }
    fclose(fp);
    return counter;

}

int countCols(char* filePath){
    /*
     * input: file Name
     * output: number of columns
     * details: open files, reads first line of file (loops until first '\n').
     *          counts number of ",", return counter+1 if not 0, otherwise 0.
     */
    FILE *fp =  fopen(filePath,"r");
    char c;
    int counter=0;

    if (fp==NULL){
        longjmp(savebuf,1);
    }

    for (c= getc(fp); c!='\n'; c= getc(fp)){
        if (c==','){
            counter+=1;
        }
    }

    fclose(fp);

    if (counter==0){
        return 0;
    } else{
        return ++counter;
    }
}

void printMatrix(double** mat, int rows, int cols){
    int i,j;
    for (i=0; i<rows;i++){
        for (j=0;j<cols;j++){
            printf("  %.4f",mat[i][j]);
        }
        printf("\n");
    }
}


double** buildMatrix(int rows, int cols){
    /*
     * Creates empty matrix of size rows x cols
     */
    int i;
    double **a = calloc(rows, sizeof(int*));
    if (a==NULL){
        longjmp(savebuf,1);
    }
    for (i=0;i<rows;i++){
        a[i] = calloc(cols, sizeof(double));
        if (a[i]==NULL){
            longjmp(savebuf,1);
        }
    }
    return a;
}

double** createMatrix(int rows, int cols, char* filePath){
    /*
     * Creates empty matrix and fills it with read values from file
     */
    double** matrix = buildMatrix(rows,cols);
    int lineSize = cols*18; // 17 + 1
    FILE *fp =  fopen(filePath,"r");
    char *token; // String pointer
    int i=0,j=0;
    double tmp;
    char line[lineSize];

    if (fp==NULL){
        longjmp(savebuf,1);
    }

    // Reads each line as a string
    while (fgets(line,lineSize,fp)!=NULL){
        token = strtok(line,","); // token is a string between 2 commas
        while (token!=NULL){               // in end of line token is NULL
            matrix[i][j] = atof(token); // converts the string token to double
            token = strtok(NULL,","); // move forward to the next comma. Pointer=NULL so it will continue from the last place
            j++;
        }
        // finished line
        i++;
        j=0;
    }
    fclose(fp);

    return matrix;

}

double* sub_vectors(const double *A, const double *B, int n){
    int i;
    double* res = (double*)malloc(n*sizeof(double));
    if (res==NULL){
        longjmp(savebuf,1);
    }
    for(i=0; i<n; i++){
        res[i] = A[i] - B[i];
    }
    return res;
}

double* add_vectors(const double *A, const double *B, int n){
    int i;
    double* res = (double*)malloc(n*sizeof(double));
    if (res==NULL){
        longjmp(savebuf,1);
    }
    for(i=0; i<n; i++){
        res[i] = A[i] + B[i];
    }
    return res;
}
double squared_dot_product(const double *A, const double *B, int n){
    int i;
    double res = 0;
    for(i=0; i<n; i++){
        res = res + (A[i] * B[i]);
    }
    return res;
}

double** copy(double** data, int K, int cols){
    int i,j;
    double** new_mat = buildMatrix(K, cols);
    for(i=0; i<K; i++){
        for(j=0; j<cols; j++){
            new_mat[i][j] = data[i][j];
        }
    }
    return new_mat;
}

FILE* write_output(char* output_filename, int rows, int cols,double** centroids){

    char tmp_str[100];
    FILE* fp;
    fp = fopen(output_filename, "w");
    int r,c;

    if (fp==NULL){
        longjmp(savebuf,1);
    }
    for (r=0;r<rows;r++){
        c = 0;
        for (;c<cols-1;c++){
            sprintf(tmp_str,"%.4f",centroids[r][c]) ; // saves centroids[r][c] in tmp_str
            fputs(tmp_str,fp);
            fputs(",",fp);
        }
        sprintf(tmp_str,"%.4f",centroids[r][c]) ;
        fputs(tmp_str,fp);
        fputs("\n", fp);
    }
    fclose(fp);
    return fp;
}

double** K_means(int K, int max_iter, char* input_filename, char* output_filename){
    /*
     * recieves input file, K = number of clusters, max_iter = max number of iterations
     * connects every point to the closest cluster
     * returns vector of centroids
     */
    int rows = countLines(input_filename);
    int cols = countCols(input_filename);
    double ** data = createMatrix(rows,cols,input_filename);
    double** centroids = copy(data, K, cols); //K first data points
    int idx, arg_min, counter,iter,point,cluster_ind, r, k, c;
    double min_dist;
    double** cluster_sum;
    double** old_centroids;

    //place to treat input validity

    int points_clusters[rows]; //for each point, keeps it current closest cluster

    for(iter=0; iter<max_iter; iter++){
        //iterate through points and assign to the closest cluster
        for (point=0; point<rows; point++){
            min_dist = INT_MAX;
            arg_min = -1;
            for(cluster_ind=0; cluster_ind<K; cluster_ind++){
                double* cluster = centroids[cluster_ind];
                double* tmp_arr = sub_vectors(cluster,data[point], cols);
                double dist_point_cluster = squared_dot_product(tmp_arr,tmp_arr,cols);
                if(dist_point_cluster<min_dist){
                    min_dist = dist_point_cluster;
                    arg_min = cluster_ind;
                }
            }
            points_clusters[point] = arg_min;
        }
        //calculate new centroids
        old_centroids = copy(centroids,K, cols); //for changes checking
        cluster_sum = buildMatrix(K, cols); //zero matrix
        double cluster_change[K];
        int cluster_counter[K];
        memset(cluster_counter, 0, sizeof(cluster_counter)); //zero array
        memset(cluster_change, 0, sizeof(cluster_change)); //zero array


        //sum and count
        for(r=0; r<rows; r++){
             idx = points_clusters[r];
             cluster_counter[idx] += 1;
             cluster_sum[idx] = add_vectors(cluster_sum[idx], data[r], cols);
        }

        //update centroids
        counter = 0;

        for(k=0; k<K; k++){
            for(c=0; c<cols; c++){
                if (cluster_counter[k]==0){
                    longjmp(savebuf,1);
                }
                centroids[k][c] = cluster_sum[k][c] / cluster_counter[k];
            }
            //check change vector
            double* tmp_vec = sub_vectors(centroids[k],old_centroids[k], cols);
            cluster_change[k] = sqrt(squared_dot_product(tmp_vec, tmp_vec, cols));
            if(cluster_change[k]<epsilon){
                counter += 1;
            }
        }
        // check if all coordinates changes are less than epsilon
        if(counter == K){
            break;
        }
    }
    free(data);
    free(old_centroids);
    write_output(output_filename, K, cols, centroids);
    return centroids;
}

int validate_input_args(int argc, char* argv[]){
    /*
     * Tests:
     * 1. argc ==5
     * 2. K and max_iter are Integers
     * 3. K and max_iter > 0
     * Assumptions:
     * 1. K >= data points
     */
    if (argc!=5 && argc!=4){
        return 1;
    }
    char* K_str;
    char* max_iter_str;
    char* input_name;
    char* output_name;
    double tmp_k, tmp_max_iter;
    int K, max_iter;

    if (argc==5){
        K_str = argv[1];
        max_iter_str = argv[2];
        input_name = argv[3];
        output_name = argv[4];
    } else {
        K_str = argv[1];
        max_iter_str = "200";
        input_name = argv[2];
        output_name = argv[3];
    }

    tmp_k = atof(K_str);
    K = atoi(K_str);
    tmp_max_iter = atof(max_iter_str);
    max_iter = atoi(max_iter_str);

    if (K!=tmp_k || max_iter!=tmp_max_iter || K<=1 || max_iter<=0){
        return 1;
    }
    return 0;
}

void kmean_test(int K, int max_iter, char* input_path, char* output_path){
    int cols = countCols(input_path);
    double** mat = K_means(K, 200, input_path, output_path);
    printMatrix(mat, K, cols);
}


int main(int argc, char * argv[]) {

    char* K_str;
    char* max_iter_str;
    char* input_name;
    char* output_name;

    if (validate_input_args(argc,argv)==1){
        printf("Invalid Input!");
        return 1;
    }
    if (argc==4){
        K_str = argv[1];
        max_iter_str = "200";
        input_name = argv[2];
        output_name = argv[3];
    } else {
        K_str = argv[1];
        max_iter_str = argv[2];
        input_name = argv[3];
        output_name = argv[4];
    }
    int K = atoi(K_str);
    int max_iter = atoi(max_iter_str);

    char* input_path =  "C:\\Users\\Omri\\Desktop\\CS_Omri\\Second_Year\\SW_Project\\EX_1\\K_Means_C\\KMeans_C_project\\files\\input_2.txt";
    char* output_path = "C:\\Users\\Omri\\Desktop\\CS_Omri\\Second_Year\\SW_Project\\EX_1\\K_Means_C\\KMeans_C_project\\files\\output_test_2.txt";


    if (setjmp(savebuf)==0){
        // CMD args
//        kmean_test(K, max_iter,input_name,output_name);
        // Good args
        kmean_test(K, max_iter,input_path,output_path);
        return 0;
    } else {
        printf("An Error Has Occurred");
        return 1;
    }

}
