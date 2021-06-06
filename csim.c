#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef uint64_t addr_t;

#define MASK(high, low) ((((addr_t)1<<(high - low))-1)<<low)
#define BLOCK(addr, cfg) (addr & MASK((cfg).block,0))
#define SET(addr, cfg) \
    ((addr & MASK((cfg).block+(cfg).set,(cfg).block)) >> (cfg).block)
#define TAG(addr, cfg) (addr>> ((cfg).set+(cfg).block))

struct result {
    int hit;
    int miss;
    int evict;
};

struct configuration {
    int verbose;
    int set;
    int ways;
    int block;
    FILE *traceFile;
    struct result result;
};

/* Indexing the cache */

struct c_block {
    addr_t tag;
    int valid;
    int dirty;
    addr_t last_used;
};

typedef struct c_block c_block;
typedef c_block *c_set;
typedef c_set *c_way;
typedef c_way *cache;

#define for_each_way(wayptr, cache, cfg) \
    for(wayptr = cache; wayptr < cache + (cfg).ways; wayptr++)

#define for_each_set(setptr, way, cfg) \
    for(setptr = way; setptr < way + (1 << (cfg).set); setptr++)

#define for_each_block(blockptr, set, cfg) \
    for(blockptr = set; blockptr < set + (1 << (cfg).block); blockptr++)

void printHelpInfo () {
  printf ("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
  printf ("  -h: Optional help flag that prints usage info\n");
  printf ("  -v: Optional verbose flag that displays trace info\n");
  printf (
      "  -s <s>: Number of set index bits (S = 2^s is the number of sets)\n");
  printf ("  -E <E>: Associativity (number of lines per set)\n");
  printf ("  -b <b>: Number of block bits (B = 2^b is the block size)\n");
  printf ("  -t <tracefile>: Name of the valgrind trace to replay\n");
}

void cacheOp (cache *c, addr_t addr, int curr, struct configuration *cfg) {
  c_way *set;
  struct c_block *block = NULL;
  int hit = 0, cold = 0, last_used = curr;
  for_each_way(set, *c, *cfg) {
    if ((*set)[SET(addr, *cfg)]->tag == TAG(addr, *cfg)
        && (*set)[SET(addr, *cfg)]->valid) {
      hit = 1;
      block = (*set)[SET(addr, *cfg)];
      break;
    }
    if (cold)
      continue;
    if (!(*set)[SET(addr, *cfg)]->valid) {
      block = (*set)[SET(addr, *cfg)];
      cold = 1;
    }
    else if (last_used > (*set)[SET(addr, *cfg)]->last_used) {
      last_used = (*set)[SET(addr, *cfg)]->last_used;
      block = (*set)[SET(addr, *cfg)];
    }
  }
  block->last_used = curr;
  block->valid = 1;
  block->tag = TAG(addr, *cfg);
  if (hit) {
    cfg->result.hit++;
    if (cfg->verbose)
      printf ("S %lx hit\n", addr);
    return;
  }
  else {
    cfg->result.miss++;
    if (cold) {
      if (cfg->verbose)
        printf ("S %lx miss\n", addr);
      return;
    }
    else {
      cfg->result.evict++;
      if (cfg->verbose)
        printf ("S %lx evict\n", addr);
    }
  }
  return;
}

void sizeexpand (void (*action) (cache *, addr_t, int, struct configuration *),
                 cache *c, addr_t addr, int curr, int size, int modify,
                 struct configuration *cfg) {
  int offset;
  for (offset = 0; offset < size; offset += (1 << cfg->block)) {
    action (c, (addr >> cfg->block << cfg->block) + offset, curr, cfg);
    if (modify)
      cfg->result.hit++;
  }
}

struct configuration optparse (int argc, char **argv) {
  struct configuration config;
  const char *optString = "hvs:E:b:t:";
  char opt = getopt (argc, argv, optString);
  config.verbose = 0;
  while (opt != -1) {
    switch (opt) {
      case 'h':
        printHelpInfo ();
        exit (0);
      case 'v':
        config.verbose = 1;
        break;
      case 's':
        config.set = atoi (optarg);
        if (config.set <= 0) {
          printf ("Error: set index bits.\n");
          exit (-1);
        }

        break;
      case 'E':
        config.ways = atoi (optarg);
        if (config.ways <= 0) {
          printf ("Error: associativity.\n");
          exit (-1);
        }
        break;
      case 'b':
        config.block = atoi (optarg);
        if (config.block <= 0) {
          printf ("Error: number of block bits.\n");
          exit (-1);
        }
        break;
      case 't':
        config.traceFile = fopen (optarg, "r");
        if (!config.traceFile) {
          printf ("Error: opening file\n");
          exit (-1);
        }
        break;
    }
    opt = getopt (argc, argv, optString);
  }
  return config;
}

int main (int argc, char **argv) {
  struct configuration cfg = optparse (argc, argv);
  memset (&cfg.result, 0, sizeof (struct result));

  cache c = malloc (cfg.ways * sizeof (c_way));
  c_way *wayptr;
  for_each_way(wayptr, c, cfg) {
    c_set *setptr;
    *wayptr = malloc ((1 << cfg.set) * sizeof (c_set));
    for_each_set(setptr, *wayptr, cfg) {
      *setptr = malloc ((1 << cfg.block) * sizeof (c_block));
      memset (*setptr, 0, (1 << cfg.block) * sizeof (c_block));
    }
  }
  char inst, tmp;
  addr_t addr;
  int size, curr = 0;

  while (fscanf (cfg.traceFile, "%c%lx%c%d", &inst, &addr, &tmp, &size)
         != EOF) {
    if (inst == 'S' || inst == 'L')
      sizeexpand (cacheOp, &c, addr, curr, size, 0, &cfg);
    else if (inst == 'M')
      sizeexpand (cacheOp, &c, addr, curr, size, 1, &cfg);
    curr++;
  }
  printSummary (cfg.result.hit, cfg.result.miss, cfg.result.evict);
  return 0;
}
