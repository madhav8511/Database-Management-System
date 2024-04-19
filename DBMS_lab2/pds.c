#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include "pds.h"


// Define the global variable
struct PDS_RepoInfo repo_handle;

int pds_create(char* repo_name)
{
    char filename[30];
    strcpy(filename,repo_name);
    strcat(filename,".dat");

    FILE *fp = fopen(filename,"wb+");
    if(fp == NULL){
        return PDS_FILE_ERROR;
    }
    else{
        fclose(fp);
        return PDS_SUCCESS;
    }
}

int pds_open( char *repo_name, int rec_size  )
{
// Open data files as per the following convention
// If repo_name is "demo", then data file should be "demo.dat"
// Initialize other members of PDS_RepoInfo
    if(repo_handle.repo_status != PDS_REPO_ALREADY_OPEN){
        char filename[30];
        strcpy(filename,repo_name);
        strcat(filename,".dat");
        repo_handle.pds_data_fp = fopen(filename,"rb+");
        strcpy(repo_handle.pds_name,repo_name);
        repo_handle.repo_status = PDS_REPO_OPEN;
        repo_handle.rec_size = rec_size;
        return PDS_SUCCESS;
    }
    return PDS_FILE_ERROR;
}

int put_rec_by_key( int key, void *rec )
{
// Seek to the end of the data file
// Write the key & record at the current file location
    if(repo_handle.repo_status == PDS_REPO_OPEN){
        fseek(repo_handle.pds_data_fp,0,SEEK_END);
        fwrite(&key,sizeof(int),1,repo_handle.pds_data_fp);
        fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
        return PDS_SUCCESS;
    }
    return PDS_ADD_FAILED;

}

int get_rec_by_key( int key, void *rec )
{
 // Seek to beggining of file
 // Read key from data file
 // Read record from data file
 // Compare key of the record with the given key
 // If key match, return else loop
 // Loop 2 to 5 till key is found or end of file reached
 // Return success status if record is found else return failure status
    if(repo_handle.repo_status == PDS_REPO_OPEN){
        fseek(repo_handle.pds_data_fp,0,SEEK_SET);
        int k;
        while(fread(&k,sizeof(int),1,repo_handle.pds_data_fp) > 0){
            if(k == key){
                fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
                return PDS_SUCCESS;
            }
            // or it is sizeof(struct Contact)+1....
            fseek(repo_handle.pds_data_fp,repo_handle.rec_size+1,SEEK_CUR);
        }
        return PDS_REC_NOT_FOUND;
    }
}

int pds_close()
{
// Close the repo file
// Update file pointer and status in global struct
    fclose(repo_handle.pds_data_fp);
    repo_handle.repo_status = PDS_REPO_CLOSED;
}
