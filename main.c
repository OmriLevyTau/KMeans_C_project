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
            printf(" %.4f",mat[i][j]);
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


int main() {

    char* PATH =  "C:\\Users\\Omri\\Desktop\\CS_Omri\\Second_Year\\SW_Project\\EX_1\\K_Means_C\\KMeans_C_project\\input_1.txt";
    int rows = countLines(PATH);
    int cols = countCols(PATH);
    double ** matrix = createMatrix(rows,cols,PATH);
    printMatrix(matrix,rows,cols);
    free(matrix);

}

