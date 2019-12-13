#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if (argc == 1) exit();
  printf(1, "readcount: %d\n", getreadcount(atoi(argv[1])));
  exit();
}
