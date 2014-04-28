void getHospPatients(){
	char sql[1000];
	MYSQL_RES *results;
	MYSQL_ROW row;
	MYSQL *connection = dbConnect();
	int numRecords, p = 0, p1 = 0;
	FILE *fh;
	
	fh = fopen("data/output/sim_matrix.csv","w");
	sprintf(sql,"(SELECT DX1,DX2,DX3,DX4,DX5,DX6,DX7,DX8,DX9,DX10,DX11,"
				"DX12,DX13,DX14,DX15 FROM core_nis WHERE DXCCS1=24 OR DXCCS2=24 "
				"OR DXCCS3=24 OR DXCCS4=24 OR DXCCS5=24 OR DXCCS6=24 OR DXCCS7=24 OR DXCCS8=24 "
				"OR DXCCS9=24 OR DXCCS10=24 OR DXCCS11=24 OR DXCCS12=24 OR DXCCS13=24 OR DXCCS14=24 "
				"OR DXCCS15=24 limit 1000) "
				"UNION ALL"
				"(select DX1,DX2,DX3,DX4,DX5,DX6,DX7,DX8,DX9,DX10,DX11,"
				"DX12,DX13,DX14,DX15 FROM core_nis WHERE DXCCS1=29 OR DXCCS2=29 "
				"OR DXCCS3=29 OR DXCCS4=29 OR DXCCS5=29 OR DXCCS6=29 OR DXCCS7=29 OR DXCCS8=29 "
				"OR DXCCS9=29 OR DXCCS10=29 OR DXCCS11=29 OR DXCCS12=29 OR DXCCS13=29 OR DXCCS14=29 "
				"OR DXCCS15=29 limit 1000)");
				
	dbQuery(connection,sql);
	results = mysql_store_result(connection);
	numRecords = mysql_num_rows(results);
	patient patients[numRecords];
	
	while((row = mysql_fetch_row(results)) != NULL)
		patients[p++].row = row;
	
	for(p=0;p<numRecords;p++){
		printf("Patient %d\n",p+1);
		
		for(p1=0;p1 <= p;p1++) fprintf(fh,",");
		
		for(p1=p+1;p1<numRecords;p1++)
			fprintf(fh,"%f,",getPatientDist(patients[p],patients[p1]));
		
		fprintf(fh,"\n");
	}
		
	mysql_free_result(results);
	mysql_close(connection);
	fclose(fh);
}

float getPatientDist(patient patient1,patient patient2){
	int i,j,DRGCount=0;
	float dist = 0.0;
	
	for(i=0;i<sizeof(patient1.row);i++){
		for(j=0;j<sizeof(patient2.row);j++){
			if(strcmp(patient1.row[i],"     ") && strcmp(patient2.row[j],"     ")){
				dist += computeICD9Dist(patient1.row[i],patient2.row[j]);
				DRGCount++;
			}
		}
	}
	
	return dist/DRGCount;
}

float computeICD9Dist(const char *code1, const char *code2){
	int i;
	float dist = 0.0;
	
	for(i=0;i<5;i++)
		if(strncmp(code1+i,code2+i,1))
			dist += 0.2;
		else
			break;
			
	return dist;
}
