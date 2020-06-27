#ifndef _VECTORS_H_
#define _VECTORS_H_

#define NMI_VECTOR			(2)
#define START_VECTOR		(32)

#define V_LAPIC_TIMER		(110)
#define I_LAPIC_TIMER		(V_LAPIC_TIMER - START_VECTOR)

/*
 * Borrow the platform IPI vector from Linux kernel to be the IPC irq
 */
#define V_X86_PLATFORM_IPI		(0xf7)
#define I_X86_PLATFORM_IPI		(V_X86_PLATFORM_IPI - START_VECTOR)

#endif
