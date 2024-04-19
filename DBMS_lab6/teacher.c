#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pds.h"
#include "teacher.h"

void print_teacher( struct Teacher *t )
{
    printf("Teacher Id :%d, Teacher Name :%s, Teacher Phone-No. : %s\n", t->teacher_id,t->teacher_name,t->phone);
}

int search_teacher( int teacher_id, struct Teacher *t )
{
    return get_rec_by_ndx_key( teacher_id, t );
}

int add_teacher( struct Teacher *t )
{
    int status;

	status = put_rec_by_key( t->teacher_id, t);

	if( status != PDS_SUCCESS ){
		fprintf(stderr, "Unable to add contact with key %d. Error %d", t->teacher_id, status );
		return FAILURE;
	}
	return status;
}

int delete_teacher(int teacher_id)
{
    int status = delete_rec_by_ndx_key(teacher_id);
	if(status == PDS_SUCCESS){
		return SUCCESS;
	}
	return FAILURE;
}

int search_teacher_by_phone( char *phone, struct Teacher *t, int *io_count )
{
	int status = get_rec_by_non_ndx_key(phone,t,match_teacher_phone,io_count);

	if(status == PDS_REC_NOT_FOUND){
		return FAILURE;
	}
	return SUCCESS;
	// Call function
}

int match_teacher_phone( void *rec, void *key )
{

	struct Teacher *t = (struct Teacher*)rec;
	char *k = (char *)key;

	if(strcmp(t->phone,k)==0){
		return 0;
	}
	else{
		return 1;
	}
	return 2;
}