#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "teacher.h"
#include "department.h"
#include "pds.h"
// Include the header file that contains the struct declarations for parent and child records
char* Names[10] = {"Emma", "Liam", "Oliv", "Noah", "Avan", "Will", "Soph", "Elij", "Isab", "Jame"};
char* PhoneNumbers[10] = {"9123456780","9234567891","9345678902","9456789013","9567890124","9678901235","9789012346","9890123457","9901234568","9012345679"};
char* Departments[10] = {"Computer Science", "Electrical Engineering", "Mechanical Engineering", "Physics", "Chemistry", "Biology", "Economics", "Psychology", "History", "English"};
// Declare global variables for 10 parent and 10 child records

void show_menu();
void setup_data();
void process_option( int option );
void process_option1();
void process_option2();

int main()
{
    int option;
    setup_data();
    do{
        show_menu();
        scanf("%d",&option);
        process_option(option);
    } while (option != 0);
    return 0;
}

void show_menu()
{
    printf("\nLINKED DATA DEMO\n\n");
    printf("0. Exit\n");
    printf("1. Add linked data\n");
    printf("2. Get linked data\n");
    printf("\nEnter option: ");  
}

void setup_data()
{
    // statically create 10 parent and 10 child records individually
    int st1 = pds_create("teacher","department");
    int st2 = pds_open("teacher","department",sizeof(struct Teacher),sizeof(struct Department));

    printf("Teacher data in System\n");
    for(int i=0;i<10;i++)
    {
        struct Teacher *t = (struct Teacher *)malloc(sizeof(struct Teacher));
        strcpy(t->teacher_name,Names[i]);
        strcpy(t->phone,PhoneNumbers[i]);
        t->teacher_id = i;
        add_teacher(t);
        print_teacher(t);
    }
    printf("\n");
    printf("Department data in System\n");
    for(int i=0;i<10;i++)
    {
        struct Department *d = (struct Department *)malloc(sizeof(struct Department));
        strcpy(d->department_name,Departments[i]);
        d->department_id = i+100;
        add_department(d);
        print_department(d);
    }
}

void process_option( int option )
{
    switch(option){
        case 0: 
            // do nothing
            break;
        case 1:
            process_option1();
            break;
        case 2:
            process_option2();
            break;
        default:
            printf("Invalid option\n");
            break;
    }
}

void process_option1()
{
    int parent_key, child_key;
    printf("Enter parent key and child key for linking : ");
    scanf("%d %d", &parent_key, &child_key);
    // TODO: Call appropriate PDS functions here to link the two records
    struct Teacher *t = (struct Teacher *)malloc(sizeof(struct Teacher));
    struct Department *d = (struct Department *)malloc(sizeof(struct Department));
    int st1 = search_teacher(parent_key,t);
    int st2 = search_department(child_key,d);
    if(st1 == SUCCESS && st2 == SUCCESS)
    {
        int status = pds_link_rec(parent_key,child_key);
        printf("\n");
        if(status == PDS_SUCCESS)
        {
            printf("Linked Successfully\n");
        }
        else
        {
            printf("Link not happen\n");
        }
    }
    else
    {
        printf("Either teacher or department not exist.\n");
    }
}

void process_option2()
{
    int parent_key;
    printf("Enter parent key for searching department linked with it : ");
    scanf("%d", &parent_key);
    struct Teacher *t = (struct Teacher *)malloc(sizeof(struct Teacher));
    int st1 = search_teacher(parent_key,t);
    if(st1 == PDS_SUCCESS)
    {
        int a[100];
        int *k = (int *)malloc(sizeof(int));
        int status = pds_get_linked_rec(parent_key,a,k);
        printf("\n");
        if((*k)==0)
        {
            printf("No department is linked with teacher id : %d\n",parent_key);
        }
        else
        {
            printf("Department Keys linked with Teacher id : %d\n",parent_key);
            for(int i=0;i<(*k);i++)
            {
                printf("Got key : %d\n",a[i]);
            }
            printf("\n");
            printf("Departments data : \n");
            for(int i=0;i<(*k);i++)
            {
                struct Department *d = (struct Department *)malloc(sizeof(struct Department));
                int st1 = get_linked_rec_by_key(a[i],d);
                print_department(d);
            }
        }

    }
    else
    {
        printf("Teacher not exist.\n");
    }
    // TODO: Call appropriate PDS functions here to fetch arrach of child keys
    // Call your PDS functions to display the list of child keys retrieved for the given parent key
    // Display all field values of each child record in a nicely formatted manner
}
