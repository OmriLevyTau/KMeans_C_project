#include <stdio.h>
#include <string.h>
# include <stdlib.h>
# include <math.h>
# define epsilon 0.001

int countLines(char* filePath){
    /*
     * input: file Name
     * output: number of lines in file
     */

    FILE *fp =  fopen(filePath,"r");;
    char c;
    int counter=0;

    if (fp==NULL){
        printf("Invalid Input!");
        return 1;
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
        printf("Invalid Input!");
        return 1;
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
    for (int i=0; i<rows;i++){
        for (int j=0;j<cols;j++){
            printf("  %.4f",mat[i][j]);
        }
        printf("\n");
    }
}


double** buildMatrix(int rows, int cols){
    /*
     * Creates empty matrix of size rows x cols
     */
    double **a = calloc(rows, sizeof(int*));
    for (int i=0;i<rows;i++){
        a[i] = calloc(cols, sizeof(double));
    }
    return a;
}

double** createMatrix(int rows, int cols, char* filePath){
    /*
     * Creates empty matrix and fills it with read values from file
     */
    double** matrix = buildMatrix(rows,cols);
    int lineSize = cols*16;
    FILE *fp =  fopen(filePath,"r");
    char *token; // String pointer
    int i=0,j=0;
    double tmp;

    if (fp==NULL){
        printf("Invalid Input!");
    }

    char line[lineSize];
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
    double* res = (double*)malloc(n*sizeof(double));
    for(int i=0; i<n; i++){
        res[i] = A[i] - B[i];
    }
    return res;
}

double* add_vectors(const double *A, const double *B, int n){
    double* res = (double*)malloc(n*sizeof(double));
    for(int i=0; i<n; i++){
        res[i] = A[i] + B[i];
    }
    return res;
}
double squared_dot_product(const double *A, const double *B, int n){
    double res = 0;
    for(int i=0; i<n; i++){
        res = res + (A[i] * B[i]);
    }
    return res;
}

double** copy(double** data, int K, int cols){
    double** new_mat = buildMatrix(K, cols);
    for(int i=0; i<K; i++){
        for(int j=0; j<cols; j++){
            new_mat[i][j] = data[i][j];
        }
    }
    return new_mat;
}

FILE* write_output(char* output_filename, int rows, int cols,double** centroids){

    char tmp_str[100];
    FILE* fp;
    fp = fopen(output_filename, "w");
    if (fp==NULL){
        printf("An Error Has Occurred");
        return 1;
    }
    for (int r=0;r<rows;r++){
        int c = 0;
        for (;c<cols-1;c++){
            sprintf(tmp_str,"%.4f",centroids[r][c]) ;
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
    int idx, arg_min, counter;
    double min_dist;
    double** cluster_sum;
    double** old_centroids;

    //place to treat input validity

    int points_clusters[rows]; //for each point, keeps it current closest cluster

    for(int iter=0; iter<max_iter; iter++){
        //iterate through points and assign to the closest cluster
        for (int point=0; point<rows; point++){
            min_dist = INT_MAX;
            arg_min = -1;
            for(int cluster_ind=0; cluster_ind<K; cluster_ind++){
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
        for(int r=0; r<rows; r++){
             idx = points_clusters[r];
             cluster_counter[idx] += 1;
             cluster_sum[idx] = add_vectors(cluster_sum[idx], data[r], cols);
        }

        //update centroids
        counter = 0;

        for(int k=0; k<K; k++){
            for(int c=0; c<cols; c++){
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

int validate_input_args(int argc, char* K_str, char* max_iter_str){
    /*
     * Tests:
     * 1. argc ==5
     * 2. K and max_iter are Integers
     * 3. K and max_iter > 0
     * Assumptions:
     * 1. K >= data points
     */

    double tmp_k = atof(K_str);
    int K = atoi(K_str);
    double tmp_max_iter = atof(max_iter_str);
    int max_iter = atoi(max_iter_str);

    if (argc!=5 || K!=tmp_k || max_iter!=tmp_max_iter || K<=0 || max_iter<=0){
        return 1;
    }
    return 0;
}

void kmean_test(){
    char* input_path =  "C:\\Users\\Omri\\Desktop\\CS_Omri\\Second_Year\\SW_Project\\EX_1\\K_Means_C\\KMeans_C_project\\files\\input_3.txt";
    char* output_path = "C:\\Users\\Omri\\Desktop\\CS_Omri\\Second_Year\\SW_Project\\EX_1\\K_Means_C\\KMeans_C_project\\files\\output_test_3.txt";
    int rows = countLines(input_path);
    int cols = countCols(input_path);
    int K = 15;
    double** mat = K_means(K, 200, input_path, output_path);
    printMatrix(mat, K, cols);

}


int main(int argc, char * argv[]) {
//    kmean_test();

    char* K_str = argv[1];
    char* max_iter_str = argv[2];
    char* input_name = argv[3];
    char* output_name = argv[4];

    if (validate_input_args(argc,K_str,max_iter_str)==1){
        printf("Invalid Input!");
        return 1;
    }
    //valid data
    int K = atoi(K_str);
    int max_iter = atoi(max_iter_str);


//    printf("%d", argc);
//    printf("\n");
//    printf("\n");
//    printf(max_iter);
//    printf("\n");
//    printf(input_name);
//    printf("\n");
//    printf(output_name);





}

