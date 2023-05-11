#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct Transaction
{
    int id;
    char date[11];
    int product_id;
    int quantity;
    float revenue;
};

void readSalesData(char *filename, struct Transaction *transactions, int *num_transactions)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: could not open file %s\n", filename);
        exit(1);
    }

    char buffer[1024];
    fgets(buffer, sizeof(buffer), fp);
    buffer[strcspn(buffer, "\r\n")] = 0;

    int i = 0;
    while (fgets(buffer, sizeof(buffer), fp))
    {
        struct Transaction transaction;
        sscanf(buffer, "%d,%[^,],%d,%d,%f", &transaction.id, transaction.date, &transaction.product_id, &transaction.quantity, &transaction.revenue);
        transactions[i] = transaction;
        i++;
    }

    *num_transactions = i;

    fclose(fp);
}

void writeNewTransaction(char *filename)
{
    FILE *fp = fopen(filename, "a");
    if (fp == NULL)
    {
        printf("Error: could not open file %s\n", filename);
        exit(1);
    }

    struct Transaction transaction;
    printf("Enter transaction details:\n");
    printf("ID: ");
    scanf("%d", &transaction.id);
    printf("Date (YYYY-MM-DD): ");
    scanf("%s", transaction.date);
    printf("Product ID: ");
    scanf("%d", &transaction.product_id);
    printf("Quantity: ");
    scanf("%d", &transaction.quantity);
    printf("Revenue: ");
    scanf("%f", &transaction.revenue);

    fprintf(fp, "%d,%s,%d,%d,%.2f\n", transaction.id, transaction.date, transaction.product_id, transaction.quantity, transaction.revenue);

    fclose(fp);
}

float calculateTotalRevenue(struct Transaction *transactions, int num_transactions)
{
    float total_revenue = 0.0;
    for (int i = 0; i < num_transactions; i++)
    {
        total_revenue += transactions[i].revenue;
    }
    return total_revenue;
}

float calculateAverageRevenue(struct Transaction *transactions, int num_transactions)
{
    float total_revenue = calculateTotalRevenue(transactions, num_transactions);
    float average_revenue = total_revenue / (num_transactions);
    return average_revenue;
}

int compareByRevenue(const void *a, const void *b)
{
    struct Transaction *t1 = (struct Transaction *)a;
    struct Transaction *t2 = (struct Transaction *)b;
    if (t1->revenue < t2->revenue)
        return 1;
    if (t1->revenue > t2->revenue)
        return -1;
    return 0;
}

int compareByQuantity(const void *a, const void *b)
{
    struct Transaction *t1 = (struct Transaction *)a;
    struct Transaction *t2 = (struct Transaction *)b;
    if (t1->quantity < t2->quantity)
        return 1;
    if (t1->quantity > t2->quantity)
        return -1;
    return 0;
}

void identifyTopSellingProducts(struct Transaction *transactions, int num_transactions, char *filename, char *mode)
{

    if (strcmp(mode, "revenue") == 0)
    {
        qsort(transactions, num_transactions, sizeof(struct Transaction), compareByRevenue);
    }
    else if (strcmp(mode, "quantity") == 0)
    {
        qsort(transactions, num_transactions, sizeof(struct Transaction), compareByQuantity);
    }
    else
    {
        printf("Error: invalid mode specified\n");
        return;
    }

    int num_products = 0;
    int *product_sales = (int *)malloc(num_transactions * sizeof(int));
    memset(product_sales, 0, num_transactions * sizeof(int));
    for (int i = 0; i < num_transactions; i++)
    {
        if (i == 0 || transactions[i].product_id != transactions[i - 1].product_id)
        {
            num_products++;
        }
        product_sales[num_products - 1] += transactions[i].quantity;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Error: could not open file %s\n", filename);
        exit(1);
    }
    if (strcmp(mode, "revenue") == 0)
    {
        fprintf(fp, "Top Selling Products by Revenue\n");
    }
    else
    {
        fprintf(fp, "Top Selling Products by Quantity\n");
    }
    fprintf(fp, "Rank,Product ID,Total Sales\n");
    for (int i = 0; i < num_products; i++)
    {
        int max_index = 0;
        for (int j = 1; j < num_products; j++)
        {
            if (product_sales[j] > product_sales[max_index])
            {
                max_index = j;
            }
        }
        fprintf(fp, "%d,%d,%d\n", i + 1, transactions[max_index * num_transactions / num_products].product_id, product_sales[max_index]);
        product_sales[max_index] = 0;
    }

    fclose(fp);
    free(product_sales);
}

int main()
{
    char input_filename[1024] = "sales.csv";
    char output_filename[1024] = "output.csv";
    struct Transaction transactions[1000];
    int num_transactions = 0;

    readSalesData(input_filename, transactions, &num_transactions);

    int choice;
    do
    {
        printf("\nMenu:\n");
        printf("1. Add new transaction\n");
        printf("2. Calculate total revenue\n");
        printf("3. Calculate average revenue per sale\n");
        printf("4. Identify top-selling products by revenue\n");
        printf("5. Identify top-selling products by quantity\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            writeNewTransaction(input_filename);
            readSalesData(input_filename, transactions, &num_transactions);
            break;
        case 2:
            printf("Total Revenue: $%.2f\n", calculateTotalRevenue(transactions, num_transactions));
            break;
        case 3:
            printf("Average Revenue per Sale: $%.2f\n", calculateAverageRevenue(transactions, num_transactions));
            break;
        case 4:

            identifyTopSellingProducts(transactions, num_transactions, output_filename, "revenue");
            printf("Top selling products by revenue written to %s\n", output_filename);
            break;
        case 5:

            identifyTopSellingProducts(transactions, num_transactions, output_filename, "quantity");
            printf("Top selling products by quantity written to %s\n", output_filename);
            break;
        case 0:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice, please try again\n");
            break;
        }
    } while (choice != 0);

    return 0;
}
