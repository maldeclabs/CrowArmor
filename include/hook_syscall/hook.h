#pragma once

#include "crowarmor/datacrow.h"
#include "err/err.h"
#include <linux/kprobes.h>
#include <linux/version.h>

struct hook_syscall {
  void *new_syscall;
  void *old_syscall;
  bool unknown_hook;
  int idx;
};

const __must_check ERR hook_init(struct crow **crow) notrace;
void hook_end(void) notrace;

const __must_check void *hook_get_old_syscall(unsigned int) notrace;
void hook_remove_unknown_syscall(struct hook_syscall *) notrace;
void hook_check_hooked_syscall(struct hook_syscall *, unsigned int) notrace;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 8, 0)

const __must_check ERR hook_sys_call_table_x64(void) notrace;
void hook_remove_sys_call_table_x64(void) notrace;

#endif