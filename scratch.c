#include <assert.h>
#include <stdio.h>
#include <string.h>

#define register_type(T) static constexpr T* __type_id_##T;
#define type_id(T) #T

static constexpr char type_str[] = type_id(int);
static constexpr unsigned long type_str_len = sizeof(type_str) - 1;

int main(void) {
  static_assert(__builtin_strcmp(type_id(long), type_id(int)) == 0, "types do not match\n");
  printf("%s\n", type_id(int));
  printf("%lu\n", type_str_len);
  return 0;
}
