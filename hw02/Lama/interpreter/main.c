/* Lama SM Bytecode iterative interpreter */

#include "../runtime/gc.h"
#include "../runtime/runtime.h"
#include "../runtime/runtime_common.h"

#include <errno.h>
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

extern int Lread();
extern int Lwrite(int);
extern int Llength(void *);
extern void *Lstring(void *);
extern int LtagHash(char *);
extern int Btag(void *, int, int);
extern void *Bsta(void *, int, void *);
extern void *Belem(void *, int);
extern void *Bstring(void *);
extern int Bstring_patt(void *, void *);
extern int Barray_patt(void *, int);

void *__start_custom_data;
void *__stop_custom_data;

/* The unpacked representation of bytecode file */
typedef struct {
  char *eobf;           /* A pointer to the to the end of this bytefile   */
  char *string_ptr;     /* A pointer to the beginning of the string table */
  int *public_ptr;      /* A pointer to the beginning of publics table    */
  char *code_ptr;       /* A pointer to the bytecode itself               */
  int *global_ptr;      /* A pointer to the global area                   */
  int stringtab_size;   /* The size (in bytes) of the string table        */
  int global_area_size; /* The size (in words) of global area             */
  int public_symbols_number; /* The number of public symbols */
  char buffer[0];
} bytefile;

/* Bytefile to interpret */
bytefile *bf;

#define MEMORY_SIZE (1 << 20) // 1 Mb

int call_stack[MEMORY_SIZE];
int *call_stack_top;
int *call_stack_bottom = call_stack + MEMORY_SIZE;

int *global_area;

char *ip;
int *fp;

int cur_n_args = 0;
int cur_n_locals = 0;
bool cur_is_closure = false;

/* Gets a string from a string table by an index */
char *get_string(int pos) {
  if (pos >= bf->stringtab_size) {
    failure("ERROR: index out of bounds of string pool\n");
  }
  return &bf->string_ptr[pos];
}

/* Reads a binary bytecode file by name and unpacks it */
void read_file(char *fname) {
  FILE *f = fopen(fname, "rb");
  long size;
  bytefile *file;

  if (f == 0) {
    failure("%s\n", strerror(errno));
  }

  if (fseek(f, 0, SEEK_END) == -1) {
    failure("%s\n", strerror(errno));
  }

  int file_size = sizeof(int) * 4 + sizeof(char *) + (size = ftell(f));
  file = (bytefile *)malloc(file_size);
  file->eobf = (char *)file + file_size;

  if (file == 0) {
    failure("*** FAILURE: unable to allocate memory.\n");
  }

  rewind(f);

  if (size != fread(&file->stringtab_size, 1, size, f)) {
    failure("%s\n", strerror(errno));
  }

  fclose(f);

  file->string_ptr =
      &file->buffer[file->public_symbols_number * 2 * sizeof(int)];
  file->public_ptr = (int *)file->buffer;
  file->code_ptr = &file->string_ptr[file->stringtab_size];
  file->global_ptr = (int *)malloc(file->global_area_size * sizeof(int));

  bf = file;
}

/* Stack pointer */

extern size_t __gc_stack_top;
extern size_t __gc_stack_bottom;

int *sp() { return (int *)__gc_stack_top; }

void sp_assign(int new_sp) { *sp() = new_sp; }

void sp_add(int delta) { __gc_stack_top = (size_t)(sp() + delta); }

int sp_peek() { return *(sp() + 1); }

void sp_push(int value) {
  sp_assign(value);
  sp_add(-1);
  if ((size_t)sp() == __gc_stack_bottom - 1) {
    failure("ERROR: gc stack out of memory\n");
  }
}

void sp_push_unboxed(int value) { sp_push(BOX(value)); }

int sp_pop() {
  if ((size_t)sp() == __gc_stack_bottom - 1) {
    failure("ERROR: try to pop from empty gc stack\n");
  }
  sp_add(1);
  return *sp();
}

int sp_pop_unboxed() { return UNBOX(sp_pop()); }

/* Call stack */

void call_stack_push(int value) {
  *call_stack_top = value;
  call_stack_top -= 1;
  if (call_stack_top <= call_stack) {
    failure("ERROR: call stack out of memory\n");
  }
}

int call_stack_pop() {
  if (call_stack_top > call_stack_bottom) {
    failure("ERROR: try to pop from empty call stack\n");
  }
  call_stack_top += 1;
  return *call_stack_top;
}

/* Instruction pointer */

void ip_assign(char *new_ip) {
  if (!(bf->code_ptr <= new_ip && new_ip < bf->eobf)) {
    failure("ERROR: new ip out of bounds\n");
  }
  ip = new_ip;
}

char ip_next_byte() {
  if (ip + 1 >= bf->eobf) {
    failure("ERROR: ip out of bounds (ip + 1 >= eobf)\n");
  }
  return *ip++;
}

int ip_next_int() {
  if (ip + sizeof(int) >= bf->eobf) {
    failure("ERROR: ip out of bounds (ip + %d >= eobf)\n", sizeof(int));
  }
  return (ip += sizeof(int), *(int *)(ip - sizeof(int)));
}

/* Utils */

int *var_addr(int scope, int i) {
  if (i < 0) {
    failure("ERROR: index < 0 while var_addr(%d, %d)", scope, i);
  }

  switch (scope) {
  case 0: // global
    if ((size_t)(global_area + i) >= __gc_stack_bottom) {
      failure("ERROR: global index variable out of bounds: %d\n", i);
    }
    return global_area + i;

  case 1: // local
    if (i >= cur_n_locals) {
      failure("ERROR: local index variable out of bounds %d\n", i);
    }
    return fp - i;

  case 2: // argument
    if (i >= cur_n_args) {
      failure("ERROR: argument index variable out of bounds\n", i);
    }
    return fp + cur_n_args - i;

  case 3: // closure
    int *closure_addr = (int *)fp[cur_n_args + 1];
    data *closure = TO_DATA(closure_addr);

    int tag = TAG(closure->data_header);
    if (tag != CLOSURE_TAG) {
      failure("ERROR: invalid tag in closure argument: %d", tag);
    }

    return (int *)closure->contents + i + 1;

  default:
    failure("ERROR: unknown scope %d\n", scope);
  }
}

/* Main loop */

void run() {
#define FAIL failure("ERROR: invalid opcode %d-%d\n", h, l)

  ip = bf->code_ptr;
  do {
    char x = ip_next_byte(), h = (x & 0xF0) >> 4, l = x & 0x0F;

    switch (h) {
    case 0: // BINOP
      int b = sp_pop_unboxed();
      int a = sp_pop_unboxed();
      switch (l - 1) {
      case 0: // PLUS
        sp_push_unboxed(a + b);
        break;

      case 1: // MINUS
        sp_push_unboxed(a - b);
        break;

      case 2: // MULTIPLY
        sp_push_unboxed(a * b);
        break;

      case 3: // DIVIDE
        sp_push_unboxed(a / b);
        break;

      case 4: // MOD
        sp_push_unboxed(a % b);
        break;

      case 5: // LESS
        sp_push_unboxed(a < b);
        break;

      case 6: // LESS_EQUAL
        sp_push_unboxed(a <= b);
        break;

      case 7: // GREATER
        sp_push_unboxed(a > b);
        break;

      case 8: // GREATER_EQUAL
        sp_push_unboxed(a >= b);
        break;

      case 9: // EQUAL
        sp_push_unboxed(a == b);
        break;

      case 10: // NOT_EQUAL
        sp_push_unboxed(a != b);
        break;

      case 11: // AND
        sp_push_unboxed(a && b);
        break;

      case 12: // OR
        sp_push_unboxed(a || b);
        break;

      default:
        FAIL;
      }
      break;

    case 1: // EXT 1
      switch (l) {
      case 0: // CONST
        sp_push_unboxed(ip_next_int());
        break;

      case 1: // STRING
        sp_push((int)Bstring(get_string(ip_next_int())));
        break;

      case 2: // SEXP
        char *tag = get_string(ip_next_int());
        int n = ip_next_int();
        sexp *sexp = alloc_sexp(n);
        sexp->tag = UNBOX(LtagHash(tag));
        for (int i = n - 1; i >= 0; i--) {
          sexp->contents[i] = sp_pop();
        }
        sp_push((int)((data *)sexp)->contents);
        break;

      case 3: { // STI
        int val = sp_pop();
        int *ref = (int *)sp_pop();
        *ref = val;
        sp_push(val);
        break;
      }

      case 4: { // STA
        int value = sp_pop();
        int i = sp_pop();
        if (UNBOXED(i)) {
          Bsta((void *)value, i, (void *)sp_pop());
        } else {
          int *ref = (int *)i;
          *ref = value;
        }
        sp_push(value);
        break;
      }

      case 5: // JMP
        ip_assign(bf->code_ptr + ip_next_int());
        break;

      case 6: // END
      case 7: // RET
        int ret = sp_pop();
        sp_add(cur_n_args + cur_n_locals);
        if (call_stack_pop()) {
          sp_pop(); // closure
        }
        sp_push(ret);

        cur_n_locals = call_stack_pop();
        cur_n_args = call_stack_pop();
        fp = (int *)call_stack_pop();

        if (call_stack_top != call_stack_bottom - 1) {
          ip = (char *)call_stack_pop();
        } else {
          return;
        }

        break;

      case 8: // DROP
        sp_pop();
        break;

      case 9: // DUP
        sp_push(sp_peek());
        break;

      case 10: // SWAP
        int top1 = sp_pop();
        int top2 = sp_pop();
        sp_push(top1);
        sp_push(top2);
        break;

      case 11: // ELEM
        int i = sp_pop();
        char *a = (char *)sp_pop();
        sp_push((int)Belem(a, i));
        break;

      default:
        FAIL;
      }
      break;

    case 2: // LD
      sp_push(*var_addr(l, ip_next_int()));
      break;

    case 3: // LDA
      sp_push((int)var_addr(l, ip_next_int()));
      break;

    case 4: // ST
      *var_addr(l, ip_next_int()) = sp_peek();
      break;

    case 5: // EXT 5
      switch (l) {
      case 0: { // CJMPZ
        int next = ip_next_int();
        if (!sp_pop_unboxed()) { // condition
          ip_assign(bf->code_ptr + next);
        }
        break;
      }

      case 1: { // CJMPNZ
        int next = ip_next_int();
        if (sp_pop_unboxed()) { // condition
          ip_assign(bf->code_ptr + next);
        }
        break;
      }

      case 2:   // BEGIN
      case 3: { // CBEGIN
        int n_args = ip_next_int();
        int n_locals = ip_next_int();

        call_stack_push((int)fp);
        call_stack_push(cur_n_args);
        call_stack_push(cur_n_locals);
        call_stack_push(cur_is_closure);

        fp = sp();
        cur_n_args = n_args;
        cur_n_locals = n_locals;
        for (int i = 0; i < n_locals; i++) {
          sp_push_unboxed(0);
        }

        break;
      }

      case 4: { // CLOSURE
        char *closure_addr = (char *)ip_next_int();
        int n = ip_next_int();

        data *closure_data = (data *)alloc_closure(n + 1);
        push_extra_root((void **)&closure_data);

        ((char **)closure_data->contents)[0] = closure_addr;
        for (int i = 1; i <= n; i++) {
          char scope = ip_next_byte();
          int var_i = ip_next_int();
          int *addr = var_addr(scope, var_i);
          ((int *)closure_data->contents)[i] = *addr;
        }

        pop_extra_root((void **)&closure_data);
        sp_push((int)closure_data->contents);

        break;
      }

      case 5: { // CALLC
        int n_args = ip_next_int();
        int sp_closure_addr = sp()[n_args + 1];
        data *closure = TO_DATA(sp_closure_addr);

        int tag = TAG(closure->data_header);
        if (tag != CLOSURE_TAG) {
          failure("ERROR: invalid tag in closure argument: %d", tag);
        }

        cur_is_closure = true;
        call_stack_push((int)ip);

        int closure_addr = ((int *)closure->contents)[0];
        ip_assign(bf->code_ptr + closure_addr);

        break;
      }

      case 6: // CALL
        int func_addr = ip_next_int();
        ip_next_int(); // skip n_args

        cur_is_closure = false;
        call_stack_push((int)ip);

        ip_assign(bf->code_ptr + func_addr);

        break;

      case 7: // TAG
        void *sexp = (void *)sp_pop();

        char *tag = get_string(ip_next_int());
        int n = ip_next_int();

        sp_push(Btag(sexp, LtagHash(tag), BOX(n)));

        break;

      case 8: // ARRAY_KEY
        void *data = (void *)sp_pop();
        int cap = BOX(ip_next_int());

        sp_push(Barray_patt(data, cap));

        break;

      case 9: // FAIL
        int line = ip_next_int();
        int col = ip_next_byte();
        failure("Fail at %d:%d\n", line, col);

      case 10: // LINE
        ip_next_int();
        break;

      default:
        FAIL;
      }
      break;

    case 6: // PATTERN
      int addr = sp_pop();

      if (l != 5 /* val */ && UNBOXED(addr)) {
        sp_push_unboxed(false);
      } else {
        int tag = TAG(TO_DATA(addr)->data_header);

        switch (l) {
        case 0: // string literal
          sp_push(Bstring_patt((void *)sp_pop(), (void *)addr));
          break;

        case 1: // string
          sp_push_unboxed(tag == STRING_TAG);
          break;

        case 2: // array
          sp_push_unboxed(tag == ARRAY_TAG);
          break;

        case 3: // sexp
          sp_push_unboxed(tag == SEXP_TAG);
          break;

        case 4: // ref
          sp_push_unboxed(true);
          break;

        case 5: // val
          sp_push(BOX(UNBOXED(addr)));
          break;

        case 6: // closure
          sp_push_unboxed(tag == CLOSURE_TAG);
          break;

        default:
          failure("ERROR: unknown tag %d\n", tag);
        }
      }
      break;

    case 7: // EXTERN
      switch (l) {
      case 0: // read
        sp_push(Lread());
        break;

      case 1: // write
        sp_push(Lwrite(sp_pop()));
        break;

      case 2: // length
        sp_push(Llength((char *)sp_pop()));
        break;

      case 3: // string
        sp_push((int)Lstring((char *)sp_pop()));
        break;

      case 4: // array
        int n = ip_next_int();

        data *a = (data *)alloc_array(n);
        for (int i = n - 1; i >= 0; i--) {
          ((int *)a->contents)[i] = sp_pop();
        }

        sp_push((int)a->contents);

        break;

      default:
        FAIL;
      }
      break;

    default:
      FAIL;
    }
  } while (1);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    failure("ERROR: no bytecode file\n");
  }

  read_file(argv[1]);

  __gc_init();
  __gc_stack_bottom = (size_t)call_stack + MEMORY_SIZE;
  global_area = (int *)__gc_stack_bottom - bf->global_area_size;
  __gc_stack_top = (size_t)(global_area - 1);

  call_stack_top = (int *)call_stack_bottom - 1;

  for (int i = 0; i < bf->global_area_size; i++) {
    global_area[i] = BOX(0);
  }

  run();

  return 0;
}
