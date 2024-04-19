#ifndef TEACHER_H
#define TEACHER_H

#define SUCCESS 0
#define FAILURE 1
 
struct Teacher{
	int teacher_id;
	char teacher_name[30];
	char phone[15];
};

extern struct PDS_RepoInfo *repoHandle;

int add_teacher( struct Teacher *t );

void print_teacher( struct Teacher *t );

int search_teacher( int teacher_id, struct Teacher *t );

//int store_teachers( char *contact_data_file );

int search_teacher_by_phone( char *phone, struct Teacher *t, int *io_count );

int match_teacher_phone( void *rec, void *key );

int delete_teacher(int teacher_id);

#endif