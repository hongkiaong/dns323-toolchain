#ifndef _ASM_M32R_PROCESSOR_H
#define _ASM_M32R_PROCESSOR_H

/* $Id: processor.h,v 1.2 2004/10/22 16:58:47 mmazur Exp $ */

/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2001  by Hiroyuki Kondo, Hirokazu Takata, and Hitoshi Yamamoto
 */

/*
 * include/asm-m32r/processor.h
 *
 * Copyright (C) 1994 Linus Torvalds
 */
#include <linux/kernel.h>
#include <asm/cache.h>
#include <asm/ptrace.h>  /* pt_regs */

#include <asm/cachectl.h>

/*
 * Default implementation of macro that returns current
 * instruction pointer ("program counter").
 */
#define current_text_addr() ({ __label__ _l; _l: &&_l; })

/*
 *  CPU type and hardware bug flags. Kept separately for each CPU.
 *  Members of this structure are referenced in head.S, so think twice
 *  before touching them. [mj]
 */

struct cpuinfo_m32r {
	unsigned long pgtable_cache_sz;
	unsigned long cpu_clock;
	unsigned long bus_clock;
	unsigned long timer_divide;
	unsigned long loops_per_jiffy;
};

/*
 * capabilities of CPUs
 */

extern struct cpuinfo_m32r boot_cpu_data;

#ifdef CONFIG_SMP
extern struct cpuinfo_m32r cpu_data[];
#define current_cpu_data cpu_data[smp_processor_id()]
#else
#define cpu_data &boot_cpu_data
#define current_cpu_data boot_cpu_data
#endif

/*
 * User space process size: 2GB (default).
 */
#ifdef CONFIG_MMU
#define TASK_SIZE  (0x80000000UL)
#else
#define TASK_SIZE  (0x00400000UL)
#endif

/* This decides where the kernel will search for a free chunk of vm
 * space during mmap's.
 */
#define TASK_UNMAPPED_BASE	PAGE_ALIGN(TASK_SIZE / 3)

typedef struct {
	unsigned long seg;
} mm_segment_t;

struct debug_trap {
	int nr_trap;
	unsigned long	addr;
	unsigned long	insn;
};

struct thread_struct {
	unsigned long address;
	unsigned long trap_no;		/* Trap number  */
	unsigned long error_code;	/* Error code of trap */
	unsigned long lr;		/* saved pc */
	unsigned long sp;		/* user stack pointer */
	struct debug_trap debug_trap;
};

#define INIT_SP	(sizeof(init_stack) + (unsigned long) &init_stack)

#define INIT_THREAD	{	\
	.sp = INIT_SP,		\
}

/*
 * Do necessary setup to start up a newly executed thread.
 */

/* User process Backup PSW */
#define USERPS_BPSW (M32R_PSW_BSM|M32R_PSW_BIE|M32R_PSW_BPM)

#define start_thread(regs, new_pc, new_spu) 				\
	do {								\
		set_fs(USER_DS); 					\
		regs->psw = (regs->psw | USERPS_BPSW) & 0x0000FFFFUL;	\
		regs->bpc = new_pc;					\
		regs->spu = new_spu;					\
	} while (0)

/* Forward declaration, a strange C thing */
struct task_struct;
struct mm_struct;

/* Free all resources held by a thread. */
extern void release_thread(struct task_struct *);

#define prepare_to_copy(tsk)	do { } while (0)

/*
 * create a kernel thread without removing it from tasklists
 */
extern int kernel_thread(int (*fn)(void *), void * arg, unsigned long flags);

/* Copy and release all segment info associated with a VM */
extern void copy_segments(struct task_struct *p, struct mm_struct * mm);
extern void release_segments(struct mm_struct * mm);

extern unsigned long thread_saved_pc(struct task_struct *);

/* Copy and release all segment info associated with a VM */
#define copy_segments(p, mm)  do { } while (0)
#define release_segments(mm)  do { } while (0)

unsigned long get_wchan(struct task_struct *p);
#define KSTK_EIP(tsk)  ((tsk)->thread.lr)
#define KSTK_ESP(tsk)  ((tsk)->thread.sp)

#define THREAD_SIZE (2*PAGE_SIZE)

/* REP NOP (PAUSE) is a good thing to insert into busy-wait loops. */
static __inline__ void rep_nop(void)
{
	__asm__ __volatile__(
		"nop \n\t"
		"nop \n\t"
		:
		:
		: "memory");
}

#define cpu_relax()     rep_nop()

#endif /* _ASM_M32R_PROCESSOR_H */
