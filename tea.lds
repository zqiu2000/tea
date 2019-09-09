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

	. = ALIGN(8);
	.initcall0 : {
		_initcall0_start = ADDR(.initcall0);
                KEEP(*(.initcall0*));
		_initcall0_end = .;
        }

	. = ALIGN(8);
	.initcall1 : {
		_initcall1_start = ADDR(.initcall1);
                KEEP(*(.initcall1*));
		_initcall1_end = .;
        }

	. = ALIGN(8);
	.initcall2 : {
		_initcall2_start = ADDR(.initcall2);
                KEEP(*(.initcall2*));
		_initcall2_end = .;
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
	. += 0x2000;
	mem_log_end = .;
	/DISCARD/ : {
		*(.comment)
		*(.eh_frame*)
	}
}
