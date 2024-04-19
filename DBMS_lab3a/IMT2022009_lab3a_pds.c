#include <stdio.h>
#include<string.h>
#include "pds.h"

struct PDS_RepoInfo repo_handle;

// pds_create
// Open the data file and index file in "wb" mode
// Initialize index file by storing "0" to indicate there are zero entries in index file
// close the files
int pds_create(char *repo_name){
	char filename_1[30];
	char filename_2[30];
    strcpy(filename_1,repo_name);
	strcpy(filename_2,repo_name);
    strcat(filename_1,".dat");
	strcat(filename_2,".ndx");

    FILE *fp_1 = fopen(filename_1,"wb+");
	FILE *fp_2 = fopen(filename_2,"wb+");
    if(fp_1 == NULL || fp_2 == NULL){
        return PDS_FILE_ERROR;
    }
    else{
		int a = 0;
		fwrite(&a,sizeof(int),1,fp_2);
        fclose(fp_1);
		fclose(fp_2);
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
int pds_open( char *repo_name, int rec_size ){
	if(repo_handle.repo_status != PDS_REPO_ALREADY_OPEN){

        char filename_1[30];
		char filename_2[30];
    	strcpy(filename_1,repo_name);
		strcpy(filename_2,repo_name);
    	strcat(filename_1,".dat");
		strcat(filename_2,".ndx");

		//Open the file's
        repo_handle.pds_data_fp = fopen(filename_1,"rb+");
		repo_handle.pds_ndx_fp = fopen(filename_2,"rb+");

		//Store the data...
        strcpy(repo_handle.pds_name,repo_name);
        repo_handle.repo_status = PDS_REPO_OPEN;
        repo_handle.rec_size = rec_size;
		int k;
		fread(&k,sizeof(int),1,repo_handle.pds_ndx_fp);
		repo_handle.rec_count = k;
		pds_load_ndx();
		//Close index file..
		fclose(repo_handle.pds_ndx_fp);
        return PDS_SUCCESS;
    }
	return PDS_FILE_ERROR;
}

// pds_load_ndx
// Internal function used by pds_open to read index entries into ndx_array
int pds_load_ndx(){
	int status = fread(repo_handle.ndx_array,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
	if(status == repo_handle.rec_count){
		return PDS_SUCCESS;
	}
	return PDS_FILE_ERROR;
}

// put_rec_by_key
// Seek to the end of the data file
// Create an index entry with the current data file location using ftell
// Add index entry to ndx_array using offset returned by ftell
// Increment record count
// Write the record at the end of the file
// Return failure in case of duplicate key
int put_rec_by_key( int key, void *rec ){
	if(repo_handle.repo_status == PDS_REPO_OPEN){
        fseek(repo_handle.pds_data_fp,0,SEEK_END);
		repo_handle.ndx_array[repo_handle.rec_count].key = key;
		repo_handle.ndx_array[repo_handle.rec_count].offset = ftell(repo_handle.pds_data_fp);
		repo_handle.rec_count++;
		fwrite(&key,sizeof(int),1,repo_handle.pds_data_fp);
		fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
        return PDS_SUCCESS;
    }
	return PDS_ADD_FAILED;
}

// get_rec_by_key
// Search for index entry in ndx_array
// Seek to the file location based on offset in index entry
// Read the key at the current location 
// Read the record from the current location
int get_rec_by_key( int key, void *rec ){
	if(repo_handle.repo_status == PDS_REPO_OPEN){
		int location;
		for(int i=0;i<repo_handle.rec_count;i++){
			if(repo_handle.ndx_array[i].key == key){
				location = repo_handle.ndx_array[i].offset;
				fseek(repo_handle.pds_data_fp,0,SEEK_SET);
				fseek(repo_handle.pds_data_fp,location,SEEK_CUR);
				int k;
				fread(&k,sizeof(int),1,repo_handle.pds_data_fp);
				fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
				return PDS_SUCCESS;
			}
		}
		return PDS_REC_NOT_FOUND;	
	}
	return PDS_REPO_NOT_OPEN;
}

// pds_close
// Open the index file in wb mode (write mode, not append mode)
// Store the number of records
// Unload the ndx_array into the index file by traversing it (overwrite the entire index file)
// Close the index file and data file
int pds_close(){
	if(repo_handle.repo_status == PDS_REPO_OPEN){
		char filename[30];
		strcpy(filename,repo_handle.pds_name);
		strcat(filename,".ndx");
		repo_handle.pds_ndx_fp = fopen(filename,"wb");
		fwrite(&repo_handle.rec_count,sizeof(int),1,repo_handle.pds_ndx_fp);
		fwrite(repo_handle.ndx_array,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
		// for(int i=0;i<repo_handle.rec_count;i++){
		// 	fwrite(&repo_handle.ndx_array[i].key,sizeof(int),1,repo_handle.pds_ndx_fp);
		// 	fwrite(&repo_handle.ndx_array[i].offset,sizeof(int),1,repo_handle.pds_ndx_fp);
		// }
		repo_handle.repo_status = PDS_REPO_CLOSED;
		fclose(repo_handle.pds_ndx_fp);
		fclose(repo_handle.pds_data_fp);
		return PDS_SUCCESS;
	}
	return PDS_FILE_ERROR;
}
