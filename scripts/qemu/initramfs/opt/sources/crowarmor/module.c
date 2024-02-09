#include <linux/kernel.h> /* We are doing kernel work */
#include <linux/module.h> /* Specifically, a module  */

#include "crowarmor.h"

int __init
init_module(void)
{
  ERR retval = ERR_SUCCESS;

  pr_info("crowamor: Starting driver crowarmor \n");

  if (!IS_ERR_FAILURE(retval) && !IS_ERR_FAILURE(hook_init()))
  {
    retval = ERR_SUCCESS;

    /*...*/
  }

  if (!IS_ERR_FAILURE(retval) &&
      !IS_ERR_FAILURE(inspector_registers_controls_init()))
  {
    retval = ERR_SUCCESS;

    /*...*/
  }

  return retval;
}

void __exit
cleanup_module(void)
{
  pr_warn("crowamor: Shutdown driver crowarmor\n");
  hook_end();
  inspector_registers_controls_end();
}

MODULE_AUTHOR("Moblog Security Researchers <https://www.moblog.in>");
MODULE_DESCRIPTION("Driver to detect anomalies");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0.0");