/********************************************************************
* Includes
********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mysql.h>

/********************************************************************
* Configurations
********************************************************************/
//number of samples in every file
#define MAX_SAMPLE_NUM 30000

//orginal dataset with all the features and records
#define SOURCE_FILE 		"/home/mohammed/Research/Datasets/NIS/NIS2005/NIS_2005_Core.ASC"

//output directory for all the output files
#define OUTPUT_DIR			"data/output/"

/********************************************************************
* MYSQL configs
********************************************************************/
#define HOSTNAME	"localhost"
#define USERNAME 	"root"
#define PASSWORD 	"root"
#define DBNAME		"nis"

struct coreconfig{
	int element_number;
	char element_name[50];
	int start_column;
	int end_column;
} coreconfig[127];

typedef struct{
	MYSQL_ROW row;
} patient;
	
/********************************************************************
* Function prototypes
********************************************************************/
//functions specific to the dataset
void loadCoreFileConfig();
void exportDiseaseMatrix(char *);
void splitDataset(int, char *);
void exportIcdBins(int);
void sampleClassData(char *, int, int);
void diseasePrevalence();
void usageError();

//MySQL functions
MYSQL *dbConnect();
void dbQuery(MYSQL *, char *);
void dbFreeResutls(MYSQL_RES *);
void dbClose(MYSQL *);

//Load to DB functions
void createTable();
void loadDataToDB();
void saveRecord();

//general functions
char *substring(int, int, char *, char *);
char **split(const char *, char *);
int *randomNumbers(int, int, int);
void quicksort(int[], int, int);
void swap(int *, int *);

//patient distance
void getHospPatients();
float getPatientDist(patient,patient);
float computeICD9Dist(const char *, const char *);

//DRG occurrence
void getDRGOccurrence();
void writeMatrixToFile(int **);
