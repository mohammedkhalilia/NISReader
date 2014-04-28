/***********************************************************************
* createTable()
***********************************************************************/
void createTable(){
	char sql[5000];
	int i;
	MYSQL *connection = dbConnect();
	
	strcpy(sql,"create table core_nis(\nid int auto_increment,");
	
	for(i=1;i<127;i++){
		strcat(sql,coreconfig[i].element_name);
		strcat(sql," varchar(50),\n");
	}
	
	strcat(sql,"primary key(id));");

	dbQuery(connection,sql);
	mysql_close(connection);
}

/***********************************************************************
* loadDataToDB()
***********************************************************************/
void loadDataToDB(){
	FILE *fh;
	int i,record=0, field_length;
	char sql[5000], buffer[2000], *value;
	
	if((fh = fopen(SOURCE_FILE, "r")) == NULL){
		fprintf(stderr, "ERROR: %s does not exist.\n",SOURCE_FILE);
		exit(0);
	}
	
	while(fgets(buffer,sizeof(buffer),fh) != NULL){
		sprintf(sql,"insert into core_nis values (%d,",++record);
		
		for(i=1;i<127;i++){
			field_length = coreconfig[i].end_column-coreconfig[i].start_column+1;
			value = malloc(sizeof(char)*field_length);
			substring(coreconfig[i].start_column,field_length,buffer, value);
				
			strcat(sql,"'");
			strcat(sql,value);
			strcat(sql,"'");
			
			if(i<126) strcat(sql,",");
			
			free(value);
		}
		
		strcat(sql,");");
		saveRecord(sql);
		
		if(record % 1000 == 0)
			printf("Saving record #%d\n",record);
	}
	
	fclose(fh);
}

/***********************************************************************
* saveRecord()
***********************************************************************/
void saveRecord(char *sql){
	MYSQL *connection = dbConnect();
	dbQuery(connection,sql);
	mysql_close(connection);	
}
