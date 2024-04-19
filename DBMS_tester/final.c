#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pds.h"
#include "teacher.h"
#include "department.h"

#define TREPORT(a1,a2) printf("Status: %s - %s\n\n",a1,a2); fflush(stdout);

void process_line( char *test_case );

int main(int argc, char *argv[])
{
	FILE *cfptr;
	char test_case[50];

	if( argc != 2 ){
		fprintf(stderr, "Usage: %s testcasefile\n", argv[0]);
		exit(1);
	}

	cfptr = (FILE *) fopen(argv[1], "r");
	while(fgets(test_case, sizeof(test_case)-1, cfptr)){
		// printf("line:%s",test_case);
		if( !strcmp(test_case,"\n") || !strcmp(test_case,"") )
			continue;
		process_line( test_case );
	}
}

void process_line( char *test_case )
{
	char repo_name[30];
	char link_name[30];
	char command[15], param1[15], param2[15], info[1024], param3[15];
	int teacher_id, status, rec_size, expected_status, linked_rec_size, department_id;
	int parent_key, child_key, expec_child;
	struct Teacher testTeacher;
	struct Department testDepartment;

	// strcpy(testContact.contact_name, "dummy name");
	// strcpy(testContact.phone, "dummy number");

	rec_size = sizeof(struct Teacher);
	linked_rec_size = sizeof(struct Department);

	sscanf(test_case, "%s%s%s%s", command, param1, param2, param3);
	printf("Test case: %s", test_case); fflush(stdout);
	if( !strcmp(command,"CREATE") ){
		strcpy(repo_name, param1);
		strcpy(link_name,param2);
		if( !strcmp(param3,"0") )
			expected_status = SUCCESS;
		else
			expected_status = FAILURE;

		status = pds_create( repo_name, link_name );
		if(status == PDS_SUCCESS)
			status = SUCCESS;
		else
			status = FAILURE;
		if( status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_create returned status %d",status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"OPEN") ){
		strcpy(repo_name, param1);
		strcpy(link_name, param2);
		if( !strcmp(param3,"0") )
			expected_status = SUCCESS;
		else
			expected_status = FAILURE;

		status = pds_open( repo_name, link_name,rec_size,linked_rec_size );
		if(status == PDS_SUCCESS)
			status = SUCCESS;
		else
			status = FAILURE;
		if( status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_open returned status %d",status);
			TREPORT("FAIL", info);
		}
	}

	else if(!strcmp(command,"LINK"))
	{
		if( !strcmp(param3,"0") )
			expected_status = SUCCESS;
		else
			expected_status = FAILURE;
		
		sscanf(param1, "%d", &parent_key);
		sscanf(param2, "%d", &child_key);


		int dpt = get_linked_rec_by_key(child_key,&testDepartment);
		if(dpt == PDS_SUCCESS)
			status = pds_link_rec(parent_key,child_key);
		else 
			status = PDS_REC_NOT_FOUND;

		if(status == PDS_SUCCESS)
			status = SUCCESS;
		else
			status = FAILURE;
		if( status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_link_rec returned status %d",status);
			TREPORT("FAIL", info);
		}
	}

	else if(!strcmp(command,"GETLINK"))
	{
		if( !strcmp(param3,"0") )
			expected_status = SUCCESS;
		else
			expected_status = FAILURE;
		sscanf(param1, "%d", &parent_key);
		sscanf(param2,"%d", &expec_child);

		char *tokens[4+expec_child];
		int c1 = 0; 
    	char *token; 
		token = strtok(test_case, " ");
		while (token != NULL) {
			tokens[c1] = token; // Store the token in the array
			c1++; // Increment the count
			token = strtok(NULL, " "); // Get the next token
    	}

		int expected_child[expec_child];
		for(int i=4; i<4+expec_child; i++){
			sscanf(tokens[i], "%d", &expected_child[i-4]);
		}
		
		int linked_child[100];
		int *res = (int *)malloc(sizeof(int));
		status = pds_get_linked_rec(parent_key,linked_child,res);
		if(status == PDS_SUCCESS )
			status = SUCCESS;
		else
			status = FAILURE;
		int count = 0;
		if(status == SUCCESS)
		{
			if((*res) == expec_child)
			{
				for(int i=0;i<(*res);i++)
				{
					for(int j=0;j<expec_child;j++)
					{
						if(linked_child[i] == expected_child[j])
						{
                            char expected_name[30];
				            sprintf(expected_name, "Name-of-%d", expected_child[j]);
                            testDepartment.department_id = -1;
                            int k = get_linked_rec_by_key(expected_child[j],&testDepartment);
                            if(k == PDS_SUCCESS && !strcmp(expected_name,testDepartment.department_name) && testDepartment.department_id == expected_child[j])
							    count++;
						}
					}
				}
				if(count != (*res))
				{
					status = FAILURE;
				}
			}
			else
			{
				status = FAILURE;
			}
		}
		if( status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_get_linked_rec returned status %d",status);
			TREPORT("FAIL", info);
		}
	}

	else if( !strcmp(command,"STORE") ){
		if( !strcmp(param3,"0") )
			expected_status = SUCCESS;
		else
			expected_status = FAILURE;

		if(!strcmp(param1,"teacher"))
		{
			sscanf(param2, "%d", &teacher_id);
			testTeacher.teacher_id = teacher_id;
			sprintf(testTeacher.phone, "Phone-of-%d", teacher_id);
			sprintf(testTeacher.teacher_name, "Name-of-%d", teacher_id);
			status = add_teacher( &testTeacher );
		}
		else if(!strcmp(param1,"department"))
		{
			sscanf(param2, "%d", &department_id);
			testDepartment.department_id = department_id;
			sprintf(testDepartment.department_name, "Name-of-%d", department_id);
			status = add_department( &testDepartment );
		}

		if(status == PDS_SUCCESS)
			status = SUCCESS;
		else
			status = FAILURE;
		if( status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"add_contact returned status %d",status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"NDX_SEARCH") ){
		if( !strcmp(param3,"0") )
			expected_status = SUCCESS;
		else
			expected_status = FAILURE;

		if(!strcmp(param1,"teacher"))
		{
			
			sscanf(param2, "%d", &teacher_id);
			testTeacher.teacher_id = -1;
			status = search_teacher( teacher_id, &testTeacher );
			if(status == PDS_SUCCESS)
				status = SUCCESS;
			else
				status = FAILURE;
				
			if( status != expected_status ){
				sprintf(info,"search key: %d; Got status %d",teacher_id, status);
				TREPORT("FAIL", info);
			}

			else{
			// Check if the retrieved values match
				char expected_phone[30];
				sprintf(expected_phone, "Phone-of-%d", teacher_id);
				char expected_name[30];
				sprintf(expected_name, "Name-of-%d", teacher_id);
				if( expected_status == 0 ){
					if (testTeacher.teacher_id == teacher_id && 
						strcmp(testTeacher.teacher_name,expected_name) == 0 &&
						strcmp(testTeacher.phone, expected_phone) == 0){
							//print_teacher(&testTeacher);
							TREPORT("PASS", "");
					}
					else{
						sprintf(info,"Contact data not matching... Expected:{%d,%s,%s} Got:{%d,%s,%s}\n",
							teacher_id, expected_name, expected_phone, 
							testTeacher.teacher_id, testTeacher.teacher_name, testTeacher.phone
						);
						TREPORT("FAIL", info);
					}
				}
				else
					TREPORT("PASS", "");
			}
		}
		else if(!strcmp(param1,"department"))
		{
			sscanf(param2,"%d",&department_id);
			testDepartment.department_id = -1;
			status = search_department( department_id, &testDepartment );
			if(status == PDS_SUCCESS)
				status = SUCCESS;
			else
				status = FAILURE;
				
			if( status != expected_status ){
				sprintf(info,"search key: %d; Got status %d",teacher_id, status);
				TREPORT("FAIL", info);
			}
			
			else{
			// Check if the retrieved values match
				char expected_name[30];
				sprintf(expected_name, "Name-of-%d", department_id);
				if( expected_status == 0 ){
					if (testDepartment.department_id == department_id && 
						strcmp(testDepartment.department_name,expected_name) == 0 ){
							//print_department(&testDepartment);
							TREPORT("PASS", "");
					}
					else{
						sprintf(info,"Contact data not matching... Expected:{%d,%s} Got:{%d,%s}\n",
							teacher_id, expected_name, 
							testDepartment.department_id, testDepartment.department_name
						);
						TREPORT("FAIL", info);
					}
				}
				else
					TREPORT("PASS", "");
			}
		}
		
	}
	else if( !strcmp(command,"NDX_DELETE") ){
		if( strcmp(param2,"0") == 0 )
			expected_status = SUCCESS;
		else
			expected_status = SUCCESS;

		sscanf(param1, "%d", &teacher_id);
		testTeacher.teacher_id = -1;
		status = delete_teacher( teacher_id );
		if( status != expected_status ){
			sprintf(info,"delete key: %d; Got status %d",teacher_id, status);
			TREPORT("FAIL", info);
		}
		else{
			TREPORT("PASS", "");
		}
	}

	else if( !strcmp(command,"NON_NDX_SEARCH") ){
		char phone_num[30], expected_name[30], expected_phone[30];
		int expected_io, actual_io;
		
		if( !strcmp(param2,"-1") == 0 )
			expected_status = SUCCESS;
		else
			expected_status = FAILURE;

		sscanf(param1, "%s", phone_num);
		sscanf(param2, "%d", &expected_io);
		testTeacher.teacher_id = -1;
		actual_io = 0;
		status = search_teacher_by_phone( phone_num, &testTeacher, &actual_io );
		if(status == PDS_SUCCESS)
			status = SUCCESS;
		else
			status = FAILURE;
		if( status != expected_status ){
			sprintf(info,"search key: %d; Got status %d",teacher_id, status);
			TREPORT("FAIL", info);
		}
		else{
			sscanf(phone_num+sizeof("Phone-of"), "%d", &teacher_id);
			sprintf(expected_name,"Name-of-%d",teacher_id);
			sprintf(expected_phone,"Phone-of-%d",teacher_id);
			if( expected_status == 0 ){
				if (testTeacher.teacher_id == teacher_id && 
					strcmp(testTeacher.teacher_name, expected_name) == 0 &&
					strcmp(testTeacher.phone, expected_phone) == 0 ){
						if( expected_io == actual_io ){
							//print_teacher(&testTeacher);
							TREPORT("PASS", "");
						}
						else{
							sprintf(info,"Num I/O not matching for contact %d... Expected:%d Got:%d\n",
								testTeacher.teacher_id, expected_io, actual_io
							);
							TREPORT("FAIL", info);
						}
				}
				else{
					sprintf(info,"Contact data not matching... Expected:{%d,%s,%s} Got:{%d,%s,%s}\n",
						teacher_id, expected_name, expected_phone, 
						testTeacher.teacher_id, testTeacher.teacher_name, testTeacher.phone
					);
					TREPORT("FAIL", info);
				}
			}
			else
				TREPORT("PASS", "");
		}
	}

	else if( !strcmp(command,"CLOSE") ){
		if( !strcmp(param1,"0") )
			expected_status = SUCCESS;
		else
			expected_status = FAILURE;

		status = pds_close();
		if(status == PDS_SUCCESS)
			status = SUCCESS;
		else
			status = FAILURE;
		if( status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_close returned status %d",status);
			TREPORT("FAIL", info);
		}
	}
}


