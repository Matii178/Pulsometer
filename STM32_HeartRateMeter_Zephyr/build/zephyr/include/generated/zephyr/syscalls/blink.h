/* auto-generated by gen_syscalls.py, don't edit */

#ifndef Z_INCLUDE_SYSCALLS_BLINK_H
#define Z_INCLUDE_SYSCALLS_BLINK_H


#include <zephyr/tracing/tracing_syscall.h>

#ifndef _ASMLANGUAGE

#include <stdarg.h>

#include <zephyr/syscall_list.h>
#include <zephyr/syscall.h>

#include <zephyr/linker/sections.h>


#ifdef __cplusplus
extern "C" {
#endif

extern int z_impl_blink_set_period_ms(const struct device * dev, unsigned int period_ms);

__pinned_func
static inline int blink_set_period_ms(const struct device * dev, unsigned int period_ms)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		union { uintptr_t x; const struct device * val; } parm0 = { .val = dev };
		union { uintptr_t x; unsigned int val; } parm1 = { .val = period_ms };
		return (int) arch_syscall_invoke2(parm0.x, parm1.x, K_SYSCALL_BLINK_SET_PERIOD_MS);
	}
#endif
	compiler_barrier();
	return z_impl_blink_set_period_ms(dev, period_ms);
}

#if defined(CONFIG_TRACING_SYSCALL)
#ifndef DISABLE_SYSCALL_TRACING

#define blink_set_period_ms(dev, period_ms) ({ 	int syscall__retval; 	sys_port_trace_syscall_enter(K_SYSCALL_BLINK_SET_PERIOD_MS, blink_set_period_ms, dev, period_ms); 	syscall__retval = blink_set_period_ms(dev, period_ms); 	sys_port_trace_syscall_exit(K_SYSCALL_BLINK_SET_PERIOD_MS, blink_set_period_ms, dev, period_ms, syscall__retval); 	syscall__retval; })
#endif
#endif


#ifdef __cplusplus
}
#endif

#endif
#endif /* include guard */
