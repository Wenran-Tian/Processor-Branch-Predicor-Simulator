#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct
{
    int data_len;
    char data[0];
}buff_st_1;

typedef struct
{
    int data_len;
    char *data;
}buff_st_2;

typedef struct
{
    int data_len;
    char data[];
}buff_st_3;

typedef struct
{
    uint32_t id;
    uint32_t age;
}student_st;


void print_stu(const student_st *stu)
{
    printf("id:%u,age:%u\n", stu->id, stu->age);
}

int main()
{
    student_st *stu = (student_st *)malloc(sizeof(student_st));
    stu->id = 100;
    stu->age = 23;

    student_st *tmp = NULL;

    buff_st_1 *buff1 = (buff_st_1 *)malloc(sizeof(buff_st_1) + sizeof(student_st));
    buff1->data_len = sizeof(student_st);
    memcpy(buff1->data, stu, buff1->data_len);
    printf("buff1 address:%p,buff1->data_len address:%p,buff1->data address:%p\n",
        buff1, &(buff1->data_len), buff1->data);

    tmp = (student_st*)buff1->data;
    print_stu(tmp);

    buff_st_2 *buff2 = (buff_st_2 *)malloc(sizeof(buff_st_2));
    buff2->data_len = sizeof(student_st);
    buff2->data = (char *)malloc(buff2->data_len);
    memcpy(buff2->data, stu, buff2->data_len);
    printf("buff2 address:%p,buff2->data_len address:%p,buff2->data address:%p\n",
        buff2, &(buff2->data_len), buff2->data);

    tmp = (student_st *)buff2->data;
    print_stu(tmp);

    buff_st_3 *buff3 = (buff_st_3 *)malloc(sizeof(buff_st_3) + sizeof(student_st));
    buff3->data_len = sizeof(student_st);
    memcpy(buff3->data, stu, buff3->data_len);
    printf("buff3 address:%p,buff3->data_len address:%p,buff3->data address:%p\n",
        buff3, &(buff3->data_len), buff3->data);

    tmp = (student_st*)buff1->data;
    print_stu(tmp);

    free(buff1);

    free(buff2->data);
    free(buff2);

    free(buff3);
    free(stu);
    return 0;
}
