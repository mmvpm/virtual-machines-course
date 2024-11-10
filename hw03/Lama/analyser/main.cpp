/* Lama SM Bytecode static analyzer */

#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

/* Enums */
enum { SCOPE_GLOBAL, SCOPE_LOCAL, SCOPE_ARGUMENT, SCOPE_CLOSURE };
enum { BINOP, EXT_1, LD, LDA, ST, EXT_5, PATTERN, EXTERN, STOP = 15 };
enum { CONST, TSTRING, TSEXP, STI, STA, JMP, END, RET, DROP, DUP, SWAP, ELEM };
enum { EXTERN_READ, EXTERN_WRITE, EXTERN_LENGTH, EXTERN_STRING, EXTERN_ARRAY };
enum { CJMPZ, CJMPNZ, BEGIN, CBEGIN, CLOSUREOP, CALLC, CALL, TAG, ARRAY_KEY, FAILOP, LINE };
enum { PATTERN_LITERAL, PATTERN_STRING, PATTERN_ARRAY, PATTERN_SEXP, PATTERN_REF, PATTERN_VAL, PATTERN_CLOSURE };
enum { PLUS, MINUS, MULTIPLY, DIVIDE, MOD, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL, EQUAL, NOT_EQUAL, AND, OR };

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
bytefile *bf;

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
  if (ip + 1 > bf->eobf) {
    throw std::runtime_error("ERROR: ip out of bounds (ip + 1 >= eobf)");
  }
  return *ip++;
}

static inline int ip_next_int() {
  if (ip + sizeof(int) >= bf->eobf) {
    throw std::runtime_error("ERROR: ip out of bounds (ip + sizeof(int) >= eobf)");
  }
  return (ip += sizeof(int), *(int *)(ip - sizeof(int)));
}

static inline std::string ip_next_int_str() {
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

  int file_size = sizeof(int) * 4 + sizeof(char *) + (size = ftell(f));
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

  file->string_ptr =
      &file->buffer[file->public_symbols_number * 2 * sizeof(int)];
  file->public_ptr = (int *)file->buffer;
  file->code_ptr = &file->string_ptr[file->stringtab_size];

  bf = file;
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

static inline std::string handle_closure() {
  std::string arguments;
  int addr = ip_next_int();
  int n = ip_next_int();
  for (int i = 1; i <= n; i++) {
    unsigned char scope = ip_next_byte();
    std::string scope_str = get_scope(scope);
    std::string var_i = ip_next_int_str();
    arguments += scope_str + var_i;
  }
  return "CLOSURE " + std::to_string(addr) + " " + ip_next_int_str() + " " + arguments;
}

/* Moves ip to the next instruction and decodes it */

const std::vector<std::string> patterns = { "=str", "#string", "#array", "#sexp", "#ref", "#val", "#fun" };
const std::vector<std::string> binops = { "+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=", "&&", "||" };

#define INVALID_OPCODE throw std::runtime_error("ERROR: invalid opcode " + std::to_string(h) + " " + std::to_string(l));

static bool move_next(const std::function<void(const std::string&)>& process) {
  unsigned char x = ip_next_byte(), h = (x & 0xF0) >> 4, l = x & 0x0F;

  switch (h) {
  case STOP:    return false;
  case BINOP:   process("BINOP " + binops[l - 1]); break;
  case LD:      process("LD " + get_scope(l) + " " + ip_next_int_str()); break;
  case LDA:     process("LDA " + get_scope(l) + " " + ip_next_int_str()); break;
  case ST:      process("ST " + get_scope(l) + " " + ip_next_int_str()); break;
  case PATTERN: process("PATTERN " + patterns[l]); break;

  case EXT_1:
    switch (l) {
    case CONST:   process("CONST " + ip_next_int_str()); break;
    case TSTRING: process("STRING " + get_string_ip()); break;
    case TSEXP:   process("SEXP " + get_string_int_ip()); break;
    case STI:     process("STI"); break;
    case STA:     process("STA"); break;
    case JMP:     process("JMP " + ip_next_int_str()); break;
    case END:     process("END"); break;
    case RET:     process("RET"); break;
    case DROP:    process("DROP"); break;
    case DUP:     process("DUP"); break;
    case SWAP:    process("SWAP"); break;
    case ELEM:    process("ELEM"); break;
    default:      INVALID_OPCODE;
    }
    break;

  case EXT_5:
    switch (l) {
    case CJMPZ:     process("CJMPZ " + ip_next_int_str()); break;
    case CJMPNZ:    process("CJMPNZ " + ip_next_int_str()); break;
    case BEGIN:     process("BEGIN " + get_string_string_ip()); break;
    case CBEGIN:    process("CBEGIN " + get_string_string_ip()); break;
    case CLOSUREOP: process(handle_closure()); break;
    case CALLC:     process("CALLC " + ip_next_int_str()); break;
    case CALL:      process("CALL " + get_string_string_ip()); break;
    case TAG:       process("TAG " + get_string_int_ip()); break;
    case ARRAY_KEY: process("ARRAY " + ip_next_int_str()); break;
    case FAILOP:    process("FAIL " + get_string_string_ip()); break;
    case LINE:      process("LINE " + ip_next_int_str()); break;
    default:        INVALID_OPCODE;
    }
    break;

  case EXTERN:
    switch (l) {
    case EXTERN_READ:   process("CALL Lread"); break;
    case EXTERN_WRITE:  process("CALL Lwrite"); break;
    case EXTERN_LENGTH: process("CALL Llength"); break;
    case EXTERN_STRING: process("CALL Lstring"); break;
    case EXTERN_ARRAY:  process("CALL Larray " + ip_next_int_str()); break;
    default:            INVALID_OPCODE;
    }
    break;

  default: INVALID_OPCODE;
  }
  return true;
}

/* Hashing bytes */

struct ByteRange {
  const char* start;
  const char* end;

  bool operator==(const ByteRange& other) const {
    return (end - start == other.end - other.start) && std::memcmp(start, other.start, end - start) == 0;
  }
};

namespace std {
  template<> struct hash<ByteRange> {
    size_t operator()(const ByteRange& range) const {
      size_t hash = 0;
      for (const char* ptr = range.start; ptr != range.end; ++ptr) {
        hash = (hash * 31) ^ (size_t)(*ptr);
      }
      return hash;
    }
  };
}

/* Main loop */

int main(int argc, char *argv[]) {
  if (argc < 2) {
    throw std::runtime_error("ERROR: no bytecode file\n");
  }
  read_file(argv[1]);
  ip = bf->code_ptr;

  char* prev_ip = nullptr;
  std::unordered_map<ByteRange, int> freq;

  auto noop = [](const std::string& decoded) {};
  auto print = [](const std::string& decoded) { std::cout << decoded; };

  while (true) {
    char* current_ip = ip;
    if (!move_next(noop)) break;

    freq[ByteRange{current_ip, ip}]++;
    if (prev_ip) freq[ByteRange{prev_ip, ip}]++;

    prev_ip = current_ip;
  }

  std::vector<std::pair<ByteRange, int>> sorted_freq(freq.begin(), freq.end());
  std::sort(sorted_freq.begin(), sorted_freq.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

  for (const auto& [range, frequency] : sorted_freq) {
    std::cout << frequency << " times: ";
    ip = (char *)range.start;
    while (ip < range.end) {
      move_next(print);
      std::cout << "; ";
    }
    std::cout << std::endl;
  }

  return 0;
}