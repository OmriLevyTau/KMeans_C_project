#include <stdio.h>
#include <string.h>
# include <stdlib.h>


int countLines(char* filePath){
    /*
     * input: file Name
     * output: number of lines in file
     */

    FILE *fp =  fopen(filePath,"r");;
    char c;
    int counter=0;

    if (fp==NULL){
        printf("Invalid Input");
        return 0;
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
        printf("Invalid Input");
        return 0;
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
        printf("Invalid Input");
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

double** K_means(int K, char* input_filename, int max_iter){
    int rows = countLines(input_filename);
    int cols = countCols(input_filename);
    double ** data = createMatrix(rows,cols,input_filename);
    double** centroids = copy(data, K, cols);
    int idx, arg_min;
    double min_dist;
    //place to treat input validity

    int points_clusters[rows];

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
        double** old_centroids = copy(centroids,K, cols);
        double** cluster_sum = buildMatrix(K, cols); //zero matrix
        int cluster_counter[K];
        memset(cluster_counter, 0, sizeof(cluster_counter)); //zero array

        for(int r=0; r<rows; r++){
             idx = points_clusters[r];
             cluster_counter[idx] += 1;
             cluster_sum[idx] = add_vectors(cluster_sum[idx], data[r], cols);
        }

        //update centroids
        for(int k=0; k<K; k++){
            for(int c=0; c<cols; c++){
                centroids[k][c] = cluster_sum[k][c] / cluster_counter[k];
            }
        }
    }
    free(data);
    return centroids;
}


int main() {

    char* PATH =  "C:\\Users\\Omri\\Desktop\\CS_Omri\\Second_Year\\SW_Project\\EX_1\\K_Means_C\\KMeans_C_project\\input_3.txt";
    int rows = countLines(PATH);
    int cols = countCols(PATH);
//    double ** matrix = createMatrix(rows,cols,PATH);
//    printMatrix(matrix,rows,cols);
//
//    double** new_mat = copy(matrix, 3, 3);
//    printMatrix(new_mat, 3,3);
//
//    free(matrix);
    double** mat = K_means(15, PATH, 200);
    printMatrix(mat, rows,cols);

}

