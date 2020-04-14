/*
 *  cmpe283-1-amd.c - Kernel module for CMPE283 assignment 1
 *  (AMD version)
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

#define MAX_MSG 80

/*
 * struct capability_info
 *
 * Represents a single capability (bit number and description).
 * Used by report_capability to output SVM capabilities.
 */
struct capability_info {
	uint8_t bit;
	const char *name;
};

/*
 * See AMD Programmer Reference Guide Vol 3 Appendix E
 */
struct capability_info svm0a_edx[13] =
{
	{ 0, "SVM Nested Paging" },
	{ 1, "LBR Virtualization" },
	{ 2, "SVM Lock" },
	{ 3, "NRIP save support" },
	{ 4, "MSR TSC support"},
	{ 5, "VMCB clean bits upport" },
	{ 6, "VMCB TLB_Control support" },
	{ 7, "Decode assists support" },
	{ 10, "Pause intercept filter support" },
	{ 12, "PAUSE filter cycle count threshold support" },
	{ 13, "Advanced Virtual Interrupt Controller" },
	{ 15, "VMSAVE and VMLOAD virtualization" },
	{ 16, "Global Interrupt Flag virtualization" }
	/*  ... add the rest here ... */
};

/*
 * report_capability
 *
 * Reports capabilities present in 'cap'.
 *
 * Parameters:
 *  cap: capability_info structure for this feature
 *  len: number of entries in 'cap'
 *  function: CPUID function number
 *  reg: register to use (0 = eax, 1 = ebx, 2 = ecx, 3 = edx)
 */
void
report_capability(struct capability_info *cap, uint8_t len,
    uint32_t function, uint8_t reg)
{
	uint8_t i;
	uint32_t eax, ebx, ecx, edx, r;
	struct capability_info *c;
	char msg[MAX_MSG];

	memset(msg, 0, sizeof(msg));

	cpuid(function, &eax, &ebx, &ecx, &edx);

	switch (reg) {
	case 0: r = eax; break;
	case 1: r = ebx; break;
	case 2: r = ecx; break;
	case 3: r = edx; break;
	default: return;
	}

	for (i = 0; i < len; i++) {
		c = &cap[i];
		snprintf(msg, 79, "  %s: Can set=%s\n",
		    c->name,
		    (r & (1) << (c->bit)) ? "Yes" : "No");
		printk(msg);
	}
}

/*
 * detect_svm_features
 *
 * Detects and prints SVM capabilities of this host's CPU.
 */
void
detect_svm_features(void)
{
	report_capability(svm0a_edx,
	    13 /* CHANGE THIS */,
	    0x8000000A,
	    3); /* 3 == %edx */
}

/*
 * init_module
 *
 * Module entry point
 *
 * Return Values:
 *  Always 0
 */
int
init_module(void)
{
	printk(KERN_INFO "CMPE 283 Assignment 1 Module Start\n");

	detect_svm_features();

	/*
	 * A non 0 return means init_module failed; module can't be loaded.
	 */
	return 0;
}

/*
 * cleanup_module
 *
 * Function called on module unload
 */
void
cleanup_module(void)
{
	printk(KERN_INFO "CMPE 283 Assignment 1 Module Exits\n");
}
