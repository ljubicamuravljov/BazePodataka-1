
#include <iostream>
#include "sqlite3.h"
#include <vector>
#include <exception>

using namespace std;
class exceptionNestalaStruja : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Nestala struja!";
    }
};

void print_all_users(sqlite3* db) {

    sqlite3_stmt* stmt;
    const char* sql = " SELECT idKom,Naziv,Adresa FROM Komitent";
    int rc = sqlite3_prepare(db, sql, 256, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s \n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
            
    }
    printf("Svi useri: \n");

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        if (sqlite3_column_type(stmt, 0) == SQLITE_INTEGER) {
            printf("%d \t", sqlite3_column_int(stmt, 0));

        }
        printf("%s \t", sqlite3_column_text(stmt, 1));
        printf("%s \n", sqlite3_column_text(stmt, 2));

    }
    printf("\n");
    sqlite3_finalize(stmt);

}
void print_users_address(sqlite3* db, const char* address) {

    sqlite3_stmt* stmt;
    const char* sql = "SELECT idKom,Naziv,Adresa from Komitent where Adresa=?";
    
    int rc = sqlite3_prepare(db, sql, 256, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s \n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_bind_text(stmt, 1, address, -1, nullptr);
    printf("Svi useri na adresi %s:  \n",address);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        printf("%d \t", sqlite3_column_int(stmt, 0));
        printf("%s \t", sqlite3_column_text(stmt, 1));
        printf("%s \n", sqlite3_column_text(stmt, 2));

    }

    printf("\n");
    sqlite3_finalize(stmt);
    
}

void print_all_bankaccounts(sqlite3* db) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT idRac,Stanje, Status from Racun";
    int rc = sqlite3_prepare(db, sql, 256, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s \n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;

    }
    printf("Svi racuni: \n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    printf("%d \t", sqlite3_column_int(stmt, 0));
    printf("%d \t", sqlite3_column_int(stmt, 1));
    printf("%s \n", sqlite3_column_text(stmt, 2));
    }
    printf("\n");
    sqlite3_finalize(stmt);

}
int insert_user(sqlite3* db, const char* name, const char* address) {
    sqlite3_stmt* stmt;
    const char* sql = "insert into komitent(naziv,adresa) values(?,?)";
    int rc = sqlite3_prepare(db, sql, 256, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        printf("SQL error %s \n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;

    }
    sqlite3_bind_text(stmt, 1, name, -1, nullptr);
    sqlite3_bind_text(stmt, 2, address, -1, nullptr);

    if (sqlite3_step(stmt) == SQLITE_DONE) {

        int id = sqlite3_last_insert_rowid(db);
        printf("Dodat je komitent sa id %d\n", id);
        sqlite3_finalize(stmt);
        return id;

    }
    else {
        printf("SQL error %s \n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;

    }



}
void moneyTransfer(sqlite3* db, int idRacFrom, int idRacTo, int iznos, bool nestalaStruja) {

    bool errorFlag = false;
    try {

        sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

        const char* sql = "UPDATE Racun SET Stanje= Stanje+ ? WHERE idRac=?";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare(db, sql, 256, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            errorFlag = true;
        }
        else {

            sqlite3_bind_int(stmt, 1, -iznos);
            sqlite3_bind_int(stmt, 2, idRacFrom);
            if ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
                errorFlag = true;
            }

         if (nestalaStruja)
                throw exceptionNestalaStruja();

            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);

            sqlite3_bind_int(stmt, 1, iznos);
            sqlite3_bind_int(stmt, 2, idRacTo);
            if ((rc = sqlite3_step(stmt)) != SQLITE_DONE)
                errorFlag = true;
        }
        sqlite3_finalize(stmt);
        if (errorFlag) {

            printf("Greska: %s \n", sqlite3_errmsg(db));
            sqlite3_exec(db, "ROLLBACK TRANSACTION", nullptr, nullptr, nullptr);


        }
        else {
            sqlite3_exec(db, "COMMIT TRANSACTION", nullptr, nullptr, nullptr);
        }
    }
        catch (std::exception) {
            sqlite3_exec(db, "ROLLBACK TRANSACTION", nullptr, nullptr, nullptr);

        }




    



}

void ispisStavki(sqlite3* db) {

    sqlite3_stmt* stmt;
    const char* sql = "Select * From Stavka";
    int rc = sqlite3_prepare(db, sql, 256, &stmt, nullptr);
      if (rc != SQLITE_OK) {
        printf("SQL error: %s \n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
            
    }
    printf("Sve stavke: \n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {


        printf("%d \t", sqlite3_column_int(stmt, 0));
        printf("%d \t", sqlite3_column_int(stmt, 1));
        printf("%s \t", sqlite3_column_text(stmt,2));
        printf("%s \t", sqlite3_column_text(stmt,3));
        printf("%d \t", sqlite3_column_int(stmt,4));
        printf("%d \t", sqlite3_column_int(stmt,5));
        printf("%d \n", sqlite3_column_int(stmt,6));


    }

    printf("\n");
    sqlite3_finalize(stmt);
    return;


}
void ispisRacuna(sqlite3* db) {

    sqlite3_stmt* stmt;
    const char* sql = "Select * FROM Racun";

    int rc = sqlite3_prepare(db, sql, 256, &stmt, nullptr);
    if (rc != SQLITE_OK) {



    }
    printf("Svi racuni: \n");
       
    printf("IdRac\tStatus\tBrojStavki\tDozvMinus\tStanje\tIdFil\tIDKom\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        printf("%d \t", sqlite3_column_int(stmt, 0));
        printf("%s \t", sqlite3_column_text(stmt, 1));
        printf("%d \t\t", sqlite3_column_int(stmt, 2));
        printf("%d \t\t", sqlite3_column_int(stmt, 3));
        printf("%d \t", sqlite3_column_int(stmt, 4));
        printf("%d \t", sqlite3_column_int(stmt, 5));
        printf("%d \n", sqlite3_column_int(stmt, 6));




    }

    printf("\n");
    sqlite3_finalize(stmt);
    return;

}
void ispisUplata(sqlite3* db) {

     sqlite3_stmt* stmt;
    const char* sql = "Select * FROM Uplata";

    int rc = sqlite3_prepare(db, sql, 256, &stmt, nullptr);
    if (rc != SQLITE_OK) {



    }
    printf("Sve uplate: \n");
       
    printf("IdSta\tOsnov\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        printf("%d \t", sqlite3_column_int(stmt, 0));
        printf("%s \n", sqlite3_column_text(stmt, 1));


    }

    printf("\n");
    sqlite3_finalize(stmt);
    return;


}

int maxIdSta(sqlite3* db) {
	sqlite3_stmt* stmt;
	const char* sql = "SELECT MAX(IdSta) FROM Stavka";
	int idSta = -1;
	int rc = sqlite3_prepare(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		printf("SQL Error: %s \n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		idSta = sqlite3_column_int(stmt, 0) + 1;
	}
	sqlite3_finalize(stmt);
	return idSta;
}

int Fja(sqlite3* db, int idKom, int idFil) {
    sqlite3_stmt* stmt;

    const char* sql = "UPDATE Racun SET Stanje=-3000,Status='B' WHERE idKom=? AND Stanje>=0";
    int rc = sqlite3_prepare(db, sql, 256, &stmt, nullptr);
     sqlite3_bind_int(stmt, 1, idKom);
     rc = sqlite3_step(stmt);
       sqlite3_finalize(stmt);
       ispisRacuna(db);

   sql = "Select Stanje,idRac  From Racun WHERE idKom=? AND Stanje<0";
    vector<int> racuni;
    vector<int> vrednosti;
    rc = sqlite3_prepare(db, sql, 256, &stmt, nullptr);
    if (rc != SQLITE_OK) {


    }
    int suma = 0;

    sqlite3_bind_int(stmt, 1, idKom);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        suma -= sqlite3_column_int(stmt, 0);

        racuni.push_back(sqlite3_column_int(stmt, 1));
        vrednosti.push_back(-sqlite3_column_int(stmt, 0));


    }
    sqlite3_finalize(stmt);

    //Update Racuna



    sql = "UPDATE Racun SET Stanje=0,Status='A' WHERE idKom=? AND Stanje<0";
    rc = sqlite3_prepare(db, sql, 256, &stmt, nullptr);
    if (rc != SQLITE_OK) {

    }


    sqlite3_bind_int(stmt, 1, idKom);


    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

    }
    sqlite3_finalize(stmt);

    //Update Stavke
    int nIdSta = maxIdSta(db);
    sql = "INSERT INTO Stavka(idSta,RedBroj,Datum,Vreme,Iznos,idFil,idRac) VALUES (?,?,DATE(),TIME(),?,?,?)";

    rc=sqlite3_prepare(db, sql, 256, &stmt, nullptr);
    if (rc != SQLITE_OK) {

    }

    int nredBroj = 0;
    int i = 0, j = 0;
    while (racuni.size()>j) {
        sqlite3_bind_int(stmt, 1, nIdSta++);
        sqlite3_bind_int(stmt, 2, nredBroj++ );
        sqlite3_bind_int(stmt, 3, vrednosti[i++] );
        sqlite3_bind_int(stmt, 4, idFil);
        sqlite3_bind_int(stmt, 5, racuni[j++]);

        rc = sqlite3_step(stmt);

         sqlite3_reset(stmt);
       sqlite3_clear_bindings(stmt);
      
     
    }

    sqlite3_finalize(stmt);
    return suma;


}

int main()
{
    sqlite3* db;
    int rc = sqlite3_open("Banka.db", &db);
    if (rc != SQLITE_OK) {
        printf("Greska pri otvaranju!\n");
        return 0;
    }
    printf("Uspesno otvorena!\n");


   

    ispisStavki(db);
    ispisRacuna(db);
    ispisUplata(db);


    printf("%d ", Fja(db, 2, 2));
    ispisRacuna(db);
    ispisStavki(db);
    return 0;
}

