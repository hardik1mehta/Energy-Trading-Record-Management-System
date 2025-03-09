#ifndef ENERGY_H
#define ENERGY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define any necessary constants
#define SIZE_ID 6
#define SIZE_OF_DATE 25

// Struct Definitions
typedef struct buyer_node_tag
{
    char bid[SIZE_ID];
    int total_amt;
    struct buyer_node_tag *next;
} buyer;

typedef struct regularBuyer
{
    char bid[SIZE_ID];
    struct regularBuyer *next;
} regularBuyer;

typedef struct seller_node_tag
{
    char seller_id[SIZE_ID];
    float perKWH_price_GT_300;
    float perKWH_price_LE_300;
    regularBuyer *regularBuyers;
    struct seller_node_tag *next;
} seller;

typedef struct pairs_tag
{
    char seller_id[SIZE_ID];
    char bid[SIZE_ID];
    int number_transactions;
    struct pairs_tag *next;
} pair;

typedef struct transaction_id_tag
{
    char transaction_id[SIZE_ID];
    char seller_id[SIZE_ID];
    char bid[SIZE_ID];
    int amount_energy;
    float price_kwh;
    char timestamp[SIZE_OF_DATE];
    struct transaction_id_tag *next;
} transaction;


// Function declarations
int isTransactionIdUnique(char *transactionID, transaction *transaction_ptr);
void CurrentTimestamp(char* buffer);
transaction *createTransactionNode(char *transactionID, char *buyerID, char *sellerID, int amt_energy, float price, const char *date_time);
seller *findorcreate_seller(seller **sellerptr, char *sellerID);
buyer *findorcreate_buyer(buyer **buyerptr, char *buyerID, int amnt);
pair *findOrCreate_pair(pair **pairptr, char *buyerID, char *sellerID);
void add_transaction(transaction **transaction_ptr, seller **sellerptr, buyer **buyerptr, pair **pairptr,char *transactionID, char *sellerID, char *buyerID, int amt_energy, char *date_time);
void printTableHeader();
void printTableRow(transaction *temp);
void displayTransactions(transaction *transaction_ptr);
void displayRegularBuyers(seller *sellerptr);
void update_transaction(transaction *transaction_ptr, seller *sellerptr, char *transactionID);

void between_transaction(char *start, char *end, transaction *transaction_ptr);
float total_revenue_seller(transaction *transaction_ptr, char *sellerID);
void display_particular_detail(transaction *ptr, char *person);
void list_of_seller(transaction *transaction_ptr, seller *sellerptr);
void list_of_buyer(transaction *transaction_ptr, buyer *buyerptr);
transaction *trans_merge(transaction *list1, transaction *list2);
transaction *trans_divide(transaction *list_ptr);
transaction* trans_sort(transaction **transaction_ptr);
void display_highest_energy(transaction *ptr);
buyer* merge(buyer *list1, buyer *list2);
buyer* divide(buyer *list_ptr);
buyer* buyer_sort(buyer **buyerptr);
pair* pair_merge(pair *list1, pair *list2);
pair* pair_divide(pair *list_ptr);
pair* pair_sort(pair **pairptr);
void display_pair(pair *lptr);
void readSellers(seller **sellerptr, const char *filename);
void readTransactions(transaction **transaction_ptr, seller **sellerptr, buyer **buyerptr, pair **pairptr, const char *filename);
void cleanUpMemory(transaction **transHead, seller **sellerHead, buyer **buyerHead, pair **pairHead);
void writeSellersToFile(seller *sellerptr, const char *filename);
void writeTransactionsToFile(transaction *transaction_ptr, const char *filename);
int validTransaction(char *s);
int validSeller(char *s);
int validBuyer(char *s);
#endif // ENERGY_H