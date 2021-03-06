#include <iostream>
#include <string>
#include "sqlite3.h"

enum state {
	CLEAN = 0,
	UNCHECKED = 1,
	VERIFIED = 2
};

int GenerateDB(void);
int FetchPrint(void);

static int callback(void* data, int argc, char** argv, char** azColName)
{
	int i;
	//fprintf(stderr, "%s: ", (const char*)data);

	for (i = 0; i < argc; i++) {
		printf("%s = %s ", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	printf("\n");
	return 0;
}

int main(void) {
	std::cout << "Select mode: (1 = GenerateDB, 2 = Printing, 3 = Error detection) " << std::endl;
	int Mode;
	std::cin >> Mode;
	if (Mode == 1) {
		GenerateDB();
	}
	else if(Mode == 2) {
		FetchPrint();
	}
	else {
		std::cout << "Not implemented" << std::endl;
	}
}

int GenerateDB(void)
{
	sqlite3* DB;
	int exit = 0;
	exit = sqlite3_open("MDB_DB.db", &DB);

	if (exit) {
		std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
		return (-1);
	}
	else
		std::cout << "Opened Database Successfully!" << std::endl;

	std::string DB_name;
	std::cout << "Input database name: ";
	std::getline(std::cin,DB_name);

	std::string sql = "CREATE TABLE " + DB_name + "("
		"ID INT PRIMARY KEY     NOT NULL, "
		"TEXT           TEXT    NOT NULL, "
		"STATE            INT     NOT NULL);";

	char* messaggeError;
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

	if (exit != SQLITE_OK) {
		std::cerr << "Error Create Table" << std::endl;
		sqlite3_free(messaggeError);
	}
	else
		std::cout << "Table created Successfully" << std::endl;
	

	for (size_t i = 0; i < 20; i++)
	{
		std::string sql = "INSERT INTO " + DB_name + " (ID, STATE, TEXT)"
			"VALUES(" + std::to_string(i) + ", 0, 'Label " + std::to_string(i + 1) + "');";
		char* messaggeError;
		exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);
		if (exit != SQLITE_OK) {
			std::cerr << "Error add data" << std::endl;
			sqlite3_free(messaggeError);
			return (-1);
		}
	}
	std::string query = "SELECT * FROM " + DB_name + ";";
	std::cout << "20 entries have been added" << std::endl;
	sqlite3_exec(DB, query.c_str(), callback, NULL, NULL);

	sqlite3_close(DB);
	return (0);
}


int FetchPrint(void)
{
	sqlite3* DB;
	int exit = 0;
	exit = sqlite3_open("MDB_DB.db", &DB);

	if (exit) {
		std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
		return (-1);
	}
	else
		std::cout << "Opened Database Successfully!" << std::endl;

	std::string DB_name = "ExampleDB";


	std::cout << "Fetching text to print: " << std::endl;

	std::string sql = "SELECT TEXT, MIN(ID) FROM " + DB_name + " WHERE STATE=0;";
	char* messaggeError;
	exit = sqlite3_exec(DB, sql.c_str(), callback, 0, &messaggeError);

	if (exit != SQLITE_OK) {
		std::cerr << "Error fetch: " << messaggeError << std::endl;
		sqlite3_free(messaggeError);
		return (-1);
	}


	std::string query = "SELECT * FROM " + DB_name + ";";
	sqlite3_exec(DB, query.c_str(), callback, NULL, NULL);

	sqlite3_close(DB);
	return (0);
}