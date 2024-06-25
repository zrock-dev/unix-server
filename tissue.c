#include <argp.h>
#include <stdlib.h>

const char *argp_program_version = "tissue v0.0.0-alpha";
const char *argp_program_bug_address = "<zrock_git@zohomail.com>";

/* Program documentation. */
static char doc[] = "Tissue, is a unix server";

/* A description of the arguments we accept. */
static char args_doc[] = "number to start listening";

/* The options we understand. */
static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output"},
    {"quiet", 'q', 0, 0, "Don't produce any output"},
    {"silent", 's', 0, OPTION_ALIAS},
    {"output", 'o', "FILE", 0, "Output to FILE instead of standard output"},
    {"port", 'p', "PORT", 0, "Listen on port"},
    {0}};

/* Used by main to communicate with parse_opt. */
struct arguments {
  int silent, verbose;
  char *output_file;
  int port;
};

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key) {
  case 'q':
  case 's':
    arguments->silent = 1;
    break;
  case 'v':
    arguments->verbose = 1;
    break;
  case 'o':
    arguments->output_file = arg;
    break;
  case 'p':
    arguments->port = atoi(arg);
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {
  struct arguments arguments;

  /* Default values. */
  arguments.silent = 0;
  arguments.verbose = 0;
  arguments.output_file = "-";
  arguments.port = 8080;

  /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  printf("Port %d", arguments.port);

  exit(0);
}
