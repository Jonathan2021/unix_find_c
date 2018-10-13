#include <assert.h>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

extern "C" {
#include "../src/parse_tree.h"
}

#define MAXDEPTH 3

typedef std::vector<struct node*> nodevector;

void __attribute__((noreturn)) error(const char* msg) {
  fprintf(stderr, "error: %s\n", msg);
  exit(1);
}

int hasChildren(enum type type) {
  return type == AND || type == OR;
}

int isLeaf(enum type type) {
  return !hasChildren(type);
}

int equivalent(struct node* left, struct node* right);

int strictlyEquivalent(struct node* left, struct node* right) {
  if (left == right)
    return 1;
  if (!left || !right)
    return 0;
  if (left->barre != right->barre)
    return 0;
  if (left->is_plus != right->is_plus)
    return 0;
  if (left->type != right->type)
    return 0;
  if ((!left->arg) != (!right->arg))
    return 0;
  if (left->arg && right->arg && strcmp(left->arg, right->arg))
    return 0;

  //  All operators are commutative.
  if (equivalent(left->left, right->left) &&
      equivalent(left->right, right->right))
    return 1;

  if (equivalent(left->right, right->left) &&
      equivalent(left->left, right->right))
    return 1;

  return 0;
}

void flatten(struct node* node, nodevector& flat) {
  if (node->left->type == node->type)
    flatten(node->left, flat);
  else
    flat.push_back(node->left);

  if (node->right->type == node->type)
    flatten(node->right, flat);
  else
    flat.push_back(node->right);
}

int equivalent(struct node* left, struct node* right) {
  if (strictlyEquivalent(left, right))
    return 1;

  // Allow and ignore pointless TRUE nodes in AND.
  if (left->type == AND) {
    if (left->left->type == TRUE && equivalent(left->right, right))
      return 1;
    if (left->right->type == TRUE && equivalent(left->left, right))
      return 1;
  }
  if (right->type == AND) {
    if (right->left->type == TRUE && equivalent(right->right, left))
      return 1;
    if (right->right->type == TRUE && equivalent(right->left, left))
      return 1;
  }
  //  TODO: In theory both might also be AND with a TRUE left or right, but it
  //  doesn't happen in practice with the depths of tree we consider.

  // May need to rotate operators, eg (A AND B) AND C = A AND (B AND C).
  if (!hasChildren(left->type) || !hasChildren(right->type))
    return 0;
  if (left->type != right->type)
    return 0;

  // Flatten (A AND B) AND C into [A, B, C].
  nodevector lflat, rflat;
  flatten(left, lflat);
  flatten(right, rflat);

  // For each node in the left list find an equivalent node in the right list.
  std::set<struct node*> used;
  for (nodevector::iterator i = lflat.begin(), e = lflat.end(); i != e; ++i) {
    bool found = false;
    for (nodevector::iterator j = rflat.begin(), f = rflat.end(); j != f; ++j) {
      if (!used.count(*j)) {
        if (equivalent(*i, *j)) {
          used.insert(*j);
          found = true;
          break;
        }
      }
    }
    if (!found)
      return 0;
  }
  return 1;
}

struct node* allocateNode(void) {
  struct node* node = (struct node*) calloc(1, sizeof(struct node));
  if (!node)
    error("Memory exhausted");
  return node;
}

struct LeafGenState {
  enum type nextType;
  int nextBarre;
  int nextVariant;
  char nextIsPlus;
};

void initState(struct LeafGenState *state) {
  memset(state, 0, sizeof(struct LeafGenState));
}

int numVariants(enum type type) {
  switch (type) {
    default: error("Unexpected type");
    case   TYPE: return 2;
    case   NAME: return 4;
    case   PRINT: return 1;
    case EXECDIR:
    case    EXEC: return 1;
    case  DELETE: return 1;
    case    PERM: return 7;
    case    USER: return 2;
    case   GROUP: return 2;
    case   NEWER: return 1;
    case    TRUE: return 1;
  }
}

void setTYPEArg(const char** arg, int variant) {
  switch (variant) {
    default: error("Invalid variant");
    case  0: *arg = "f"; break;
    case  1: *arg = "d"; break;
  }
}

void setNAMEArg(const char** arg, int variant) {
  switch (variant) {
    default: error("Invalid variant");
    case  0: *arg = "-hello"; break;
    case  1: *arg = "there"; break;
    case  2: *arg = "(left"; break;
    case  3: *arg = "right)"; break;
  }
}

void setPRINTArg(const char** arg, int variant) {
  switch (variant) {
    default: error("Invalid variant");
    case  0: *arg = 0; break;
  }
}

void setEXECArg(const char** arg, int variant) {
  switch (variant) {
    default: error("Invalid variant");
    case  0: *arg = "./function arg1 arg2"; break;
  }
}

void setEXECDIRArg(const char** arg, int variant) {
  setEXECArg(arg, variant);
}

void setDELETEArg(const char** arg, int variant) {
  switch (variant) {
    default: error("Invalid variant");
    case  0: *arg = 0; break;
  }
}

void setPERMArg(const char** arg, int variant) {
  switch (variant) {
    default: error("Invalid variant");
    case  0: *arg = "664"; break;
    case  1: *arg = "ug+rw"; break;
    case  2: *arg = "-664"; break;
    case  3: *arg = "-g+w,u+w"; break;
    case  4: *arg = "/222"; break;
    case  5: *arg = "/u+w,g+w"; break;
    case  6: *arg = "/u=w,g=w"; break;
  }
}

void setUSERArg(const char** arg, int variant) {
  switch (variant) {
    default: error("Invalid variant");
    case  0: *arg = "jonathan"; break;
    case  1: *arg = "1000"; break;
  }
}

void setGROUPArg(const char** arg, int variant) {
  switch (variant) {
    default: error("Invalid variant");
    case  0: *arg = "users"; break;
    case  1: *arg = "100"; break;
  }
}

void setNEWERArg(const char** arg, int variant) {
  switch (variant) {
    default: error("Invalid variant");
    case  0: *arg = "somefile"; break;
  }
}

void setTRUEArg(const char** arg, int variant) {
  switch (variant) {
    default: error("Invalid variant");
    case  0: *arg = 0; break;
  }
}

struct node* generateLeaf(struct LeafGenState *state) {
  struct node* node;

  if (state->nextType == NOT_VALID)
    return 0; // Finished

  assert(isLeaf(state->nextType));
  assert(state->nextVariant < numVariants(state->nextType));

  node = allocateNode();
  node->barre = state->nextBarre;
  node->type = state->nextType;

  switch (node->type) {
    default:
      error("Unknown leaf type");

    case TYPE:
      setTYPEArg(&node->arg, state->nextVariant);
      break;
    case NAME:
      setNAMEArg(&node->arg, state->nextVariant);
      break;
    case PRINT:
      setPRINTArg(&node->arg, state->nextVariant);
      break;
    case EXEC:
      setEXECArg(&node->arg, state->nextVariant);
      node->is_plus = state->nextIsPlus;
      break;
    case EXECDIR:
      setEXECDIRArg(&node->arg, state->nextVariant);
      node->is_plus = state->nextIsPlus;
      break;
    case DELETE:
      setDELETEArg(&node->arg, state->nextVariant);
      break;
    case PERM:
      setPERMArg(&node->arg, state->nextVariant);
      break;
    case USER:
      setUSERArg(&node->arg, state->nextVariant);
      break;
    case GROUP:
      setGROUPArg(&node->arg, state->nextVariant);
      break;
    case NEWER:
      setNEWERArg(&node->arg, state->nextVariant);
      break;
    case TRUE:
      setTRUEArg(&node->arg, state->nextVariant);
      break;
  }

  // Advance to next state.
  ++state->nextVariant;
  if (state->nextVariant >= numVariants(state->nextType)) {
    state->nextVariant = 0;
    state->nextBarre = !state->nextBarre;
    if (!state->nextBarre) {
      if (state->nextIsPlus == ';') {
        state->nextIsPlus = '+';
      } else {
        state->nextIsPlus = 0;
        do {
          state->nextType = (enum type)(1+(int)state->nextType);
        } while (!isLeaf(state->nextType));
        if (state->nextType == EXEC || state->nextType == EXECDIR) {
          state->nextIsPlus = ';';
        }
      }
    }
  }

  return node;
}

typedef std::vector<std::string> stringvector;

void outputLeaf(struct node* node, stringvector &strings) {
  char is_plus[2];

  if (node->barre)
    strings.push_back("!");

  switch (node->type) {
    default: error("Unknown leaf type");
    case TYPE:
      strings.push_back("-type");
      strings.push_back(node->arg);
      break;
    case NAME:
      strings.push_back("-name");
      strings.push_back(node->arg);
      break;
    case PRINT:
      strings.push_back("-print");
      break;
    case EXEC:
      strings.push_back("-exec");
      strings.push_back(node->arg);
      assert(node->is_plus);
      is_plus[0] = node->is_plus;
      is_plus[1] = 0;
      strings.push_back(is_plus);
      break;
    case EXECDIR:
      strings.push_back("-execdir");
      strings.push_back(node->arg);
      assert(node->is_plus);
      is_plus[0] = node->is_plus;
      is_plus[1] = 0;
      strings.push_back(is_plus);
      break;
    case DELETE:
      strings.push_back("-delete");
      break;
    case PERM:
      strings.push_back("-perm");
      strings.push_back(node->arg);
      break;
    case USER:
      strings.push_back("-user");
      strings.push_back(node->arg);
      break;
    case GROUP:
      strings.push_back("-group");
      strings.push_back(node->arg);
      break;
    case NEWER:
      strings.push_back("-newer");
      strings.push_back(node->arg);
      break;
    case TRUE:
      strings.push_back("-true");
      break;
  }
}

void outputNode(struct node* node, stringvector &strings) {
  if (isLeaf(node->type)) {
    outputLeaf(node, strings);
    return;
  }
  switch (node->type) {
    default: error("Unexpected type");
    case AND: {
      bool leftParens = node->left->type == OR;
      bool rightParens = node->right->type == OR;
      if (leftParens)
        strings.push_back("(");
      outputNode(node->left, strings);
      if (leftParens)
        strings.push_back(")");
      if (rightParens)
        strings.push_back("(");
      outputNode(node->right, strings);
      if (rightParens)
        strings.push_back(")");
      break;
    }
    case OR: {
      outputNode(node->left, strings);
      strings.push_back("-o");
      outputNode(node->right, strings);
      break;
    }
  }
}

void printStrings(stringvector strings) {
  for (stringvector::iterator i = strings.begin(), e = strings.end();
    i != e; ++i) {
    printf("\"%s\" ", i->c_str());
  }
  printf("\n");
}

void printNode(struct node* node) {
  stringvector strings;
  outputNode(node, strings);
  printStrings(strings);
}

void generate_all(int maxdepth, nodevector& nodes) {
  if (maxdepth < 0)
    error("Negative depth");
  if (maxdepth == 0)
    return;
  if (maxdepth == 1) {
    struct LeafGenState state;
    initState(&state);
    do {
      struct node* node = generateLeaf(&state);
      if (!node)
        return;
      nodes.push_back(node);
    } while (1);
  }

  generate_all(maxdepth - 1, nodes);
  size_t len = nodes.size();
  for (size_t il = 0; il != len; ++il) {
    for (size_t ir = 0; ir != len; ++ir) {
      for (int typenum = (int)AND; typenum <= (int)OR; ++typenum) {
        struct node* op = allocateNode();
        op->type = (enum type)typenum;
        op->left = nodes[il];
        op->right = nodes[ir];
        nodes.push_back(op);
      }
    }
  }
}

stringvector args;

void dumpOnExit(int, void*) {
  printf("Input: ");
  printStrings(args);
}

int main(void) {
  on_exit(dumpOnExit, 0);

  nodevector v;
  generate_all(MAXDEPTH, v);

  for (nodevector::iterator i = v.begin(), e = v.end(); i != e; ++i) {
    struct node* orig = *i;

    outputNode(orig, args);

    const char** exp = (const char**)calloc(args.size(), sizeof(char*));
    for (size_t i = 0; i < args.size(); ++i)
      exp[i] = args[i].c_str();

    int end;
    struct node* parsed = build_tree((char**)exp, (int)args.size(), 0, &end);
    if (!equivalent(orig, parsed)) {
      printf("Parsed as: ");
      printNode(parsed);
      error("Did not match");
    }

    free_tree(parsed);
    free(exp);
    args.clear();
  }
}
