#include "hook_syscall/hook.h"
#include "control_registers/cr0.h"
#include "crowarmor/datacrow.h"
#include "kpobres/kallsyms_lookup.h"
#include "syscall.h"

#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/nospec.h>
#include <linux/string.h>

static unsigned long **old_syscall_table;
static unsigned long **syscall_table;
static unsigned long **crowarmor_syscall_table;
static struct crow **armor;

static struct hook_syscall syscalls[] = {
    /**
     * hooked system calls
     */
    {.new_syscall = syscall_memfd_create,
     .old_syscall = NULL,
     .idx = __NR_memfd_create},

    /**
     * null byte array
     */
    {.new_syscall = NULL, .old_syscall = NULL, .idx = -1}

};

static void set_new_syscall(struct hook_syscall *syscall) {
  disable_register_cr0_wp();
  syscall->old_syscall = syscall_table[syscall->idx];
  syscall_table[syscall->idx] = syscall->new_syscall;
  enable_register_cr0_wp();
}

static void set_old_syscall(struct hook_syscall *syscall) {
  disable_register_cr0_wp();
  syscall_table[syscall->idx] = syscall->old_syscall;
  enable_register_cr0_wp();
}

void hook_remove_unknown_syscall(struct hook_syscall *syscall) {
  disable_register_cr0_wp();
  syscall_table[syscall->idx] = syscall->old_syscall;
  enable_register_cr0_wp();
}

const void *hook_get_old_syscall(unsigned int idx) { return old_syscall_table[idx]; }

void hook_check_hooked_syscall(struct hook_syscall *syscall, unsigned int idx) {
  syscall->unknown_hook = false;

  if (syscall_table[idx] != crowarmor_syscall_table[idx]) {
    syscall->new_syscall = syscall_table[idx];
    syscall->old_syscall = crowarmor_syscall_table[idx];
    syscall->idx = idx;

    syscall->unknown_hook = true;
  }
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 8, 0)

static void *symbol_x64_sys_call;
static unsigned char x64_sys_call_recovery[20] = {};

const ERR hook_sys_call_table_x64(void) {  
  pr_info("crowarmor: Adding sys_call_table kernel patch\n");
  symbol_x64_sys_call = (void *)kallsyms_lookup_name("x64_sys_call");

  if (!symbol_x64_sys_call) {
    pr_info("crowarmor: Symbol 'x64_sys_call' not found\n");
    return ERR_FAILURE;
  }
  
  memcpy(x64_sys_call_recovery, symbol_x64_sys_call, 20);

  disable_register_cr0_wp();
  // Write the modified bytes into x64_sys_call memory
  memcpy((char *)symbol_x64_sys_call, "\x48\xB8",
          2); // movq syscall_table, %rax

  *(unsigned long *)(symbol_x64_sys_call + 2) =
      (unsigned long)syscall_table;
  memcpy((char *)symbol_x64_sys_call + 10, "\x48\x8d\x04\xF0", 4); // leaq  (%rax, %rsi, 8), %rax
  memcpy((char *)symbol_x64_sys_call + 14, "\x48\x8B\x00", 3); // movq  (%rax), %rax
  memcpy((char *)symbol_x64_sys_call + 17, "\xFF\xE0", 2); // jmp *%rax
  
  enable_register_cr0_wp();

  return ERR_SUCCESS;
}

void hook_remove_sys_call_table_x64(void) {
  pr_warn("crowarmor: Removing sys_call_table kernel patch ...\n");
  disable_register_cr0_wp();
  memcpy(symbol_x64_sys_call, x64_sys_call_recovery, 20);
  enable_register_cr0_wp();
}

#endif

ERR hook_init(struct crow **crow) {
  unsigned int i;

  syscall_table = (unsigned long **)kallsyms_lookup_name("sys_call_table");

  if (syscall_table == NULL) {
    pr_info("crowarmor: Failed to get syscall table\n");
    return ERR_FAILURE;
  }

  old_syscall_table = kmalloc(sizeof(void *) * __NR_syscalls, __GFP_HIGH);

  if (old_syscall_table == NULL)
    return ERR_FAILURE;

  crowarmor_syscall_table = kmalloc(sizeof(void *) * __NR_syscalls, __GFP_HIGH);

  if (crowarmor_syscall_table == NULL)
    return ERR_FAILURE;

  memcpy(old_syscall_table, syscall_table, sizeof(void *) * __NR_syscalls);

  for (i = 0; !IS_NULL_PTR(syscalls[i].new_syscall); i++)
    set_new_syscall(&syscalls[i]);

  memcpy(crowarmor_syscall_table, syscall_table,
         sizeof(void *) * __NR_syscalls);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 8, 0)

  pr_info("crowarmor: This version of the kernel was identified as no longer "
          "using sys_call_table, patching the kernel...\n");
  if (IS_ERR_FAILURE(hook_sys_call_table_x64())) {
    pr_warn(
        "crowarmor: Error in kernel patching, sys_call_table not restored\n");
    return ERR_FAILURE;
  }

#endif

  (*crow)->hook_is_actived = true;
  armor = crow;

  return ERR_SUCCESS;
}

void hook_end(void) {
  pr_warn("crowarmor: Hook syscalls shutdown ...\n");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 8, 0)
  hook_remove_sys_call_table_x64();
#endif

  for (unsigned int i = 0; !IS_NULL_PTR(syscalls[i].new_syscall); i++)
    set_old_syscall(&syscalls[i]);

  kfree(old_syscall_table);
  kfree(crowarmor_syscall_table);

  (*armor)->hook_is_actived = false;
}
