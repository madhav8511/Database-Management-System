create table EMPLOYEE (
    Fname varchar(50),
    Minit varchar(50),
    Lname varchar(50),
    Ssn int,
    Bdate date,
    Address varchar(100),
    Sex varchar(10),
    Salary int,
    Super_ssn int,
    Dno smallint
);

create table DEPARTMENT(
    dname varchar(30) NOT NULL,
    dnumber smallint,
    mgr_ssn int,
    mgr_start_date date,
    constraint pk_department PRIMARY KEY (dnumber)    
);

create table DEPT_LOCATION(
    dnumber smallint,
    dlocation varchar(50)
);

create table PROJECT(
    pname varchar(50),
    pnumber int,
    plocation varchar(50),
    dnum smallint
);

create table WORKS_ON(
    Essn int,
    Pno int,
    Hours int
);

create table DEPENDENT(
    Essn int,
    Dependent_name varchar(50),
    Sex varchar(10),
    Bdate date,
    Relationship varchar(50)
);