PHDRS
{
  text PT_LOAD FILEHDR FLAGS (7) ;
  bss PT_LOAD ;
}

MEMORY 
{
  ram (rwx) : org = 0xabcdef000, l = 40K
  ephemeral (rw) : org = 0xfedcba000, l = 1K
}

SECTIONS
{
  .text : {
    *(.text.entry)
    *(.rodata*)
    *(.data)
    *(.text*)
    . = ALIGN(0x8000);
  } >ram :text =0x90
 /* NOP fill for consistent length */
  .bss : {
    *(.bss)
  } >ephemeral :bss =0x90

  /DISCARD/ : {
    *(.bss)
    *(.eh_frame)
  }

}

ENTRY(entry)
