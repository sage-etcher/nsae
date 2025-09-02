
#ifndef NS_LOG_H
#define NS_LOG_H

#include "log.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

extern bool g_log_cpu;
extern bool g_log_mmu;
extern bool g_log_ram;
extern bool g_log_fdc;
extern bool g_log_crt;
extern bool g_log_kb;
extern bool g_log_mobo;

#define log_cpu(...)  log_conditional (g_log_cpu,  stdout, __VA_ARGS__);
#define log_mmu(...)  log_conditional (g_log_mmu,  stdout, __VA_ARGS__);
#define log_fdc(...)  log_conditional (g_log_fdc,  stdout, __VA_ARGS__);
#define log_ram(...)  log_conditional (g_log_ram,  stdout, __VA_ARGS__);
#define log_crt(...)  log_conditional (g_log_crt,  stdout, __VA_ARGS__);
#define log_kb(...)   log_conditional (g_log_kb,   stdout, __VA_ARGS__);
#define log_mobo(...) log_conditional (g_log_mobo, stdout, __VA_ARGS__);

#endif /* NS_LOG_H */
/* end of file */
