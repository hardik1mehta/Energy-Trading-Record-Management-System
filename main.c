#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "energy_trading_system.h"
int main()
{
    transaction *transaction_ptr = NULL;
    seller *sellerptr = NULL;
    buyer *buyerptr = NULL;
    pair *pairptr = NULL;
    void *temptransptr = NULL;
    void *tempbuyer = NULL;
    void *temppairptr = NULL;

    // Read sellers and transactions from files
    readSellers(&sellerptr, "sellers.txt");
    readTransactions(&transaction_ptr, &sellerptr, &buyerptr, &pairptr, "transactions.txt");

    int choice;
    char transactionID[SIZE_ID], sellerID[SIZE_ID], buyerID[SIZE_ID], timestamp[SIZE_OF_DATE];
    int amt_energy;
    float revenue;
    char start[SIZE_OF_DATE], end[SIZE_OF_DATE];
    do
    {
        printf("\nEnergy Trading Record Management System\n");
        printf("...................................................");
        printf("\n1. Add Transaction");
        printf("\n2. Display All Transactions");
        printf("\n3. List Transactions by Seller");
        printf("\n4. List Transactions by Buyer");
        printf("\n5. Display Transactions in a Time Period");
        printf("\n6. Calculate Total Revenue of a Seller");
        printf("\n7. Display the Transaction with the Highest Energy Amount.");
        printf("\n8. Sort buyer's by energy");
        printf("\n9. Sort Seller/buyer pairs in decreasing order of the number of transactions");
        printf("\n10. Update a Transaction");
        printf("\n11. Display Regular Buyers");
        printf("\n12. Exit");
        printf("\n-----------------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            do
            {
                printf("Enter Transaction ID ('TXXXX' ,0<X<9 and be unique): ");
                scanf("%5s", transactionID);
            } while (!validTransaction(transactionID) || !isTransactionIdUnique(transactionID, transaction_ptr));

            do
            {
                printf("Enter Seller ID (must start with 'SXXXX'): ");
                scanf("%5s", sellerID);
            } while (!validSeller(sellerID));

            do
            {
                printf("Enter Buyer ID (must start with 'BXXXX'): ");
                scanf("%5s", buyerID);
            } while (!validBuyer(buyerID));

            printf("Enter Energy Amount: ");
            scanf("%d", &amt_energy);
            CurrentTimestamp(timestamp);
            add_transaction(&transaction_ptr, &sellerptr, &buyerptr, &pairptr, transactionID, sellerID, buyerID, amt_energy, timestamp);
            break;

        case 2:
            displayTransactions(transaction_ptr);
            break;

        case 3:
            list_of_seller(transaction_ptr, sellerptr);
            break;

        case 4:
            list_of_buyer(transaction_ptr, buyerptr);
            break;

        case 5:
            printf("Enter Start Timestamp (YYYY-MM-DD HH:MM:SS): ");
            scanf(" %[^\n]", start);
            printf("Enter End Timestamp (YYYY-MM-DD HH:MM:SS): ");
            scanf(" %[^\n]", end);
            between_transaction(start, end, transaction_ptr);
            break;

        case 6:
            printf("Enter Seller ID to calculate revenue: ");
            scanf("%s", sellerID);
            revenue = total_revenue_seller(transaction_ptr, sellerID);
            printf("Total Revenue for Seller %s: $%.2f\n", sellerID, revenue);
            printf("---------------------------------------------------\n");
            break;

        case 7:
            transaction_ptr = trans_sort(&transaction_ptr);
            temptransptr = transaction_ptr;
            display_highest_energy((transaction*)temptransptr);

            break;

        case 8:
            buyerptr = buyer_sort(&buyerptr);
            tempbuyer = buyerptr;
            printf("Energy sorted");
            list_of_buyer(transaction_ptr, (buyer*)tempbuyer);

            break;

        case 9:
            pairptr = pair_sort(&pairptr);
            temppairptr = pairptr;
            display_pair((pair*)temppairptr);

            break;

        case 10:
            printf("Enter Transaction ID to update: ");
            scanf("%s", transactionID);
            update_transaction(transaction_ptr, sellerptr, transactionID);
            break;

        case 11:
            displayRegularBuyers(sellerptr);
            break;

        case 12:
            printf("Exiting program...\n");
            writeSellersToFile(sellerptr, "sellers.txt");
            writeTransactionsToFile(transaction_ptr, "transactions.txt");
            cleanUpMemory(&transaction_ptr, &sellerptr, &buyerptr, &pairptr);
            return 0;

        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 0);

    return 0;
}
