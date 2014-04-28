/*********************************************************************
* Author: Mohammed Khalilia
*
* Purpose:
*	Extract data from the NIS 2005 Core file, file specification is
*	available @:
*	http://www.hcup-us.ahrq.gov/db/nation/nis/tools/stats/FileSpecifications_NIS_2005_Core.TXT
*	Data Elements: http://www.hcup-us.ahrq.gov/db/nation/nis/nisdde.jsp
*	Number of records: 7,995,048
*
* 7 top chronic diseases:
*  Source: http://www.chronicdiseaseimpact.com/
*********************************************************************/

/********************************************************************
* main()
********************************************************************/
int main(int argc, char **argv){
	//validation: provide an option
	if(argc < 2)
		usageError();

	//if option provided
	switch(atoi(argv[1])){
		case 0:
			/**
			 * export the disease matrix for the entire dataset
			 * matrix: 7,995,048 x 262
			 * This will be a binary matrix generated as a CSV file
			 * Execpt the last 3 columns will have continous values (age, race and sex)
			 */
			if(argc < 3)
				usageError();
				
			loadCoreFileConfig();
			exportDiseaseMatrix(argv[2]);
			break;
		case 1:
			//no disease code, then error
			if(argc < 4)
				usageError();
				
			/**
			 * generate testing/training data by disease code
			 * this will generate 2 files, one containing active data
			 * the other has inactive data both of the same size.
			 * The format will be the same as the above case
			 */
			splitDataset(atoi(argv[2]),argv[3]);
			break;
		case 2:
			if(argc < 4)
				usageError();
			/**
			* sample records from the specified file
			*	Argument 1: file to samples from
			*	Argument 2: prefix to the file (i.e "sample")
			*	Argument 3: number of records to sample
			*/
			sampleClassData(argv[2], atoi(argv[3]), atoi(argv[4]));
			break;
		case 3:
			/**
			* output some statistics about the data
			* 	disease prevalance, age, race and sex statistics as well.
			*/
			diseasePrevalence();
			break;
		case 4:
			/**
			* output some statistics about the data
			* 	disease prevalance, age, race and sex statistics as well.
			*/
			exportIcdBins(atoi(argv[2]));
			break;
		case 5:
			/**
			 * Parse data and store it in a database
			 */
			 loadCoreFileConfig();
			 createTable();
			 loadDataToDB();
			break;
		case 6:
			getHospPatients();
			break;
		case 7:
			getDRGOccurrence();
			break;
		case 8:
			loadCoreFileConfig();
			julieDataExport();			
			break;
		default:
			/**
			* None of the options obove, then error
			*/
			usageError();
			break;
	}
	return 0;
}
