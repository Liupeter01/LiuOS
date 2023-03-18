#include"stdlib.h"

char *itoa(int val, char *buf, unsigned radix)
{
    char   *p;             
    char   *firstdig;      
    char   temp;           
    unsigned   digval;     
    p = buf;
    if(radix == 16){        //radix为16进制
        //*(p++) = '0';
        //*(p++) = 'x';
    }
    if(val <0)
    {
        *p++ = '-';
        val = (unsigned long)(-(long)val);
    }
    firstdig = p; 
    do{
        digval = (unsigned)(val % radix);
        val /= radix;
       
        if  (digval > 9)
            *p++ = (char)(digval - 10 + 'a'); 
        else
            *p++ = (char)(digval + '0');      
    }while(val > 0);
   
    *p-- = '\0';         
    do{
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;
        --p;
        ++firstdig;        
    }while(firstdig < p);  
    return buf;
}

int 
atoi(const char *arr)
{
    if(arr == (void*)0)
    {
        //非法输入
        return -1;
    }   
    int index = 0;//记录数组下标
    int num = 0;//存放最终要返回的值
    int flag = 1;//设置符号标志位
        
    while(arr[index] ==' ')//是空格返回非零值(不一定是1)，否则返回0
    {
        index++;//跳过空格字符,不只有空格，还有\n，\r,\f,\t,\v之类的字符
    }   
    if(arr[index] == '-')
    {
        flag = -1; //除空格外遇到的第一个数字是符号，标记为-1
    }   
    //注意：正负号不算作1
    if(arr[index] == '-' || arr[index] == '+')//如果遇到正负号，说明可以往后继续走
    {
        index++;
    }
    while(arr[index] >= '0' && arr[index] <= '9')//如果遇到的数字说明可以继续往下走
    {
        num = num*10 + arr[index] - '0';
        index++;
    }
    return flag*num;
}