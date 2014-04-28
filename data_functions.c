void loadCoreFileConfig(){
	FILE *fh;
	char buffer[100], **tokens;
	const char *delimiter = ",";
	
	fh = fopen("data/hcup_core_file_config.csv", "r");
	while(fgets(buffer,sizeof(buffer),fh)!=NULL){
		tokens = split(delimiter, buffer);
		
		coreconfig[atoi(tokens[0])].element_number = atoi(tokens[0]);
		strcpy(coreconfig[atoi(tokens[0])].element_name,tokens[1]);
		coreconfig[atoi(tokens[0])].start_column = atoi(tokens[2])-1;
		coreconfig[atoi(tokens[0])].end_column = atoi(tokens[3])-1;
	}
	fclose(fh);
}

void exportIcdBins(int target){
	char line[4096],code[4],icdcode[5],race[2]="",sex[2]="", age[4]="",icdstr[200],binstr[200];
	FILE *fh, *aicdfh, *abinfh, *ademfh,*iicdfh, *ibinfh, *idemfh;
	int d, pos,icdpos,bin,active_status,active_count=0,inactive_count=0;

	//open the files
	aicdfh = fopen("../output/parser/active_patient_icds_50.csv", "w");
	abinfh = fopen("../output/parser/active_patient_bins_50.csv", "w");
	ademfh = fopen("../output/parser/active_patient_dems_50.csv", "w");
	
	iicdfh = fopen("../output/parser/inactive_patient_icds_50.csv", "w");
	ibinfh = fopen("../output/parser/inactive_patient_bins_50.csv", "w");
	idemfh = fopen("../output/parser/inactive_patient_dems_50.csv", "w");

	if((fh = fopen(SOURCE_FILE, "r")) == NULL){
		fprintf(stderr, "ERROR: %s does not exist.\n",SOURCE_FILE);
		exit(0);
	}
	
	while(fgets(line,sizeof(line),fh) != NULL){
		active_status=0;
		strcpy(icdstr,"");
		strcpy(binstr,"");

		//there are only 15 codes
		pos = 138; //get the ICD9/Bin position
		icdpos = 63;
		for(d=0;d<15;d++){		
			bin=atoi(substring(pos,4,line,code));
			if(bin==target) active_status = 1;

			strcat(binstr,code);
			strcat(binstr,",");

			strcat(icdstr,substring(icdpos,5,line,icdcode));
			strcat(icdstr,",");

			pos += 4; //move to the next code
			icdpos += 5;
		}
		
		if(active_status == 1 && active_count < 5000){
			printf("Active %d\n",active_count);
			active_count++;
			fprintf(aicdfh,"%s\n",icdstr);
			fprintf(abinfh,"%s\n",binstr);
			fprintf(ademfh,"%s,%s,%s\n",substring(0,3,line,age),substring(474,1,line,race),substring(237,1,line,sex));
		}
		else if(active_status == 0 && inactive_count < 5000){
			printf("Inactive %d\n",inactive_count);
			inactive_count++;
			fprintf(iicdfh,"%s\n",icdstr);
			fprintf(ibinfh,"%s\n",binstr);
			fprintf(idemfh,"%s,%s,%s\n",substring(0,3,line,age),substring(474,1,line,race),substring(237,1,line,sex));
		}

		if(active_count >= 5000 && inactive_count >= 5000) break;
		
	}

	fclose(fh); //close input file
	fclose(aicdfh); //close output files 
	fclose(abinfh); 
	fclose(ademfh); 
	fclose(iicdfh); 
	fclose(ibinfh); 
	fclose(idemfh); 
}

/******************************************************************
* exportDiseaseStatus()
*	Purpose: export a matrix of the disease for every patient, the output
*			 will be stored in a CSV file. There will be 262 columns (features)
*			 in the spreadsheet. Columns 1-259 are the diseases and they have 
*			 binary values, the other 3 columns (age, race and sex) are 
*			 continuos.
*	Sample:
*	We have 1-259 Bins
*				1 2 3 4 ................................... 258 259
*	patient#1   0 0 1 0 ...................................  1   0
*	.           ..................................................
* 	
*	Patient #1 has disease 3 and 258
******************************************************************/
void exportDiseaseMatrix(char *outputFilename){
	char line[4096],code[4],
		 race[2]="",sex[2]="", age[4]="";
	FILE *fh, *fo;
	int DM[262], d, bin,i, field_length, iage, pos=0, neg=0, isex;

	//open the files
	fo = fopen(outputFilename, "w");

	//open the HCUP data set 
	if((fh = fopen(SOURCE_FILE, "r")) == NULL){
		fprintf(stderr, "ERROR: %s does not exist.\n",SOURCE_FILE);
		exit(0);
	}

	//create a header for the CSV file
	for(d=1;d<260;d++) fprintf(fo,"%d,",d);
	//fprintf(fo,"Age,Race,Sex\n");
	fprintf(fo,"\n");

	//for every record in the HCUP data set
	while(fgets(line,sizeof(line),fh) != NULL){
		for(i=0;i<262;i++) DM[i] = 0; //initialize the diseases for that patient to 0

		//get age, race and sex for that record
		field_length = coreconfig[1].end_column-coreconfig[1].start_column+1;
		iage = atoi(substring(coreconfig[1].start_column,field_length,line,age));
		
		field_length = coreconfig[122].end_column-coreconfig[122].start_column+1;
		substring(coreconfig[122].start_column,field_length,line,race);
		
		field_length = coreconfig[57].end_column-coreconfig[57].start_column+1;
		isex = atoi(substring(coreconfig[57].start_column,field_length,line,sex));
		
		for(d=33;d<48;d++){
			field_length = coreconfig[d].end_column-coreconfig[d].start_column+1;
			bin = atoi(substring(coreconfig[d].start_column,field_length,line,code));

			if(bin > 0 && bin < 260) DM[bin] = 1;
		}

		//write the disease and the demographics for that patient to file
		if(iage >= 40 && iage <= 45 && isex == 0){
			if((pos < 1000 && DM[50]==1) || (neg < 1000 && DM[50]==0)){
				for(d=1;d<260;d++) fprintf(fo,"%d,",DM[d]);
				fprintf(fo,"\n");

				if(DM[50] == 0) neg++;
				else pos++;
			}
			//fprintf(fo,"%s,%s,%s\n",age,race,sex);	
		}

		if(pos+neg > 2000) break;
	}

	fclose(fh); //close input file
	fclose(fo); //close output file
}

/******************************************************************
* splitDataset()
*	Purpose: this function will take a specific disease, then 
*			 it will take the HCUP data set and split the data set
*			 into 2 data sets. One data set contains the active
*			 records for that disease, the other set will contain 
*			 the inactive records.
*		     You need to generate the DISEASE_MATRIX_FILE first 
*			 before you get to this step.
*
*	Parameters:
*		code - the disease code you want to split upon
*
*	Return:
*		None
******************************************************************/
void splitDataset(int code, char *fileToSplit){
	int activeTotal=0, inactiveTotal=0;
	FILE *fhi, *fh_c1, *fh_c0;
	char line[4096], temp[4096], class1_file[100], class0_file[100], **patient;

	sprintf(class1_file,"%sactive_%d",OUTPUT_DIR, code);//filename where active samples will be stroed
	sprintf(class0_file,"%sinactive_%d",OUTPUT_DIR, code);//filename where inactive samples will be stroed
	printf("Splitting Active and Inactive records to separate files for disease code %d\n",code);

	fh_c1 = fopen(class1_file, "w");
	fh_c0 = fopen(class0_file, "w");

	//open the disease matrix file, must be genrated ahead of time
	if((fhi = fopen(fileToSplit,"r")) == NULL){
		fprintf(stderr, "ERROR: %s does not exist.\n",fileToSplit);
		exit(0);
	}

	fgets(line, sizeof(line),fhi);
	fprintf(fh_c1, "%s",line); //copy the header from DISEASE_MATRIX_FILE to the active samples file
	fprintf(fh_c0, "%s",line); //copy the header from DISEASE_MATRIX_FILE to the inactive samples file

	//for each record in the DISEASE_MATRIX_FILE
	while(fgets(line, sizeof(line),fhi) != NULL){
		strcpy(temp, line);
		patient = split(",",temp);

		if(atoi(patient[code]) == 1){
			fprintf(fh_c1, "%s",line);
			activeTotal++;
		}
		else{
			fprintf(fh_c0, "%s",line);
			inactiveTotal++;
		}

		free(patient);
	}

	printf("Keep record of the following statistics:\nTotal Active: %d\nTotal Inactive: %d\n",activeTotal,inactiveTotal);


	fclose(fhi);
	fclose(fh_c1);
	fclose(fh_c0);
}

/******************************************************************
* exportDiseaseStatus()
*	Purpose: export a matrix of the disease for every patient, the output
*			 will be stored in a CSV file. There will be 262 columns (features)
*			 in the spreadsheet. Columns 1-259 are the diseases and they have 
*			 binary values, the other 3 columns (age, race and sex) are 
*			 continuos.
*	Sample:
*	We have 1-259 Bins
*				1 2 3 4 ................................... 258 259
*	patient#1   0 0 1 0 ...................................  1   0
*	.           ..................................................
* 	
*	Patient #1 has disease 3 and 258
******************************************************************/
void sampleClassData(char *filename, int fileprefix, int totalRecords){
	int *samples, i=0, ctotal = 0, record=0, numSamples=MAX_SAMPLE_NUM;
	FILE *fhi, *fho;
	char line[4096], samplefile[100];

	printf("Generating sub-sample from %d total samples\n",totalRecords);
	
	if(totalRecords < numSamples) numSamples = totalRecords;
	samples = randomNumbers(0,totalRecords,numSamples);

	sprintf(samplefile,"%s_%d.sample", filename, fileprefix);
	fho = fopen(samplefile, "w");

	printf("Creating sample %s\n",samplefile);

	if((fhi = fopen(filename,"r")) == NULL){
		fprintf(stderr, "ERROR: %s does not exist.\n",filename);
		exit(0);
	}
	
	fgets(line, sizeof(line),fhi);
	fprintf(fho,"%s",line);

	while(fgets(line, sizeof(line),fhi) != NULL && ctotal < 30000){
		if(samples[ctotal] == record){
			fprintf(fho, "%s",line);
			ctotal++;
		}
		record++;
	}

	free(samples);
	fclose(fhi);

	fclose(fho);
}

/******************************************************************
* exportDiseaseStatus()
*	Purpose: export a matrix of the disease for every patient, the output
*			 will be stored in a CSV file. There will be 262 columns (features)
*			 in the spreadsheet. Columns 1-259 are the diseases and they have 
*			 binary values, the other 3 columns (age, race and sex) are 
*			 continuos.
*	Sample:
*	We have 1-259 Bins
*				1 2 3 4 ................................... 258 259
*	patient#1   0 0 1 0 ...................................  1   0
*	.           ..................................................
* 	
*	Patient #1 has disease 3 and 258
******************************************************************/
void diseasePrevalence(){
	FILE *fh, *fho;
	int pos, bin, binstat[260]={0}, agestat[125]={0}, racestat[7]={0}, sexstat[2]={0},d;
	int iage,irace,isex,record=1, i;
	char race[2]="",sex[2]="", age[4]="";
	char line[4096],code[4];
	
	fho = fopen("stats.csv", "w");
	if((fh = fopen(SOURCE_FILE, "r")) == NULL){
		fprintf(stderr, "ERROR: %s does not exist.\n",SOURCE_FILE);
		exit(0);
	}
	
	while(fgets(line,sizeof(line),fh) != NULL){
		if((record++ % 10000)==0) printf("Current Record #%d\n",record);
		
		iage = atoi(substring(0,3,line,age)); //get age
		irace = atoi(substring(474,1,line,race)); //get race
		isex = atoi(substring(237,1,line,sex)); //get sex
		
		if(iage>=0 && iage<=124) agestat[iage] += 1;
		
		if(irace>=1 && irace<=6) racestat[irace] += 1;
		else racestat[0] += 1;
		
		if(isex>=0 && isex<=1) sexstat[isex] += 1;
		
		//there are only 15 codes
		pos = 138; //get the ICD9/Bin position
		for(d=0;d<15;d++){
			bin = atoi(substring(pos,4,line,code));
			pos += 4; //move to the next code

			if(bin > 0 && bin < 260)
				binstat[bin] += 1;
		}
	}
	
	printf("Writing STATS to a file\n");
	
	for(i=1;i<260;i++) fprintf(fho,"%d,%d,%f\n",i,binstat[i],((float)binstat[i]/(float)record)*100);
	fprintf(fho,"\n\n");
	for(i=0;i<125;i++) fprintf(fho,"%d,%d,%f\n",i,agestat[i],((float)agestat[i]/(float)record)*100);
	fprintf(fho,"\n\n");
	for(i=0;i<7;i++) fprintf(fho,"%d,%d,%f\n",i,racestat[i],((float)racestat[i]/(float)record)*100);
	fprintf(fho,"\n\n");
	for(i=0;i<2;i++) fprintf(fho,"%d,%d,%f\n",i,sexstat[i],((float)sexstat[i]/(float)record)*100);
	
	printf("DONE\n");
	fclose(fh);
	fclose(fho);
}

void julieDataExport(){
	FILE *fh, *fhi;
	char line[4096], *icd9, code[6], *key, *state;
	int field_length, d, ikey, count=0;
	int idx[7] = {60,14,58,67,17,59,126};

	if((fh = fopen("data/output/julie.csv","w")) == NULL){
		fprintf(stderr,"Could not create a destination file.\n");
		exit(0);
	}

	if((fhi = fopen(SOURCE_FILE,"r")) == NULL){
		fprintf(stderr, "ERROR: %s does not exist.\n",SOURCE_FILE);
		exit(0);
	}
	
	fprintf(fh,"KEY,DRG,HOSPID,NDX,DSHOSPID,HOSPST,ZIPInc_Qrtl,DX1,DX2,DX3,DX4,DX5,DX6,DX7,DX8,DX9,DX10,DX11,DX12,DX13,DX14,DX15\n");

	while(fgets(line, sizeof(line), fhi) != NULL){
		state = malloc(15 * sizeof(char));
		field_length = coreconfig[59].end_column-coreconfig[59].start_column+1;	
		substring(coreconfig[59].start_column,field_length,line,state);

		/*if(strcmp(state,"MO")==0) {
			printf("%s\n",line);
		}*/

		/*for(d=0;d<4;d++){
			key = malloc(15 * sizeof(char));	
			field_length = coreconfig[idx[d]].end_column-coreconfig[idx[d]].start_column+1;
			ikey = atoi(substring(coreconfig[idx[d]].start_column,field_length,line,key));
			if(strcmp(state,"MO")==0) {
				printf("%d\n",ikey);
				fprintf(fh,"%d,",ikey);
			}
		}*/

		for(d=0;d<7;d++){
			key = malloc(15 * sizeof(char));	
			icd9 = malloc(15 * sizeof(char));
			field_length = coreconfig[idx[d]].end_column-coreconfig[idx[d]].start_column+1;
			icd9 = substring(coreconfig[idx[d]].start_column,field_length,line,key);
			if(strcmp(state,"MO")==0)fprintf(fh,"%s,",icd9);
		}

		for(d=18;d<33;d++){
			field_length = coreconfig[d].end_column-coreconfig[d].start_column+1;
			icd9 = malloc(6 * sizeof(char));
			icd9 = substring(coreconfig[d].start_column,field_length,line,code);
			if(strcmp(state,"MO")==0)fprintf(fh,"%s,",icd9);
		}

		if(strcmp(state,"MO")==0) {
			count++;
			fprintf(fh,"\n");
		}
		
		if(count == 100000) {
			return;
		}
	}

	fclose(fhi);
	fclose(fh);
}

/******************************************************************
* usageError()
*	Purpose: display error message with the code usage information
******************************************************************/
void usageError(){
	char errormsg[3000] = "Usage: ./nis [OPTION]\n"
						  "------------------------------------------------------------------\n"
						  "OPTIONS:\n"
						  "  [0] = ./nis 0\n"
						  "      This option will take the entire dataset and create a \n"
						  "      file called disease matrix, which will contain all the \n"
						  "      records and only 262 features. These features are the 259\n"
						  "      disease classification codes, age, sex and race. The values\n"
						  "      for the 259 codes will be binary. The final output is a CSV file\n"
						  "      with 7,995,048 records and 262 features.\n\n"
						  "  [1] = ./nis 1 [DISEASE_CLASSIFICATION_CODE]\n"
						  "      This option will take an additional argument which is the\n"
						  "      disease classification code. For that code it will create\n"
						  "      two files, one contains the active records (patients that have\n"
						  "      that disease), the other will contain the inactive records.\n"
						  "      You MUST generate the disease matrix before you run this command.\n"
						  "      See option [0] above. This option will output two number that you\n"
						  "      need to record, #of active records and #of inactive records.\n"
						  "      These numbers arewill be used in option [2] to generate random\n"
						  "      samples for the final dataset.\n"
						  "      Example usage: ./nis 1 98 (split for disease code Hypertension).\n\n"
						  "  [2] = ./nis 2 [FILE_TO_SAMPLE_FROM] [FILE_PREFIX] [NUM_OF_RECORDS]\n"
						  "      Generate dataset from a specific disease code. 30,000 samples are\n"
						  "      chossen randomly from the active and inactive files generated in \n"
						  "      in option [1]. This option will take additional arguments:\n"
						  "          FILE_TO_SAMPLE_FROM: the file that you want to choose samples\n"
						  "                               from (generated from option [1]).\n"
						  "          FILE_PREFIX:         a prefix that you identify the file with\n"
						  "          NUM_OF_RECORDS:      this defines the number of active or inactive\n"
						  "                               recods as reported by option [1]. For example,\n"
						  "                               if number of records is 444,000, then we will\n"
						  "                               choose random 30,000 samples from the 444,000\n"
						  "      You will run this option twice, once to sample active records and once\n"
						  "      for inactive records.\n"
						  "      Example: ./nis 2 output/active_98 sample 444,000\n"
						  "               ./nis 2 output/inactive_98 sample 7,551,048\n\n"
						  "  [3] = ./nis 3\n"
						  "      Generate a file containing statistics about the data. The output file\n"
						  "      will contain:\n"
						  "         1) Disease Prevalence: shows percentage of records having this disease\n"
						  "         2) Age Statistics\n"
						  "         3) Race Statistics\n"
						  "         4) Sex Statistics\n\n";

	fprintf(stderr, "%s", errormsg);
	exit(0);
}
