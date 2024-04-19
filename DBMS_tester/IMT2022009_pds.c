#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pds.h"
#include "bst.h"

struct PDS_RepoInfo repo_handle;

// pds_create
// Open the data file and index file in "wb" mode
// Initialize index file by storing "0" to indicate there are zero entries in index file
// close the files
int pds_create(char *repo_name, char *linked_repo_name){

	char filename_1[30];
	char filename_2[30];
	char filename_3[30];
	char filename_4[30];

    strcpy(filename_1,repo_name);
	strcpy(filename_2,repo_name);
	strcpy(filename_3,linked_repo_name);
	strcpy(filename_4,repo_name);

	strcat(filename_4,"_");
	strcat(filename_4,filename_3);
	strcat(filename_4,".dat");
    strcat(filename_1,".dat");
	strcat(filename_2,".ndx");
	strcat(filename_3,".dat");


    FILE *fp_1 = fopen(filename_1,"wb");
	FILE *fp_2 = fopen(filename_2,"wb");
	FILE *fp_3 = fopen(filename_3,"wb");
	FILE *fp_4 = fopen(filename_4,"wb");

    if(fp_1 == NULL || fp_2 == NULL || fp_3 == NULL || fp_4 == NULL){
        return PDS_FILE_ERROR;
    }
    else{
		int a = 0;
		fwrite(&a,sizeof(int),1,fp_2);
        fclose(fp_1);
		fclose(fp_2);
		fclose(fp_3);
		fclose(fp_4);
        return PDS_SUCCESS;
    }
	return PDS_FILE_ERROR;
}

// pds_open
// Open the data file and index file in rb+ mode
// Update the fields of PDS_RepoInfo appropriately
// Read the number of records form the index file
// Load the index into the array and store in ndx_array by reading index entries from the index file
// Close only the index file
int pds_open( char *repo_name, char *linked_repo_name, int rec_size, int linked_rec_size ){
	if(repo_handle.repo_status != PDS_REPO_ALREADY_OPEN){

		char filename_1[30];
		char filename_2[30];
		char filename_3[30];
		char filename_4[30];

		strcpy(filename_1,repo_name);
		strcpy(filename_2,repo_name);
		strcpy(filename_3,linked_repo_name);
		strcpy(filename_4,repo_name);

		strcat(filename_4,"_");
		strcat(filename_4,filename_3);
		strcat(filename_4,".dat");
		strcat(filename_1,".dat");
		strcat(filename_2,".ndx");
		strcat(filename_3,".dat");
		//Open the file's
    	repo_handle.pds_data_fp = fopen(filename_1,"rb+");
		repo_handle.pds_ndx_fp = fopen(filename_2,"rb+");

		if(linked_repo_name != NULL)
		{
			repo_handle.pds_linked_data_fp = fopen(filename_3,"rb+");
			repo_handle.pds_link_fp = fopen(filename_4,"rb+");
		}

		if(repo_handle.pds_data_fp == NULL || repo_handle.pds_ndx_fp == NULL || repo_handle.pds_link_fp == NULL || repo_handle.pds_linked_data_fp == NULL){
			return PDS_FILE_ERROR;
		}

		strcpy(repo_handle.pds_name,repo_name);
		repo_handle.repo_status = PDS_REPO_OPEN;
    	repo_handle.rec_size = rec_size;
		repo_handle.linked_rec_size = linked_rec_size;
		repo_handle.ndx_root = NULL;
		int k;
		fread(&k,sizeof(int),1,repo_handle.pds_ndx_fp);
		repo_handle.rec_count = k;
		pds_load_ndx();
		fclose(repo_handle.pds_ndx_fp);
		return PDS_SUCCESS;
	}

	return PDS_FILE_ERROR;
}

// pds_load_ndx - Internal function
// Load the index entries into the BST ndx_root by calling bst_add_node repeatedly for each 
// index entry. Unlike array, for BST, you need read the index file one by one in a loop
int pds_load_ndx(){
	for(int i=0;i<repo_handle.rec_count;i++){
		struct PDS_NdxInfo *a;
		a = (struct PDS_NdxInfo*)malloc(sizeof(struct PDS_NdxInfo));
		fread(a,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
		if(a->is_deleted == 0)
			bst_add_node(&repo_handle.ndx_root,a->key,a);
	}
	return PDS_FILE_ERROR;
}

// put_rec_by_key
// Seek to the end of the data file
// Create an index entry with the current data file location using ftell
// Add index entry to BST by calling bst_add_node
// Increment record count
// Write the record at the end of the file
// Return failure in case of duplicate key
int put_rec_by_key( int key, void *rec ){
	if(repo_handle.repo_status == PDS_REPO_OPEN){
		fseek(repo_handle.pds_data_fp,0,SEEK_END);
		struct PDS_NdxInfo *a;
		a = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
		a->key = key;
		a->offset = ftell(repo_handle.pds_data_fp);
		a->is_deleted = 0;

		if(bst_search(repo_handle.ndx_root,key) != NULL){
			struct PDS_NdxInfo *a = (struct PDS_NdxInfo *)bst_search(repo_handle.ndx_root,key)->data;
			if(a->is_deleted == 1)
			{
				a->key = key;
				a->offset = ftell(repo_handle.pds_data_fp);
				a->is_deleted = 0;
				fwrite(&key,sizeof(int),1,repo_handle.pds_data_fp);
				fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
				return PDS_SUCCESS;
			}
			return PDS_ADD_FAILED;
		}
		else{
			fwrite(&key,sizeof(int),1,repo_handle.pds_data_fp);
			fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
			bst_add_node(&repo_handle.ndx_root,key,a);
			repo_handle.rec_count++;
		}
		return PDS_SUCCESS;
	}
	return PDS_ADD_FAILED;
}

//To put data in linked data file....
int put_linked_rec_by_key( int key, void *rec )
{
	if(repo_handle.repo_status == PDS_REPO_OPEN)
	{
		fseek(repo_handle.pds_linked_data_fp,0,SEEK_END);
		fwrite(&key,sizeof(int),1,repo_handle.pds_linked_data_fp);
		fwrite(rec,repo_handle.linked_rec_size,1,repo_handle.pds_linked_data_fp);
		return PDS_SUCCESS;
	}
	return PDS_REPO_NOT_OPEN;
}

// get_rec_by_key
// Search for index entry in BST by calling bst_search
// Seek to the file location based on offset in index entry
// Read the record from the current location
int get_rec_by_ndx_key( int key, void *rec ){
	if(bst_search(repo_handle.ndx_root,key)!=NULL && repo_handle.repo_status == PDS_REPO_OPEN){
		//struct PDS_NdxInfo *a;
		//a = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
		struct PDS_NdxInfo *a = (struct PDS_NdxInfo *)bst_search(repo_handle.ndx_root,key)->data;
		if(a == NULL || a->is_deleted == 1){
			return PDS_REC_NOT_FOUND;
		}
		fseek(repo_handle.pds_data_fp,0,SEEK_SET);
		fseek(repo_handle.pds_data_fp,a->offset,SEEK_CUR);
		int k;
		fread(&k,sizeof(int),1,repo_handle.pds_data_fp);
		fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
		return PDS_SUCCESS;
	}
	return PDS_REC_NOT_FOUND;
}

// Brute-force retrieval using an arbitrary search key
// io_count = 0
// Loop through data file till EOF
//	fread the key and record 
//	io_count++
//      Invoke the matcher with the current record and input non-ndx search key
//	if mathcher returns success, return the current record, else continue the loop
// end loop
int get_rec_by_non_ndx_key(void *non_ndx_key, void *rec, int (*matcher)(void *rec, void *non_ndx_key), int *io_count)
{
	if(repo_handle.repo_status == PDS_REPO_OPEN){
		*io_count = 0;
		fseek(repo_handle.pds_data_fp,0,SEEK_SET);
		int k;
		for(int i=0;i<repo_handle.rec_count;i++){
			fread(&k,sizeof(int),1,repo_handle.pds_data_fp);
			fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
			(*io_count)++;
			if(matcher(rec,non_ndx_key) == 0){
				struct PDS_NdxInfo *a;
				a = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
				a = bst_search(repo_handle.ndx_root,k)->data;
				if(a->is_deleted == 1){
					(*io_count) = -1;
					return PDS_REC_NOT_FOUND;
				}
				return PDS_SUCCESS;
			}
		}
		*io_count = -1;
		return PDS_REC_NOT_FOUND;
	}
	return PDS_REPO_NOT_OPEN;
}

int delete_rec_by_ndx_key( int key )
{
	if(bst_search(repo_handle.ndx_root,key)!=NULL && repo_handle.repo_status == PDS_REPO_OPEN){
		struct PDS_NdxInfo *a;
		a = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
		a = bst_search(repo_handle.ndx_root,key)->data;
		if(a == NULL  || a->is_deleted == 1){
			return PDS_REC_NOT_FOUND;
		}
		else{
			a->is_deleted = 1;
			//Not do rec_count-- because we check at time of search with the help of bst_tree that particular data is deleted or not.
		}
		return PDS_SUCCESS;
	}
	return PDS_DELETE_FAILED;
}

int get_linked_rec_by_key( int key, void *rec )
{
	if(repo_handle.repo_status == PDS_REPO_OPEN){
		fseek(repo_handle.pds_linked_data_fp,0,SEEK_SET);
		int k;
		while(fread(&k,sizeof(int),1,repo_handle.pds_linked_data_fp) > 0){
			if(k == key)
			{
				fread(rec,repo_handle.linked_rec_size,1,repo_handle.pds_linked_data_fp);
				return PDS_SUCCESS;
			}
			fseek(repo_handle.pds_linked_data_fp,repo_handle.linked_rec_size,SEEK_CUR);
		}
		return PDS_REC_NOT_FOUND;
	}
	return PDS_REPO_NOT_OPEN;
}

int pds_link_rec (int key1, int key2)
{
	if(repo_handle.repo_status == PDS_REPO_OPEN){
		if(bst_search(repo_handle.ndx_root,key1)!=NULL){
			struct PDS_link_info *a;
			a = (struct PDS_link_info *)malloc(sizeof(struct PDS_link_info));
			a->parent_key = key1;
			a->child_key = key2;
			fseek(repo_handle.pds_link_fp,0,SEEK_END);
			fwrite(a,sizeof(struct PDS_link_info),1,repo_handle.pds_link_fp);
			return PDS_SUCCESS;
		}
		else{
			return PDS_REC_NOT_FOUND;
		}
	}
	return PDS_REPO_NOT_OPEN;
}

int pds_get_linked_rec(int parent_key, int linked_keys_result[], int *result_set_size)
{
	if(repo_handle.repo_status == PDS_REPO_OPEN){
		if(bst_search(repo_handle.ndx_root,parent_key)!=NULL){
			struct PDS_NdxInfo *a;
			a = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
			a = bst_search(repo_handle.ndx_root,parent_key)->data;
			if(a != NULL && a->is_deleted == 0){
				fseek(repo_handle.pds_link_fp,0,SEEK_SET);
				*result_set_size = 0;
				struct PDS_link_info *a;
				a = (struct PDS_link_info *)malloc(sizeof(struct PDS_link_info));
				while(fread(a,sizeof(struct PDS_link_info),1,repo_handle.pds_link_fp) > 0)
				{
					if(a->parent_key == parent_key)
					{
						linked_keys_result[(*result_set_size)++] = a->child_key;
					}
				}
				return PDS_SUCCESS;
			}
			else{
				return PDS_REC_NOT_FOUND;
			}
		}
		else
		{
			return PDS_REC_NOT_FOUND;
		}
	}
	return PDS_REPO_NOT_OPEN;
}

// pds_close
// Open the index file in wb mode (write mode, not append mode)
// Store the number of records
// Unload the ndx_array into the index file by traversing the BST in pre-order mode (overwrite the entire index file)
// Think why should it NOT be in-order?
// Close the index file and data file

void preorder( struct BST_Node *root )
{
	if( root == NULL )
		return;
	else{
		struct PDS_NdxInfo *a;
		a = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
		a = root->data;
		if(a->is_deleted == 0)
			fwrite(root->data,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
		preorder( root->left_child );
		preorder( root->right_child );
	}
}

int pds_close(){
	if(repo_handle.repo_status == PDS_REPO_OPEN){
		char filename[30];
		strcpy(filename,repo_handle.pds_name);
		strcat(filename,".ndx");
		repo_handle.pds_ndx_fp = fopen(filename,"wb");
		if(repo_handle.pds_ndx_fp == NULL){
			return PDS_FILE_ERROR;
		}
		fwrite(&repo_handle.rec_count,sizeof(int),1,repo_handle.pds_ndx_fp);
		preorder(repo_handle.ndx_root);
		bst_destroy(repo_handle.ndx_root);

		fclose(repo_handle.pds_data_fp);
		fclose(repo_handle.pds_ndx_fp);
		fclose(repo_handle.pds_linked_data_fp);
		fclose(repo_handle.pds_link_fp);
		
		repo_handle.repo_status = PDS_REPO_CLOSED;
		return PDS_SUCCESS;
	}
	return PDS_FILE_ERROR;
}
