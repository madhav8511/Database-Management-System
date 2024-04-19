#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pds.h"
#include "department.h"

int add_department( struct Department *d )
{
    int status;

	status = put_linked_rec_by_key( d->department_id, d);

	if( status != PDS_SUCCESS ){
		fprintf(stderr, "Unable to add contact with key %d. Error %d", d->department_id, status );
		return FAILURE;
	}
	return status;
}

void print_department( struct Department *d )
{
    printf("Department Id: %d, Department Name : %s\n", d->department_id,d->department_name);
}

int search_department( int department_id, struct Department *d )
{
    return get_linked_rec_by_key( department_id, d );
}