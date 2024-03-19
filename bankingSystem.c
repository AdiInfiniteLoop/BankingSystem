#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Structure for representing a bank account
typedef struct Account {
    char accNumber[20];
    char accHolderName[100];
    float balance;
    char cardNumber[20];
    char cardType[20];
    struct Account *left;
    struct Account *right;
    struct Transaction *transactions;
} Account;

// Structure for representing a transaction
typedef struct Transaction {
    time_t timestamp;
    float amount;
    char type[20]; 
    char sourceAccNumber[20];
    struct Transaction *next;
} Transaction;

// Function to create a new account
Account *createAccount(const char *accNumber, const char *accHolderName, float balance) {
    Account *newAccount = (Account *)malloc(sizeof(Account));
    if (newAccount == NULL) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    strncpy(newAccount->accNumber, accNumber, sizeof(newAccount->accNumber) - 1);
    newAccount->accNumber[sizeof(newAccount->accNumber) - 1] = '\0';
    strncpy(newAccount->accHolderName, accHolderName, sizeof(newAccount->accHolderName) - 1);
    newAccount->accHolderName[sizeof(newAccount->accHolderName) - 1] = '\0';
    newAccount->balance = balance;
    newAccount->left = NULL;
    newAccount->right = NULL;
    newAccount->transactions = NULL;
    newAccount->cardNumber[0] = '\0';
    newAccount->cardType[0] = '\0';
    return newAccount;
}

// Function to insert a new account into the binary search tree
Account *insertAccount(Account *root, Account *newAccount) {
    if (root == NULL)
        return newAccount;
    if (strcmp(newAccount->accNumber, root->accNumber) < 0)
        root->left = insertAccount(root->left, newAccount);
    else if (strcmp(newAccount->accNumber, root->accNumber) > 0)
        root->right = insertAccount(root->right, newAccount);
    return root;
}

// Function to search for an account by account number
Account *searchAccount(Account *root, const char *accNumber) {
    if (root == NULL || strcmp(root->accNumber, accNumber) == 0)
        return root;
    if (strcmp(accNumber, root->accNumber) < 0)
        return searchAccount(root->left, accNumber);
    return searchAccount(root->right, accNumber);
}

// Function to perform deposit
void deposit(Account *acc, float amount) {
    acc->balance += amount;

    // Record transaction
    Transaction *transaction = (Transaction *)malloc(sizeof(Transaction));
    if (transaction == NULL) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    time(&transaction->timestamp);
    transaction->amount = amount;
    strcpy(transaction->type, "Deposit");
    strcpy(transaction->sourceAccNumber, "");
    transaction->next = acc->transactions;
    acc->transactions = transaction;
}

// Function to perform withdrawal
int withdraw(Account *acc, float amount) {
    if (acc->balance >= amount) {
        acc->balance -= amount;

        // Record transaction
        Transaction *transaction = (Transaction *)malloc(sizeof(Transaction));
        if (transaction == NULL) {
            printf("Memory allocation failed!\n");
            exit(EXIT_FAILURE);
        }
        time(&transaction->timestamp);
        transaction->amount = amount;
        strcpy(transaction->type, "Withdrawal");
        strcpy(transaction->sourceAccNumber, "");
        transaction->next = acc->transactions;
        acc->transactions = transaction;

        return 1; // Successful withdrawal
    } else {
        printf("Insufficient balance!\n");
        return 0; // Failed withdrawal
    }
}

// Function to transfer funds between accounts
void transfer(Account *sourceAcc, Account *destAcc, float amount) {
    if (withdraw(sourceAcc, amount)) {
        deposit(destAcc, amount);

        // Record transaction in source account
        Transaction *transaction = (Transaction *)malloc(sizeof(Transaction));
        if (transaction == NULL) {
            printf("Memory allocation failed!\n");
            exit(EXIT_FAILURE);
        }
        time(&transaction->timestamp);
        transaction->amount = amount;
        strcpy(transaction->type, "Transfer");
        strcpy(transaction->sourceAccNumber, destAcc->accNumber); // Source is the destination for transfers
        transaction->next = sourceAcc->transactions;
        sourceAcc->transactions = transaction;

        // Record transaction in destination account
        transaction = (Transaction *)malloc(sizeof(Transaction));
        if (transaction == NULL) {
            printf("Memory allocation failed!\n");
            exit(EXIT_FAILURE);
        }
        time(&transaction->timestamp);
        transaction->amount = amount;
        strcpy(transaction->type, "Transfer");
        strcpy(transaction->sourceAccNumber, sourceAcc->accNumber); // Destination is the source for transfers
        transaction->next = destAcc->transactions;
        destAcc->transactions = transaction;

        printf("Transfer successful!\n");
    } else {
        printf("Transfer failed due to insufficient balance.\n");
    }
}

// Function to display account details
void displayAccount(Account *acc) {
    printf("Account Number: %s\n", acc->accNumber); 
    printf("Account Holder Name: %s\n", acc->accHolderName);
    printf("Balance: %.2f\n", acc->balance);
    printf("Card Number: %s\n", acc->cardNumber);
    printf("Card Type: %s\n", acc->cardType);
}

void displayAllAccounts(Account *root) {
    if (root != NULL) {
        displayAllAccounts(root->left);
        displayAccount(root);
        displayAllAccounts(root->right);
    }
}

// Function to display transaction history for an account
void displayTransactionHistory(Account *acc) {
    printf("Transaction History for Account Number %s:\n", acc->accNumber); 
    Transaction *transaction = acc->transactions;
    while (transaction != NULL) {
        printf("Timestamp: %s", ctime(&transaction->timestamp));
        printf("Type: %s\n", transaction->type);
        printf("Amount: %.2f\n", transaction->amount);
        if (strcmp(transaction->type, "Transfer") == 0) {
            printf("Source Account: %s\n", transaction->sourceAccNumber); 
        }
        transaction = transaction->next;
    }
}

// Function to calculate interest for a savings account
void calculateInterest(Account *acc) {
    float rate = 4.00;
    float interest = acc->balance * rate / 100.0;
    printf("The interest is: %0.2f\n", interest);
    acc->balance += interest;

    // Record interest transaction
    Transaction *transaction = (Transaction *)malloc(sizeof(Transaction));
    if (transaction == NULL) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    time(&transaction->timestamp);
    transaction->amount = interest;
    strcpy(transaction->type, "Interest");
    strcpy(transaction->sourceAccNumber, "");
    transaction->next = acc->transactions;
    acc->transactions = transaction;

    printf("\nInterest calculated and added to account balance.\n");
    printf("The current balance is: %f", acc->balance);
}

// Function to close an account
void closeAccount(Account **root, const char *accNumber) {
    Account *temp = *root;
    Account *parent = NULL;
    Account *succ = NULL; // Successor node for deleting a node with two children.

    // Search for the account to be closed
    while (temp != NULL && strcmp(temp->accNumber, accNumber) != 0) {
        parent = temp;
        if (strcmp(accNumber, temp->accNumber) < 0)
            temp = temp->left;
        else
            temp = temp->right;
    }

    // Account not found
    if (temp == NULL) {
        printf("Account not found!\n");
        return;
    }

    // Case 1: Account has no children
    if (temp->left == NULL && temp->right == NULL) {
        if (parent == NULL)
            *root = NULL;
        else if (temp == parent->left)
            parent->left = NULL;
        else
            parent->right = NULL;
        free(temp);
    }
    // Case 2: Account has only one child (either left or right)
    else if (temp->left == NULL) {
        if (parent == NULL)
            *root = temp->right;
        else if (temp == parent->left)
            parent->left = temp->right;
        else
            parent->right = temp->right;
        free(temp);
    } else if (temp->right == NULL) {
        if (parent == NULL)
            *root = temp->left;
        else if (temp == parent->left)
            parent->left = temp->left;
        else
            parent->right = temp->left;
        free(temp);
    }
    // Case 3: Account has two children
    else {
        succ = temp->right;
        parent = NULL;
        while (succ->left != NULL) {
            parent = succ;
            succ = succ->left;
        }
        // Copy data of successor to the account being closed
        strcpy(temp->accHolderName, succ->accHolderName);
        strcpy(temp->accNumber, succ->accNumber);
        temp->balance = succ->balance;
        strcpy(temp->cardNumber, succ->cardNumber);
        strcpy(temp->cardType, succ->cardType);
        // Delete the successor node
        if (parent == NULL)
            temp->right = succ->right;
        else
            parent->left = succ->right;
        free(succ);
    }
    printf("Account closed successfully!\n");
}

// Function to add card details to an account
void addCardDetails(Account *acc, const char *cardNumber, const char *cardType) {
    strcpy(acc->cardNumber, cardNumber);
    strcpy(acc->cardType, cardType);
    printf("Card details added successfully!\n");
}

int main() {
    Account *root = NULL;
    int choice;
    char accNumber[20], destAccNumber[20];
    char accHolderName[100];
    float balance, amount, interestRate;
    char cardNumber[20], cardType[20];
    Account *acc, *destAcc;

    do {
        printf("\nBanking System Menu\n");
        printf("1. Create Account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Transfer Funds\n");
        printf("5. Display All Accounts\n");
        printf("6. Display Account Details\n");
        printf("7. Display Transaction History\n");
        printf("8. Calculate Interest\n");
        printf("9. Close Account\n");
        printf("10. Add Card Details\n");
        printf("11. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter Account Number: (Upto 8 characters)");
                scanf("%s", accNumber);
                if (strlen(accNumber) >= 8) {
                    printf("Invalid account number length!\n");
                    break;
                }
                if (searchAccount(root, accNumber) != NULL) {
                    printf("Account already exists!\n");
                    break;
                }
                printf("Enter Account Holder Name: ");
                scanf("%s", accHolderName);
                printf("Enter Initial Balance: ");
                scanf("%f", &balance);
                root = insertAccount(root, createAccount(accNumber, accHolderName, balance));
                printf("Account created successfully!\n");
                break;
            case 2:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                acc = searchAccount(root, accNumber);
                if (acc != NULL) {
                    printf("Enter Amount to Deposit: ");
                    scanf("%f", &amount);
                    deposit(acc, amount);
                    printf("Amount deposited successfully!\n");
                } else {
                    printf("Account not found!\n");
                }
                break;
            case 3:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                acc = searchAccount(root, accNumber);
                if (acc != NULL) {
                    printf("Enter Amount to Withdraw: ");
                    scanf("%f", &amount);
                    if (withdraw(acc, amount)) {
                        printf("Amount withdrawn successfully!\n");
                    }
                } else {
                    printf("Account not found!\n");
                }
                break;
            case 4:
                printf("Enter Source Account Number: ");
                scanf("%s", accNumber);
                acc = searchAccount(root, accNumber);
                if (acc != NULL) {
                    printf("Enter Destination Account Number: ");
                    scanf("%s", destAccNumber);
                    destAcc = searchAccount(root, destAccNumber);
                    if(destAcc == acc) {
                        printf("Not Possible to transfer to same account\n");
                        break;
                    }
                    if (destAcc != NULL) {
                        printf("Enter Amount to Transfer: ");
                        scanf("%f", &amount);
                        transfer(acc, destAcc, amount);
                    } else {
                        printf("Destination Account not found!\n");
                    }
                } else {
                    printf("Source Account not found!\n");
                }
                break;
            case 5:
                if(!root) {
                    printf("No Accounts are avaialble\n");
                    break;
                }
                displayAllAccounts(root);
                break;
            case 6:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                acc = searchAccount(root, accNumber);
                if (acc != NULL) {
                    displayAccount(acc);
                } else {
                    printf("Account not found!\n");
                }
                break;
            case 7:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                acc = searchAccount(root, accNumber);
                if (acc != NULL) {
                    displayTransactionHistory(acc);
                } else {
                    printf("Account not found!\n");
                }
                break;
            case 8:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                acc = searchAccount(root, accNumber);
                if (acc != NULL) {
                    calculateInterest(acc);
                } else {
                    printf("Account not found!\n");
                }
                break;
            case 9:
                printf("Enter Account Number to Close: ");
                scanf("%s", accNumber);
                closeAccount(&root, accNumber);
                break;
            case 10:
                printf("Enter Account Number: ");
                scanf("%s", accNumber);
                acc = searchAccount(root, accNumber);
                if (acc != NULL) {
                    printf("Enter Card Number: ");
                    scanf("%s", cardNumber);
                    printf("Enter Card Type: ");
                    scanf("%s", cardType);
                    addCardDetails(acc, cardNumber, cardType);
                } else {
                    printf("Account not found!\n");
                }
                break;
            case 11:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice!\n");
        }
    } while (choice != 11);

    return 0;
}