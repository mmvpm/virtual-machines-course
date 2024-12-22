/* Lama SM Bytecode verifier */

#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>

#define debug if (false) std::cout

/* Enums */
enum { SCOPE_GLOBAL, SCOPE_LOCAL, SCOPE_ARGUMENT, SCOPE_CLOSURE };
enum { BINOP, EXT_1, LD, LDA, ST, EXT_5, PATTERN, EXTERN, STOP = 15 };
enum { CONST, TSTRING, TSEXP, STI, STA, JMP, END, RET, DROP, DUP, SWAP, ELEM };
enum { EXTERN_READ, EXTERN_WRITE, EXTERN_LENGTH, EXTERN_STRING, EXTERN_ARRAY };
enum { CJMPZ, CJMPNZ, BEGIN, CBEGIN, CLOSUREOP, CALLC, CALL, TAG, ARRAY_KEY, FAILOP, LINE };
enum { PATTERN_LITERAL, PATTERN_STRING, PATTERN_ARRAY, PATTERN_SEXP, PATTERN_REF, PATTERN_VAL, PATTERN_CLOSURE, PATTERN_ITEMS };
enum { PLUS, MINUS, MULTIPLY, DIVIDE, MOD, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL, EQUAL, NOT_EQUAL, AND, OR, BINOPS_ITEMS };

/* The unpacked representation of bytecode file */
typedef struct {
  char *eobf;                /* A pointer to the to the end of this bytefile   */
  char *string_ptr;          /* A pointer to the beginning of the string table */
  int *public_ptr;           /* A pointer to the beginning of publics table    */
  char *code_ptr;            /* A pointer to the bytecode itself               */
  int *global_ptr;           /* A pointer to the global area                   */
  int stringtab_size;        /* The size (in bytes) of the string table        */
  int global_area_size;      /* The size (in words) of global area             */
  int public_symbols_number; /* The number of public symbols */
  char buffer[0];
} bytefile;

/* Bytefile to interpret */
static bytefile *bf;

/* Gets a string from a string table by an index */
static inline std::string get_string(int pos) {
  if (pos >= bf->stringtab_size) {
    throw std::runtime_error("ERROR: index out of bounds of string pool");
  }
  return std::string(&bf->string_ptr[pos]);
}

/* Instruction pointer */
static char *ip;

static inline unsigned char ip_next_byte() {
  return *ip++;
}

static inline int ip_next_int() {
  return (ip += sizeof(int), *(int *)(ip - sizeof(int)));
}

static inline int ip_int() {
  return *((int *)ip);
}

static inline std::string int_hex(int x) {
  char* buffer = new char[11];
  sprintf(buffer, "0x%08x", x);
  return std::string(std::move(buffer));
}

static inline std::string ip_next_int_str(bool hex = false) {
  if (hex) return int_hex(ip_next_int());
  return std::to_string(ip_next_int());
}

static inline std::string get_string_ip() {
  return get_string(ip_next_int());
}

static inline std::string get_string_string_ip() {
  std::string first = ip_next_int_str();
  std::string second = ip_next_int_str();
  return first + " " + second;
}

static inline std::string get_string_int_ip() {
  std::string first = get_string_ip();
  std::string second = ip_next_int_str();
  return first + " " + second;
}

/* Reads a binary bytecode file by name and unpacks it */
static void read_file(char *fname) {
  FILE *f = fopen(fname, "rb");
  long size;
  bytefile *file;

  if (f == 0) {
    throw std::runtime_error("ERROR: file is not found");
  }

  if (fseek(f, 0, SEEK_END) == -1) {
    throw std::runtime_error("ERROR: fseek failed");
  }

  int file_size = sizeof(int) * 5 + (size = ftell(f));
  file = (bytefile *)malloc(file_size);
  if (file == 0) {
    throw std::runtime_error("ERROR: unable to allocate memory");
  }
  file->eobf = (char *)file + file_size;

  rewind(f);

  if (size != fread(&file->stringtab_size, 1, size, f)) {
    throw std::runtime_error("ERROR: size != fread(&file->stringtab_size, 1, size, f)");
  }

  fclose(f);

  file->string_ptr = &file->buffer[file->public_symbols_number * 2 * sizeof(int)];
  file->public_ptr = (int *)file->buffer;
  file->code_ptr = &file->string_ptr[file->stringtab_size];

  bf = file;
}

/* Writes to binary bytefile file */
void write_file(const char *fname, const bytefile *file) {
  FILE *f = fopen(fname, "wb");
  if (f == NULL) {
    fprintf(stderr, "ERROR: unable to open file for writing\n");
    return;
  }

  int size = file->eobf - (char*)file;
  int file_size = size - sizeof(int) * 5;

  if (fwrite(&file->stringtab_size, 1, file_size, f) != file_size) {
    fprintf(stderr, "ERROR: unable to write buffer to file\n");
    fclose(f);
    return;
  }

  fclose(f);
}

/* Appends suffix to the name of the binary bytefile */
char* add_suffix(const char *filename, const char *suffix) {
    size_t basename_len = strlen(filename) - 3; // ".bc"
    size_t new_len = basename_len + strlen(suffix) + 3 + 1;
    char *new_filename = (char*) malloc(new_len);
    if (new_filename == NULL) return NULL;
    strncpy(new_filename, filename, basename_len);
    new_filename[basename_len] = '\0';
    strcat(new_filename, suffix);
    strcat(new_filename, ".bc");
    return new_filename;
}

/* Utils */

static inline std::string get_scope(int scope) {
  switch (scope) {
  case SCOPE_GLOBAL:   return "global";
  case SCOPE_LOCAL:    return "local";
  case SCOPE_ARGUMENT: return "arg";
  case SCOPE_CLOSURE:  return "cl_arg";
  default: throw std::runtime_error("ERROR: unknown scope " + std::to_string(scope));
  }
}

static inline std::string handle_closure(int addr, int n) {
  std::string arguments;
  for (int i = 1; i <= n; i++) {
    unsigned char scope = ip_next_byte();
    std::string scope_str = get_scope(scope);
    std::string var_i = ip_next_int_str();
    arguments += scope_str + var_i;
  }
  return "CLOSURE " + int_hex(addr) + " " + std::to_string(n) + " " + arguments;
}

static inline bool check_offset(int offset) {
  return 0 <= offset && offset < (bf->eobf - bf->code_ptr);
}

/* NB: call after check_offset */
static inline bool check_is_begin(int offset) {
  unsigned char x = *(bf->code_ptr + offset), h = (x & 0xF0) >> 4, l = x & 0x0F;
  return h == EXT_5 && (l == BEGIN || l == CBEGIN);
}

/* Traverses bytecode */

void debug_out(int offset, const std::string& instruction, int cur_stack_size, int delta) {
  std::string sign = delta > 0 ? "+" : "";
  debug << int_hex(offset) << " " << instruction << std::setw(50 - instruction.size() - 9)
        << " next_stack_size = " << cur_stack_size + delta << " (" << sign << delta << ")"  << std::endl;
}

const int POP = -1;
const int PUSH = 1;

static const std::array<const std::string, 7> patterns = { "=str", "#string", "#array", "#sexp", "#ref", "#val", "#fun" };
static const std::array<const std::string, 13> binops = { "+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=", "&&", "||" };

#define INVALID_OPCODE throw std::runtime_error("ERROR: invalid opcode " + std::to_string(h) + " " + std::to_string(l));

struct Node {
  int offset;
  int offset_begin;

  Node() {}
  Node(int offset, int offset_begin) : offset(offset), offset_begin(offset_begin) {}
};

static void traverse() {
  ip = bf->code_ptr;

  std::vector<bool> visited(bf->eobf - ip + 1, false);
  std::vector<int> stack_size(bf->eobf - ip + 1, -1); // stack size before this instruction (offset)
  std::vector<int> max_stack_size(bf->eobf - ip + 1, 0);

  // offsets stack
  std::vector<Node> cur_offset_stack;
  auto push_to_offset_stack = [&](size_t offset, int offset_begin) {
    if (!visited[offset]) {
      visited[offset] = true;
      cur_offset_stack.emplace_back(offset, offset_begin);
    }
  };

  // collect initial offsets
  for (int i = 0; i < bf->public_symbols_number; i++) {
    if ((char*)(bf->public_ptr) + i * 2 + 1 > bf->eobf) {
      throw std::runtime_error("ERROR: public symbol #" + std::to_string(i) + " refers to beyond the bytefile\n");
    }
    int offset_begin = bf->public_ptr[i * 2 + 1];
    stack_size[offset_begin] = 0;
    push_to_offset_stack(offset_begin, offset_begin);
  }

  // instruction processing function
  auto process = [&](const Node& node, const std::string& instruction, int& cur_stack_size, int delta) {
    debug_out(node.offset, instruction, cur_stack_size, delta);
    cur_stack_size += delta;
    if (cur_stack_size > max_stack_size[node.offset_begin]) {
      max_stack_size[node.offset_begin] = cur_stack_size;
    }
    if (cur_stack_size < 0) {
      throw std::runtime_error("ERROR: stack size is out of bounds: " + std::to_string(cur_stack_size) + "\n");
    }
  };

  // instruction processing loop
  while (!cur_offset_stack.empty()) {
    Node node = cur_offset_stack.back();
    int offset = node.offset;
    int cur_stack_size = stack_size[offset];
    ip = bf->code_ptr + offset;
    cur_offset_stack.pop_back();

    unsigned char x = ip_next_byte(), h = (x & 0xF0) >> 4, l = x & 0x0F;

    std::vector<int> next_offsets;

    std::string name;
    bool is_end = false;
    bool is_call = false;
    bool is_one_way = false;
    switch (h) {
    case BINOP:
      if (l - 1 >= BINOPS_ITEMS) INVALID_OPCODE
      name = "BINOP " + binops[l - 1];
      process(node, name, cur_stack_size, 2 * POP);
      process(node, name, cur_stack_size, PUSH);
      break;

    case EXT_1:
      switch (l) {
      case CONST:
        name = "CONST " + ip_next_int_str();
        process(node, name, cur_stack_size, PUSH);
        break;

      case TSTRING:
        name = "STRING " + get_string_ip();
        process(node, name, cur_stack_size, PUSH);
        break;

      case TSEXP: {
        std::string tag = get_string(ip_next_int());
        int n = ip_next_int();
        name = "SEXP " + tag + " " + std::to_string(n);
        process(node, name, cur_stack_size, n * POP);
        process(node, name, cur_stack_size, PUSH);
        break;
      }

      case STI:
        name = "STI";
        process(node, name, cur_stack_size, 2 * POP);
        process(node, name, cur_stack_size, PUSH);
        break;

      case STA:
        name = "STA";
        process(node, name, cur_stack_size, 3 * POP);
        process(node, name, cur_stack_size, PUSH);
        break;

      case JMP: {
        int jmp_offset = ip_int();
        if (!check_offset(jmp_offset)) {
          throw std::runtime_error("ERROR: JMP to " + int_hex(jmp_offset) + " > end of bytefile");
        }
        is_one_way = true;
        next_offsets.push_back(jmp_offset); // only one way
        name = "JMP " + ip_next_int_str(true);
        process(node, name, cur_stack_size, 0);
        break;
      }

      case END:
        is_end = true;
        name = "END";
        process(node, name, cur_stack_size, 0);
        break;

      case RET:
        is_end = true;
        name = "RET";
        process(node, name, cur_stack_size, 0);
        break;

      case DROP:
        name = "DROP";
        process(node, name, cur_stack_size, POP);
        break;

      case DUP:
        name = "DUP";
        process(node, name, cur_stack_size, PUSH); 
        break;

      case SWAP:
        name = "SWAP";
        process(node, name, cur_stack_size, 2 * POP);
        process(node, name, cur_stack_size, 2 * PUSH);
        break;

      case ELEM:
        name = "ELEM";
        process(node, name, cur_stack_size, 2 * POP);
        process(node, name, cur_stack_size, PUSH);
        break;

      default:
        INVALID_OPCODE
      }
      break;

    case LD:
      name = "LD " + get_scope(l) + " " + ip_next_int_str();
      process(node, name, cur_stack_size, PUSH);
      break;

    case LDA:
      name = "LDA " + get_scope(l) + " " + ip_next_int_str();
      process(node, name, cur_stack_size, 2 * PUSH);
      break;

    case ST:
      name = "ST " + get_scope(l) + " " + ip_next_int_str();
      process(node, name, cur_stack_size, 0);
      break;

    case EXT_5:
      switch (l) {
      case CJMPZ: {
        int jmp_offset = ip_int();
        if (!check_offset(jmp_offset)) {
          throw std::runtime_error("ERROR: CJMPZ to " + int_hex(jmp_offset) + " > end of bytefile");
        }
        next_offsets.push_back(jmp_offset); // one way of two
        name = "CJMPZ " + ip_next_int_str(true);
        process(node, name, cur_stack_size, POP);
        break;
      }

      case CJMPNZ: {
        int jmp_offset = ip_int();
        if (!check_offset(jmp_offset)) {
          throw std::runtime_error("ERROR: CJMPNZ to " + int_hex(jmp_offset) + " > end of bytefile");
        }
        next_offsets.push_back(jmp_offset); // one way of two
        name = "CJMPNZ " + ip_next_int_str(true);
        process(node, name, cur_stack_size, POP);
        break;
      }

      case BEGIN: {
        int n_args = ip_next_int();
        int n_locals = ip_next_int();
        name = "BEGIN " + std::to_string(n_args) + " " + std::to_string(n_locals);
        debug_out(offset, name, cur_stack_size, 0);
        break;
      }

      case CBEGIN: {
        int n_args = ip_next_int();
        int n_locals = ip_next_int();
        name = "CBEGIN " + std::to_string(n_args) + " " + std::to_string(n_locals);
        debug_out(offset, name, cur_stack_size, 0);
        break;
      }

      case CLOSUREOP: {
        int offset = ip_next_int();
        if (!check_offset(offset)) {
          throw std::runtime_error("ERROR: CLOSUREOP offset " + int_hex(offset) + " > end of bytefile");
        }
        if (!check_is_begin(offset)) {
          throw std::runtime_error("ERROR: CLOSUREOP offset " + int_hex(offset) + " is not (C)BEGIN");
        }
        int n = ip_next_int();
        name = handle_closure(offset, n);
        process(node, name, cur_stack_size, PUSH); 
        break;
      }

      case CALLC: {
        int n_args = ip_int();
        name = "CALLC " + ip_next_int_str();
        process(node, name, cur_stack_size, n_args * POP);
        break;
      }
      case CALL: {
        is_call = true;
        int func_offset = ip_next_int();
        if (!check_is_begin(func_offset)) {
          throw std::runtime_error("ERROR: CALL func_offset " + int_hex(func_offset) + " is not (C)BEGIN");
        }
        int n_args = ip_next_int();
        next_offsets.push_back(func_offset); // one way of two
        name = "CALL " + int_hex(func_offset) + " " + std::to_string(n_args);
        process(node, name, cur_stack_size, n_args * POP);
        process(node, name, cur_stack_size, PUSH); // ret
        break;
      }

      case TAG:
        name = "TAG " + get_string_int_ip();
        process(node, name, cur_stack_size, POP);
        process(node, name, cur_stack_size, PUSH);
        break;

      case ARRAY_KEY:
        name = "ARRAY " + ip_next_int_str();
        process(node, name, cur_stack_size, POP);
        process(node, name, cur_stack_size, PUSH);
        break;

      case FAILOP:
        is_end = true;
        name = "FAIL " + get_string_string_ip();
        process(node, name, cur_stack_size, 0);
        break;

      case LINE:
        name = "LINE " + ip_next_int_str();
        process(node, name, cur_stack_size, 0);
        break;

      default:
        INVALID_OPCODE
      }
      break;

    case PATTERN:
      if (l == PATTERN_ITEMS) {
        throw std::runtime_error("ERROR: unknown tag: " + std::to_string(l));
      }
      name = "PATTERN " + patterns[l];
      process(node, name, cur_stack_size, 2 * POP);
      process(node, name, cur_stack_size, PUSH);
      if (l != PATTERN_LITERAL) {
        process(node, name, cur_stack_size, PUSH);
      }
      break;

    case EXTERN:
      switch (l) {
      case EXTERN_READ:
        name = "CALL Lread";
        process(node, name, cur_stack_size, PUSH);
        break;

      case EXTERN_WRITE:
        name = "CALL Lwrite";
        process(node, name, cur_stack_size, POP);
        process(node, name, cur_stack_size, PUSH);
        break;

      case EXTERN_LENGTH:
        name = "CALL Llength";
        process(node, name, cur_stack_size, POP);
        process(node, name, cur_stack_size, PUSH);
        break;

      case EXTERN_STRING:
        name = "CALL Lstring";
        process(node, name, cur_stack_size, POP);
        process(node, name, cur_stack_size, PUSH);
        break;

      case EXTERN_ARRAY: {
        int n = ip_next_int();
        name = "CALL Larray " + std::to_string(n);
        process(node, name, cur_stack_size, n * POP);
        process(node, name, cur_stack_size, PUSH);
        break;
      }

      default:
        INVALID_OPCODE
      }
      break;

    case STOP:
      is_end = true;
      name = "STOP";
      process(node, name, cur_stack_size, 0);
      break;

    default:
      INVALID_OPCODE
    }

    if (is_end) {
      int* begin_h_ptr = (int*)(bf->code_ptr + node.offset_begin + 1);
      *begin_h_ptr = ((*begin_h_ptr) & 0x0000FFFF) + (max_stack_size[node.offset_begin] << 16);
      debug << "Required stack size for " << int_hex(node.offset_begin) << " = " << max_stack_size[node.offset_begin] << std::endl;
      continue;
    }

    if (is_call) {
      if (next_offsets.size() == 0) {
        throw std::runtime_error("ERROR: next offset is empty for CALL(C) at " + int_hex(offset));
      }
      int next_offset = next_offsets[0];
      stack_size[next_offset] = 0;
      push_to_offset_stack(next_offset, next_offset); // new code block
      next_offsets.pop_back();
    }
    
    if (!is_one_way) {
      next_offsets.push_back(ip - bf->code_ptr); // next offset
    }

    for (int next_offset : next_offsets) {
      push_to_offset_stack(next_offset, node.offset_begin); // current code block
      
      if (stack_size[next_offset] == -1 || stack_size[next_offset] == cur_stack_size) {
        stack_size[next_offset] = cur_stack_size;
        debug << "Stack size at " << int_hex(offset) << " (from " << int_hex(next_offset) << ") = " << cur_stack_size << std::endl;
      } else {
        throw std::runtime_error(
          "ERROR: stack sizes at " + int_hex(next_offset) + " (from " + int_hex(offset) +  ") do not match: " +
          "old = " + std::to_string(stack_size[next_offset]) + ", " +
          "new = " + std::to_string(cur_stack_size) + "\n"
        );
      }
    }
  }
}

/* Main loop */

int main(int argc, char *argv[]) {
  if (argc < 2) {
    throw std::runtime_error("ERROR: no bytecode file\n");
  }
  char* filename = argv[1];
  char *new_filename = add_suffix(filename, "_verified");
  
  read_file(filename);
  traverse();
  write_file(new_filename, bf);
  
  return 0;
}
