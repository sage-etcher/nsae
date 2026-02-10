
#include "nsae.h"
#include "nsae_help.h"
#include "nsae_version.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define VERSION_STR SRC_NSAE_NSAE_VERSION
#define VERSION_LEN SRC_NSAE_NSAE_VERSION_LEN

#define USAGE_STR ___SRC_NSAE_NSAE_HELP
#define USAGE_LEN ___SRC_NSAE_NSAE_HELP_LEN

static void version (void);
static void usage (void);

static void 
version (void)
{
    printf ("%.*s", VERSION_LEN, VERSION_STR);
}

static void 
usage (void)
{
    printf ("%.*s", USAGE_LEN, USAGE_STR);
}

int
main (int argc, char **argv)
{
    int opt = 0;
    nsae_config_t config = {
        .verbosity = 0,
        .socket_addr = "tcp://localhost:5555",
        .disk_a = NULL,
        .disk_b = NULL,
        .init_paused = 0,
        .init_speaker = 1,
    };
    nsae_t nsae = { 0 };

    while ((opt = getopt (argc, argv, "A:B:f:hpstvV")) != -1)
    {
        switch (opt)
        {
        case 'f': config.socket_addr = optarg; break;

        case 'A': config.disk_a = optarg; break;
        case 'B': config.disk_b = optarg; break;
        case 'p': config.init_paused  = 1; break;
        case 's': config.init_speaker = 0; break;

        case 't': config.verbosity = 0; break;
        case 'v': config.verbosity = 1; break;

        case 'V': version (); exit (EXIT_SUCCESS); break;
        case 'h': usage ();   exit (EXIT_SUCCESS); break;
        default:  usage ();   exit (EXIT_FAILURE); break;
        }
    }

    return nsae_start (&nsae, &config);
}

/* end of file */
