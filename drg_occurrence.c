void getDRGOccurrence(){
	char sql[1000];
	int **drg_occurrence;
	MYSQL_RES *results;
	MYSQL_ROW row;
	MYSQL *connection = dbConnect();
	int i, j, start, end, drg, drg1, drg2, code;
	
	//allocate memory
	drg_occurrence = malloc(sizeof(int *) * 260);
	for(i=0;i<261;i++)
		drg_occurrence[i] = malloc(sizeof(int) * 260);
		
	//initialize DRG occurrence array
	for(i=0;i<261;i++)
		for(j=0;j<261;j++)
			drg_occurrence[i][j] = 0;
	
	for(drg=1;drg<260;drg++){
		printf("DRG %d co-occurrences\n",drg);
		sprintf(sql,"SELECT DXCCS1,DXCCS2,DXCCS3,DXCCS4,DXCCS5,DXCCS6,DXCCS7,DXCCS8,DXCCS9,DXCCS10,DXCCS11, DXCCS12,DXCCS13,"
					"DXCCS14,DXCCS15 "
					"FROM core_nis "
					"WHERE DXCCS1=%d OR DXCCS2=%d OR DXCCS3=%d OR DXCCS4=%d OR DXCCS5=%d OR DXCCS6=%d OR DXCCS7=%d "
					"OR DXCCS8=%d OR DXCCS9=%d OR DXCCS10=%d OR DXCCS11=%d OR DXCCS12=%d OR DXCCS13=%d OR DXCCS14=%d OR DXCCS15=%d",
					drg,drg,drg,drg,drg,drg,drg,drg,drg,drg,drg,drg,drg,drg,drg);
		/*sprintf(sql,"SELECT DXCCS1,DXCCS2,DXCCS3,DXCCS4,DXCCS5,DXCCS6,DXCCS7,DXCCS8,DXCCS9,DXCCS10,DXCCS11, DXCCS12,DXCCS13,"
					"DXCCS14,DXCCS15 "
					"FROM core_nis limit %d,1000000",start*1000000);
			*/		
		dbQuery(connection,sql);
		results = mysql_store_result(connection);
		
		while((row = mysql_fetch_row(results)) != NULL){
			for(i=0;i<15;i++){
				code = atoi(row[i]);
				if(code > 0 && code < 260)
					drg_occurrence[drg][code]++;
			}
			
			/*for(i=1;i<14;i++){
				drg1 = atoi(row[i]);
				drg2 = atoi(row[i+1]);
				
				if(strcmp(row[i],"-999") == 0 || strcmp(row[i+1],"-999") == 0)
					break;
				
				if(drg1 > 0 && drg1 < 260 && drg2 > 0 && drg2 < 260)
					drg_occurrence[drg1][drg2]++;
			}*/
		}
		
		mysql_free_result(results);
	}
		
	mysql_close(connection);
	writeMatrixToFile(drg_occurrence);
}

void writeMatrixToFile(int **drg_occurrence){
	FILE *fh;
	int i, j;
	
	fh = fopen("data/output/drg_occurrence.csv","w");
	
	for(i=1;i<260;i++){
		for(j=1;j<260;j++)
			fprintf(fh,"%d,",drg_occurrence[i][j]);
		
		fprintf(fh,"\n");
	}
	
	fclose(fh);
}
