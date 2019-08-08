OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start16)

SECTIONS
{
	. = 0x1000;
	.trampoline : {
		*(.start16);
	}

	__text_start = .;
	.text  : {
		*(.text*);
		*(.isr_handle);
	}

	. = ALIGN(4);
	.data : { *(.data*); }
	.bss : { *(.bss*) }

	. = ALIGN(4096);
	_stack_end = .;
	. += 0x1000;
	_stack_start = .;

	. = ALIGN(4096);
	. = 0xa000;
	mem_log = .;
	. += 0x1000;
	mem_log_end = .;
	/DISCARD/ : {
		*(.comment)
		*(.eh_frame*)
	}
}
