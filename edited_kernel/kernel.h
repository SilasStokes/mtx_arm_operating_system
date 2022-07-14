
#ifndef KERNEL
#define KERNEL

#include "../type.h"


int bcopy(const void *src, void *dest, unsigned int n) ;
int bzero(void *dest, unsigned int n);
char *memcpy(void *dest, const void *src, unsigned int n) ;
char *memset(void *dest, int c, unsigned int n);
int memcmp(const void *s1, const void *s2, unsigned int n) ;
char *kmemcpy(void *dest, const void *src, unsigned int n) ;
char *kmemset(void *dest, int c, unsigned int n) ;
int kmemcmp(const void *s1, const void *s2, unsigned int n) ;
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
int strlen(const char *s);
int strcat(char *s1, char *s2);
int strncpy(char *s1, char *s2, int n);
int strncmp(char *s1, char *s2, int n);
int kstrcmp(const char *s1, const char *s2);
char *kstrcpy(char *dest, const char *src);
int kstrlen(const char *s) ;
int kstrcat(char *s1, char *s2);
int kstrncpy(char *s1, char *s2, int n);
int kstrncmp(char *s1, char *s2, int n);
/******************
int strcmp(const char *s1, const char *s2)
{
  while ((*s1 && *s2) && (*s1==*s2)){
      s1++; s2++;
  }
  if (*s1==0 && *s2==0)
    return 0;
  return *s1-*s2;
}

char *strcpy(char *dest, const char *src) {
	char *p = dest;
	while ( (*dest++ = *src++))
		;
	*dest = 0;
	return p;
}


int strlen(const char *s) {
	unsigned int n = 0;
	while (*s++)
		++n;
	return n;
}

int strcat(char *s1, char *s2)
{
  while(*s1)
    s1++;
  while(*s2)
    *s1++ = *s2++;
  *s1 = 0;
}

int strncpy(char *s1, char *s2, int n)
{
  char *p = s1;
  while(n && *s2){
    *s1++ = *s2++;
    n--;
  }
  *s1 = 0;
  return (int)p;
}

int strncmp(char *s1, char *s2, int n)
{
  if (n==0) return 0;
  do{
    if (*s1 != *s2++)
      return *s1-*(s2-1);
    if (*s1++ == 0)
      break;
  } while(--n != 0);
  return 0;
}
******************/
char *strstr(char *s1, char *s2);
int setzero(char *dst, int size);
void delay();
int copy(char *dest, char *src);
int atoi(char *s);
int geti();


// load.c
int search(INODE *ip, char *name);

int load(char *filename, PROC *p);

#endif