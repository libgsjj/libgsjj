/*------------------------------------------------------------------------*/
/* (C)2002-2003, Armin Biere, ETH Zurich, 2005, 2012, Armin Biere, JKU Linz.

All rights reserved. Redistribution and use in source and binary forms, with
or without modification, are permitted provided that the following
conditions are met:

  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  3. All advertising materials mentioning features or use of this software
     must display the following acknowledgement:

    This product includes software developed by Armin Biere, ETH Zurich,
    and JKU Linz.

  4. Neither the name of the University nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.
   
THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\*------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "limboole.h"

/*------------------------------------------------------------------------*/
#ifdef LIMBOOLE_USE_LINGELING
#include "lglib.h"
#else
typedef struct LGL LGL;
#endif
#ifdef LIMBOOLE_USE_PICOSAT
#include "picosat.h"
#else
typedef struct PicoSAT PicoSAT;
#endif
/*------------------------------------------------------------------------*/
/* These are the node types we support.  They are ordered in decreasing
 * priority: if a parent with type t1 has a child with type t2 and t1 > t2,
 * then pretty printing the parent requires parentheses.  See 'pp_aux' for
 * more details.
 */
enum Type
{
  VAR = 0,
  LP = 1,
  RP = 2,
  NOT = 3,
  AND = 4,
  OR = 5,
  IMPLIES = 6,
  SEILPMI = 7,
  IFF = 8,
  DONE = 9,
  ERROR = 10,
};

/*------------------------------------------------------------------------*/

typedef enum Type Type;
typedef struct Node Node;
typedef union Data Data;

/*------------------------------------------------------------------------*/

union Data
{
  char *as_name;		/* variable data */
  Node *as_child[2];		/* operator data */
};

/*------------------------------------------------------------------------*/

struct Node
{
  Type type;
  int idx;			/* tseitin index */
  Node *next;			/* collision chain in hash table */
  Node *next_inserted;		/* chronological list of hash table */
  Data data;
};

/*------------------------------------------------------------------------*/

typedef struct Mgr Mgr;

struct Mgr
{
  const bool *stopTrigger; // Modified on 10 March 2019
  unsigned nodes_size;
  unsigned nodes_count;
  int idx;
  Node **nodes;
  Node *first;
  Node *last;
  Node *root;
  char *buffer;
  char *name;
  unsigned buffer_size;
  unsigned buffer_count;
  int saved_char;
  int saved_char_is_valid;
  int last_y;
  int verbose;
  int use_picosat;
  int use_lingeling;
  unsigned x;
  unsigned y;
  FILE *in;
  FILE *log;
  FILE *out;
  int close_in;
  int close_log;
  int close_out;
  Type token;
  unsigned token_x;
  unsigned token_y;
  Node **idx2node;
  int check_satisfiability;
  int dump;
  PicoSAT * picosat;
  LGL * lgl;
};

/*------------------------------------------------------------------------*/

static unsigned
hash_var (Mgr * mgr, const char *name)
{
  unsigned res, tmp;
  const char *p;

  res = 0;

  for (p = name; *p; p++)
    {
      tmp = res & 0xf0000000;
      res <<= 4;
      res += *p;
      if (tmp)
	res ^= (tmp >> 28);
    }

  res &= (mgr->nodes_size - 1);

  return res;
}

/*------------------------------------------------------------------------*/

static unsigned
hash_op (Mgr * mgr, Type type, Node * c0, Node * c1)
{
  unsigned res;

  res = (unsigned) type;
  res += 4017271 * (unsigned) (long) c0;
  res += 70200511 * (unsigned) (long) c1;

  res &= (mgr->nodes_size - 1);

  return res;
}

/*------------------------------------------------------------------------*/

static unsigned
hash (Mgr * mgr, Type type, void *c0, Node * c1)
{
  if (type == VAR)
    return hash_var (mgr, (char *) c0);
  else
    return hash_op (mgr, type, c0, c1);
}

/*------------------------------------------------------------------------*/

static int
eq_var (Node * n, const char *str)
{
  return n->type == VAR && !strcmp (n->data.as_name, str);
}

/*------------------------------------------------------------------------*/

static int
eq_op (Node * n, Type type, Node * c0, Node * c1)
{
  return n->type == type && n->data.as_child[0] == c0
    && n->data.as_child[1] == c1;
}

/*------------------------------------------------------------------------*/

static int
eq (Node * n, Type type, void *c0, Node * c1)
{
  if (type == VAR)
    return eq_var (n, (char *) c0);
  else
    return eq_op (n, type, c0, c1);
}

/*------------------------------------------------------------------------*/

static Node **
find (Mgr * mgr, Type type, void *c0, Node * c1)
{
  Node **p, *n;
  unsigned h;

  h = hash (mgr, type, c0, c1);
  for (p = mgr->nodes + h; (n = *p); p = &n->next)
    if (eq (n, type, c0, c1))
      break;

  return p;
}

/*------------------------------------------------------------------------*/

static void
enlarge_nodes (Mgr * mgr)
{
  Node **old_nodes, *p, *next;
  unsigned old_nodes_size, h, i;

  old_nodes = mgr->nodes;
  old_nodes_size = mgr->nodes_size;
  mgr->nodes_size *= 2;
  mgr->nodes = (Node **) calloc (mgr->nodes_size, sizeof (Node *));

  for (i = 0; i < old_nodes_size; i++)
    {
      for (p = old_nodes[i]; p; p = next)
	{
	  next = p->next;
	  if (p->type == VAR)
	    h = hash_var (mgr, p->data.as_name);
	  else
	    h =
	      hash_op (mgr, p->type, p->data.as_child[0],
		       p->data.as_child[1]);
	  p->next = mgr->nodes[h];
	  mgr->nodes[h] = p;
	}
    }

  free (old_nodes);
}

/*------------------------------------------------------------------------*/

static void
insert (Mgr * mgr, Node * node)
{
  if (mgr->last)
    mgr->last->next_inserted = node;
  else
    mgr->first = node;
  mgr->last = node;
  mgr->nodes_count++;
}

/*------------------------------------------------------------------------*/

static Node *
var (Mgr * mgr, const char *str)
{
  Node **p, *n;

  if (mgr->nodes_size <= mgr->nodes_count)
    enlarge_nodes (mgr);

  p = find (mgr, VAR, (void *) str, 0);
  n = *p;
  if (!n)
    {
      n = (Node *) malloc (sizeof (*n));
      memset (n, 0, sizeof (*n));
      n->type = VAR;
      n->data.as_name = strdup (str);

      *p = n;
      insert (mgr, n);
    }

  return n;
}

/*------------------------------------------------------------------------*/

static Node *
op (Mgr * mgr, Type type, Node * c0, Node * c1)
{
  Node **p, *n;

  if (mgr->nodes_size <= mgr->nodes_count)
    enlarge_nodes (mgr);

  p = find (mgr, type, c0, c1);
  n = *p;
  if (!n)
    {
      n = (Node *) malloc (sizeof (*n));
      memset (n, 0, sizeof (*n));
      n->type = type;
      n->data.as_child[0] = c0;
      n->data.as_child[1] = c1;

      *p = n;
      insert (mgr, n);
    }

  return n;
}

/*------------------------------------------------------------------------*/

static Mgr *
init (void)
{
  Mgr *res;

  res = (Mgr *) malloc (sizeof (*res));
  memset (res, 0, sizeof (*res));
  res->nodes_size = 2;
  res->nodes = (Node **) calloc (res->nodes_size, sizeof (Node *));
  res->buffer_size = 2;
  res->buffer = (char *) malloc (res->buffer_size);
  res->in = stdin;
  res->log = stderr;
  res->out = stdout;

  return res;
}

/*------------------------------------------------------------------------*/

static void
connect_solver (Mgr * mgr)
{
#ifdef LIMBOOLE_USE_PICOSAT
  if (mgr->use_picosat)
    {
      mgr->picosat = picosat_init (mgr->stopTrigger); // Modified on 10 March 2019
      if (mgr->verbose)
	picosat_set_verbosity (mgr->picosat, 1);
      picosat_set_prefix (mgr->picosat, "c PicoSAT ");
      picosat_set_output (mgr->picosat, mgr->log);
    }
#endif
#ifdef LIMBOOLE_USE_LINGELING
  if (mgr->use_lingeling)
    {
      mgr->lgl = lglinit ();
      if (mgr->verbose)
	lglsetopt (mgr->lgl, "verbose", 1);
      lglsetprefix (mgr->lgl, "c Lingeling ");
      lglsetout (mgr->lgl, mgr->log);
    }
#endif
}

/*------------------------------------------------------------------------*/

static void
release (Mgr * mgr)
{
  Node *p, *next;

#ifdef LIMBOOLE_USE_PICOSAT
  if (mgr->picosat)
    picosat_reset (mgr->picosat);
#endif
#ifdef LIMBOOLE_USE_LINGELING
  if (mgr->lgl)
    lglrelease (mgr->lgl);
#endif

  for (p = mgr->first; p; p = next)
    {
      next = p->next_inserted;
      if (p->type == VAR)
	free (p->data.as_name);
      free (p);
    }


  if (mgr->close_in)
    fclose (mgr->in);
  if (mgr->close_out)
    fclose (mgr->out);
  if (mgr->close_log)
    fclose (mgr->log);

  free (mgr->idx2node);
  free (mgr->nodes);
  free (mgr->buffer);
  free (mgr);
}

/*------------------------------------------------------------------------*/

static void
print_token (Mgr * mgr)
{
  switch (mgr->token)
    {
    case VAR:
      fputs (mgr->buffer, mgr->log);
      break;
    case LP:
      fputc ('(', mgr->log);
      break;
    case RP:
      fputc (')', mgr->log);
      break;
    case NOT:
      fputc ('!', mgr->log);
      break;
    case AND:
      fputc ('&', mgr->log);
      break;
    case OR:
      fputc ('|', mgr->log);
      break;
    case IMPLIES:
      fputs ("->", mgr->log);
      break;
    case SEILPMI:
      fputs ("<-", mgr->log);
      break;
    case IFF:
      fputs ("<->", mgr->log);
      break;
    default:
      fputs ("EOF", mgr->log);
      break;
    }
}

/*------------------------------------------------------------------------*/

static void
parse_error (Mgr * mgr, const char *fmt, ...)
{
  va_list ap;
  char *name;

  name = mgr->name ? mgr->name : "<stdin>";
  fprintf (mgr->log, "%s:%u:%u: ", name, mgr->token_x + 1, mgr->token_y);
  if (mgr->token == ERROR)
    fputs ("scan error: ", mgr->log);
  else
    {
      fputs ("parse error at '", mgr->log);
      print_token (mgr);
      fputs ("' ", mgr->log);
    }
  va_start (ap, fmt);
  vfprintf (mgr->log, fmt, ap);
  va_end (ap);
  fputc ('\n', mgr->log);
}

/*------------------------------------------------------------------------*/

static int
is_var_letter (int ch)
{
  if (isalnum (ch))
    return 1;

  switch (ch)
    {
    case '-':
    case '_':
    case '.':
    case '[':
    case ']':
    case '$':
    case '@':
      return 1;

    default:
      return 0;
    }
}

/*------------------------------------------------------------------------*/

static void
enlarge_buffer (Mgr * mgr)
{
  mgr->buffer_size *= 2;
  mgr->buffer = (char *) realloc (mgr->buffer, mgr->buffer_size);
}

/*------------------------------------------------------------------------*/

static int
next_char (Mgr * mgr)
{
  int res;

  mgr->last_y = mgr->y;

  if (mgr->saved_char_is_valid)
    {
      mgr->saved_char_is_valid = 0;
      res = mgr->saved_char;
    }
  else
    res = fgetc (mgr->in);

  if (res == '\n')
    {
      mgr->x++;
      mgr->y = 0;
    }
  else
    mgr->y++;

  return res;
}

/*------------------------------------------------------------------------*/

static void
unget_char (Mgr * mgr, int ch)
{

  mgr->saved_char_is_valid = 1;
  mgr->saved_char = ch;

  if (ch == '\n')
    {
      mgr->x--;
      mgr->y = mgr->last_y;
    }
  else
    mgr->y--;
}

/*------------------------------------------------------------------------*/

static void
next_token (Mgr * mgr)
{
  int ch;

  mgr->token = ERROR;
  ch = next_char (mgr);

RESTART_NEXT_TOKEN:

  while (isspace ((int) ch))
    ch = next_char (mgr);

  if (ch == '%')
    {
      while ((ch = next_char (mgr)) != '\n' && ch != EOF)
	;

      goto RESTART_NEXT_TOKEN;
    }

  mgr->token_x = mgr->x;
  mgr->token_y = mgr->y;

  if (ch == EOF)
    mgr->token = DONE;
  else if (ch == '<')
    {
      ch = next_char (mgr);
      if (ch == '-')
	{
	  ch = next_char (mgr);
	  if (ch == '>')
	    mgr->token = IFF;
	  else
	    {
	      unget_char (mgr, ch);
	      mgr->token = SEILPMI;
	    }
	}
      else
	parse_error (mgr, "expected '-' after '<'");
    }
  else if (ch == '-')
    {
      ch = next_char (mgr);
      if (ch == '>')
	mgr->token = IMPLIES;
      else
	{
	  unget_char (mgr, ch);
	  mgr->token = NOT;
	}
    }
  else if (ch == '&')
    {
      mgr->token = AND;
    }
  else if (ch == '|')
    {
      mgr->token = OR;
    }
  else if (ch == '!' || ch == '~')
    {
      mgr->token = NOT;
    }
  else if (ch == '(')
    {
      mgr->token = LP;
    }
  else if (ch == ')')
    {
      mgr->token = RP;
    }
  else if (is_var_letter (ch))
    {
      mgr->buffer_count = 0;

      while (is_var_letter (ch))
	{
	  if (mgr->buffer_size <= mgr->buffer_count + 1)
	    enlarge_buffer (mgr);

	  mgr->buffer[mgr->buffer_count++] = ch;
	  ch = next_char (mgr);
	}

      unget_char (mgr, ch);
      mgr->buffer[mgr->buffer_count] = 0;

      if (mgr->buffer[mgr->buffer_count - 1] == '-')
	parse_error (mgr, "variable '%s' ends with '-'", mgr->buffer);
      else
	mgr->token = VAR;
    }
  else
    parse_error (mgr, "invalid character '%c'", ch);
}

/*------------------------------------------------------------------------*/

static Node *parse_expr (Mgr *);

/*------------------------------------------------------------------------*/

static Node *
parse_basic (Mgr * mgr)
{
  Node *child;
  Node *res;

  res = 0;

  if (mgr->token == LP)
    {
      next_token (mgr);
      child = parse_expr (mgr);
      if (mgr->token != RP)
	{
	  if (mgr->token != ERROR)
	    parse_error (mgr, "expected ')'");
	}
      else
	res = child;
      next_token (mgr);
    }
  else if (mgr->token == VAR)
    {
      res = var (mgr, mgr->buffer);
      next_token (mgr);
    }
  else if (mgr->token != ERROR)
    parse_error (mgr, "expected variable or '('");

  return res;
}

/*------------------------------------------------------------------------*/

static Node *
parse_not (Mgr * mgr)
{
  Node *child, *res;

  if (mgr->token == NOT)
    {
      next_token (mgr);
      child = parse_not (mgr);
      if (child)
	res = op (mgr, NOT, child, 0);
      else
	res = 0;
    }
  else
    res = parse_basic (mgr);

  return res;
}

/*------------------------------------------------------------------------*/

static Node *
parse_associative_op (Mgr * mgr, Type type, Node * (*lower) (Mgr *))
{
  Node *res, *child;
  int done;

  res = 0;
  done = 0;

  do
    {
      child = lower (mgr);
      if (child)
	{
	  res = res ? op (mgr, type, res, child) : child;
	  if (mgr->token == type)
	    next_token (mgr);
	  else
	    done = 1;
	}
      else
	res = 0;
    }
  while (res && !done);

  return res;
}


/*------------------------------------------------------------------------*/

static Node *
parse_and (Mgr * mgr)
{
  return parse_associative_op (mgr, AND, parse_not);
}

/*------------------------------------------------------------------------*/

static Node *
parse_or (Mgr * mgr)
{
  return parse_associative_op (mgr, OR, parse_and);
}

/*------------------------------------------------------------------------*/

static Node *
parse_implies (Mgr * mgr)
{
  Node *l, *r;
  Type token;

  if (!(l = parse_or (mgr)))
    return 0;
  token = mgr->token;
  if (token != IMPLIES && token != SEILPMI)
    return l;
  next_token (mgr);
  if (!(r = parse_or (mgr)))
    return 0;

  return op (mgr, token, l, r);
}

/*------------------------------------------------------------------------*/

static Node *
parse_iff (Mgr * mgr)
{
  return parse_associative_op (mgr, IFF, parse_implies);
}

/*------------------------------------------------------------------------*/

static Node *
parse_expr (Mgr * mgr)
{
  return parse_iff (mgr);
}

/*------------------------------------------------------------------------*/

static int
parse (Mgr * mgr)
{
  next_token (mgr);

  if (mgr->token == ERROR)
    return 0;

  if (!(mgr->root = parse_expr (mgr)))
    return 0;

  if (mgr->token == DONE)
    return 1;

  if (mgr->token != ERROR)
    parse_error (mgr, "expected operator or EOF");

  return 0;
}

/*------------------------------------------------------------------------*/

static void
add_lit (Mgr * mgr, int lit)
{
#ifdef LIMBOOLE_USE_PICOSAT
  if (mgr->picosat)
    picosat_add (mgr->picosat, lit);
#endif
#ifdef LIMBOOLE_USE_LINGELING
  if (mgr->lgl)
    lgladd (mgr->lgl, lit);
#endif
}

/*------------------------------------------------------------------------*/

static void
add_clause (Mgr * mgr, int * clause)
{
  const int * p;
  for (p = clause; *p; p++)
    {
      add_lit (mgr, *p);
      if (mgr->dump)
	fprintf (mgr->out, "%d ", *p);
    }
  add_lit (mgr, 0);
  if (mgr->dump)
    fprintf (mgr->out, "0\n");
}

/*------------------------------------------------------------------------*/

static void
unit_clause (Mgr * mgr, int a)
{
  int clause[2];

  clause[0] = a;
  clause[1] = 0;

  add_clause (mgr, clause);
}

/*------------------------------------------------------------------------*/

static void
binary_clause (Mgr * mgr, int a, int b)
{
  int clause[3];

  clause[0] = a;
  clause[1] = b;
  clause[2] = 0;

  add_clause (mgr, clause);
}

/*------------------------------------------------------------------------*/

static void
ternary_clause (Mgr * mgr, int a, int b, int c)
{
  int clause[4];

  clause[0] = a;
  clause[1] = b;
  clause[2] = c;
  clause[3] = 0;

  add_clause (mgr, clause);
}

/*------------------------------------------------------------------------*/

static void
tseitin (Mgr * mgr)
{
  int num_clauses;
  int sign;
  Node *p;

  num_clauses = 0;

  for (p = mgr->first; p && !(*mgr->stopTrigger); p = p->next_inserted) // Modified on 10 March 2019
    {
      p->idx = ++mgr->idx;

      if (mgr->dump && p->type == VAR)
	fprintf (mgr->out, "c %d %s\n", p->idx, p->data.as_name);

      switch (p->type)
	{
	case IFF:
	  num_clauses += 4;
	  break;
	case OR:
	case AND:
	case IMPLIES:
	case SEILPMI:
	  num_clauses += 3;
	  break;
	case NOT:
	  num_clauses += 2;
	  break;
	default:
	  break;
	}
    }

  mgr->idx2node = (Node **) calloc (mgr->idx + 1, sizeof (Node *));
  for (p = mgr->first; p && !(*mgr->stopTrigger); p = p->next_inserted) // Modified on 10 March 2019
    mgr->idx2node[p->idx] = p;

  if (mgr->dump)
    fprintf (mgr->out, "p cnf %d %u\n", mgr->idx, num_clauses + 1);

  for (p = mgr->first; p && !(*mgr->stopTrigger); p = p->next_inserted) // Modified on 10 March 2019
    {
      switch (p->type)
	{
	case IFF:
	  ternary_clause (mgr, p->idx,
			  -p->data.as_child[0]->idx,
			  -p->data.as_child[1]->idx);
	  ternary_clause (mgr, p->idx, p->data.as_child[0]->idx,
			  p->data.as_child[1]->idx);
	  ternary_clause (mgr, -p->idx, -p->data.as_child[0]->idx,
			  p->data.as_child[1]->idx);
	  ternary_clause (mgr, -p->idx, p->data.as_child[0]->idx,
			  -p->data.as_child[1]->idx);
	  break;
	case IMPLIES:
	  binary_clause (mgr, p->idx, p->data.as_child[0]->idx);
	  binary_clause (mgr, p->idx, -p->data.as_child[1]->idx);
	  ternary_clause (mgr, -p->idx,
			  -p->data.as_child[0]->idx,
			  p->data.as_child[1]->idx);
	  break;
	case SEILPMI:
	  binary_clause (mgr, p->idx, -p->data.as_child[0]->idx);
	  binary_clause (mgr, p->idx, p->data.as_child[1]->idx);
	  ternary_clause (mgr, -p->idx,
			  p->data.as_child[0]->idx,
			  -p->data.as_child[1]->idx);
	  break;
	case OR:
	  binary_clause (mgr, p->idx, -p->data.as_child[0]->idx);
	  binary_clause (mgr, p->idx, -p->data.as_child[1]->idx);
	  ternary_clause (mgr, -p->idx,
			  p->data.as_child[0]->idx, p->data.as_child[1]->idx);
	  break;
	case AND:
	  binary_clause (mgr, -p->idx, p->data.as_child[0]->idx);
	  binary_clause (mgr, -p->idx, p->data.as_child[1]->idx);
	  ternary_clause (mgr, p->idx,
			  -p->data.as_child[0]->idx,
			  -p->data.as_child[1]->idx);
	  break;
	case NOT:
	  binary_clause (mgr, p->idx, p->data.as_child[0]->idx);
	  binary_clause (mgr, -p->idx, -p->data.as_child[0]->idx);
	  break;
	default:
	  break;
	}
    }


  sign = (mgr->check_satisfiability) ? 1 : -1;
  unit_clause (mgr, sign * mgr->root->idx);
}

/*------------------------------------------------------------------------*/

static void
pp_aux (Mgr * mgr, Node * node, Type outer)
{
  int le, lt;

  le = outer <= node->type;
  lt = outer < node->type;

  switch (node->type)
    {
    case NOT:
      fputc ('!', mgr->out);
      pp_aux (mgr, node->data.as_child[0], node->type);
      break;
    case IMPLIES:
    case SEILPMI:
      if (le)
	fputc ('(', mgr->out);
      pp_aux (mgr, node->data.as_child[0], node->type);
      fputs (node->type == IMPLIES ? " -> " : " <- ", mgr->out);
      pp_aux (mgr, node->data.as_child[1], node->type);
      if (le)
	fputc (')', mgr->out);
      break;

    case OR:
    case AND:
    case IFF:
      if (lt)
	fputc ('(', mgr->out);
      pp_aux (mgr, node->data.as_child[0], node->type);
      if (node->type == OR)
	fputs (" | ", mgr->out);
      else if (node->type == AND)
	fputs (" & ", mgr->out);
      else
	fputs (" <-> ", mgr->out);
      pp_aux (mgr, node->data.as_child[1], node->type);
      if (lt)
	fputc (')', mgr->out);
      break;

    default:
      fprintf (mgr->out, "%s", node->data.as_name);
      break;
    }
}

/*------------------------------------------------------------------------*/

static void
pp_and (Mgr * mgr, Node * node)
{
  if (node->type == AND)
    {
      pp_and (mgr, node->data.as_child[0]);
      fprintf (mgr->out, "\n&\n");
      pp_and (mgr, node->data.as_child[1]);
    }
  else
    pp_aux (mgr, node, AND);
}

/*------------------------------------------------------------------------*/

static void
pp_or (Mgr * mgr, Node * node)
{
  if (node->type == OR)
    {
      pp_or (mgr, node->data.as_child[0]);
      fprintf (mgr->out, "\n|\n");
      pp_or (mgr, node->data.as_child[1]);
    }
  else
    pp_aux (mgr, node, OR);
}

/*------------------------------------------------------------------------*/

static void
pp_and_or (Mgr * mgr, Node * node, Type outer)
{

  if (node->type == AND)
    pp_and (mgr, node);
  else if (node->type == OR)
    pp_or (mgr, node);
  else
    pp_aux (mgr, node, outer);
}

/*------------------------------------------------------------------------*/

static void
pp_iff_implies (Mgr * mgr, Node * node, Type outer)
{
  if (node->type == IFF || node->type == IMPLIES)
    {
      pp_and_or (mgr, node->data.as_child[0], node->type);
      fprintf (mgr->out, "\n%s\n", node->type == IFF ? "<->" : "->");
      pp_and_or (mgr, node->data.as_child[1], node->type);
    }
  else
    pp_and_or (mgr, node, outer);
}

/*------------------------------------------------------------------------*/

static void
pp (Mgr * mgr)
{
  pp_iff_implies (mgr, mgr->root, DONE);
  fputc ('\n', mgr->out);
}

/*------------------------------------------------------------------------*/

static void
print_assignment (Mgr * mgr)
{
  int idx, val;
  Node * n;
  for (idx = 1; idx <= mgr->idx; idx++)
    {
      val = 0;
#ifdef LIMBOOLE_USE_PICOSAT
      if (mgr->picosat)
	val = picosat_deref (mgr->picosat, idx);
#endif
#ifdef LIMBOOLE_USE_LINGELING
      if (mgr->lgl)
	val = lglderef (mgr->lgl, idx);
#endif
      n = mgr->idx2node[idx];
      if (n->type == VAR)
	fprintf (mgr->out, "%s = %d\n", n->data.as_name, val > 0);
    }
}

/*------------------------------------------------------------------------*/
#if defined(LIMBOOLE_USE_PICOSAT) && defined(LIMBOOLE_USE_LINGELING)
#define PICOSAT_USAGE \
"  --picosat      use PicoSAT as SAT solver back-end (disabled by default)\n"
#define LINGELING_USAGE \
"  --lingeling    use Lingeling as SAT solver back-end (default)\n"
#endif
#if !defined(LIMBOOLE_USE_PICOSAT) && defined(LIMBOOLE_USE_LINGELING)
#define PICOSAT_USAGE \
"  --picosat      no support for PicoSAT SAT solver compiled in\n"
#define LINGELING_USAGE \
"  --lingeling    using Lingeling (as the only available SAT solver back-end)\n"
#endif
#if defined(LIMBOOLE_USE_PICOSAT) && !defined(LIMBOOLE_USE_LINGELING)
#define PICOSAT_USAGE \
"  --picosat      using PicoSAT (as the only available SAT solver back-end)\n"
#define LINGELING_USAGE \
"  --lingeling    no support for Lingeling SAT solver compiled in\n"
#endif
#if !defined(LIMBOOLE_USE_PICOSAT) && !defined(LIMBOOLE_USE_LINGELING)
#error "At least one of Lingeling or PicoSAT has to be available!"
#endif

#define USAGE \
"usage: limboole [ <option> ... ]\n" \
"\n" \
"  -h             print this command line summary and exit\n" \
"  --version      print the version and exit\n" \
"  -v             increase verbosity\n" \
"  -p             pretty print input formula only\n" \
"  -d             dump generated CNF only\n" \
"  -s             check satisfiability (default is to check validity)\n" \
"  -o <out-file>  set output file (default <stdout>)\n" \
"  -l <log-file>  set log file (default <stderr>)\n" \
LINGELING_USAGE \
PICOSAT_USAGE \
"  <in-file>      input file (default <stdin>)\n"

/*------------------------------------------------------------------------*/

int limboole (int argc, const char **argv, const bool *stopTrigger) // Modified on 10 March 2019
{
  const int *assignment;
  int pretty_print;
  FILE *file;
  int error;
  Mgr *mgr;
  int done;
  int res;
  int i;

  done = 0;
  error = 0;
  pretty_print = 0;

  mgr = init ();
  mgr->stopTrigger = stopTrigger; // Modified on 10 March 2019

#ifdef LIMBOOLE_USE_LINGELING
  mgr->use_lingeling = 1;
#else
  mgr->use_picosat = 1;
#endif

  for (i = 1; !done && !error && i < argc; i++)
    {
      if (!strcmp (argv[i], "-h"))
	{
	  fprintf (mgr->out, USAGE);
	  done = 1;
	}
      else if (!strcmp (argv[i], "--version"))
	{
	  fprintf (mgr->out, "%d\n", VERSION);
	  done = 1;
	}
      else if (!strcmp (argv[i], "-v"))
	{
	  mgr->verbose += 1;
	}
      else if (!strcmp (argv[i], "-p"))
	{
	  pretty_print = 1;
	}
      else if (!strcmp (argv[i], "-d"))
	{
	  mgr->dump = 1;
	}
      else if (!strcmp (argv[i], "-s"))
	{
	  mgr->check_satisfiability = 1;
	}
      else if (!strcmp (argv[i], "-o"))
	{
	  if (i == argc - 1)
	    {
	      fprintf (mgr->log, "*** argument to '-o' missing (try '-h')\n");
	      error = 1;
	    }
	  else if (!(file = fopen (argv[++i], "w")))
	    {
	      fprintf (mgr->log, "*** could not write '%s'\n", argv[i]);
	      error = 1;
	    }
	  else if (mgr->close_out)
	    {
	      /* We moved this down for coverage in testing purposes */
	      fclose (file);
	      fprintf (mgr->log, "*** '-o' specified twice (try '-h')\n");
	      error = 1;
	    }
	  else
	    {
	      mgr->out = file;
	      mgr->close_out = 1;
	    }
	}
      else if (!strcmp (argv[i], "-l"))
	{
	  if (i == argc - 1)
	    {
	      fprintf (mgr->log, "*** argument to '-l' missing (try '-h')\n");
	      error = 1;
	    }
	  else if (!(file = fopen (argv[++i], "a")))
	    {
	      fprintf (mgr->log, "*** could not append to '%s'\n", argv[i]);
	      error = 1;
	    }
	  else if (mgr->close_log)
	    {
	      /* We moved this down for coverage in testing purposes */
	      fclose (file);
	      fprintf (mgr->log, "*** '-l' specified twice (try '-h')\n");
	      error = 1;
	    }
	  else
	    {
	      mgr->log = file;
	      mgr->close_log = 1;
	    }
	}
#ifdef LIMBOOLE_USE_PICOSAT
      else if (!strcmp (argv[i], "--picosat"))
	{
	  mgr->use_lingeling = 0;
	  mgr->use_picosat = 1;
	}
#endif
#ifdef LIMBOOLE_USE_LINGELING
      else if (!strcmp (argv[i], "--lingeling"))
	{
	  mgr->use_lingeling = 1;
	  mgr->use_picosat = 0;
	}
#endif
      else if (argv[i][0] == '-')
	{
	  fprintf (mgr->log,
		   "*** invalid command line option '%s' (try '-h')\n",
		   argv[i]);
	  error = 1;
	}
      else if (mgr->close_in)
	{
	  fprintf (mgr->log,
		   "*** can not read more than two files (try '-h')\n");
	  error = 1;
	}
      else if (!(file = fopen (argv[i], "r")))
	{
	  fprintf (mgr->log, "*** could not read '%s'\n", argv[i]);
	  error = 1;
	}
      else
	{
	  mgr->in = file;
	  mgr->name = argv[i];
	  mgr->close_in = 1;
	}
    }

  // Added if on 10 March 2019
  if (*mgr->stopTrigger) {
    return false;
  }

  if (!error && !done)
    {
      error = !parse (mgr);

      if (!error)
	{
	  if (pretty_print)
	    pp (mgr);
	  else
	    {
        // Added if on 10 March 2019
        if (*mgr->stopTrigger) {
          return false;
        }
	      connect_solver (mgr);
	      tseitin (mgr);
	      if (!mgr->dump)
		{
#ifdef LIMBOOLE_USE_LINGELING
		  res = 0;
		  if (mgr->lgl)
		    res = lglsat (mgr->lgl);
#endif
#ifdef LIMBOOLE_USE_PICOSAT
      // Added if on 10 March 2019
      if (*mgr->stopTrigger) {
        return false;
      }
		  if (mgr->picosat)
		    res = picosat_sat (mgr->picosat, -1);
#endif
		  if (res == 10)
		    {
		      if (mgr->check_satisfiability)
			fprintf (mgr->out,
				 "%% SATISFIABLE formula"
				 " (satisfying assignment follows)\n");
		      else
			fprintf (mgr->out,
				 "%% INVALID formula"
				 " (falsifying assignment follows)\n");

		      print_assignment (mgr);
		    }
		  else if (res == 20)
		    {
		      if (mgr->check_satisfiability)
			fprintf (mgr->out, "%% UNSATISFIABLE formula\n");
		      else
			fprintf (mgr->out, "%% VALID formula\n");
		    }
		  else {
		    fprintf (mgr->out, "%% UNKNOWN result\n");
		  }
		}
	    }
	}
    }

  if (mgr->verbose)
    {
#ifdef LIMBOOLE_USE_LINGELING
      if (mgr->lgl)
	lglstats (mgr->lgl);
#endif
#ifdef LIMBOOLE_USE_PICOSAT
      if (mgr->picosat)
	picosat_stats (mgr->picosat);
#endif
    }
  release (mgr);

  return error != 0;
}
