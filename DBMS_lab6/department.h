#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#define SUCCESS 0
#define FAILURE 1
 
struct Department{
	int department_id;
	char department_name[30];
};

extern struct PDS_RepoInfo *repoHandle;

int add_department( struct Department *d );

void print_department( struct Department *d );

int search_department( int department_id, struct Department *d );


#endif