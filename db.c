/***********************************************************************
* dbConnect()
***********************************************************************/
MYSQL *dbConnect(){
	MYSQL *connection;
	connection = mysql_init(NULL);
	if(!mysql_real_connect(connection,HOSTNAME,USERNAME,PASSWORD,DBNAME,0,NULL,0)){
		fprintf(stderr,"%s\n",mysql_error(connection));
		exit(1);
	}
	return connection;
}

/***********************************************************************
* dbQuery()
***********************************************************************/
void dbQuery(MYSQL *connection, char *sql){
	if(mysql_query(connection,sql)){
		fprintf(stderr,"%s\n",mysql_error(connection));
		exit(1);
	}
}
