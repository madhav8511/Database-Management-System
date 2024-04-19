# include <stdio.h> 

struct student {
  int rollnum;
  char name[30];
  int age;
};

int save_num_text( char *filename ) {
  //write your code
  int numbers[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
  //Open file in write mode...
  FILE *file = fopen(filename,"w");
  if(file == NULL){
    return 1;
  }
  for(int i=0;i<20;i++){
    //Write content in file...
    fprintf(file,"%d\n",numbers[i]);
  }
  fclose(file);
  return 0;
}

int read_num_text( char *filename ) {
    //write your code
    FILE *file = fopen(filename,"r"); //Opened file in read mode..
    if(file == NULL){
      return 1;
    }
    int n;
    for(int i=0;i<20;i++){
      fscanf(file,"%d",&n); //Read the content from file
      printf("%d\n",n); //Print on screen...
    }
    fclose(file); //Close the file..
    return 0;
}

int save_struct_text( char *filename ) {
  //write your code
    struct student s1 = {1,"madhav",20};
    struct student s2 = {2,"nikhil",15};
    struct student s3 = {3,"ryan",12};
    struct student s4 = {4,"harsh",20};
    struct student s5 = {5,"sunil",25};
    struct student S[5] = {s1,s2,s3,s4,s5};
    FILE *file = fopen(filename,"w"); //Open file in write mode..
    if(file == NULL){
      return 1;
    }
    for(int i=0;i<5;i++){
      fprintf(file,"%d %s %d\n",S[i].rollnum,S[i].name,S[i].age); //Store the struct data in file...
    }
    fclose(file); //Close the file..
    return 0;
}

int read_struct_text( char *filename ) {
//write your code
  char name[10];
  int age,rollnum;
  FILE *file = fopen(filename,"r"); //Open file in read mode..
  if(file == NULL){
    return 1;
  }
  for(int i=0;i<5;i++){
    fscanf(file,"%d %s %d",&rollnum,name,&age); //Read the strcut data one by one from the file...
    printf("Roll No: %d, Name: %s, Age: %d\n",rollnum,name,age); //Print the data on terminal screen..
  }
  fclose(file); //Close the file...
  return 0;
}


int save_num_binary( char *filename ) {
//write your code
  int numbers[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
  FILE *file = fopen(filename,"wb"); //Open the file to write in binary format..
  if(file == NULL){
    return 1;
  }
  fwrite(numbers,sizeof(int),20,file); //Write these numbers in file...
  fclose(file);
  return 0;
}

int read_num_binary( char *filename ) {
 //write your code
  FILE *file = fopen(filename,"rb"); //Open the file in read binary format...
  if(file == NULL){
    return 1;
  }
  int number[20];
  fread(number,sizeof(int),20,file); //read the number and store it in array...
  for(int i=0;i<20;i++){
    printf("%d\n",number[i]); //Print the numbers...
  }
  fclose(file);
  return 0;
}

int save_struct_binary( char *filename ) {
  //write your code
    struct student s1 = {1,"madhav",20};
    struct student s2 = {2,"nikhil",15};
    struct student s3 = {3,"ryan",12};
    struct student s4 = {4,"harsh",20};
    struct student s5 = {5,"sunil",25};
    struct student S[5] = {s1,s2,s3,s4,s5};
    FILE *file = fopen(filename,"wb");
    if(file == NULL){
      return 1;
    }
    fwrite(S,sizeof(struct student),5,file); //Store the struct array...
    fclose(file); //Close the file...
    return 0;
}

int read_struct_binary( char *filename ) {
 //write your code
  FILE *file = fopen(filename,"rb");
  if(file == NULL){
    return 1;
  }
  struct student S[5];
  fread(S,sizeof(struct student),5,file); //Read content from file and store it in struct array....
  for(int i=0;i<5;i++){
    printf("Roll No: %d Name: %s Age: %d\n",S[i].rollnum,S[i].name,S[i].age); //Print on terminal screen...
  }
  fclose(file); //Close the file...
  return 0;
}

