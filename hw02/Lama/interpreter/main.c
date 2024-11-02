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

#define MEMORY_SIZE (1 << 20)

static int call_stack[MEMORY_SIZE]; // 4 Mb
static int *call_stack_top;
static int *call_stack_bottom = call_stack + MEMORY_SIZE;

static int *global_area;

static char *ip;
static int *fp;

static int cur_n_args = 0;
static int cur_n_locals = 0;
static bool cur_is_closure = false;

/* Enums */

enum { SCOPE_GLOBAL, SCOPE_LOCAL, SCOPE_ARGUMENT, SCOPE_CLOSURE };
enum { BINOP, EXT_1, LD, LDA, ST, EXT_5, PATTERN, EXTERN };
enum { CONST, TSTRING, TSEXP, STI, STA, JMP, END, RET, DROP, DUP, SWAP, ELEM };
enum { CJMPZ, CJMPNZ, BEGIN, CBEGIN, CLOSUREOP, CALLC, CALL, TAG, ARRAY_KEY, FAILOP, LINE };
enum { PLUS, MINUS, MULTIPLY, DIVIDE, MOD, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL, EQUAL, NOT_EQUAL, AND, OR };

/* Gets a string from a string table by an index */
static char *get_string(int pos) {
  if (pos >= bf->stringtab_size) {
    failure("ERROR: index out of bounds of string pool\n");
  }
  return &bf->string_ptr[pos];
}

/* Reads a binary bytecode file by name and unpacks it */
static void read_file(char *fname) {
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
  if (file == 0) {
    failure("*** FAILURE: unable to allocate memory.\n");
  }
  file->eobf = (char *)file + file_size;

  rewind(f);

  if (size != fread(&file->stringtab_size, 1, size, f)) {
    failure("%s\n", strerror(errno));
  }

  fclose(f);

  file->string_ptr =
      &file->buffer[file->public_symbols_number * 2 * sizeof(int)];
  file->public_ptr = (int *)file->buffer;
  file->code_ptr = &file->string_ptr[file->stringtab_size];

  bf = file;
}

/* Stack pointer */

extern int *__gc_stack_top;
extern int *__gc_stack_bottom;

static inline int *sp() { return __gc_stack_top; }

static inline void sp_assign(int new_sp) { *sp() = new_sp; }

static inline void sp_add(int delta) { __gc_stack_top = sp() + delta; }

static inline int sp_peek() { return *(sp() + 1); }

static inline void sp_push(int value) {
  sp_assign(value);
  sp_add(-1);
  if (sp() == call_stack) {
    failure("ERROR: gc stack out of memory\n");
  }
}

static inline void sp_push_unboxed(int value) { sp_push(BOX(value)); }

static inline int sp_pop() {
  if (sp() == global_area - 1) {
    failure("ERROR: try to pop from empty gc stack\n");
  }
  sp_add(1);
  return *sp();
}

static inline int sp_pop_unboxed() { return UNBOX(sp_pop()); }

/* Call stack */

static inline void call_stack_push(int value) {
  *call_stack_top = value;
  call_stack_top -= 1;
  if (call_stack_top <= call_stack) {
    failure("ERROR: call stack out of memory\n");
  }
}

static inline int call_stack_pop() {
  if (call_stack_top >= call_stack_bottom - 1) {
    failure("ERROR: try to pop from empty call stack\n");
  }
  call_stack_top += 1;
  return *call_stack_top;
}

/* Instruction pointer */

static inline void ip_assign(char *new_ip) {
  if (!(bf->code_ptr <= new_ip && new_ip < bf->eobf)) {
    failure("ERROR: new ip out of bounds\n");
  }
  ip = new_ip;
}

static inline unsigned char ip_next_byte() {
  if (ip + 1 >= bf->eobf) {
    failure("ERROR: ip out of bounds (ip + 1 >= eobf)\n");
  }
  return *ip++;
}

static inline int ip_next_int() {
  if (ip + sizeof(int) >= bf->eobf) {
    failure("ERROR: ip out of bounds (ip + %d >= eobf)\n", sizeof(int));
  }
  return (ip += sizeof(int), *(int *)(ip - sizeof(int)));
}

/* Utils */

static int *var_addr(int scope, int i) {
  if (i < 0) {
    failure("ERROR: index < 0 while var_addr(%d, %d)", scope, i);
  }

  switch (scope) {
  case SCOPE_GLOBAL:
    if (global_area + i >= __gc_stack_bottom) {
      failure("ERROR: global index variable out of bounds: %d\n", i);
    }
    return global_area + i;

  case SCOPE_LOCAL:
    if (i >= cur_n_locals) {
      failure("ERROR: local index variable out of bounds %d\n", i);
    }
    return fp - i;

  case SCOPE_ARGUMENT:
    if (i >= cur_n_args) {
      failure("ERROR: argument index variable out of bounds\n", i);
    }
    return fp + cur_n_args - i;

  case SCOPE_CLOSURE:
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

static void run() {
#define FAIL failure("ERROR: invalid opcode %d-%d\n", h, l)
#define CASE_APPLY_BINOP(name, op) \
        case name: \
          sp_push_unboxed(a op b); \
          break;

  ip = bf->code_ptr;
  do {
    unsigned char x = ip_next_byte(), h = (x & 0xF0) >> 4, l = x & 0x0F;

    switch (h) {
    case BINOP:
      int b = sp_pop_unboxed();
      int a = sp_pop_unboxed();
      switch (l - 1) {
      CASE_APPLY_BINOP(PLUS, +)
      CASE_APPLY_BINOP(MINUS, -)
      CASE_APPLY_BINOP(MULTIPLY, *)
      CASE_APPLY_BINOP(DIVIDE, /)
      CASE_APPLY_BINOP(MOD, %)
      CASE_APPLY_BINOP(LESS, <)
      CASE_APPLY_BINOP(LESS_EQUAL, <=)
      CASE_APPLY_BINOP(GREATER, >)
      CASE_APPLY_BINOP(GREATER_EQUAL, >=)
      CASE_APPLY_BINOP(EQUAL, ==)
      CASE_APPLY_BINOP(NOT_EQUAL, !=)
      CASE_APPLY_BINOP(AND, &&)
      CASE_APPLY_BINOP(OR, ||)
      default:
        FAIL;
      }
      break;

    case EXT_1:
      switch (l) {
      case CONST:
        sp_push_unboxed(ip_next_int());
        break;

      case TSTRING:
        sp_push((int)Bstring(get_string(ip_next_int())));
        break;

      case TSEXP:
        char *tag = get_string(ip_next_int());
        int n = ip_next_int();
        sexp *sexp = alloc_sexp(n);
        sexp->tag = UNBOX(LtagHash(tag));
        for (int i = n - 1; i >= 0; i--) {
          sexp->contents[i] = sp_pop();
        }
        sp_push((int)((data *)sexp)->contents);
        break;

      case STI: {
        int val = sp_pop();
        int *ref = (int *)sp_pop();
        *ref = val;
        sp_push(val);
        break;
      }

      case STA: {
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

      case JMP:
        ip_assign(bf->code_ptr + ip_next_int());
        break;

      case END:
      case RET:
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

      case DROP:
        sp_pop();
        break;

      case DUP:
        sp_push(sp_peek());
        break;

      case SWAP:
        int top1 = sp_pop();
        int top2 = sp_pop();
        sp_push(top1);
        sp_push(top2);
        break;

      case ELEM:
        int i = sp_pop();
        char *a = (char *)sp_pop();
        sp_push((int)Belem(a, i));
        break;

      default:
        FAIL;
      }
      break;

    case LD:
      sp_push(*var_addr(l, ip_next_int()));
      break;

    case LDA:
      sp_push((int)var_addr(l, ip_next_int()));
      break;

    case ST:
      *var_addr(l, ip_next_int()) = sp_peek();
      break;

    case EXT_5:
      switch (l) {
      case CJMPZ: {
        int next = ip_next_int();
        if (!sp_pop_unboxed()) { // condition
          ip_assign(bf->code_ptr + next);
        }
        break;
      }

      case CJMPNZ: {
        int next = ip_next_int();
        if (sp_pop_unboxed()) { // condition
          ip_assign(bf->code_ptr + next);
        }
        break;
      }

      case BEGIN:  
      case CBEGIN: {
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

      case CLOSUREOP: {
        char *closure_addr = (char *)ip_next_int();
        int n = ip_next_int();

        data *closure_data = (data *)alloc_closure(n + 1);
        push_extra_root((void **)&closure_data);

        ((char **)closure_data->contents)[0] = closure_addr;
        for (int i = 1; i <= n; i++) {
          unsigned char scope = ip_next_byte();
          int var_i = ip_next_int();
          int *addr = var_addr(scope, var_i);
          ((int *)closure_data->contents)[i] = *addr;
        }

        pop_extra_root((void **)&closure_data);
        sp_push((int)closure_data->contents);

        break;
      }

      case CALLC: {
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

      case CALL:
        int func_addr = ip_next_int();
        ip_next_int(); // skip n_args

        cur_is_closure = false;
        call_stack_push((int)ip);

        ip_assign(bf->code_ptr + func_addr);

        break;

      case TAG:
        void *sexp = (void *)sp_pop();

        char *tag = get_string(ip_next_int());
        int n = ip_next_int();

        sp_push(Btag(sexp, LtagHash(tag), BOX(n)));

        break;

      case ARRAY_KEY:
        void *data = (void *)sp_pop();
        int cap = BOX(ip_next_int());

        sp_push(Barray_patt(data, cap));

        break;

      case FAILOP:
        int line = ip_next_int();
        int col = ip_next_byte();
        failure("Fail at %d:%d\n", line, col);

      case LINE:
        ip_next_int();
        break;

      default:
        FAIL;
      }
      break;

    case PATTERN:
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

    case EXTERN:
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

  // Init memory layout:
  //
  //     <--                MEMORY_SIZE                  -->
  //     [ - - - - - - - - - - - - - - - - - - - - - - - - ]
  //     ^                        ^ ^                     ^ ^
  //     |                       /   \                   /  |
  // call_stack    __gc_stack_top     global_area       /  __gc_stack_bottom
  //                                      call_stack_top   call_stack_bottom

  __gc_init();
  __gc_stack_bottom = (int *)((size_t)call_stack + MEMORY_SIZE);
  global_area = __gc_stack_bottom - bf->global_area_size;
  __gc_stack_top = global_area - 1;

  call_stack_top = call_stack_bottom - 1;

  for (int i = 0; i < bf->global_area_size; i++) {
    global_area[i] = BOX(0);
  }

  run();

  return 0;
}
