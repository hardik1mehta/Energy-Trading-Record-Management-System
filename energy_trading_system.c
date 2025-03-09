#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "energy_trading_system.h"
int isTransactionIdUnique(char *transactionID, transaction *transaction_ptr)
{
    transaction *temp = transaction_ptr;
    while (temp != NULL)
    {
        if (strcmp(temp->transaction_id, transactionID) == 0)
        {
            printf("Error: Transaction ID already exists. Try again.\n");
            return 0; // Not unique
        }
        temp = temp->next;
    }
    return 1; // Unique
}

// Function to get the current timestamp

void CurrentTimestamp(char *buffer)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, SIZE_OF_DATE, "%Y-%m-%d %H:%M:%S", t);
}
transaction *createTransactionNode(char *transactionID, char *buyerID, char *sellerID, int amt_energy, float price, const char *date_time)
{
    transaction *newtransaction = (transaction *)malloc(sizeof(transaction));
    if (!newtransaction)
    {
        return NULL;
    }
    strcpy(newtransaction->transaction_id, transactionID);
    strcpy(newtransaction->bid, buyerID);
    strcpy(newtransaction->seller_id, sellerID);
    newtransaction->amount_energy = amt_energy;
    newtransaction->price_kwh = price;
    strcpy(newtransaction->timestamp, date_time);
    newtransaction->next = NULL;
    return newtransaction;
}

// Find or create a seller
seller *findorcreate_seller(seller **sellerptr, char *sellerID)
{
    int flag = 0;
    seller *ptr = *sellerptr;
    while (ptr != NULL && !flag)
    {
        if (strcmp(ptr->seller_id, sellerID) == 0)
        {
            flag = 1;
        }
        if (flag == 0)
        {
            ptr = ptr->next;
        }
    }
    seller *newSeller = (seller *)malloc(sizeof(seller));
    if (!flag)
    {
        strcpy(newSeller->seller_id, sellerID);
        printf("Enter rate for seller %s below 300 kWh: ", sellerID);
        scanf("%f", &newSeller->perKWH_price_LE_300);
        printf("Enter rate for seller %s above 300 kWh: ", sellerID);
        scanf("%f", &newSeller->perKWH_price_GT_300);
        newSeller->regularBuyers = NULL;
        newSeller->next = *sellerptr;
        *sellerptr = newSeller;
        ptr = newSeller;
    }
    return ptr;
}

// Find or create a buyer
buyer *findorcreate_buyer(buyer **buyerptr, char *buyerID, int amnt)
{
    int flag = 0;
    buyer *ptr = *buyerptr;
    while (ptr != NULL && !flag)
    {
        if (strcmp(ptr->bid, buyerID) == 0)
        {
            ptr->total_amt = ptr->total_amt + amnt;
            flag = 1;
        }
        if (flag == 0)
        {
            ptr = ptr->next;
        }
    }
    buyer *newBuyer = (buyer *)malloc(sizeof(buyer));
    if (!flag)
    {
        strcpy(newBuyer->bid, buyerID);
        newBuyer->total_amt = amnt;
        newBuyer->next = *buyerptr;
        *buyerptr = newBuyer;
        ptr = newBuyer;
    }
    return ptr;
}
// Find or create a buyer-seller pair
pair *findOrCreate_pair(pair **pairptr, char *buyerID, char *sellerID)
{
    pair *temp = *pairptr;
    while (temp != NULL)
    {
        if (strcmp(temp->bid, buyerID) == 0 && strcmp(temp->seller_id, sellerID) == 0)
        {
            return temp;
        }
        temp = temp->next;
    }
    pair *newpair = (pair *)malloc(sizeof(pair));
    if (!newpair)
        return NULL;

    strcpy(newpair->bid, buyerID);
    strcpy(newpair->seller_id, sellerID);
    newpair->number_transactions = 0;
    newpair->next = *pairptr;
    *pairptr = newpair;
    return newpair;
}

void add_transaction(transaction **transaction_ptr, seller **sellerptr, buyer **buyerptr, pair **pairptr,
                     char *transactionID, char *sellerID, char *buyerID, int amt_energy, char *date_time)
{

    seller *seller = findorcreate_seller(sellerptr, sellerID);
    buyer *buyer = findorcreate_buyer(buyerptr, buyerID, amt_energy);
    pair *pair = findOrCreate_pair(pairptr, buyerID, sellerID);

    float price = (amt_energy > 300) ? seller->perKWH_price_GT_300 : seller->perKWH_price_LE_300;
    transaction *newtransaction = createTransactionNode(transactionID, buyerID, sellerID, amt_energy, price, date_time);

    newtransaction->next = *transaction_ptr;
    *transaction_ptr = newtransaction;

    pair->number_transactions++;

    if (pair->number_transactions > 5)
    {
        regularBuyer *regBuyer = seller->regularBuyers;
        int isRegular = 0;

        while (regBuyer != NULL)
        {
            if (strcmp(regBuyer->bid, buyerID) == 0)
            {
                isRegular = 1;
                break;
            }
            regBuyer = regBuyer->next;
        }

        if (!isRegular)
        {
            regularBuyer *newRegular = (regularBuyer *)malloc(sizeof(regularBuyer));
            if (newRegular != NULL)
            {
                strcpy(newRegular->bid, buyerID);
                newRegular->next = seller->regularBuyers;
                seller->regularBuyers = newRegular;
            }
        }
    }
}
void printTableHeader()
{
    printf("------------------------------------------------------------------------------------------------\n");
    printf("| %-10s | %-10s | %-10s | %-10s | %-10s | %-20s |\n", "Trans ID", "Seller ID", "Buyer ID", "Energy", "Price/kWh", "Timestamp");
    printf("--------------------------------------------------------------------------------------------------\n");
}
// Display single transactions
void printTableRow(transaction *temp)
{
    printf("| %-10s | %-10s | %-10s | %-10d | %-10.2f | %-20s |\n",
           temp->transaction_id, temp->seller_id, temp->bid, temp->amount_energy, temp->price_kwh, temp->timestamp);
}

// display all transaction
void displayTransactions(transaction *transaction_ptr)
{
    transaction *temp = transaction_ptr;
    printf("\nAll Transactions:\n");
    printTableHeader();
    while (temp != NULL)
    {
        printTableRow(temp);
        temp = temp->next;
    }
    printf("--------------------------------------------------------------\n");
}
// display regylar buyer
void displayRegularBuyers(seller *sellerptr)
{
    printf("\nRegular Buyers:\n");
    printf("---------------------------------------------------\n");
    printf("| %-10s | %-30s |\n", "Seller ID", "Regular Buyers");
    printf("---------------------------------------------------\n");
    while (sellerptr != NULL)
    {
        regularBuyer *temp = sellerptr->regularBuyers;
        printf("| %-10s | ", sellerptr->seller_id);
        while (temp != NULL)
        {
            printf("%-10s ", temp->bid);
            temp = temp->next;
        }
        printf("\n");
        sellerptr = sellerptr->next;
    }
    printf("---------------------------------------------------\n");
}

// Function to update transaction details by transaction ID
void update_transaction(transaction *transaction_ptr, seller *sellerptr, char *transactionID)
{
    transaction *temp = transaction_ptr;

    while (temp != NULL)
    {
        if (strcmp(temp->transaction_id, transactionID) == 0)
        {
            int new_amount;
            float new_price;
            char choice;
            printTableHeader();
            printTableRow(temp);

            // Ask user if they want to update the energy amount
            printf("\nDo you want to update the amount of energy? (y/n): ");
            scanf(" %c", &choice);
            if (choice == 'y' || choice == 'Y')
            {
                printf("Enter new amount of energy: ");
                scanf("%d", &new_amount);
                temp->amount_energy = new_amount;

                seller *seller = sellerptr;
                while (seller != NULL)
                {
                    if (strcmp(seller->seller_id, temp->seller_id) == 0)
                    {
                        temp->price_kwh = (temp->amount_energy > 300) ? seller->perKWH_price_GT_300 : seller->perKWH_price_LE_300;
                        break;
                    }
                    seller = seller->next;
                }
            }

            printf("\nDo you want to update the price per kWh manually? (y/n): ");
            scanf(" %c", &choice);
            if (choice == 'y' || choice == 'Y')
            {
                printf("Enter new price per kWh: ");
                scanf("%f", &new_price);
                temp->price_kwh = new_price;
            }

            printf("\nTransaction %s updated successfully!\n", transactionID);
            printf("Updated Amount: %d kWh\n", temp->amount_energy);
            printf("Updated Price per kWh: %.2f\n", temp->price_kwh);
            printf("\nUpdated Transaction::\n");
            printTableHeader();
            printTableRow(temp);
            return;
        }
        temp = temp->next;
    }
    printf("Transaction ID %s not found!\n", transactionID);
    printf("---------------------------------------------------\n");
}
// display transaction btw time
void between_transaction(char *start, char *end, transaction *transaction_ptr)
{
    transaction *temp = transaction_ptr;
    int flag = 0;
    printTableHeader();
    while (temp != NULL)
    {
        if (strcmp(start, temp->timestamp) <= 0 && strcmp(end, temp->timestamp) >= 0)
        {
            flag = 1;
            printTableRow(temp);
        }
        temp = temp->next;
    }
    if (flag == 0)
    {
        printf("NO transaction exist btween the entered time period!!!\n");
    }
    printf("---------------------------------------------------\n");
}
// total revenue by particular seller
float total_revenue_seller(transaction *transaction_ptr, char *sellerID)
{
    transaction *ptr = transaction_ptr;
    float total_revenue = 0;
    while (ptr != NULL)
    {
        if (strcmp(ptr->seller_id, sellerID) == 0)
        {
            total_revenue = total_revenue + (ptr->amount_energy) * (ptr->price_kwh);
        }
        ptr = ptr->next;
    }
    return total_revenue;
}
// display detail of particular seller or buyer;
void display_particular_detail(transaction *ptr, char *person)
{
    printf("| %-12s | %-12s | %-15d | %-10.2f | %-20s |\n",
           ptr->transaction_id, person, ptr->amount_energy, ptr->price_kwh, ptr->timestamp);
}
// list of seller
void list_of_seller(transaction *transaction_ptr, seller *sellerptr)
{
    printf("\nTransactions Grouped by Seller:\n");
    transaction *t_ptr = NULL;
    seller *s_ptr = sellerptr;
    while (s_ptr != NULL)
    {
        printf("-------------------------------------------------------------------------------------------------------\n");
        printf("Seller ID: %s\n", s_ptr->seller_id);
        printf("| Transaction ID | Buyer ID    | Energy Amount (kWh) | Price/kWh | Timestamp  |\n");
        printf("-------------------------------------------------------------------------------------------------------\n");
        t_ptr = transaction_ptr;
        while (t_ptr != NULL)
        {
            if (strcmp(s_ptr->seller_id, t_ptr->seller_id) == 0)
            {
                display_particular_detail(t_ptr, t_ptr->bid);
            }
            t_ptr = t_ptr->next;
        }
        printf("---------------------------\n");
        s_ptr = s_ptr->next;
    }
    printf("---------------------------------------------------\n");
}
// list of buyer
void list_of_buyer(transaction *transaction_ptr, buyer *buyerptr)
{
    printf("\nTransactions Grouped by Buyer:\n");
    transaction *t_ptr = NULL;
    buyer *b_ptr = buyerptr;
    while (b_ptr != NULL)
    {
        printf("Buyer ID: %s Total_Amount:%d\n", b_ptr->bid, b_ptr->total_amt);
        printf("-------------------------------------------------------------------------------------------------------\n");
        printf("| Transaction ID | Seller ID    | Energy Amount (kWh) | Price/kWh | Timestamp  |\n");
        printf("-------------------------------------------------------------------------------------------------------\n");
        t_ptr = transaction_ptr;
        while (t_ptr != NULL)
        {
            if (strcmp(b_ptr->bid, t_ptr->bid) == 0)
            {
                display_particular_detail(t_ptr, t_ptr->seller_id);
            }
            t_ptr = t_ptr->next;
        }
        printf("---------------------------\n");
        b_ptr = b_ptr->next;
    }
    printf("---------------------------------------------------\n");
}
// sort transaction acc to amount of energy!!
transaction *trans_merge(transaction *list1, transaction *list2)
{
    transaction *result, *ptr1, *ptr2, *tail;
    ptr1 = list1;
    ptr2 = list2;
    if (list1->amount_energy < list2->amount_energy)
    {
        result = tail = list1;
        ptr1 = ptr1->next;
    }
    else
    {
        result = tail = list2;
        ptr2 = ptr2->next;
    }
    while ((ptr1 != NULL) && (ptr2 != NULL))
    {
        if (ptr1->amount_energy < ptr2->amount_energy)
        {
            tail->next = ptr1;
            tail = ptr1;
            ptr1 = ptr1->next;
        }
        else
        {
            tail->next = ptr2;
            tail = ptr2;
            ptr2 = ptr2->next;
        }
    }
    if (ptr1 != NULL)
    {
        tail->next = ptr1;
    }
    else
    {
        tail->next = ptr2;
    }
    return result;
}

transaction *trans_divide(transaction *list_ptr)
{
    transaction *nptr, *fast, *slow;
    fast = slow = list_ptr;
    fast = fast->next->next;
    while (fast != NULL)
    {
        slow = slow->next;
        fast = fast->next;
        if (fast != NULL)
        {
            fast = fast->next;
        }
    }
    nptr = slow->next;
    slow->next = NULL;
    return nptr;
}

transaction *trans_sort(transaction **transaction_ptr)
{
    transaction *nptr;
    transaction *list_ptr;
    transaction *lptr;
    lptr = *transaction_ptr;
    list_ptr = *transaction_ptr;
    if (transaction_ptr != NULL && lptr->next != NULL)
    {
        nptr = trans_divide(list_ptr);
        list_ptr = trans_sort(&list_ptr);
        nptr = trans_sort(&nptr);
        lptr = trans_merge(list_ptr, nptr);
    }
    return lptr;
}
// display highest amount transaction
void display_highest_energy(transaction *ptr)
{
    if (ptr == NULL)
    {
        printf("No transactions available.\n");
        return;
    }
    while (ptr->next != NULL)
    {
        ptr = ptr->next;
    }

    int max_energy = ptr->amount_energy;
    transaction *temp = ptr;

    printTableHeader();
    while (temp != NULL)
    {
        if (temp->amount_energy == max_energy)
        {
            printTableRow(temp);
        }
        temp = temp->next;
    }
}

// sort buyer acc to amount
buyer *merge(buyer *list1, buyer *list2)
{
    buyer *result, *ptr1, *ptr2, *tail;
    ptr1 = list1;
    ptr2 = list2;
    if (list1->total_amt < list2->total_amt)
    {
        result = tail = list1;
        ptr1 = ptr1->next;
    }
    else
    {
        result = tail = list2;
        ptr2 = ptr2->next;
    }
    while ((ptr1 != NULL) && (ptr2 != NULL))
    {
        if (ptr1->total_amt < ptr2->total_amt)
        {
            tail->next = ptr1;
            tail = ptr1;
            ptr1 = ptr1->next;
        }
        else
        {
            tail->next = ptr2;
            tail = ptr2;
            ptr2 = ptr2->next;
        }
    }
    if (ptr1 != NULL)
    {
        tail->next = ptr1;
    }
    else
    {
        tail->next = ptr2;
    }
    return result;
}
buyer *divide(buyer *list_ptr)
{
    buyer *nptr, *fast, *slow;
    fast = slow = list_ptr;
    fast = fast->next->next;
    while (fast != NULL)
    {
        slow = slow->next;
        fast = fast->next;
        if (fast != NULL)
        {
            fast = fast->next;
        }
    }
    nptr = slow->next;
    slow->next = NULL;
    return nptr;
}
buyer *buyer_sort(buyer **buyerptr)
{
    buyer *nptr;
    buyer *list_ptr;
    buyer *lptr;
    lptr = *buyerptr;
    list_ptr = *buyerptr;
    if (buyerptr != NULL && lptr->next != NULL)
    {
        nptr = divide(list_ptr);
        list_ptr = buyer_sort(&list_ptr);
        nptr = buyer_sort(&nptr);
        lptr = merge(list_ptr, nptr);
    }
    return lptr;
}
// Sort Seller/buyer pairs in decreasing order of the number of transactions
pair *pair_merge(pair *list1, pair *list2)
{
    pair *result, *ptr1, *ptr2, *tail;
    ptr1 = list1;
    ptr2 = list2;
    if (list1->number_transactions > list2->number_transactions)
    {
        result = tail = list1;
        ptr1 = ptr1->next;
    }
    else
    {
        result = tail = list2;
        ptr2 = ptr2->next;
    }
    while ((ptr1 != NULL) && (ptr2 != NULL))
    {
        if (ptr1->number_transactions > ptr2->number_transactions)
        {
            tail->next = ptr1;
            tail = ptr1;
            ptr1 = ptr1->next;
        }
        else
        {
            tail->next = ptr2;
            tail = ptr2;
            ptr2 = ptr2->next;
        }
    }
    if (ptr1 != NULL)
    {
        tail->next = ptr1;
    }
    else
    {
        tail->next = ptr2;
    }
    return result;
}
pair *pair_divide(pair *list_ptr)
{
    pair *nptr, *fast, *slow;
    fast = slow = list_ptr;
    fast = fast->next->next;
    while (fast != NULL)
    {
        slow = slow->next;
        fast = fast->next;
        if (fast != NULL)
        {
            fast = fast->next;
        }
    }
    nptr = slow->next;
    slow->next = NULL;
    return nptr;
}

pair *pair_sort(pair **pairptr)
{
    pair *nptr;
    pair *list_ptr;
    pair *lptr;
    lptr = *pairptr;
    list_ptr = *pairptr;
    if (pairptr != NULL && lptr->next != NULL)
    {
        nptr = pair_divide(list_ptr);
        list_ptr = pair_sort(&list_ptr);
        nptr = pair_sort(&nptr);
        lptr = pair_merge(list_ptr, nptr);
    }
    return lptr;
}
void display_pair(pair *lptr)
{
    pair *temp = lptr;
    while (temp != NULL)
    {
        printf("Buyer ID: %s\n", temp->bid);
        printf("Seller ID: %s\n", temp->seller_id);
        printf("Number of Transactions: %d \n", temp->number_transactions);
        printf("---------------------------\n");
        temp = temp->next;
    }
}
void readSellers(seller **sellerptr, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open sellers file");
        return;
    }

    char sellerID[SIZE_ID];
    float perKWH_price_LE_300, perKWH_price_GT_300;

    while (fscanf(file, "%[^,],%f,%f\n", sellerID, &perKWH_price_LE_300, &perKWH_price_GT_300) == 3)
    {
        seller *newSeller = (seller *)malloc(sizeof(seller));
        strcpy(newSeller->seller_id, sellerID);
        newSeller->perKWH_price_LE_300 = perKWH_price_LE_300;
        newSeller->perKWH_price_GT_300 = perKWH_price_GT_300;
        newSeller->regularBuyers = NULL;
        newSeller->next = *sellerptr;
        *sellerptr = newSeller;
    }

    fclose(file);
}

void readTransactions(transaction **transaction_ptr, seller **sellerptr, buyer **buyerptr, pair **pairptr, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open transactions file");
        return;
    }

    char transactionID[SIZE_ID], sellerID[SIZE_ID], buyerID[SIZE_ID], timestamp[SIZE_OF_DATE];
    int amt_energy;

    while (fscanf(file, "%[^,],%[^,],%[^,],%d,%[^\n]\n", transactionID, sellerID, buyerID, &amt_energy, timestamp) == 5)
    {
        add_transaction(transaction_ptr, sellerptr, buyerptr, pairptr, transactionID, sellerID, buyerID, amt_energy, timestamp);
    }

    fclose(file);
}
// cleaning up the memeory
void cleanUpMemory(transaction **transHead, seller **sellerHead, buyer **buyerHead, pair **pairHead)
{
    printf("Cleaning up memory...\n");

    // Free transaction list
    transaction *tempTrans;
    while (*transHead != NULL)
    {
        tempTrans = *transHead;
        *transHead = (*transHead)->next;
        free(tempTrans);
    }
    *transHead = NULL;

    // Free seller list, including regular buyers
    seller *tempSeller;
    while (*sellerHead != NULL)
    {
        tempSeller = *sellerHead;

        // Free regular buyers linked list
        regularBuyer *tempBuyer;
        while (tempSeller->regularBuyers != NULL)
        {
            tempBuyer = tempSeller->regularBuyers;
            tempSeller->regularBuyers = tempSeller->regularBuyers->next;
            free(tempBuyer);
        }

        *sellerHead = (*sellerHead)->next;
        free(tempSeller);
    }
    *sellerHead = NULL;

    // Free buyer list
    buyer *tempBuyerNode;
    while (*buyerHead != NULL)
    {
        tempBuyerNode = *buyerHead;
        *buyerHead = (*buyerHead)->next;
        free(tempBuyerNode);
    }
    *buyerHead = NULL;

    // Free pairs list
    pair *tempPair;
    while (*pairHead != NULL)
    {
        tempPair = *pairHead;
        *pairHead = (*pairHead)->next;
        free(tempPair);
    }
    *pairHead = NULL;

    printf("Memory cleanup complete.\n");
}
void writeSellersToFile(seller *sellerptr, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Failed to open sellers file for writing");
        return;
    }

    seller *temp = sellerptr;
    while (temp != NULL)
    {
        fprintf(file, "%s,%.2f,%.2f\n", temp->seller_id, temp->perKWH_price_LE_300, temp->perKWH_price_GT_300);
        temp = temp->next;
    }

    fclose(file);
}

void writeTransactionsToFile(transaction *transaction_ptr, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Failed to open transactions file for writing");
        return;
    }

    transaction *temp = transaction_ptr;
    while (temp != NULL)
    {
        fprintf(file, "%s,%s,%s,%d,%s\n", temp->transaction_id, temp->seller_id, temp->bid, temp->amount_energy, temp->timestamp);
        temp = temp->next;
    }

    fclose(file);
}
int validTransaction(char *s)
{
    int ans = 1;
    if (s[0] != 'T' || s[1] == '0' || strlen(s) < 5)
    {
        ans = 0;
    }
    if (ans)
    {
        for (int i = 1; s[i] != '\0' && ans; i++)
        {
            if (s[i] < '0' || s[i] > '9')
            {
                ans = 0;
            }
        }
    }
    return ans;
}
int validSeller(char *s)
{
    int ans = 1;
    if (s[0] != 'S' || strlen(s) < 5)
    {
        ans = 0;
    }
    if (ans)
    {
        for (int i = 1; s[i] != '\0' && ans; i++)
        {
            if (s[i] < '0' || s[i] > '9')
            {
                ans = 0;
            }
        }
    }
    return ans;
}

int validBuyer(char *s)
{
    int ans = 1;
    if (s[0] != 'B' || strlen(s) < 5)
    {
        ans = 0;
    }
    if (ans)
    {
        for (int i = 1; s[i] != '\0' && ans; i++)
        {
            if (s[i] < '0' || s[i] > '9')
            {
                ans = 0;
            }
        }
    }
    return ans;
}
