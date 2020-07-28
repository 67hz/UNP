#include <stdio.h>


int
main(int argc, char **argv)
{
  union {
    short s;
    char c[sizeof(short)];
  } un;

  union {
    int i;
    char c[sizeof(int)];
  } un_int;

  un.s = 0x0102;
  un_int.i = 0x01020304;

  if (sizeof(short) == 2) {
    if (un.c[0] == 1 && un.c[1] == 2)
      printf("big endian\n");
    else if (un.c[0] == 2 && un.c[1] == 1)
      printf("little endian\n");
    else
      printf("unknown\n");
  } else {
    printf("sizeof(short) = %lu\n", sizeof(short));
  }

}
