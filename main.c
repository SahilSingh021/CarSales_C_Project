#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

#define Maximum_Number_Of_Cars 15
const char * fileName = "CarSales_DB.json";
struct User activeUser;

struct User {
    char* customer_name;
    int customer_age;
};

struct Date {
    int day;
    int month;
    int year;
};

struct Feedback {
    char* customer_name;
    char* car_make;
    char* car_model;
    int year_of_manufacture;
    int number_of_purchased_cars;
    int feedback_rating;
    char* feedback_comment;
};

struct Car {
    int car_id;
    char *car_make;
    char *car_model;
    int price;
    int year_of_manufacture;
    int remaining_amount;
    int sold_amount;
    int total_sales_amount;
};

struct Transaction {
    struct User customer;
    char* car_make;
    char* car_model;
    int year_of_manufacture;
    int total_price_of_transaction;
    bool discount_given;
    int discount_value;
    int number_of_purchased_cars;
    struct Date date_of_purchase;
};

void FlushInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void CloseFile(FILE *filePtr) {
    if (filePtr != NULL) {
        fclose(filePtr);
    }
}

void PressEnterToContinue()
{
    printf("\nPress {Enter} to continue...");
    while (getchar() != '\n') {}
}

void PressEnterToExit()
{
    printf("\nPress {Enter} to exit...");
    while (getchar() != '\n') {}
}

void SwapIndexOfTypeCar(struct Car *a, struct Car *b)
{
    struct Car cpy_indexA = *a;
    *a = *b;
    *b = cpy_indexA;
}

void SwapIndexOfTypeTransaction(struct Transaction *a, struct Transaction *b)
{
    struct Transaction cpy_indexA = *a;
    *a = *b;
    *b = cpy_indexA;
}

void SortCarData_Descending_By_YearOfManufacture(struct Car carData[], int numberOfCars)
{
    for (int i = 0; i < numberOfCars - 1; i++)
    {
        for (int j = 0; j < numberOfCars - i - 1; j++)
        {
            if (carData[j].year_of_manufacture < carData[j + 1].year_of_manufacture)
            {
                SwapIndexOfTypeCar(&carData[j], &carData[j + 1]);
            }
        }
    }
}

void SortTransactionData_Descending_By_SalesAmount(struct Transaction* transactionData, int transactionsCount)
{
    for (int i = 0; i < transactionsCount - 1; i++)
    {
        for (int j = 0; j < transactionsCount - i - 1; j++)
        {
            if (transactionData[j].total_price_of_transaction < transactionData[j + 1].total_price_of_transaction)
            {
                SwapIndexOfTypeTransaction(&transactionData[j], &transactionData[j + 1]);
            }
        }
    }
}

char *ReadDataIntoMemory(FILE *filePtr) {
    if (filePtr == NULL) {
        printf("\nFailed to open or create the database file.\n");
        exit(1);
    }

    // Get the file size
    fseek(filePtr, 0, SEEK_END);
    long fileSize = ftell(filePtr);

    // If the file is empty, write the initialJsonData into our newly created file
    if (fileSize == 0) {
        const char *initialJsonData = "{\n"
                                      "  \"cars\": [\n"
                                      "    {\n"
                                      "      \"car_id\": 1,\n"
                                      "      \"car_make\": \"Ford\",\n"
                                      "      \"car_model\": \"Fusion\",\n"
                                      "      \"price\": 22000,\n"
                                      "      \"year_of_manufacture\": 2020,\n"
                                      "      \"remaining_amount\": 15,\n"
                                      "      \"sold_amount\": 0,\n"
                                      "      \"total_sales_amount\": 0\n"
                                      "    },\n"
                                      "    {\n"
                                      "      \"car_id\": 2,\n"
                                      "      \"car_make\": \"Mercedes-Benz\",\n"
                                      "      \"car_model\": \"C-Class\",\n"
                                      "      \"price\": 45000,\n"
                                      "      \"year_of_manufacture\": 2021,\n"
                                      "      \"remaining_amount\": 12,\n"
                                      "      \"sold_amount\": 0,\n"
                                      "      \"total_sales_amount\": 0\n"
                                      "    },\n"
                                      "    {\n"
                                      "      \"car_id\": 3,\n"
                                      "      \"car_make\": \"Audi\",\n"
                                      "      \"car_model\": \"A6\",\n"
                                      "      \"price\": 48000,\n"
                                      "      \"year_of_manufacture\": 2022,\n"
                                      "      \"remaining_amount\": 9,\n"
                                      "      \"sold_amount\": 0,\n"
                                      "      \"total_sales_amount\": 0\n"
                                      "    },\n"
                                      "    {\n"
                                      "      \"car_id\": 4,\n"
                                      "      \"car_make\": \"BMW\",\n"
                                      "      \"car_model\": \"5 Series\",\n"
                                      "      \"price\": 80000,\n"
                                      "      \"year_of_manufacture\": 2022,\n"
                                      "      \"remaining_amount\": 7,\n"
                                      "      \"sold_amount\": 0,\n"
                                      "      \"total_sales_amount\": 0\n"
                                      "    }\n"
                                      "  ]\n"
                                      "}";

        fclose(filePtr);
        filePtr = fopen(fileName, "w");

        if (filePtr == NULL) {
            printf("\nFailed to open the database file for writing.\n");
            exit(1);
        }

        fwrite(initialJsonData, 1, strlen(initialJsonData), filePtr);
        fclose(filePtr);
    }

    filePtr = fopen(fileName, "rb");
    if (filePtr == NULL)
    {
        printf("\nFailed to open the database file for reading.\n");
        exit(1);
    }

    fseek(filePtr, 0, SEEK_END);
    fileSize = ftell(filePtr);

    // Create a buffer the size of the data we are reading + 1
    char *dataBuffer = (char *) malloc(fileSize + 1);

    if (dataBuffer == NULL) {
        printf("Unable to allocate memory for data from the database.");
        CloseFile(filePtr);
        exit(1);
    }

    // Set the pointer back to the beginning
    if (fseek(filePtr, 0, SEEK_SET) != 0)
    {
        printf("\nFailed to set the file pointer.\n");
        fclose(filePtr);
        exit(1);
    }

    // Read the data into dataBuffer
    size_t bytesRead = fread(dataBuffer, 1, fileSize, filePtr);
    if (bytesRead != fileSize) {
        printf("Error reading from file.\n");
        // Handle the error or exit the program as needed
    }

    fclose(filePtr);
    return dataBuffer;
}

int GetUserAge() {
    int userAgeInt = 0;
    while (true) {
        printf("Please enter your age: ");;
        if (scanf("%d", &userAgeInt) != 1) {
            FlushInputBuffer();
            printf("Please enter a valid age.\n");
            continue;
        }

        FlushInputBuffer();

        if (userAgeInt < 1 || userAgeInt > 120) {
            printf("Please enter an valid age.\n");
            continue;
        }

        if (userAgeInt < 12) {
            printf("You must be at least 12 years of age, to continue.");
            PressEnterToExit();
            exit(0);
        }

        break;
    }
    return userAgeInt;
}

char* GetUsername()
{
    char* username = NULL;
    while (true)
    {
        printf("Enter your name: \n");
        if (scanf("%m[^\n]", &username) != 1) {
            FlushInputBuffer();
            printf("\nPlease enter a valid name.");
            continue;
        }

        FlushInputBuffer();

        bool invalidUsername = false;
        for (int i = 0; i < sizeof(username); i++)
        {
            if (isdigit(username[i]))
            {
                invalidUsername = true;
            }
        }

        if (invalidUsername)
        {
            printf("Please enter a valid name.\n");
            continue;
        }

        break;
    }

    char * tempCharUsername = (char*) malloc(20 * sizeof(char));
    if (tempCharUsername == NULL)
    {
        printf("Unable to allocate memory for tempCharUsername.");
        exit(1);
    }

    strcpy(tempCharUsername, username);

    return tempCharUsername;
}

int GetUserSelection()
{
    int userChoiceInt = 0;
    char userChoiceChar[50] = "";

    while (true) {
        printf("\nWelcome to Car Sales!\n\n");
        printf("1) View Car Stock.\n");
        printf("2) Buy Cars.\n");
        printf("3) View Sales Data.\n");
        printf("4) Customer Feedback.\n");
        printf("5) Exit\n");
        scanf("%s", &userChoiceChar);

        FlushInputBuffer();

        if (isdigit(userChoiceChar[0]))
        {
            userChoiceInt = (int) userChoiceChar[0] - '0'; // Subtract the ASCII value of '0' which is (48)
        }

        if (userChoiceInt < 1 || userChoiceInt > 5 || userChoiceChar[1] != '\0')
        {
            printf("Invalid choice. Please select a valid option (1-5).\n");
            userChoiceInt = 0;
            continue;
        }
        break;
    }

    return userChoiceInt;
}

int CountRecordsInDatabase(const char *jsonDataBuffer, char *dataHeader, int dataHeaderLength)
{
    int count = 0;
    bool flag_insideDataArray = false;

    for (int i = 0; jsonDataBuffer[i] != '\0'; i++)
    {
        if (jsonDataBuffer[i] == '"' && strncmp(&jsonDataBuffer[i], dataHeader, dataHeaderLength) == 0)
        {
            flag_insideDataArray = true;
        }
        else if (jsonDataBuffer[i] == ']' && flag_insideDataArray)
        {
            flag_insideDataArray = false;
            return count;
        }
        else if (flag_insideDataArray && jsonDataBuffer[i] == '{')
        {
            count++;
        }
    }
    return count;
}

char* FindStringValue(const char* string, char key)
{
    int length = 0;

    // Increment 'length' until the key or null terminator is found
    while (string[length] != key && string[length] != '\0')
    {
        length++;
    }

    // Allocate memory for our string and the null terminator
    char *tempString = (char *)malloc(length + 1);

    if (tempString == NULL)
    {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    // Copy the string to our allocated memory
    for (int i = 0; i < length; i++) {
        tempString[i] = string[i];
    }

    // Add null terminator to the end
    tempString[length] = '\0';

    return tempString;
}


void LoadCarDataFromBufferToStruct(struct Car carData[], char *dataBuffer, int carCountInDatabase)
{
    for (int i = 0; i < carCountInDatabase; i++) {
        dataBuffer = strstr(dataBuffer, "\"car_id\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        carData[i].car_id = atoi(dataBuffer + 9);

        dataBuffer = strstr(dataBuffer, "\"car_make\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        dataBuffer += 13;
        carData[i].car_make = FindStringValue(dataBuffer, '\"');

        dataBuffer = strstr(dataBuffer, "\"car_model\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        dataBuffer += 14;
        carData[i].car_model = FindStringValue(dataBuffer, '\"');

        dataBuffer = strstr(dataBuffer, "\"price\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        carData[i].price = atoi(dataBuffer + 8);

        dataBuffer = strstr(dataBuffer, "\"year_of_manufacture\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        carData[i].year_of_manufacture = atoi(dataBuffer + 22);

        dataBuffer = strstr(dataBuffer, "\"remaining_amount\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        carData[i].remaining_amount = atoi(dataBuffer + 19);

        dataBuffer = strstr(dataBuffer, "\"sold_amount\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        carData[i].sold_amount = atoi(dataBuffer + 15);

        dataBuffer = strstr(dataBuffer, "\"total_sales_amount\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        carData[i].total_sales_amount = atoi(dataBuffer + 22);
    }
}

void LoadTransactionDataFromBufferToStruct(struct Transaction transactionData[], char *dataBuffer, int transactionsCount)
{
    for (int i = 0; i < transactionsCount; i++) {
        dataBuffer = strstr(dataBuffer, "\"customer_name\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        dataBuffer += 18;
        transactionData[i].customer.customer_name = FindStringValue(dataBuffer, '\"');

        dataBuffer = strstr(dataBuffer, "\"customer_age\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        transactionData[i].customer.customer_age = atoi(dataBuffer + 16);

        dataBuffer = strstr(dataBuffer, "\"transaction_car_make\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        dataBuffer += 25;
        transactionData[i].car_make = FindStringValue(dataBuffer, '\"');

        dataBuffer = strstr(dataBuffer, "\"transaction_car_model\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        dataBuffer += 26;
        transactionData[i].car_model = FindStringValue(dataBuffer, '\"');


        dataBuffer = strstr(dataBuffer, "\"transaction_year_of_manufacture\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        transactionData[i].year_of_manufacture = atoi(dataBuffer + 35);

        dataBuffer = strstr(dataBuffer, "\"total_price_of_transaction\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        transactionData[i].total_price_of_transaction = atoi(dataBuffer + 30);

        bool bTemp = false;
        dataBuffer = strstr(dataBuffer, "\"discount_given\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        dataBuffer += 18;

        if (strncmp(dataBuffer, "true", 4) == 0) bTemp = true;

        transactionData[i].discount_given = bTemp;

        dataBuffer = strstr(dataBuffer, "\"discount_value\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        transactionData[i].discount_value = atoi(dataBuffer + 18);

        dataBuffer = strstr(dataBuffer, "\"number_of_purchased_cars\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        transactionData[i].number_of_purchased_cars = atoi(dataBuffer + 28);

        dataBuffer = strstr(dataBuffer, "\"date_of_purchase\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        dataBuffer += 21;
        char* dateString = FindStringValue(dataBuffer, '\"');
        char *token = strtok(dateString, "\\");

        transactionData[i].date_of_purchase.day = atoi(token);
        token = strtok(NULL, "\\");
        transactionData[i].date_of_purchase.month = atoi(token);
        token = strtok(NULL, "\\");
        transactionData[i].date_of_purchase.year = atoi(token);
    }
}

void LoadFeedbackDataFromBufferToStruct(struct Feedback feedbackData[], char *dataBuffer, int feedbackCount)
{
    for (int i = 0; i < feedbackCount; i++) {
        dataBuffer = strstr(dataBuffer, "\"customer_name\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        dataBuffer += 18;
        feedbackData[i].customer_name = FindStringValue(dataBuffer, '\"');

        dataBuffer = strstr(dataBuffer, "\"feedback_car_make\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        dataBuffer += 22;
        feedbackData[i].car_make = FindStringValue(dataBuffer, '\"');

        dataBuffer = strstr(dataBuffer, "\"feedback_car_model\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        dataBuffer += 23;
        feedbackData[i].car_model = FindStringValue(dataBuffer, '\"');


        dataBuffer = strstr(dataBuffer, "\"feedback_year_of_manufacture\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        feedbackData[i].year_of_manufacture = atoi(dataBuffer + 32);

        dataBuffer = strstr(dataBuffer, "\"number_of_purchased_cars\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        feedbackData[i].number_of_purchased_cars = atoi(dataBuffer + 28);

        dataBuffer = strstr(dataBuffer, "\"feedback_rating\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        feedbackData[i].feedback_rating = atoi(dataBuffer + 19);

        dataBuffer = strstr(dataBuffer, "\"feedback_comment\":");
        if (dataBuffer == NULL) {
            printf("Database data is not valid.\n");
            break;
        }
        dataBuffer += 21;
        feedbackData[i].feedback_comment = FindStringValue(dataBuffer, '\"');
    }
}

void ViewCarStock(struct Car carData[], int numberOfCars)
{
    // Sort car data into a descending array by year of manufacture
    printf("\nView Car Stock! (Ordered by Descending).\n");
    for (int i = 0; i < numberOfCars; i++)
    {
        printf("\n%d) %s %s %d | %d remaining.", i + 1, carData[i].car_make, carData[i].car_model, carData[i].year_of_manufacture, carData[i].remaining_amount);
    }
    PressEnterToContinue();
}

void UpdateJsonFile(struct Car carData[], int numberOfCars, struct Transaction transactionData[], int transactionsCount, struct Feedback feedbackData[], int feedbackCount) {
    FILE *jsonFile = fopen(fileName, "w");

    if (jsonFile == NULL) {
        printf("\nError opening the JSON file for writing.\n");
        return;
    }

    fprintf(jsonFile, "{\n  \"cars\": [\n");

    for (int i = 0; i < numberOfCars; i++) {
        fprintf(jsonFile, "    {\n");
        fprintf(jsonFile, "      \"car_id\": %d,\n", carData[i].car_id);
        fprintf(jsonFile, "      \"car_make\": \"%s\",\n", carData[i].car_make);
        fprintf(jsonFile, "      \"car_model\": \"%s\",\n", carData[i].car_model);
        fprintf(jsonFile, "      \"price\": %d,\n", carData[i].price);
        fprintf(jsonFile, "      \"year_of_manufacture\": %d,\n", carData[i].year_of_manufacture);
        fprintf(jsonFile, "      \"remaining_amount\": %d\n", carData[i].remaining_amount);
        fprintf(jsonFile, "      \"sold_amount\": %d\n", carData[i].sold_amount);
        fprintf(jsonFile, "      \"total_sales_amount\": %d\n", carData[i].total_sales_amount);


        if (i == numberOfCars - 1) {
            fprintf(jsonFile, "    }\n");
        } else {
            fprintf(jsonFile, "    },\n");
        }
    }

    fprintf(jsonFile, "  ]\n},\n");

    fprintf(jsonFile, "{\n  \"user_transactions\": [\n");

    for (int i = 0; i < transactionsCount; i++) {
        fprintf(jsonFile, "    {\n");
        fprintf(jsonFile, "      \"customer_name\": \"%s\",\n", transactionData[i].customer.customer_name);
        fprintf(jsonFile, "      \"customer_age\": %d,\n", transactionData[i].customer.customer_age);
        fprintf(jsonFile, "      \"transaction_car_make\": \"%s\",\n", transactionData[i].car_make);
        fprintf(jsonFile, "      \"transaction_car_model\": \"%s\",\n", transactionData[i].car_model);
        fprintf(jsonFile, "      \"transaction_year_of_manufacture\": %d,\n", transactionData[i].year_of_manufacture);
        fprintf(jsonFile, "      \"total_price_of_transaction\": %d,\n", transactionData[i].total_price_of_transaction);
        fprintf(jsonFile, "      \"discount_given\": %s,\n", transactionData[i].discount_given ? "true" : "false");
        fprintf(jsonFile, "      \"discount_value\": %d,\n", transactionData[i].discount_value);
        fprintf(jsonFile, "      \"number_of_purchased_cars\": %d,\n", transactionData[i].number_of_purchased_cars);
        fprintf(jsonFile, "      \"date_of_purchase\": \"%d\\%d\\%d\"\n", transactionData[i].date_of_purchase.day, transactionData[i].date_of_purchase.month, transactionData[i].date_of_purchase.year);

        if (i == transactionsCount - 1) {
            fprintf(jsonFile, "    }\n");
        } else {
            fprintf(jsonFile, "    },\n");
        }
    }

    fprintf(jsonFile, "  ]\n},\n");

    fprintf(jsonFile, "{\n  \"customer_feedback\": [\n");

    for (int i = 0; i < feedbackCount; i++) {
        fprintf(jsonFile, "    {\n");
        fprintf(jsonFile, "      \"customer_name\": \"%s\",\n", feedbackData[i].customer_name);
        fprintf(jsonFile, "      \"feedback_car_make\": \"%s\",\n", feedbackData[i].car_make);
        fprintf(jsonFile, "      \"feedback_car_model\": \"%s\",\n", feedbackData[i].car_model);
        fprintf(jsonFile, "      \"feedback_year_of_manufacture\": %d,\n", feedbackData[i].year_of_manufacture);
        fprintf(jsonFile, "      \"number_of_purchased_cars\": %d,\n", feedbackData[i].number_of_purchased_cars);
        fprintf(jsonFile, "      \"feedback_rating\": %d,\n", feedbackData[i].feedback_rating);
        fprintf(jsonFile, "      \"feedback_comment\": \"%s\",\n", feedbackData[i].feedback_comment);

        if (i == feedbackCount - 1) {
            fprintf(jsonFile, "    }\n");
        } else {
            fprintf(jsonFile, "    },\n");
        }
    }

    fprintf(jsonFile, "  ]\n},\n");
    fclose(jsonFile);
}

void BuyCar(struct Car carData[], int numberOfCars, int discountPercentage, struct Transaction** transactionData, int *transactionsCount, struct Feedback** feedbackData, int *feedbackCount)
{
    int userCarInt;
    printf("\nBuy Cars!\n");
    for (int i = 0; i < numberOfCars; i++)
    {
        printf("\n%d) %s %s %d | %d remaining at $%d each.", i + 1, carData[i].car_make, carData[i].car_model, carData[i].year_of_manufacture, carData[i].remaining_amount, carData[i].price);
    }

    printf("\n");

    while (true)
    {
        printf("\nSelect the vehicle of your choice: ");
        if (scanf("%d", &userCarInt) != 1) {
            FlushInputBuffer();
            printf("\nPlease choose a valid option.");
            continue;
        }

        FlushInputBuffer();

        if (userCarInt < 1 || userCarInt > numberOfCars)
        {
            printf("\nPlease choose a valid option.");
            continue;
        }
        break;
    }

    int amountOfCars = 0;
    int selectedCar = userCarInt - 1;

    if (carData[selectedCar].remaining_amount < 1)
    {
        printf("\nNo units available for sale.\n");
        return;
    }

    while (true)
    {
        printf("\nHow many units do you want to buy? (Maximum: %d): ", carData[selectedCar].remaining_amount);
        if (scanf("%d", &amountOfCars) != 1) {
            FlushInputBuffer();
            printf("\nPlease enter a valid number.");
            continue;
        }

        FlushInputBuffer();

        if (amountOfCars < 1 || amountOfCars > carData[selectedCar].remaining_amount)
        {
            printf("\nPlease enter a valid number.");
            continue;
        }
        break;
    }

    int discountedTotal;
    int totalPrice = carData[selectedCar].price * amountOfCars;

    char confirmationChoice = 'n';
    char confirmationChoiceLower;
    while (true)
    {
        if (discountPercentage != 0) {
            discountedTotal = (discountPercentage * totalPrice) / 100;
            totalPrice -= discountedTotal;

            printf("\nConfirm your purchase of %d units of, %s %s %d. At a discounted price of $%d (y or n):", amountOfCars, carData[selectedCar].car_make, carData[selectedCar].car_model, carData[selectedCar].year_of_manufacture, totalPrice);
        }
        else {
            printf("\nConfirm your purchase of %d units of, %s %s %d. With a total of $%d (y or n):", amountOfCars, carData[selectedCar].car_make, carData[selectedCar].car_model, carData[selectedCar].year_of_manufacture, totalPrice);
        }

        if (scanf("%c", &confirmationChoice) != 1) {
            FlushInputBuffer();
            printf("\nPlease enter a valid option.");
            continue;
        }

        FlushInputBuffer();

        confirmationChoiceLower = tolower(confirmationChoice);
        if (confirmationChoiceLower != 'y' && confirmationChoiceLower != 'n')
        {
            printf("\nPlease enter a valid option.");
            continue;
        }
        break;
    }

    if (confirmationChoiceLower == 'y')
    {
        carData[selectedCar].remaining_amount -= amountOfCars;
        carData[selectedCar].sold_amount += amountOfCars;
        carData[selectedCar].total_sales_amount += totalPrice;

        struct Transaction activeTransaction = {
                .customer = activeUser,
                .car_make = carData[selectedCar].car_make,
                .car_model = carData[selectedCar].car_model,
                .year_of_manufacture = carData[selectedCar].year_of_manufacture,
                .total_price_of_transaction = totalPrice,
                .number_of_purchased_cars = amountOfCars
        };

        time_t t;
        struct tm *current_date;

        t = time(NULL);
        current_date = localtime(&t);

        struct Date currentDate = {
                .day = current_date->tm_mday,
                .month = current_date->tm_mon + 1,
                .year = current_date->tm_year + 1900
        };

        activeTransaction.date_of_purchase = currentDate;

        if (discountPercentage != 0)
        {
            printf("\nYou have saved a total of $%d.", discountedTotal);
            activeTransaction.discount_given = true;
            activeTransaction.discount_value = discountedTotal;
        }
        else
        {
            activeTransaction.discount_given = false;
            activeTransaction.discount_value = 0;
        }

        printf("\nYou have purchased %d units of %s %s %d for a grand total of $%d.\n", amountOfCars, carData[selectedCar].car_make, carData[selectedCar].car_model, carData[selectedCar].year_of_manufacture, totalPrice);

        char feedbackConfirmationChoice = 'n';
        char feedbackConfirmationChoiceLower;
        while (true)
        {
            printf("\nDo you want to leave feedback on your purchase? (y or n): ");

            if (scanf("%c", &feedbackConfirmationChoice) != 1) {
                FlushInputBuffer();
                printf("\nPlease enter a valid option.");
                continue;
            }

            FlushInputBuffer();

            feedbackConfirmationChoiceLower = tolower(feedbackConfirmationChoice);
            if (feedbackConfirmationChoiceLower != 'y' && feedbackConfirmationChoiceLower != 'n')
            {
                printf("\nPlease enter a valid option.");
                continue;
            }
            break;
        }

        if (feedbackConfirmationChoiceLower == 'y')
        {
            struct Feedback activeFeedback = {
                    .customer_name = activeUser.customer_name,
                    .car_make = carData[selectedCar].car_make,
                    .car_model = carData[selectedCar].car_model,
                    .year_of_manufacture = carData[selectedCar].year_of_manufacture,
                    .number_of_purchased_cars = amountOfCars
            };

            int feedbackRating = 0;
            while (true)
            {
                printf("\nRate your transaction out of 1 - 5: ");

                if (scanf("%d", &feedbackRating) != 1) {
                    FlushInputBuffer();
                    printf("\nPlease enter a valid number.");
                    continue;
                }

                FlushInputBuffer();

                if (feedbackRating < 1 || feedbackRating > 5)
                {
                    printf("\nPlease enter a valid rating (1 - 5).");
                    continue;
                }
                break;
            }

            char* feedbackComment = NULL;
            while (true)
            {
                printf("\nTell us how your transaction went: ");

                if (scanf("%m[^\n]", &feedbackComment) != 1) {
                    FlushInputBuffer();
                    printf("\nPlease enter a valid comment.");
                    continue;
                }

                FlushInputBuffer();
                break;
            }

            activeFeedback.feedback_rating = feedbackRating;
            activeFeedback.feedback_comment = feedbackComment;

            *feedbackCount += 1;
            struct Feedback* tempFeedbackData = malloc(*feedbackCount * sizeof(struct Feedback));

            if (tempFeedbackData == NULL) {
                printf("Memory reallocation failed.\n");
                exit(1);
            }

            if (*feedbackCount > 1)
            {
                for (int i = 0; i < *feedbackCount - 1; i++)
                {
                    tempFeedbackData[i] = (*feedbackData)[i];
                }
            }

            tempFeedbackData[*feedbackCount - 1] = activeFeedback;

            free(*feedbackData);

            *feedbackData = malloc(*feedbackCount * sizeof(struct Feedback));
            memcpy(*feedbackData, tempFeedbackData, *feedbackCount * sizeof(struct Feedback));

            free(tempFeedbackData);

            printf("\nThank you for leaving feedback!\n");
        }

        *transactionsCount += 1;
        struct Transaction* tempTransactionData = malloc(*transactionsCount * sizeof(struct Transaction));

        if (tempTransactionData == NULL) {
            printf("Memory reallocation failed.\n");
            exit(1);
        }

        if (*transactionsCount > 1)
        {
            for (int i = 0; i < *transactionsCount - 1; i++)
            {
                tempTransactionData[i] = (*transactionData)[i];
            }
        }

        tempTransactionData[*transactionsCount - 1] = activeTransaction;

        free(*transactionData);

        *transactionData = malloc(*transactionsCount * sizeof(struct Transaction));
        memcpy(*transactionData, tempTransactionData, *transactionsCount * sizeof(struct Transaction));

        UpdateJsonFile(carData, numberOfCars, *transactionData, *transactionsCount, *feedbackData, *feedbackCount);

        free(tempTransactionData);

        PressEnterToContinue();
    }
}

void ViewSalesData(struct Car carData[], int numberOfCars, struct Transaction transactionData[], int transactionsCount)
{
    printf("\nCar Sales Data:\n\n");
    SortTransactionData_Descending_By_SalesAmount(transactionData, transactionsCount);
    for (int i = 0; i < numberOfCars; i ++)
    {
        printf("%d) %s %s %d sold %d units for a total of $%d.\n", i + 1, carData[i].car_make, carData[i].car_model, carData[i].year_of_manufacture, carData[i].sold_amount, carData[i].total_sales_amount);
    }

    printf("\nCustomer Sales Data! (Ordered by Descending):\n\n");
    for (int i = 0; i < transactionsCount; i++)
    {
        if (transactionData[i].discount_given)
        {
            printf("%d) %s bought %d %s %s %d, with a discount of $%d at the total price of $%d. (%d\\%d\\%d).\n", i + 1, transactionData[i].customer.customer_name, transactionData[i].number_of_purchased_cars, transactionData[i].car_make, transactionData[i].car_model, transactionData[i].year_of_manufacture, transactionData[i].discount_value, transactionData[i].total_price_of_transaction, transactionData[i].date_of_purchase.day, transactionData[i].date_of_purchase.month, transactionData[i].date_of_purchase.year);
        }
        else
        {
            printf("%d) %s bought %d %s %s %d, for the total price of $%d. (%d\\%d\\%d).\n", i + 1, transactionData[i].customer.customer_name, transactionData[i].number_of_purchased_cars, transactionData[i].car_make, transactionData[i].car_model, transactionData[i].year_of_manufacture, transactionData[i].total_price_of_transaction, transactionData[i].date_of_purchase.day, transactionData[i].date_of_purchase.month, transactionData[i].date_of_purchase.year);
        }
    }
    PressEnterToContinue();
}

void ViewCustomerFeedback(struct Feedback feedbackData[], int feedbackCount)
{
    printf("\nCustomer Feedback!\n");

    for (int i = 0; i < feedbackCount; i++)
    {
        printf("\n%d) %s bought %d %s %s %d and gave it a rating of %d/5.\nThey said: \"%s\"\n", i + 1, feedbackData[i].customer_name, feedbackData[i].number_of_purchased_cars, feedbackData[i].car_make, feedbackData[i].car_model, feedbackData[i].year_of_manufacture, feedbackData[i].feedback_rating, feedbackData[i].feedback_comment);
    }
    PressEnterToContinue();
}

int main() {
    int discountPercentage = 0;

    // Create & Load data to dataBuffer
    FILE *filePtr = fopen(fileName, "a+"); // Use "a+" for reading and appending
    char *dataBuffer = ReadDataIntoMemory(filePtr);

    // Load Car Data from dataBuffer
    int carCount = CountRecordsInDatabase(dataBuffer, "\"cars\"", 5);
    int carCountInDatabase = (carCount < Maximum_Number_Of_Cars) ? carCount : Maximum_Number_Of_Cars;
    struct Car carData[carCountInDatabase];
    LoadCarDataFromBufferToStruct(carData, dataBuffer, carCountInDatabase);

    // Load Transactions from dataBuffer
    int transactionsCount = CountRecordsInDatabase(dataBuffer, "\"user_transactions\"", 18);
    struct Transaction* transactionData = (transactionsCount > 0) ? malloc(transactionsCount * sizeof(struct Transaction)) : NULL;
    LoadTransactionDataFromBufferToStruct(transactionData, dataBuffer, transactionsCount);

    // Load Feedbacks from dataBuffer
    int feedbackCount = CountRecordsInDatabase(dataBuffer, "\"customer_feedback\"", 18);
    struct Feedback* feedbackData = (feedbackCount > 0) ? malloc(feedbackCount * sizeof(struct Feedback)) : NULL;
    LoadFeedbackDataFromBufferToStruct(feedbackData, dataBuffer, feedbackCount);

    // Get User details
    activeUser.customer_name = GetUsername();
    activeUser.customer_age = GetUserAge();
    if (activeUser.customer_age <= 18) {
        discountPercentage = 10;
        printf("Discount of %d%% has been activated. This will be automatically applied at the time of purchase.", discountPercentage);
        PressEnterToContinue();
    }

    // Sort Car Data
    SortCarData_Descending_By_YearOfManufacture(carData, carCountInDatabase);

    bool loop = true;
    while (loop) {
        int userChoice = GetUserSelection();
        switch (userChoice) {
            case 1:
                ViewCarStock(carData, carCountInDatabase);
                continue;
            case 2:
                BuyCar(carData, carCountInDatabase, discountPercentage, &transactionData, &transactionsCount, &feedbackData, &feedbackCount);
                continue;
            case 3:
                ViewSalesData(carData, carCountInDatabase, transactionData, transactionsCount);
                continue;
            case 4:
                ViewCustomerFeedback(feedbackData, feedbackCount);
                continue;
            default:
                printf("Exiting...\n");
                loop = false;
        }
    }

    // Free allocated memory && Exit stuff.
    free(transactionData);
    free(feedbackData);
    free(dataBuffer);
    CloseFile(filePtr);
    return 0;
}