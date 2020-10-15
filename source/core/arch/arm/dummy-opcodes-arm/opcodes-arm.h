#ifndef CORE_ARCH_INSTRUCTION_ARM_H
#define CORE_ARCH_INSTRUCTION_ARM_H

#include "./cpu-arm.h"

struct opcode32 {
  arm_feature_set arch;      /* Architecture defining this insn.  */
  unsigned long   value;     /* If arch is 0 then value is a sentinel.  */
  unsigned long   mask;      /* Recognise insn if (op & mask) == value.  */
  const char *    assembler; /* How to disassemble this insn.  */
};

struct opcode16 {
  arm_feature_set arch;        /* Architecture defining this insn.  */
  unsigned short  value, mask; /* Recognise insn if (op & mask) == value.  */
  const char *    assembler;   /* How to disassemble this insn.  */
};

/* print_insn_coprocessor recognizes the following format control codes:

   %%			%

   %c			print condition code (always bits 28-31 in ARM mode)
   %q			print shifter argument
   %u			print condition code (unconditional in ARM mode,
                          UNPREDICTABLE if not AL in Thumb)
   %A			print address for ldc/stc/ldf/stf instruction
   %B			print vstm/vldm register list
   %I                   print cirrus signed shift immediate: bits 0..3|4..6
   %F			print the COUNT field of a LFM/SFM instruction.
   %P			print floating point precision in arithmetic insn
   %Q			print floating point precision in ldf/stf insn
   %R			print floating point rounding mode

   %<bitfield>c		print as a condition code (for vsel)
   %<bitfield>r		print as an ARM register
   %<bitfield>R		as %<>r but r15 is UNPREDICTABLE
   %<bitfield>ru        as %<>r but each u register must be unique.
   %<bitfield>d		print the bitfield in decimal
   %<bitfield>k		print immediate for VFPv3 conversion instruction
   %<bitfield>x		print the bitfield in hex
   %<bitfield>X		print the bitfield as 1 hex digit without leading "0x"
   %<bitfield>f		print a floating point constant if >7 else a
      floating point register
   %<bitfield>w         print as an iWMMXt width field - [bhwd]ss/us
   %<bitfield>g         print as an iWMMXt 64-bit register
   %<bitfield>G         print as an iWMMXt general purpose or control register
   %<bitfield>D		print as a NEON D register
   %<bitfield>Q		print as a NEON Q register
   %<bitfield>V		print as a NEON D or Q register
   %<bitfield>E		print a quarter-float immediate value

   %y<code>		print a single precision VFP reg.
        Codes: 0=>Sm, 1=>Sd, 2=>Sn, 3=>multi-list, 4=>Sm pair
   %z<code>		print a double precision VFP reg
        Codes: 0=>Dm, 1=>Dd, 2=>Dn, 3=>multi-list

   %<bitfield>'c	print specified char iff bitfield is all ones
   %<bitfield>`c	print specified char iff bitfield is all zeroes
   %<bitfield>?ab...    select from array of values in big endian order

   %L			print as an iWMMXt N/M width field.
   %Z			print the Immediate of a WSHUFH instruction.
   %l			like 'A' except use byte offsets for 'B' & 'H'
      versions.
   %i			print 5-bit immediate in bits 8,3..0
      (print "32" when 0)
   %r			print register offset address for wldt/wstr instruction.  */

enum opcode_sentinel_enum { SENTINEL_IWMMXT_START = 1, SENTINEL_IWMMXT_END, SENTINEL_GENERIC_START } opcode_sentinels;

#define UNDEFINED_INSTRUCTION     "\t\t; <UNDEFINED> instruction: %0-31x"
#define UNPREDICTABLE_INSTRUCTION "\t; <UNPREDICTABLE>"

/* UnifiedInterface coprocessor opcodes shared between Arm and Thumb-2.  */

static const struct opcode32 coprocessor_opcodes[] = {
    /* XScale instructions.  */
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e200010, 0x0fff0ff0, "mia%c\tacc0, %0-3r, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e280010, 0x0fff0ff0, "miaph%c\tacc0, %0-3r, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e2c0010, 0x0ffc0ff0, "mia%17'T%17`B%16'T%16`B%c\tacc0, %0-3r, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0c400000, 0x0ff00fff, "mar%c\tacc0, %12-15r, %16-19r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0c500000, 0x0ff00fff, "mra%c\t%12-15r, %16-19r, acc0"},

    /* Intel Wireless MMX technology instructions.  */
    {ARM_FEATURE_CORE_LOW(0), SENTINEL_IWMMXT_START, 0, ""},
    {ARM_FEATURE_COPROC(ARM_CEXT_IWMMXT), 0x0e130130, 0x0f3f0fff, "tandc%22-23w%c\t%12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e400010, 0x0ff00f3f, "tbcst%6-7w%c\t%16-19g, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e130170, 0x0f3f0ff8, "textrc%22-23w%c\t%12-15r, #%0-2d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e100070, 0x0f300ff0, "textrm%3?su%22-23w%c\t%12-15r, %16-19g, #%0-2d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e600010, 0x0ff00f38, "tinsr%6-7w%c\t%16-19g, %12-15r, #%0-2d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000110, 0x0ff00fff, "tmcr%c\t%16-19G, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0c400000, 0x0ff00ff0, "tmcrr%c\t%0-3g, %12-15r, %16-19r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e2c0010, 0x0ffc0e10, "tmia%17?tb%16?tb%c\t%5-8g, %0-3r, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e200010, 0x0fff0e10, "tmia%c\t%5-8g, %0-3r, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e280010, 0x0fff0e10, "tmiaph%c\t%5-8g, %0-3r, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e100030, 0x0f300fff, "tmovmsk%22-23w%c\t%12-15r, %16-19g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e100110, 0x0ff00ff0, "tmrc%c\t%12-15r, %16-19G"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0c500000, 0x0ff00ff0, "tmrrc%c\t%12-15r, %16-19r, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e130150, 0x0f3f0fff, "torc%22-23w%c\t%12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e120190, 0x0f3f0fff, "torvsc%22-23w%c\t%12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e2001c0, 0x0f300fff, "wabs%22-23w%c\t%12-15g, %16-19g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e0001c0, 0x0f300fff, "wacc%22-23w%c\t%12-15g, %16-19g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000180, 0x0f000ff0, "wadd%20-23w%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e2001a0, 0x0fb00ff0, "waddbhus%22?ml%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0ea001a0, 0x0ff00ff0, "waddsubhx%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000020, 0x0f800ff0, "waligni%c\t%12-15g, %16-19g, %0-3g, #%20-22d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e800020, 0x0fc00ff0, "walignr%20-21d%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e200000, 0x0fe00ff0, "wand%20'n%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e800000, 0x0fa00ff0, "wavg2%22?hb%20'r%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e400000, 0x0fe00ff0, "wavg4%20'r%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000060, 0x0f300ff0, "wcmpeq%22-23w%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e100060, 0x0f100ff0, "wcmpgt%21?su%22-23w%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0xfc500100, 0xfe500f00, "wldrd\t%12-15g, %r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0xfc100100, 0xfe500f00, "wldrw\t%12-15G, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0c100000, 0x0e100e00, "wldr%L%c\t%12-15g, %l"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e400100, 0x0fc00ff0, "wmac%21?su%20'z%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e800100, 0x0fc00ff0, "wmadd%21?su%20'x%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0ec00100, 0x0fd00ff0, "wmadd%21?sun%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000160, 0x0f100ff0, "wmax%21?su%22-23w%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000080, 0x0f100fe0, "wmerge%c\t%12-15g, %16-19g, %0-3g, #%21-23d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e0000a0, 0x0f800ff0, "wmia%21?tb%20?tb%22'n%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e800120, 0x0f800ff0, "wmiaw%21?tb%20?tb%22'n%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e100160, 0x0f100ff0, "wmin%21?su%22-23w%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000100, 0x0fc00ff0, "wmul%21?su%20?ml%23'r%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0ed00100, 0x0fd00ff0, "wmul%21?sumr%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0ee000c0, 0x0fe00ff0, "wmulwsm%20`r%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0ec000c0, 0x0fe00ff0, "wmulwum%20`r%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0eb000c0, 0x0ff00ff0, "wmulwl%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e8000a0, 0x0f800ff0, "wqmia%21?tb%20?tb%22'n%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e100080, 0x0fd00ff0, "wqmulm%21'r%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0ec000e0, 0x0fd00ff0, "wqmulwm%21'r%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000000, 0x0ff00ff0, "wor%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000080, 0x0f000ff0, "wpack%20-23w%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0xfe300040, 0xff300ef0, "wror%22-23w\t%12-15g, %16-19g, #%i"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e300040, 0x0f300ff0, "wror%22-23w%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e300140, 0x0f300ff0, "wror%22-23wg%c\t%12-15g, %16-19g, %0-3G"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000120, 0x0fa00ff0, "wsad%22?hb%20'z%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e0001e0, 0x0f000ff0, "wshufh%c\t%12-15g, %16-19g, #%Z"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0xfe100040, 0xff300ef0, "wsll%22-23w\t%12-15g, %16-19g, #%i"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e100040, 0x0f300ff0, "wsll%22-23w%8'g%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e100148, 0x0f300ffc, "wsll%22-23w%8'g%c\t%12-15g, %16-19g, %0-3G"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0xfe000040, 0xff300ef0, "wsra%22-23w\t%12-15g, %16-19g, #%i"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000040, 0x0f300ff0, "wsra%22-23w%8'g%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e000148, 0x0f300ffc, "wsra%22-23w%8'g%c\t%12-15g, %16-19g, %0-3G"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0xfe200040, 0xff300ef0, "wsrl%22-23w\t%12-15g, %16-19g, #%i"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e200040, 0x0f300ff0, "wsrl%22-23w%8'g%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e200148, 0x0f300ffc, "wsrl%22-23w%8'g%c\t%12-15g, %16-19g, %0-3G"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0xfc400100, 0xfe500f00, "wstrd\t%12-15g, %r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0xfc000100, 0xfe500f00, "wstrw\t%12-15G, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0c000000, 0x0e100e00, "wstr%L%c\t%12-15g, %l"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e0001a0, 0x0f000ff0, "wsub%20-23w%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0ed001c0, 0x0ff00ff0, "wsubaddhx%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e1001c0, 0x0f300ff0, "wabsdiff%22-23w%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e0000c0, 0x0fd00fff, "wunpckeh%21?sub%c\t%12-15g, %16-19g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e4000c0, 0x0fd00fff, "wunpckeh%21?suh%c\t%12-15g, %16-19g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e8000c0, 0x0fd00fff, "wunpckeh%21?suw%c\t%12-15g, %16-19g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e0000e0, 0x0f100fff, "wunpckel%21?su%22-23w%c\t%12-15g, %16-19g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e1000c0, 0x0f300ff0, "wunpckih%22-23w%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e1000e0, 0x0f300ff0, "wunpckil%22-23w%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_COPROC(ARM_CEXT_XSCALE), 0x0e100000, 0x0ff00ff0, "wxor%c\t%12-15g, %16-19g, %0-3g"},
    {ARM_FEATURE_CORE_LOW(0), SENTINEL_IWMMXT_END, 0, ""},

    /* Floating point coprocessor (FPA) instructions.  */
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e000100, 0x0ff08f10, "adf%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e100100, 0x0ff08f10, "muf%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e200100, 0x0ff08f10, "suf%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e300100, 0x0ff08f10, "rsf%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e400100, 0x0ff08f10, "dvf%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e500100, 0x0ff08f10, "rdf%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e600100, 0x0ff08f10, "pow%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e700100, 0x0ff08f10, "rpw%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e800100, 0x0ff08f10, "rmf%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e900100, 0x0ff08f10, "fml%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0ea00100, 0x0ff08f10, "fdv%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0eb00100, 0x0ff08f10, "frd%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0ec00100, 0x0ff08f10, "pol%c%P%R\t%12-14f, %16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e008100, 0x0ff08f10, "mvf%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e108100, 0x0ff08f10, "mnf%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e208100, 0x0ff08f10, "abs%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e308100, 0x0ff08f10, "rnd%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e408100, 0x0ff08f10, "sqt%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e508100, 0x0ff08f10, "log%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e608100, 0x0ff08f10, "lgn%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e708100, 0x0ff08f10, "exp%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e808100, 0x0ff08f10, "sin%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e908100, 0x0ff08f10, "cos%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0ea08100, 0x0ff08f10, "tan%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0eb08100, 0x0ff08f10, "asn%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0ec08100, 0x0ff08f10, "acs%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0ed08100, 0x0ff08f10, "atn%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0ee08100, 0x0ff08f10, "urd%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0ef08100, 0x0ff08f10, "nrm%c%P%R\t%12-14f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e000110, 0x0ff00f1f, "flt%c%P%R\t%16-18f, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e100110, 0x0fff0f98, "fix%c%R\t%12-15r, %0-2f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e200110, 0x0fff0fff, "wfs%c\t%12-15r"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e300110, 0x0fff0fff, "rfs%c\t%12-15r"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e400110, 0x0fff0fff, "wfc%c\t%12-15r"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e500110, 0x0fff0fff, "rfc%c\t%12-15r"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0e90f110, 0x0ff8fff0, "cmf%c\t%16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0eb0f110, 0x0ff8fff0, "cnf%c\t%16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0ed0f110, 0x0ff8fff0, "cmfe%c\t%16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0ef0f110, 0x0ff8fff0, "cnfe%c\t%16-18f, %0-3f"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0c000100, 0x0e100f00, "stf%c%Q\t%12-14f, %A"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V1), 0x0c100100, 0x0e100f00, "ldf%c%Q\t%12-14f, %A"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V2), 0x0c000200, 0x0e100f00, "sfm%c\t%12-14f, %F, %A"},
    {ARM_FEATURE_COPROC(FPU_FPA_EXT_V2), 0x0c100200, 0x0e100f00, "lfm%c\t%12-14f, %F, %A"},

    /* ARMv8-M Mainline Security Extensions instructions.  */
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8M_MAIN), 0xec300a00, 0xfff0ffff, "vlldm\t%16-19r"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8M_MAIN), 0xec200a00, 0xfff0ffff, "vlstm\t%16-19r"},

    /* Register load/store.  */
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD | FPU_NEON_EXT_V1), 0x0d2d0b00, 0x0fbf0f01, "vpush%c\t%B"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD | FPU_NEON_EXT_V1), 0x0d200b00, 0x0fb00f01, "vstmdb%c\t%16-19r!, %B"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD | FPU_NEON_EXT_V1), 0x0d300b00, 0x0fb00f01, "vldmdb%c\t%16-19r!, %B"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD | FPU_NEON_EXT_V1), 0x0c800b00, 0x0f900f01, "vstmia%c\t%16-19r%21'!, %B"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD | FPU_NEON_EXT_V1), 0x0cbd0b00, 0x0fbf0f01, "vpop%c\t%B"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD | FPU_NEON_EXT_V1), 0x0c900b00, 0x0f900f01, "vldmia%c\t%16-19r%21'!, %B"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD | FPU_NEON_EXT_V1), 0x0d000b00, 0x0f300f00, "vstr%c\t%12-15,22D, %A"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD | FPU_NEON_EXT_V1), 0x0d100b00, 0x0f300f00, "vldr%c\t%12-15,22D, %A"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0d2d0a00, 0x0fbf0f00, "vpush%c\t%y3"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0d200a00, 0x0fb00f00, "vstmdb%c\t%16-19r!, %y3"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0d300a00, 0x0fb00f00, "vldmdb%c\t%16-19r!, %y3"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0c800a00, 0x0f900f00, "vstmia%c\t%16-19r%21'!, %y3"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0cbd0a00, 0x0fbf0f00, "vpop%c\t%y3"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0c900a00, 0x0f900f00, "vldmia%c\t%16-19r%21'!, %y3"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0d000a00, 0x0f300f00, "vstr%c\t%y1, %A"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0d100a00, 0x0f300f00, "vldr%c\t%y1, %A"},

    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0d200b01, 0x0fb00f01, "fstmdbx%c\t%16-19r!, %z3\t;@ Deprecated"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0d300b01, 0x0fb00f01, "fldmdbx%c\t%16-19r!, %z3\t;@ Deprecated"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0c800b01, 0x0f900f01, "fstmiax%c\t%16-19r%21'!, %z3\t;@ Deprecated"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0c900b01, 0x0f900f01, "fldmiax%c\t%16-19r%21'!, %z3\t;@ Deprecated"},

    /* Data transfer between ARM and NEON registers.  */
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0e800b10, 0x0ff00f70, "vdup%c.32\t%16-19,7D, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0e800b30, 0x0ff00f70, "vdup%c.16\t%16-19,7D, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0ea00b10, 0x0ff00f70, "vdup%c.32\t%16-19,7Q, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0ea00b30, 0x0ff00f70, "vdup%c.16\t%16-19,7Q, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0ec00b10, 0x0ff00f70, "vdup%c.8\t%16-19,7D, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0ee00b10, 0x0ff00f70, "vdup%c.8\t%16-19,7Q, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0c400b10, 0x0ff00fd0, "vmov%c\t%0-3,5D, %12-15r, %16-19r"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0c500b10, 0x0ff00fd0, "vmov%c\t%12-15r, %16-19r, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0e000b10, 0x0fd00f70, "vmov%c.32\t%16-19,7D[%21d], %12-15r"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0e100b10, 0x0f500f70, "vmov%c.32\t%12-15r, %16-19,7D[%21d]"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0e000b30, 0x0fd00f30, "vmov%c.16\t%16-19,7D[%6,21d], %12-15r"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0e100b30, 0x0f500f30, "vmov%c.%23?us16\t%12-15r, %16-19,7D[%6,21d]"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0e400b10, 0x0fd00f10, "vmov%c.8\t%16-19,7D[%5,6,21d], %12-15r"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0x0e500b10, 0x0f500f10, "vmov%c.%23?us8\t%12-15r, %16-19,7D[%5,6,21d]"},
    /* Half-precision conversion instructions.  */
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0x0eb20b40, 0x0fbf0f50, "vcvt%7?tb%c.f64.f16\t%z1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0x0eb30b40, 0x0fbf0f50, "vcvt%7?tb%c.f16.f64\t%y1, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FP16), 0x0eb20a40, 0x0fbf0f50, "vcvt%7?tb%c.f32.f16\t%y1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FP16), 0x0eb30a40, 0x0fbf0f50, "vcvt%7?tb%c.f16.f32\t%y1, %y0"},

    /* Floating point coprocessor (VFP) instructions.  */
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ee00a10, 0x0fff0fff, "vmsr%c\tfpsid, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ee10a10, 0x0fff0fff, "vmsr%c\tfpscr, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ee60a10, 0x0fff0fff, "vmsr%c\tmvfr1, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ee70a10, 0x0fff0fff, "vmsr%c\tmvfr0, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0x0ee50a10, 0x0fff0fff, "vmsr%c\tmvfr2, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ee80a10, 0x0fff0fff, "vmsr%c\tfpexc, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ee90a10, 0x0fff0fff, "vmsr%c\tfpinst, %12-15r\t@ Impl def"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0eea0a10, 0x0fff0fff, "vmsr%c\tfpinst2, %12-15r\t@ Impl def"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ef00a10, 0x0fff0fff, "vmrs%c\t%12-15r, fpsid"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ef1fa10, 0x0fffffff, "vmrs%c\tAPSR_nzcv, fpscr"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ef10a10, 0x0fff0fff, "vmrs%c\t%12-15r, fpscr"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0x0ef50a10, 0x0fff0fff, "vmrs%c\t%12-15r, mvfr2"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ef60a10, 0x0fff0fff, "vmrs%c\t%12-15r, mvfr1"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ef70a10, 0x0fff0fff, "vmrs%c\t%12-15r, mvfr0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ef80a10, 0x0fff0fff, "vmrs%c\t%12-15r, fpexc"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ef90a10, 0x0fff0fff, "vmrs%c\t%12-15r, fpinst\t@ Impl def"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0efa0a10, 0x0fff0fff, "vmrs%c\t%12-15r, fpinst2\t@ Impl def"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0e000b10, 0x0fd00fff, "vmov%c.32\t%z2[%21d], %12-15r"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0e100b10, 0x0fd00fff, "vmov%c.32\t%12-15r, %z2[%21d]"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ee00a10, 0x0ff00fff, "vmsr%c\t<impl def %16-19x>, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ef00a10, 0x0ff00fff, "vmrs%c\t%12-15r, <impl def %16-19x>"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0e000a10, 0x0ff00f7f, "vmov%c\t%y2, %12-15r"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0e100a10, 0x0ff00f7f, "vmov%c\t%12-15r, %y2"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0eb50a40, 0x0fbf0f70, "vcmp%7'e%c.f32\t%y1, #0.0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0eb50b40, 0x0fbf0f70, "vcmp%7'e%c.f64\t%z1, #0.0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0eb00a40, 0x0fbf0fd0, "vmov%c.f32\t%y1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0eb00ac0, 0x0fbf0fd0, "vabs%c.f32\t%y1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0eb00b40, 0x0fbf0fd0, "vmov%c.f64\t%z1, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0eb00bc0, 0x0fbf0fd0, "vabs%c.f64\t%z1, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0eb10a40, 0x0fbf0fd0, "vneg%c.f32\t%y1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0eb10ac0, 0x0fbf0fd0, "vsqrt%c.f32\t%y1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0eb10b40, 0x0fbf0fd0, "vneg%c.f64\t%z1, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0eb10bc0, 0x0fbf0fd0, "vsqrt%c.f64\t%z1, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0eb70ac0, 0x0fbf0fd0, "vcvt%c.f64.f32\t%z1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0eb70bc0, 0x0fbf0fd0, "vcvt%c.f32.f64\t%y1, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0eb80a40, 0x0fbf0f50, "vcvt%c.f32.%7?su32\t%y1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0eb80b40, 0x0fbf0f50, "vcvt%c.f64.%7?su32\t%z1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0eb40a40, 0x0fbf0f50, "vcmp%7'e%c.f32\t%y1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0eb40b40, 0x0fbf0f50, "vcmp%7'e%c.f64\t%z1, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V3xD), 0x0eba0a40, 0x0fbe0f50, "vcvt%c.f32.%16?us%7?31%7?26\t%y1, %y1, #%5,0-3k"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V3), 0x0eba0b40, 0x0fbe0f50, "vcvt%c.f64.%16?us%7?31%7?26\t%z1, %z1, #%5,0-3k"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0ebc0a40, 0x0fbe0f50, "vcvt%7`r%c.%16?su32.f32\t%y1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0ebc0b40, 0x0fbe0f50, "vcvt%7`r%c.%16?su32.f64\t%y1, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V3xD), 0x0ebe0a40, 0x0fbe0f50, "vcvt%c.%16?us%7?31%7?26.f32\t%y1, %y1, #%5,0-3k"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V3), 0x0ebe0b40, 0x0fbe0f50, "vcvt%c.%16?us%7?31%7?26.f64\t%z1, %z1, #%5,0-3k"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0c500b10, 0x0fb00ff0, "vmov%c\t%12-15r, %16-19r, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V3xD), 0x0eb00a00, 0x0fb00ff0, "vmov%c.f32\t%y1, #%0-3,16-19E"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V3), 0x0eb00b00, 0x0fb00ff0, "vmov%c.f64\t%z1, #%0-3,16-19E"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V2), 0x0c400a10, 0x0ff00fd0, "vmov%c\t%y4, %12-15r, %16-19r"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V2), 0x0c400b10, 0x0ff00fd0, "vmov%c\t%z0, %12-15r, %16-19r"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V2), 0x0c500a10, 0x0ff00fd0, "vmov%c\t%12-15r, %16-19r, %y4"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0e000a00, 0x0fb00f50, "vmla%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0e000a40, 0x0fb00f50, "vmls%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0e000b00, 0x0fb00f50, "vmla%c.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0e000b40, 0x0fb00f50, "vmls%c.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0e100a00, 0x0fb00f50, "vnmls%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0e100a40, 0x0fb00f50, "vnmla%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0e100b00, 0x0fb00f50, "vnmls%c.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0e100b40, 0x0fb00f50, "vnmla%c.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0e200a00, 0x0fb00f50, "vmul%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0e200a40, 0x0fb00f50, "vnmul%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0e200b00, 0x0fb00f50, "vmul%c.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0e200b40, 0x0fb00f50, "vnmul%c.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0e300a00, 0x0fb00f50, "vadd%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0e300a40, 0x0fb00f50, "vsub%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0e300b00, 0x0fb00f50, "vadd%c.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0e300b40, 0x0fb00f50, "vsub%c.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1xD), 0x0e800a00, 0x0fb00f50, "vdiv%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_V1), 0x0e800b00, 0x0fb00f50, "vdiv%c.f64\t%z1, %z2, %z0"},

    /* Cirrus coprocessor instructions.  */
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0d100400, 0x0f500f00, "cfldrs%c\tmvf%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0c100400, 0x0f500f00, "cfldrs%c\tmvf%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0d500400, 0x0f500f00, "cfldrd%c\tmvd%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0c500400, 0x0f500f00, "cfldrd%c\tmvd%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0d100500, 0x0f500f00, "cfldr32%c\tmvfx%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0c100500, 0x0f500f00, "cfldr32%c\tmvfx%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0d500500, 0x0f500f00, "cfldr64%c\tmvdx%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0c500500, 0x0f500f00, "cfldr64%c\tmvdx%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0d000400, 0x0f500f00, "cfstrs%c\tmvf%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0c000400, 0x0f500f00, "cfstrs%c\tmvf%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0d400400, 0x0f500f00, "cfstrd%c\tmvd%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0c400400, 0x0f500f00, "cfstrd%c\tmvd%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0d000500, 0x0f500f00, "cfstr32%c\tmvfx%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0c000500, 0x0f500f00, "cfstr32%c\tmvfx%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0d400500, 0x0f500f00, "cfstr64%c\tmvdx%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0c400500, 0x0f500f00, "cfstr64%c\tmvdx%12-15d, %A"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000450, 0x0ff00ff0, "cfmvsr%c\tmvf%16-19d, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100450, 0x0ff00ff0, "cfmvrs%c\t%12-15r, mvf%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000410, 0x0ff00ff0, "cfmvdlr%c\tmvd%16-19d, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100410, 0x0ff00ff0, "cfmvrdl%c\t%12-15r, mvd%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000430, 0x0ff00ff0, "cfmvdhr%c\tmvd%16-19d, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100430, 0x0ff00fff, "cfmvrdh%c\t%12-15r, mvd%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000510, 0x0ff00fff, "cfmv64lr%c\tmvdx%16-19d, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100510, 0x0ff00fff, "cfmvr64l%c\t%12-15r, mvdx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000530, 0x0ff00fff, "cfmv64hr%c\tmvdx%16-19d, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100530, 0x0ff00fff, "cfmvr64h%c\t%12-15r, mvdx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e200440, 0x0ff00fff, "cfmval32%c\tmvax%12-15d, mvfx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100440, 0x0ff00fff, "cfmv32al%c\tmvfx%12-15d, mvax%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e200460, 0x0ff00fff, "cfmvam32%c\tmvax%12-15d, mvfx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100460, 0x0ff00fff, "cfmv32am%c\tmvfx%12-15d, mvax%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e200480, 0x0ff00fff, "cfmvah32%c\tmvax%12-15d, mvfx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100480, 0x0ff00fff, "cfmv32ah%c\tmvfx%12-15d, mvax%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e2004a0, 0x0ff00fff, "cfmva32%c\tmvax%12-15d, mvfx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e1004a0, 0x0ff00fff, "cfmv32a%c\tmvfx%12-15d, mvax%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e2004c0, 0x0ff00fff, "cfmva64%c\tmvax%12-15d, mvdx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e1004c0, 0x0ff00fff, "cfmv64a%c\tmvdx%12-15d, mvax%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e2004e0, 0x0fff0fff, "cfmvsc32%c\tdspsc, mvdx%12-15d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e1004e0, 0x0fff0fff, "cfmv32sc%c\tmvdx%12-15d, dspsc"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000400, 0x0ff00fff, "cfcpys%c\tmvf%12-15d, mvf%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000420, 0x0ff00fff, "cfcpyd%c\tmvd%12-15d, mvd%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000460, 0x0ff00fff, "cfcvtsd%c\tmvd%12-15d, mvf%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000440, 0x0ff00fff, "cfcvtds%c\tmvf%12-15d, mvd%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000480, 0x0ff00fff, "cfcvt32s%c\tmvf%12-15d, mvfx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e0004a0, 0x0ff00fff, "cfcvt32d%c\tmvd%12-15d, mvfx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e0004c0, 0x0ff00fff, "cfcvt64s%c\tmvf%12-15d, mvdx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e0004e0, 0x0ff00fff, "cfcvt64d%c\tmvd%12-15d, mvdx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100580, 0x0ff00fff, "cfcvts32%c\tmvfx%12-15d, mvf%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e1005a0, 0x0ff00fff, "cfcvtd32%c\tmvfx%12-15d, mvd%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e1005c0, 0x0ff00fff, "cftruncs32%c\tmvfx%12-15d, mvf%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e1005e0, 0x0ff00fff, "cftruncd32%c\tmvfx%12-15d, mvd%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000550, 0x0ff00ff0, "cfrshl32%c\tmvfx%16-19d, mvfx%0-3d, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000570, 0x0ff00ff0, "cfrshl64%c\tmvdx%16-19d, mvdx%0-3d, %12-15r"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000500, 0x0ff00f10, "cfsh32%c\tmvfx%12-15d, mvfx%16-19d, #%I"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e200500, 0x0ff00f10, "cfsh64%c\tmvdx%12-15d, mvdx%16-19d, #%I"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100490, 0x0ff00ff0, "cfcmps%c\t%12-15r, mvf%16-19d, mvf%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e1004b0, 0x0ff00ff0, "cfcmpd%c\t%12-15r, mvd%16-19d, mvd%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100590, 0x0ff00ff0, "cfcmp32%c\t%12-15r, mvfx%16-19d, mvfx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e1005b0, 0x0ff00ff0, "cfcmp64%c\t%12-15r, mvdx%16-19d, mvdx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e300400, 0x0ff00fff, "cfabss%c\tmvf%12-15d, mvf%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e300420, 0x0ff00fff, "cfabsd%c\tmvd%12-15d, mvd%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e300440, 0x0ff00fff, "cfnegs%c\tmvf%12-15d, mvf%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e300460, 0x0ff00fff, "cfnegd%c\tmvd%12-15d, mvd%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e300480, 0x0ff00ff0, "cfadds%c\tmvf%12-15d, mvf%16-19d, mvf%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e3004a0, 0x0ff00ff0, "cfaddd%c\tmvd%12-15d, mvd%16-19d, mvd%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e3004c0, 0x0ff00ff0, "cfsubs%c\tmvf%12-15d, mvf%16-19d, mvf%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e3004e0, 0x0ff00ff0, "cfsubd%c\tmvd%12-15d, mvd%16-19d, mvd%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100400, 0x0ff00ff0, "cfmuls%c\tmvf%12-15d, mvf%16-19d, mvf%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100420, 0x0ff00ff0, "cfmuld%c\tmvd%12-15d, mvd%16-19d, mvd%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e300500, 0x0ff00fff, "cfabs32%c\tmvfx%12-15d, mvfx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e300520, 0x0ff00fff, "cfabs64%c\tmvdx%12-15d, mvdx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e300540, 0x0ff00fff, "cfneg32%c\tmvfx%12-15d, mvfx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e300560, 0x0ff00fff, "cfneg64%c\tmvdx%12-15d, mvdx%16-19d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e300580, 0x0ff00ff0, "cfadd32%c\tmvfx%12-15d, mvfx%16-19d, mvfx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e3005a0, 0x0ff00ff0, "cfadd64%c\tmvdx%12-15d, mvdx%16-19d, mvdx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e3005c0, 0x0ff00ff0, "cfsub32%c\tmvfx%12-15d, mvfx%16-19d, mvfx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e3005e0, 0x0ff00ff0, "cfsub64%c\tmvdx%12-15d, mvdx%16-19d, mvdx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100500, 0x0ff00ff0, "cfmul32%c\tmvfx%12-15d, mvfx%16-19d, mvfx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100520, 0x0ff00ff0, "cfmul64%c\tmvdx%12-15d, mvdx%16-19d, mvdx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100540, 0x0ff00ff0, "cfmac32%c\tmvfx%12-15d, mvfx%16-19d, mvfx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100560, 0x0ff00ff0, "cfmsc32%c\tmvfx%12-15d, mvfx%16-19d, mvfx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e000600, 0x0ff00f10,
     "cfmadd32%c\tmvax%5-7d, mvfx%12-15d, mvfx%16-19d, mvfx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e100600, 0x0ff00f10,
     "cfmsub32%c\tmvax%5-7d, mvfx%12-15d, mvfx%16-19d, mvfx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e200600, 0x0ff00f10,
     "cfmadda32%c\tmvax%5-7d, mvax%12-15d, mvfx%16-19d, mvfx%0-3d"},
    {ARM_FEATURE_COPROC(ARM_CEXT_MAVERICK), 0x0e300600, 0x0ff00f10,
     "cfmsuba32%c\tmvax%5-7d, mvax%12-15d, mvfx%16-19d, mvfx%0-3d"},

    /* VFP Fused multiply add instructions.  */
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FMA), 0x0ea00a00, 0x0fb00f50, "vfma%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FMA), 0x0ea00b00, 0x0fb00f50, "vfma%c.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FMA), 0x0ea00a40, 0x0fb00f50, "vfms%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FMA), 0x0ea00b40, 0x0fb00f50, "vfms%c.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FMA), 0x0e900a40, 0x0fb00f50, "vfnma%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FMA), 0x0e900b40, 0x0fb00f50, "vfnma%c.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FMA), 0x0e900a00, 0x0fb00f50, "vfnms%c.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FMA), 0x0e900b00, 0x0fb00f50, "vfnms%c.f64\t%z1, %z2, %z0"},

    /* FP v5.  */
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0xfe000a00, 0xff800f50, "vsel%20-21c%u.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0xfe000b00, 0xff800f50, "vsel%20-21c%u.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0xfe800a00, 0xffb00f50, "vmaxnm%u.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0xfe800b00, 0xffb00f50, "vmaxnm%u.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0xfe800a40, 0xffb00f50, "vminnm%u.f32\t%y1, %y2, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0xfe800b40, 0xffb00f50, "vminnm%u.f64\t%z1, %z2, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0xfebc0a40, 0xffbc0f50, "vcvt%16-17?mpna%u.%7?su32.f32\t%y1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0xfebc0b40, 0xffbc0f50, "vcvt%16-17?mpna%u.%7?su32.f64\t%y1, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0x0eb60a40, 0x0fbe0f50, "vrint%7,16??xzr%c.f32\t%y1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0x0eb60b40, 0x0fbe0f50, "vrint%7,16??xzr%c.f64\t%z1, %z0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0xfeb80a40, 0xffbc0fd0, "vrint%16-17?mpna%u.f32\t%y1, %y0"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_ARMV8), 0xfeb80b40, 0xffbc0fd0, "vrint%16-17?mpna%u.f64\t%z1, %z0"},

    /* Generic coprocessor instructions.  */
    {ARM_FEATURE_CORE_LOW(0), SENTINEL_GENERIC_START, 0, ""},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5E), 0x0c400000, 0x0ff00000, "mcrr%c\t%8-11d, %4-7d, %12-15R, %16-19r, cr%0-3d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5E), 0x0c500000, 0x0ff00000, "mrrc%c\t%8-11d, %4-7d, %12-15Ru, %16-19Ru, cr%0-3d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V2), 0x0e000000, 0x0f000010,
     "cdp%c\t%8-11d, %20-23d, cr%12-15d, cr%16-19d, cr%0-3d, {%5-7d}"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V2), 0x0e10f010, 0x0f10f010,
     "mrc%c\t%8-11d, %21-23d, APSR_nzcv, cr%16-19d, cr%0-3d, {%5-7d}"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V2), 0x0e100010, 0x0f100010,
     "mrc%c\t%8-11d, %21-23d, %12-15r, cr%16-19d, cr%0-3d, {%5-7d}"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V2), 0x0e000010, 0x0f100010,
     "mcr%c\t%8-11d, %21-23d, %12-15R, cr%16-19d, cr%0-3d, {%5-7d}"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V2), 0x0c000000, 0x0e100000, "stc%22'l%c\t%8-11d, cr%12-15d, %A"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V2), 0x0c100000, 0x0e100000, "ldc%22'l%c\t%8-11d, cr%12-15d, %A"},

    /* V6 coprocessor instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xfc500000, 0xfff00000, "mrrc2%c\t%8-11d, %4-7d, %12-15Ru, %16-19Ru, cr%0-3d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xfc400000, 0xfff00000, "mcrr2%c\t%8-11d, %4-7d, %12-15R, %16-19R, cr%0-3d"},

    /* ARMv8.3 AdvSIMD instructions in the space of coprocessor 8.  */
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8_3A), 0xfc800800, 0xfeb00f10,
     "vcadd%c.f16\t%12-15,22V, %16-19,7V, %0-3,5V, #%24?29%24'70"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8_3A), 0xfc900800, 0xfeb00f10,
     "vcadd%c.f32\t%12-15,22V, %16-19,7V, %0-3,5V, #%24?29%24'70"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8_3A), 0xfc200800, 0xff300f10,
     "vcmla%c.f16\t%12-15,22V, %16-19,7V, %0-3,5V, #%23'90"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8_3A), 0xfd200800, 0xff300f10,
     "vcmla%c.f16\t%12-15,22V, %16-19,7V, %0-3,5V, #%23?21%23?780"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8_3A), 0xfc300800, 0xff300f10,
     "vcmla%c.f32\t%12-15,22V, %16-19,7V, %0-3,5V, #%23'90"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8_3A), 0xfd300800, 0xff300f10,
     "vcmla%c.f32\t%12-15,22V, %16-19,7V, %0-3,5V, #%23?21%23?780"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8_3A), 0xfe000800, 0xffa00f10,
     "vcmla%c.f16\t%12-15,22V, %16-19,7V, %0-3D[%5?10], #%20'90"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8_3A), 0xfe200800, 0xffa00f10,
     "vcmla%c.f16\t%12-15,22V, %16-19,7V, %0-3D[%5?10], #%20?21%20?780"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8_3A), 0xfe800800, 0xffa00f10,
     "vcmla%c.f32\t%12-15,22V, %16-19,7V, %0-3,5D[0], #%20'90"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8_3A), 0xfea00800, 0xffa00f10,
     "vcmla%c.f32\t%12-15,22V, %16-19,7V, %0-3,5D[0], #%20?21%20?780"},

    /* Dot Product instructions in the space of coprocessor 13.  */
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_DOTPROD), 0xfc200d00, 0xffb00f00,
     "v%4?usdot.%4?us8\t%12-15,22V, %16-19,7V, %0-3,5V"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_DOTPROD), 0xfe000d00, 0xff000f00,
     "v%4?usdot.%4?us8\t%12-15,22V, %16-19,7V, %0-3D[%5?10]"},

    /* ARMv8.2 FMAC Long instructions in the space of coprocessor 8.  */
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST | ARM_EXT2_V8_2A), 0xfc200810, 0xffb00f50,
     "vfmal.f16\t%12-15,22D, s%7,16-19d, s%5,0-3d"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST | ARM_EXT2_V8_2A), 0xfca00810, 0xffb00f50,
     "vfmsl.f16\t%12-15,22D, s%7,16-19d, s%5,0-3d"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST | ARM_EXT2_V8_2A), 0xfc200850, 0xffb00f50,
     "vfmal.f16\t%12-15,22Q, d%16-19,7d, d%0-3,5d"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST | ARM_EXT2_V8_2A), 0xfca00850, 0xffb00f50,
     "vfmsl.f16\t%12-15,22Q, d%16-19,7d, d%0-3,5d"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST | ARM_EXT2_V8_2A), 0xfe000810, 0xffb00f50,
     "vfmal.f16\t%12-15,22D, s%7,16-19d, s%5,0-2d[%3d]"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST | ARM_EXT2_V8_2A), 0xfe100810, 0xffb00f50,
     "vfmsl.f16\t%12-15,22D, s%7,16-19d, s%5,0-2d[%3d]"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST | ARM_EXT2_V8_2A), 0xfe000850, 0xffb00f50,
     "vfmal.f16\t%12-15,22Q, d%16-19,7d, d%0-2d[%3,5d]"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST | ARM_EXT2_V8_2A), 0xfe100850, 0xffb00f50,
     "vfmsl.f16\t%12-15,22Q, d%16-19,7d, d%0-2d[%3,5d]"},

    /* V5 coprocessor instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5), 0xfc100000, 0xfe100000, "ldc2%22'l%c\t%8-11d, cr%12-15d, %A"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5), 0xfc000000, 0xfe100000, "stc2%22'l%c\t%8-11d, cr%12-15d, %A"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5), 0xfe000000, 0xff000010,
     "cdp2%c\t%8-11d, %20-23d, cr%12-15d, cr%16-19d, cr%0-3d, {%5-7d}"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5), 0xfe000010, 0xff100010,
     "mcr2%c\t%8-11d, %21-23d, %12-15R, cr%16-19d, cr%0-3d, {%5-7d}"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5), 0xfe100010, 0xff100010,
     "mrc2%c\t%8-11d, %21-23d, %12-15r, cr%16-19d, cr%0-3d, {%5-7d}"},

    /* ARMv8.2 half-precision Floating point coprocessor 9 (VFP) instructions.
       cp_num: bit <11:8> == 0b1001.
       cond: bit <31:28> == 0b1110, otherwise, it's UNPREDICTABLE.  */
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0eb009c0, 0x0fbf0fd0, "vabs%c.f16\t%y1, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e300900, 0x0fb00f50, "vadd%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0eb40940, 0x0fbf0f50, "vcmp%7'e%c.f16\t%y1, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0eb50940, 0x0fbf0f70, "vcmp%7'e%c.f16\t%y1, #0.0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0eba09c0, 0x0fbe0fd0,
     "vcvt%c.f16.%16?us%7?31%7?26\t%y1, %y1, #%5,0-3k"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0ebe09c0, 0x0fbe0fd0,
     "vcvt%c.%16?us%7?31%7?26.f16\t%y1, %y1, #%5,0-3k"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0ebc0940, 0x0fbe0f50, "vcvt%7`r%c.%16?su32.f16\t%y1, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0eb80940, 0x0fbf0f50, "vcvt%c.f16.%7?su32\t%y1, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xfebc0940, 0xffbc0f50, "vcvt%16-17?mpna%u.%7?su32.f16\t%y1, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e800900, 0x0fb00f50, "vdiv%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0ea00900, 0x0fb00f50, "vfma%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0ea00940, 0x0fb00f50, "vfms%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e900940, 0x0fb00f50, "vfnma%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e900900, 0x0fb00f50, "vfnms%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xfeb00ac0, 0xffbf0fd0, "vins.f16\t%y1, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xfeb00a40, 0xffbf0fd0, "vmovx%c.f16\t%y1, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0d100900, 0x0f300f00, "vldr%c.16\t%y1, %A"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0d000900, 0x0f300f00, "vstr%c.16\t%y1, %A"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xfe800900, 0xffb00f50, "vmaxnm%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xfe800940, 0xffb00f50, "vminnm%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e000900, 0x0fb00f50, "vmla%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e000940, 0x0fb00f50, "vmls%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e100910, 0x0ff00f7f, "vmov%c.f16\t%12-15r, %y2"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e000910, 0x0ff00f7f, "vmov%c.f16\t%y2, %12-15r"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xeb00900, 0x0fb00ff0, "vmov%c.f16\t%y1, #%0-3,16-19E"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e200900, 0x0fb00f50, "vmul%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0eb10940, 0x0fbf0fd0, "vneg%c.f16\t%y1, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e100940, 0x0fb00f50, "vnmla%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e100900, 0x0fb00f50, "vnmls%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e200940, 0x0fb00f50, "vnmul%c.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0eb60940, 0x0fbe0f50, "vrint%7,16??xzr%c.f16\t%y1, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xfeb80940, 0xffbc0fd0, "vrint%16-17?mpna%u.f16\t%y1, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xfe000900, 0xff800f50, "vsel%20-21c%u.f16\t%y1, %y2, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0eb109c0, 0x0fbf0fd0, "vsqrt%c.f16\t%y1, %y0"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0x0e300940, 0x0fb00f50, "vsub%c.f16\t%y1, %y2, %y0"},

    /* ARMv8.3 javascript conversion instruction.  */
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8_3A), 0x0eb90bc0, 0x0fbf0fd0, "vjcvt%c.s32.f64\t%y1, %z0"},

    {ARM_FEATURE_CORE_LOW(0), 0, 0, 0}};

/* Neon opcode table:  This does not encode the top byte -- that is
   checked by the print_insn_neon routine, as it depends on whether we are
   doing thumb32 or arm32 disassembly.  */

/* print_insn_neon recognizes the following format control codes:

   %%			%

   %c			print condition code
   %u			print condition code (unconditional in ARM mode,
                          UNPREDICTABLE if not AL in Thumb)
   %A			print v{st,ld}[1234] operands
   %B			print v{st,ld}[1234] any one operands
   %C			print v{st,ld}[1234] single->all operands
   %D			print scalar
   %E			print vmov, vmvn, vorr, vbic encoded constant
   %F			print vtbl,vtbx register list

   %<bitfield>r		print as an ARM register
   %<bitfield>d		print the bitfield in decimal
   %<bitfield>e         print the 2^N - bitfield in decimal
   %<bitfield>D		print as a NEON D register
   %<bitfield>Q		print as a NEON Q register
   %<bitfield>R		print as a NEON D or Q register
   %<bitfield>Sn	print byte scaled width limited by n
   %<bitfield>Tn	print short scaled width limited by n
   %<bitfield>Un	print long scaled width limited by n

   %<bitfield>'c	print specified char iff bitfield is all ones
   %<bitfield>`c	print specified char iff bitfield is all zeroes
   %<bitfield>?ab...    select from array of values in big endian order.  */

static const struct opcode32 neon_opcodes[] = {
    /* Extract.  */
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2b00840, 0xffb00850, "vext%c.8\t%12-15,22R, %16-19,7R, %0-3,5R, #%8-11d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2b00000, 0xffb00810, "vext%c.8\t%12-15,22R, %16-19,7R, %0-3,5R, #%8-11d"},

    /* Move data element to all lanes.  */
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b40c00, 0xffb70f90, "vdup%c.32\t%12-15,22R, %0-3,5D[%19d]"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b20c00, 0xffb30f90, "vdup%c.16\t%12-15,22R, %0-3,5D[%18-19d]"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b10c00, 0xffb10f90, "vdup%c.8\t%12-15,22R, %0-3,5D[%17-19d]"},

    /* Table lookup.  */
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00800, 0xffb00c50, "vtbl%c.8\t%12-15,22D, %F, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00840, 0xffb00c50, "vtbx%c.8\t%12-15,22D, %F, %0-3,5D"},

    /* Half-precision conversions.  */
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FP16), 0xf3b60600, 0xffbf0fd0, "vcvt%c.f16.f32\t%12-15,22D, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_VFP_EXT_FP16), 0xf3b60700, 0xffbf0fd0, "vcvt%c.f32.f16\t%12-15,22Q, %0-3,5D"},

    /* NEON fused multiply add instructions.  */
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_FMA), 0xf2000c10, 0xffb00f10, "vfma%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2100c10, 0xffb00f10, "vfma%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_FMA), 0xf2200c10, 0xffb00f10, "vfms%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2300c10, 0xffb00f10, "vfms%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},

    /* Two registers, miscellaneous.  */
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_ARMV8), 0xf3ba0400, 0xffbf0c10, "vrint%7-9?p?m?zaxn%u.f32\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3b60400, 0xffbf0c10,
     "vrint%7-9?p?m?zaxn%u.f16\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_ARMV8), 0xf3bb0000, 0xffbf0c10,
     "vcvt%8-9?mpna%u.%7?us32.f32\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3b70000, 0xffbf0c10,
     "vcvt%8-9?mpna%u.%7?us16.f16\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf3b00300, 0xffbf0fd0, "aese%u.8\t%12-15,22Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf3b00340, 0xffbf0fd0, "aesd%u.8\t%12-15,22Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf3b00380, 0xffbf0fd0, "aesmc%u.8\t%12-15,22Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf3b003c0, 0xffbf0fd0, "aesimc%u.8\t%12-15,22Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf3b902c0, 0xffbf0fd0, "sha1h%u.32\t%12-15,22Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf3ba0380, 0xffbf0fd0, "sha1su1%u.32\t%12-15,22Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf3ba03c0, 0xffbf0fd0, "sha256su0%u.32\t%12-15,22Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880a10, 0xfebf0fd0, "vmovl%c.%24?us8\t%12-15,22Q, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900a10, 0xfebf0fd0, "vmovl%c.%24?us16\t%12-15,22Q, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00a10, 0xfebf0fd0, "vmovl%c.%24?us32\t%12-15,22Q, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00500, 0xffbf0f90, "vcnt%c.8\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00580, 0xffbf0f90, "vmvn%c\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b20000, 0xffbf0f90, "vswp%c\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b20200, 0xffb30fd0, "vmovn%c.i%18-19T2\t%12-15,22D, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b20240, 0xffb30fd0, "vqmovun%c.s%18-19T2\t%12-15,22D, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b20280, 0xffb30fd0, "vqmovn%c.s%18-19T2\t%12-15,22D, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b202c0, 0xffb30fd0, "vqmovn%c.u%18-19T2\t%12-15,22D, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b20300, 0xffb30fd0, "vshll%c.i%18-19S2\t%12-15,22Q, %0-3,5D, #%18-19S2"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3bb0400, 0xffbf0e90, "vrecpe%c.%8?fu%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3b70400, 0xffbf0e90, "vrecpe%c.%8?fu16\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3bb0480, 0xffbf0e90, "vrsqrte%c.%8?fu%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3b70480, 0xffbf0e90, "vrsqrte%c.%8?fu16\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00000, 0xffb30f90, "vrev64%c.%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00080, 0xffb30f90, "vrev32%c.%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00100, 0xffb30f90, "vrev16%c.%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00400, 0xffb30f90, "vcls%c.s%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00480, 0xffb30f90, "vclz%c.i%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00700, 0xffb30f90, "vqabs%c.s%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00780, 0xffb30f90, "vqneg%c.s%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b20080, 0xffb30f90, "vtrn%c.%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b20100, 0xffb30f90, "vuzp%c.%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b20180, 0xffb30f90, "vzip%c.%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b10000, 0xffb30b90, "vcgt%c.%10?fs%18-19S2\t%12-15,22R, %0-3,5R, #0"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b10080, 0xffb30b90, "vcge%c.%10?fs%18-19S2\t%12-15,22R, %0-3,5R, #0"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b10100, 0xffb30b90, "vceq%c.%10?fi%18-19S2\t%12-15,22R, %0-3,5R, #0"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b10180, 0xffb30b90, "vcle%c.%10?fs%18-19S2\t%12-15,22R, %0-3,5R, #0"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b10200, 0xffb30b90, "vclt%c.%10?fs%18-19S2\t%12-15,22R, %0-3,5R, #0"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b10300, 0xffb30b90, "vabs%c.%10?fs%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b10380, 0xffb30b90, "vneg%c.%10?fs%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00200, 0xffb30f10, "vpaddl%c.%7?us%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3b00600, 0xffb30f10, "vpadal%c.%7?us%18-19S2\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3bb0600, 0xffbf0e10,
     "vcvt%c.%7-8?usff%18-19Sa.%7-8?ffus%18-19Sa\t%12-15,22R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3b70600, 0xffbf0e10,
     "vcvt%c.%7-8?usff16.%7-8?ffus16\t%12-15,22R, %0-3,5R"},

    /* Three registers of the same length.  */
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf2000c40, 0xffb00f50, "sha1c%u.32\t%12-15,22Q, %16-19,7Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf2100c40, 0xffb00f50, "sha1p%u.32\t%12-15,22Q, %16-19,7Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf2200c40, 0xffb00f50, "sha1m%u.32\t%12-15,22Q, %16-19,7Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf2300c40, 0xffb00f50, "sha1su0%u.32\t%12-15,22Q, %16-19,7Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf3000c40, 0xffb00f50, "sha256h%u.32\t%12-15,22Q, %16-19,7Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf3100c40, 0xffb00f50, "sha256h2%u.32\t%12-15,22Q, %16-19,7Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf3200c40, 0xffb00f50,
     "sha256su1%u.32\t%12-15,22Q, %16-19,7Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_ARMV8), 0xf3000f10, 0xffb00f10, "vmaxnm%u.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3100f10, 0xffb00f10, "vmaxnm%u.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_ARMV8), 0xf3200f10, 0xffb00f10, "vminnm%u.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3300f10, 0xffb00f10, "vminnm%u.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000110, 0xffb00f10, "vand%c\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2100110, 0xffb00f10, "vbic%c\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2200110, 0xffb00f10, "vorr%c\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2300110, 0xffb00f10, "vorn%c\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3000110, 0xffb00f10, "veor%c\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3100110, 0xffb00f10, "vbsl%c\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3200110, 0xffb00f10, "vbit%c\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3300110, 0xffb00f10, "vbif%c\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000d00, 0xffb00f10, "vadd%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2100d00, 0xffb00f10, "vadd%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000d10, 0xffb00f10, "vmla%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2100d10, 0xffb00f10, "vmla%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000e00, 0xffb00f10, "vceq%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2100e00, 0xffb00f10, "vceq%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000f00, 0xffb00f10, "vmax%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2100f00, 0xffb00f10, "vmax%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000f10, 0xffb00f10, "vrecps%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2100f10, 0xffb00f10, "vrecps%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2200d00, 0xffb00f10, "vsub%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2300d00, 0xffb00f10, "vsub%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2200d10, 0xffb00f10, "vmls%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2300d10, 0xffb00f10, "vmls%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2200f00, 0xffb00f10, "vmin%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2300f00, 0xffb00f10, "vmin%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2200f10, 0xffb00f10, "vrsqrts%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2300f10, 0xffb00f10,
     "vrsqrts%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3000d00, 0xffb00f10, "vpadd%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3100d00, 0xffb00f10, "vpadd%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3000d10, 0xffb00f10, "vmul%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3100d10, 0xffb00f10, "vmul%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3000e00, 0xffb00f10, "vcge%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3100e00, 0xffb00f10, "vcge%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3000e10, 0xffb00f10, "vacge%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3100e10, 0xffb00f10, "vacge%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3000f00, 0xffb00f10, "vpmax%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3100f00, 0xffb00f10, "vpmax%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3200d00, 0xffb00f10, "vabd%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3300d00, 0xffb00f10, "vabd%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3200e00, 0xffb00f10, "vcgt%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3300e00, 0xffb00f10, "vcgt%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3200e10, 0xffb00f10, "vacgt%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3300e10, 0xffb00f10, "vacgt%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3200f00, 0xffb00f10, "vpmin%c.f32\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf3300f00, 0xffb00f10, "vpmin%c.f16\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000800, 0xff800f10, "vadd%c.i%20-21S3\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000810, 0xff800f10, "vtst%c.%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000900, 0xff800f10, "vmla%c.i%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000b00, 0xff800f10,
     "vqdmulh%c.s%20-21S6\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000b10, 0xff800f10, "vpadd%c.i%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3000800, 0xff800f10, "vsub%c.i%20-21S3\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3000810, 0xff800f10, "vceq%c.i%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3000900, 0xff800f10, "vmls%c.i%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3000b00, 0xff800f10,
     "vqrdmulh%c.s%20-21S6\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000000, 0xfe800f10,
     "vhadd%c.%24?us%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000010, 0xfe800f10,
     "vqadd%c.%24?us%20-21S3\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000100, 0xfe800f10,
     "vrhadd%c.%24?us%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000200, 0xfe800f10,
     "vhsub%c.%24?us%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000210, 0xfe800f10,
     "vqsub%c.%24?us%20-21S3\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000300, 0xfe800f10,
     "vcgt%c.%24?us%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000310, 0xfe800f10,
     "vcge%c.%24?us%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000400, 0xfe800f10,
     "vshl%c.%24?us%20-21S3\t%12-15,22R, %0-3,5R, %16-19,7R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000410, 0xfe800f10,
     "vqshl%c.%24?us%20-21S3\t%12-15,22R, %0-3,5R, %16-19,7R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000500, 0xfe800f10,
     "vrshl%c.%24?us%20-21S3\t%12-15,22R, %0-3,5R, %16-19,7R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000510, 0xfe800f10,
     "vqrshl%c.%24?us%20-21S3\t%12-15,22R, %0-3,5R, %16-19,7R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000600, 0xfe800f10,
     "vmax%c.%24?us%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000610, 0xfe800f10,
     "vmin%c.%24?us%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000700, 0xfe800f10,
     "vabd%c.%24?us%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000710, 0xfe800f10,
     "vaba%c.%24?us%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000910, 0xfe800f10,
     "vmul%c.%24?pi%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000a00, 0xfe800f10,
     "vpmax%c.%24?us%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2000a10, 0xfe800f10,
     "vpmin%c.%24?us%20-21S2\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_RDMA), 0xf3000b10, 0xff800f10,
     "vqrdmlah%c.s%20-21S6\t%12-15,22R, %16-19,7R, %0-3,5R"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_RDMA), 0xf3000c10, 0xff800f10,
     "vqrdmlsh%c.s%20-21S6\t%12-15,22R, %16-19,7R, %0-3,5R"},

    /* One register and an immediate value.  */
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800e10, 0xfeb80fb0, "vmov%c.i8\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800e30, 0xfeb80fb0, "vmov%c.i64\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800f10, 0xfeb80fb0, "vmov%c.f32\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800810, 0xfeb80db0, "vmov%c.i16\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800830, 0xfeb80db0, "vmvn%c.i16\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800910, 0xfeb80db0, "vorr%c.i16\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800930, 0xfeb80db0, "vbic%c.i16\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800c10, 0xfeb80eb0, "vmov%c.i32\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800c30, 0xfeb80eb0, "vmvn%c.i32\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800110, 0xfeb809b0, "vorr%c.i32\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800130, 0xfeb809b0, "vbic%c.i32\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800010, 0xfeb808b0, "vmov%c.i32\t%12-15,22R, %E"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800030, 0xfeb808b0, "vmvn%c.i32\t%12-15,22R, %E"},

    /* Two registers and a shift amount.  */
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880810, 0xffb80fd0, "vshrn%c.i16\t%12-15,22D, %0-3,5Q, #%16-18e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880850, 0xffb80fd0, "vrshrn%c.i16\t%12-15,22D, %0-3,5Q, #%16-18e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880810, 0xfeb80fd0, "vqshrun%c.s16\t%12-15,22D, %0-3,5Q, #%16-18e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880850, 0xfeb80fd0, "vqrshrun%c.s16\t%12-15,22D, %0-3,5Q, #%16-18e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880910, 0xfeb80fd0, "vqshrn%c.%24?us16\t%12-15,22D, %0-3,5Q, #%16-18e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880950, 0xfeb80fd0, "vqrshrn%c.%24?us16\t%12-15,22D, %0-3,5Q, #%16-18e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880a10, 0xfeb80fd0, "vshll%c.%24?us8\t%12-15,22Q, %0-3,5D, #%16-18d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900810, 0xffb00fd0, "vshrn%c.i32\t%12-15,22D, %0-3,5Q, #%16-19e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900850, 0xffb00fd0, "vrshrn%c.i32\t%12-15,22D, %0-3,5Q, #%16-19e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880510, 0xffb80f90, "vshl%c.%24?us8\t%12-15,22R, %0-3,5R, #%16-18d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3880410, 0xffb80f90, "vsri%c.8\t%12-15,22R, %0-3,5R, #%16-18e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3880510, 0xffb80f90, "vsli%c.8\t%12-15,22R, %0-3,5R, #%16-18d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3880610, 0xffb80f90, "vqshlu%c.s8\t%12-15,22R, %0-3,5R, #%16-18d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900810, 0xfeb00fd0, "vqshrun%c.s32\t%12-15,22D, %0-3,5Q, #%16-19e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900850, 0xfeb00fd0, "vqrshrun%c.s32\t%12-15,22D, %0-3,5Q, #%16-19e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900910, 0xfeb00fd0, "vqshrn%c.%24?us32\t%12-15,22D, %0-3,5Q, #%16-19e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900950, 0xfeb00fd0, "vqrshrn%c.%24?us32\t%12-15,22D, %0-3,5Q, #%16-19e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900a10, 0xfeb00fd0, "vshll%c.%24?us16\t%12-15,22Q, %0-3,5D, #%16-19d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880010, 0xfeb80f90, "vshr%c.%24?us8\t%12-15,22R, %0-3,5R, #%16-18e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880110, 0xfeb80f90, "vsra%c.%24?us8\t%12-15,22R, %0-3,5R, #%16-18e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880210, 0xfeb80f90, "vrshr%c.%24?us8\t%12-15,22R, %0-3,5R, #%16-18e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880310, 0xfeb80f90, "vrsra%c.%24?us8\t%12-15,22R, %0-3,5R, #%16-18e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2880710, 0xfeb80f90, "vqshl%c.%24?us8\t%12-15,22R, %0-3,5R, #%16-18d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00810, 0xffa00fd0, "vshrn%c.i64\t%12-15,22D, %0-3,5Q, #%16-20e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00850, 0xffa00fd0, "vrshrn%c.i64\t%12-15,22D, %0-3,5Q, #%16-20e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900510, 0xffb00f90, "vshl%c.%24?us16\t%12-15,22R, %0-3,5R, #%16-19d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3900410, 0xffb00f90, "vsri%c.16\t%12-15,22R, %0-3,5R, #%16-19e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3900510, 0xffb00f90, "vsli%c.16\t%12-15,22R, %0-3,5R, #%16-19d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3900610, 0xffb00f90, "vqshlu%c.s16\t%12-15,22R, %0-3,5R, #%16-19d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00a10, 0xfea00fd0, "vshll%c.%24?us32\t%12-15,22Q, %0-3,5D, #%16-20d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900010, 0xfeb00f90, "vshr%c.%24?us16\t%12-15,22R, %0-3,5R, #%16-19e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900110, 0xfeb00f90, "vsra%c.%24?us16\t%12-15,22R, %0-3,5R, #%16-19e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900210, 0xfeb00f90, "vrshr%c.%24?us16\t%12-15,22R, %0-3,5R, #%16-19e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900310, 0xfeb00f90, "vrsra%c.%24?us16\t%12-15,22R, %0-3,5R, #%16-19e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2900710, 0xfeb00f90, "vqshl%c.%24?us16\t%12-15,22R, %0-3,5R, #%16-19d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00810, 0xfea00fd0, "vqshrun%c.s64\t%12-15,22D, %0-3,5Q, #%16-20e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00850, 0xfea00fd0, "vqrshrun%c.s64\t%12-15,22D, %0-3,5Q, #%16-20e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00910, 0xfea00fd0, "vqshrn%c.%24?us64\t%12-15,22D, %0-3,5Q, #%16-20e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00950, 0xfea00fd0, "vqrshrn%c.%24?us64\t%12-15,22D, %0-3,5Q, #%16-20e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00510, 0xffa00f90, "vshl%c.%24?us32\t%12-15,22R, %0-3,5R, #%16-20d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3a00410, 0xffa00f90, "vsri%c.32\t%12-15,22R, %0-3,5R, #%16-20e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3a00510, 0xffa00f90, "vsli%c.32\t%12-15,22R, %0-3,5R, #%16-20d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3a00610, 0xffa00f90, "vqshlu%c.s32\t%12-15,22R, %0-3,5R, #%16-20d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00010, 0xfea00f90, "vshr%c.%24?us32\t%12-15,22R, %0-3,5R, #%16-20e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00110, 0xfea00f90, "vsra%c.%24?us32\t%12-15,22R, %0-3,5R, #%16-20e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00210, 0xfea00f90, "vrshr%c.%24?us32\t%12-15,22R, %0-3,5R, #%16-20e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00310, 0xfea00f90, "vrsra%c.%24?us32\t%12-15,22R, %0-3,5R, #%16-20e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00710, 0xfea00f90, "vqshl%c.%24?us32\t%12-15,22R, %0-3,5R, #%16-20d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800590, 0xff800f90, "vshl%c.%24?us64\t%12-15,22R, %0-3,5R, #%16-21d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800490, 0xff800f90, "vsri%c.64\t%12-15,22R, %0-3,5R, #%16-21e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800590, 0xff800f90, "vsli%c.64\t%12-15,22R, %0-3,5R, #%16-21d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800690, 0xff800f90, "vqshlu%c.s64\t%12-15,22R, %0-3,5R, #%16-21d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800090, 0xfe800f90, "vshr%c.%24?us64\t%12-15,22R, %0-3,5R, #%16-21e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800190, 0xfe800f90, "vsra%c.%24?us64\t%12-15,22R, %0-3,5R, #%16-21e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800290, 0xfe800f90, "vrshr%c.%24?us64\t%12-15,22R, %0-3,5R, #%16-21e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800390, 0xfe800f90, "vrsra%c.%24?us64\t%12-15,22R, %0-3,5R, #%16-21e"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800790, 0xfe800f90, "vqshl%c.%24?us64\t%12-15,22R, %0-3,5R, #%16-21d"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2a00e10, 0xfea00e90,
     "vcvt%c.%24,8?usff32.%24,8?ffus32\t%12-15,22R, %0-3,5R, #%16-20e"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_FP16_INST), 0xf2a00c10, 0xfea00e90,
     "vcvt%c.%24,8?usff16.%24,8?ffus16\t%12-15,22R, %0-3,5R, #%16-20e"},

    /* Three registers of different lengths.  */
    {ARM_FEATURE_COPROC(FPU_CRYPTO_EXT_ARMV8), 0xf2a00e00, 0xfeb00f50, "vmull%c.p64\t%12-15,22Q, %16-19,7D, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800e00, 0xfea00f50, "vmull%c.p%20S0\t%12-15,22Q, %16-19,7D, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800400, 0xff800f50, "vaddhn%c.i%20-21T2\t%12-15,22D, %16-19,7Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800600, 0xff800f50, "vsubhn%c.i%20-21T2\t%12-15,22D, %16-19,7Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800900, 0xff800f50,
     "vqdmlal%c.s%20-21S6\t%12-15,22Q, %16-19,7D, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800b00, 0xff800f50,
     "vqdmlsl%c.s%20-21S6\t%12-15,22Q, %16-19,7D, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800d00, 0xff800f50,
     "vqdmull%c.s%20-21S6\t%12-15,22Q, %16-19,7D, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800400, 0xff800f50,
     "vraddhn%c.i%20-21T2\t%12-15,22D, %16-19,7Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800600, 0xff800f50,
     "vrsubhn%c.i%20-21T2\t%12-15,22D, %16-19,7Q, %0-3,5Q"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800000, 0xfe800f50,
     "vaddl%c.%24?us%20-21S2\t%12-15,22Q, %16-19,7D, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800100, 0xfe800f50,
     "vaddw%c.%24?us%20-21S2\t%12-15,22Q, %16-19,7Q, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800200, 0xfe800f50,
     "vsubl%c.%24?us%20-21S2\t%12-15,22Q, %16-19,7D, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800300, 0xfe800f50,
     "vsubw%c.%24?us%20-21S2\t%12-15,22Q, %16-19,7Q, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800500, 0xfe800f50,
     "vabal%c.%24?us%20-21S2\t%12-15,22Q, %16-19,7D, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800700, 0xfe800f50,
     "vabdl%c.%24?us%20-21S2\t%12-15,22Q, %16-19,7D, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800800, 0xfe800f50,
     "vmlal%c.%24?us%20-21S2\t%12-15,22Q, %16-19,7D, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800a00, 0xfe800f50,
     "vmlsl%c.%24?us%20-21S2\t%12-15,22Q, %16-19,7D, %0-3,5D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800c00, 0xfe800f50,
     "vmull%c.%24?us%20-21S2\t%12-15,22Q, %16-19,7D, %0-3,5D"},

    /* Two registers and a scalar.  */
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800040, 0xff800f50, "vmla%c.i%20-21S6\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800140, 0xff900f50, "vmla%c.f%20-21Sa\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(ARM_EXT2_FP16_INST), 0xf2900140, 0xffb00f50, "vmla%c.f16\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800340, 0xff800f50, "vqdmlal%c.s%20-21S6\t%12-15,22Q, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800440, 0xff800f50, "vmls%c.i%20-21S6\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800540, 0xff900f50, "vmls%c.f%20-21S6\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(ARM_EXT2_FP16_INST), 0xf2900540, 0xffb00f50, "vmls%c.f16\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800740, 0xff800f50, "vqdmlsl%c.s%20-21S6\t%12-15,22Q, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800840, 0xff800f50, "vmul%c.i%20-21S6\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800940, 0xff900f50, "vmul%c.f%20-21Sa\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(ARM_EXT2_FP16_INST), 0xf2900940, 0xffb00f50, "vmul%c.f16\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800b40, 0xff800f50, "vqdmull%c.s%20-21S6\t%12-15,22Q, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800c40, 0xff800f50, "vqdmulh%c.s%20-21S6\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800d40, 0xff800f50, "vqrdmulh%c.s%20-21S6\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800040, 0xff800f50, "vmla%c.i%20-21S6\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800140, 0xff900f50, "vmla%c.f%20-21Sa\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(ARM_EXT2_FP16_INST), 0xf3900140, 0xffb00f50, "vmla%c.f16\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800440, 0xff800f50, "vmls%c.i%20-21S6\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800540, 0xff900f50, "vmls%c.f%20-21Sa\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(ARM_EXT2_FP16_INST), 0xf3900540, 0xffb00f50, "vmls%c.f16\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800840, 0xff800f50, "vmul%c.i%20-21S6\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800940, 0xff900f50, "vmul%c.f%20-21Sa\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(ARM_EXT2_FP16_INST), 0xf3900940, 0xffb00f50, "vmul%c.f16\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800c40, 0xff800f50, "vqdmulh%c.s%20-21S6\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf3800d40, 0xff800f50, "vqrdmulh%c.s%20-21S6\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800240, 0xfe800f50, "vmlal%c.%24?us%20-21S6\t%12-15,22Q, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800640, 0xfe800f50, "vmlsl%c.%24?us%20-21S6\t%12-15,22Q, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf2800a40, 0xfe800f50, "vmull%c.%24?us%20-21S6\t%12-15,22Q, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_RDMA), 0xf2800e40, 0xff800f50, "vqrdmlah%c.s%20-21S6\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_RDMA), 0xf2800f40, 0xff800f50, "vqrdmlsh%c.s%20-21S6\t%12-15,22D, %16-19,7D, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_RDMA), 0xf3800e40, 0xff800f50, "vqrdmlah%c.s%20-21S6\t%12-15,22Q, %16-19,7Q, %D"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_RDMA), 0xf3800f40, 0xff800f50, "vqrdmlsh%c.s%20-21S6\t%12-15,22Q, %16-19,7Q, %D"},

    /* Element and structure load/store.  */
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4a00fc0, 0xffb00fc0, "vld4%c.32\t%C"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4a00c00, 0xffb00f00, "vld1%c.%6-7S2\t%C"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4a00d00, 0xffb00f00, "vld2%c.%6-7S2\t%C"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4a00e00, 0xffb00f00, "vld3%c.%6-7S2\t%C"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4a00f00, 0xffb00f00, "vld4%c.%6-7S2\t%C"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4000200, 0xff900f00, "v%21?ls%21?dt1%c.%6-7S3\t%A"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4000300, 0xff900f00, "v%21?ls%21?dt2%c.%6-7S2\t%A"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4000400, 0xff900f00, "v%21?ls%21?dt3%c.%6-7S2\t%A"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4000500, 0xff900f00, "v%21?ls%21?dt3%c.%6-7S2\t%A"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4000600, 0xff900f00, "v%21?ls%21?dt1%c.%6-7S3\t%A"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4000700, 0xff900f00, "v%21?ls%21?dt1%c.%6-7S3\t%A"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4000800, 0xff900f00, "v%21?ls%21?dt2%c.%6-7S2\t%A"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4000900, 0xff900f00, "v%21?ls%21?dt2%c.%6-7S2\t%A"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4000a00, 0xff900f00, "v%21?ls%21?dt1%c.%6-7S3\t%A"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4000000, 0xff900e00, "v%21?ls%21?dt4%c.%6-7S2\t%A"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4800000, 0xff900300, "v%21?ls%21?dt1%c.%10-11S2\t%B"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4800100, 0xff900300, "v%21?ls%21?dt2%c.%10-11S2\t%B"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4800200, 0xff900300, "v%21?ls%21?dt3%c.%10-11S2\t%B"},
    {ARM_FEATURE_COPROC(FPU_NEON_EXT_V1), 0xf4800300, 0xff900300, "v%21?ls%21?dt4%c.%10-11S2\t%B"},

    {ARM_FEATURE_CORE_LOW(0), 0, 0, 0}};

/* Opcode tables: ARM, 16-bit Thumb, 32-bit Thumb.  All three are partially
   ordered: they must be searched linearly from the top to obtain a correct
   match.  */

/* print_insn_arm recognizes the following format control codes:

   %%			%

   %a			print address for ldr/str instruction
   %s                   print address for ldr/str halfword/signextend instruction
   %S                   like %s but allow UNPREDICTABLE addressing
   %b			print branch destination
   %c			print condition code (always bits 28-31)
   %m			print register mask for ldm/stm instruction
   %o			print operand2 (immediate or register + shift)
   %p			print 'p' iff bits 12-15 are 15
   %t			print 't' iff bit 21 set and bit 24 clear
   %B			print arm BLX(1) destination
   %C			print the PSR sub type.
   %U			print barrier type.
   %P			print address for pli instruction.

   %<bitfield>r		print as an ARM register
   %<bitfield>T		print as an ARM register + 1
   %<bitfield>R		as %r but r15 is UNPREDICTABLE
   %<bitfield>{r|R}u    as %{r|R} but if matches the other %u field then is UNPREDICTABLE
   %<bitfield>{r|R}U    as %{r|R} but if matches the other %U field then is UNPREDICTABLE
   %<bitfield>d		print the bitfield in decimal
   %<bitfield>W         print the bitfield plus one in decimal
   %<bitfield>x		print the bitfield in hex
   %<bitfield>X		print the bitfield as 1 hex digit without leading "0x"

   %<bitfield>'c	print specified char iff bitfield is all ones
   %<bitfield>`c	print specified char iff bitfield is all zeroes
   %<bitfield>?ab...    select from array of values in big endian order

   %e                   print arm SMI operand (bits 0..7,8..19).
   %E			print the LSB and WIDTH fields of a BFI or BFC instruction.
   %V                   print the 16-bit immediate field of a MOVT or MOVW instruction.
   %R			print the SPSR/CPSR or banked register of an MRS.  */

static const struct opcode32 arm_opcodes[] = {
    /* ARM instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0xe1a00000, 0xffffffff, "nop\t\t\t; (mov r0, r0)"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0xe7f000f0, 0xfff000f0, "udf\t#%e"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T | ARM_EXT_V5), 0x012FFF10, 0x0ffffff0, "bx%c\t%0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V2), 0x00000090, 0x0fe000f0, "mul%20's%c\t%16-19R, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V2), 0x00200090, 0x0fe000f0, "mla%20's%c\t%16-19R, %0-3R, %8-11R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V2S), 0x01000090, 0x0fb00ff0, "swp%22'b%c\t%12-15RU, %0-3Ru, [%16-19RuU]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V3M), 0x00800090, 0x0fa000f0, "%22?sumull%20's%c\t%12-15Ru, %16-19Ru, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V3M), 0x00a00090, 0x0fa000f0, "%22?sumlal%20's%c\t%12-15Ru, %16-19Ru, %0-3R, %8-11R"},

    /* V8.2 RAS extension instructions.  */
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_RAS), 0xe320f010, 0xffffffff, "esb"},

    /* V8 instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0x0320f005, 0x0fffffff, "sevl"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe1000070, 0xfff000f0, "hlt\t0x%16-19X%12-15X%8-11X%0-3X"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_ATOMICS), 0x01800e90, 0x0ff00ff0, "stlex%c\t%12-15r, %0-3r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT2_ATOMICS), 0x01900e9f, 0x0ff00fff, "ldaex%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0x01a00e90, 0x0ff00ff0, "stlexd%c\t%12-15r, %0-3r, %0-3T, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0x01b00e9f, 0x0ff00fff, "ldaexd%c\t%12-15r, %12-15T, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT2_ATOMICS), 0x01c00e90, 0x0ff00ff0, "stlexb%c\t%12-15r, %0-3r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT2_ATOMICS), 0x01d00e9f, 0x0ff00fff, "ldaexb%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT2_ATOMICS), 0x01e00e90, 0x0ff00ff0, "stlexh%c\t%12-15r, %0-3r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT2_ATOMICS), 0x01f00e9f, 0x0ff00fff, "ldaexh%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT2_ATOMICS), 0x0180fc90, 0x0ff0fff0, "stl%c\t%0-3r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT2_ATOMICS), 0x01900c9f, 0x0ff00fff, "lda%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT2_ATOMICS), 0x01c0fc90, 0x0ff0fff0, "stlb%c\t%0-3r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT2_ATOMICS), 0x01d00c9f, 0x0ff00fff, "ldab%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT2_ATOMICS), 0x01e0fc90, 0x0ff0fff0, "stlh%c\t%0-3r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT2_ATOMICS), 0x01f00c9f, 0x0ff00fff, "ldah%c\t%12-15r, [%16-19R]"},
    /* CRC32 instructions.  */
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xe1000040, 0xfff00ff0, "crc32b\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xe1200040, 0xfff00ff0, "crc32h\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xe1400040, 0xfff00ff0, "crc32w\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xe1000240, 0xfff00ff0, "crc32cb\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xe1200240, 0xfff00ff0, "crc32ch\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xe1400240, 0xfff00ff0, "crc32cw\t%12-15R, %16-19R, %0-3R"},

    /* Privileged Access Never extension instructions.  */
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_PAN), 0xf1100000, 0xfffffdff, "setpan\t#%9-9d"},

    /* Virtualization Extension instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_VIRT), 0x0160006e, 0x0fffffff, "eret%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_VIRT), 0x01400070, 0x0ff000f0, "hvc%c\t%e"},

    /* Integer Divide Extension instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_ADIV), 0x0710f010, 0x0ff0f0f0, "sdiv%c\t%16-19r, %0-3r, %8-11r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_ADIV), 0x0730f010, 0x0ff0f0f0, "udiv%c\t%16-19r, %0-3r, %8-11r"},

    /* MP Extension instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_MP), 0xf410f000, 0xfc70f000, "pldw\t%a"},

    /* Speculation Barriers.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V3), 0xe320f014, 0xffffffff, "csdb"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V3), 0xf57ff040, 0xffffffff, "ssbb"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V3), 0xf57ff044, 0xffffffff, "pssbb"},

    /* V7 instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0xf450f000, 0xfd70f000, "pli\t%P"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0x0320f0f0, 0x0ffffff0, "dbg%c\t#%0-3d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xf57ff051, 0xfffffff3, "dmb\t%U"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xf57ff041, 0xfffffff3, "dsb\t%U"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0xf57ff050, 0xfffffff0, "dmb\t%U"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0xf57ff040, 0xfffffff0, "dsb\t%U"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0xf57ff060, 0xfffffff0, "isb\t%U"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0x0320f000, 0x0fffffff, "nop%c\t{%0-7d}"},

    /* ARM V6T2 instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0x07c0001f, 0x0fe0007f, "bfc%c\t%12-15R, %E"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0x07c00010, 0x0fe00070, "bfi%c\t%12-15R, %0-3r, %E"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0x00600090, 0x0ff000f0, "mls%c\t%16-19R, %0-3R, %8-11R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0x002000b0, 0x0f3000f0, "strht%c\t%12-15R, %S"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0x00300090, 0x0f3000f0, UNDEFINED_INSTRUCTION},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0x00300090, 0x0f300090, "ldr%6's%5?hbt%c\t%12-15R, %S"},

    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0x03000000, 0x0ff00000, "movw%c\t%12-15R, %V"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0x03400000, 0x0ff00000, "movt%c\t%12-15R, %V"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0x06ff0f30, 0x0fff0ff0, "rbit%c\t%12-15R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0x07a00050, 0x0fa00070, "%22?usbfx%c\t%12-15r, %0-3r, #%7-11d, #%16-20W"},

    /* ARM Security extension instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_SEC), 0x01600070, 0x0ff000f0, "smc%c\t%e"},

    /* ARM V6K instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0xf57ff01f, 0xffffffff, "clrex"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0x01d00f9f, 0x0ff00fff, "ldrexb%c\t%12-15R, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0x01b00f9f, 0x0ff00fff, "ldrexd%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0x01f00f9f, 0x0ff00fff, "ldrexh%c\t%12-15R, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0x01c00f90, 0x0ff00ff0, "strexb%c\t%12-15R, %0-3R, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0x01a00f90, 0x0ff00ff0, "strexd%c\t%12-15R, %0-3r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0x01e00f90, 0x0ff00ff0, "strexh%c\t%12-15R, %0-3R, [%16-19R]"},

    /* ARM V6K NOP hints.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0x0320f001, 0x0fffffff, "yield%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0x0320f002, 0x0fffffff, "wfe%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0x0320f003, 0x0fffffff, "wfi%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0x0320f004, 0x0fffffff, "sev%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0x0320f000, 0x0fffff00, "nop%c\t{%0-7d}"},

    /* ARM V6 instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xf1080000, 0xfffffe3f, "cpsie\t%8'a%7'i%6'f"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xf10a0000, 0xfffffe20, "cpsie\t%8'a%7'i%6'f,#%0-4d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xf10C0000, 0xfffffe3f, "cpsid\t%8'a%7'i%6'f"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xf10e0000, 0xfffffe20, "cpsid\t%8'a%7'i%6'f,#%0-4d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xf1000000, 0xfff1fe20, "cps\t#%0-4d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06800010, 0x0ff00ff0, "pkhbt%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06800010, 0x0ff00070, "pkhbt%c\t%12-15R, %16-19R, %0-3R, lsl #%7-11d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06800050, 0x0ff00ff0, "pkhtb%c\t%12-15R, %16-19R, %0-3R, asr #32"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06800050, 0x0ff00070, "pkhtb%c\t%12-15R, %16-19R, %0-3R, asr #%7-11d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x01900f9f, 0x0ff00fff, "ldrex%c\tr%12-15d, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06200f10, 0x0ff00ff0, "qadd16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06200f90, 0x0ff00ff0, "qadd8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06200f30, 0x0ff00ff0, "qasx%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06200f70, 0x0ff00ff0, "qsub16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06200ff0, 0x0ff00ff0, "qsub8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06200f50, 0x0ff00ff0, "qsax%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06100f10, 0x0ff00ff0, "sadd16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06100f90, 0x0ff00ff0, "sadd8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06100f30, 0x0ff00ff0, "sasx%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06300f10, 0x0ff00ff0, "shadd16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06300f90, 0x0ff00ff0, "shadd8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06300f30, 0x0ff00ff0, "shasx%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06300f70, 0x0ff00ff0, "shsub16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06300ff0, 0x0ff00ff0, "shsub8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06300f50, 0x0ff00ff0, "shsax%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06100f70, 0x0ff00ff0, "ssub16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06100ff0, 0x0ff00ff0, "ssub8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06100f50, 0x0ff00ff0, "ssax%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06500f10, 0x0ff00ff0, "uadd16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06500f90, 0x0ff00ff0, "uadd8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06500f30, 0x0ff00ff0, "uasx%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06700f10, 0x0ff00ff0, "uhadd16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06700f90, 0x0ff00ff0, "uhadd8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06700f30, 0x0ff00ff0, "uhasx%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06700f70, 0x0ff00ff0, "uhsub16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06700ff0, 0x0ff00ff0, "uhsub8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06700f50, 0x0ff00ff0, "uhsax%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06600f10, 0x0ff00ff0, "uqadd16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06600f90, 0x0ff00ff0, "uqadd8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06600f30, 0x0ff00ff0, "uqasx%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06600f70, 0x0ff00ff0, "uqsub16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06600ff0, 0x0ff00ff0, "uqsub8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06600f50, 0x0ff00ff0, "uqsax%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06500f70, 0x0ff00ff0, "usub16%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06500ff0, 0x0ff00ff0, "usub8%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06500f50, 0x0ff00ff0, "usax%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06bf0f30, 0x0fff0ff0, "rev%c\t%12-15R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06bf0fb0, 0x0fff0ff0, "rev16%c\t%12-15R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06ff0fb0, 0x0fff0ff0, "revsh%c\t%12-15R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xf8100a00, 0xfe50ffff, "rfe%23?id%24?ba\t%16-19r%21'!"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06bf0070, 0x0fff0ff0, "sxth%c\t%12-15R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06bf0470, 0x0fff0ff0, "sxth%c\t%12-15R, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06bf0870, 0x0fff0ff0, "sxth%c\t%12-15R, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06bf0c70, 0x0fff0ff0, "sxth%c\t%12-15R, %0-3R, ror #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x068f0070, 0x0fff0ff0, "sxtb16%c\t%12-15R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x068f0470, 0x0fff0ff0, "sxtb16%c\t%12-15R, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x068f0870, 0x0fff0ff0, "sxtb16%c\t%12-15R, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x068f0c70, 0x0fff0ff0, "sxtb16%c\t%12-15R, %0-3R, ror #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06af0070, 0x0fff0ff0, "sxtb%c\t%12-15R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06af0470, 0x0fff0ff0, "sxtb%c\t%12-15R, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06af0870, 0x0fff0ff0, "sxtb%c\t%12-15R, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06af0c70, 0x0fff0ff0, "sxtb%c\t%12-15R, %0-3R, ror #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06ff0070, 0x0fff0ff0, "uxth%c\t%12-15R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06ff0470, 0x0fff0ff0, "uxth%c\t%12-15R, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06ff0870, 0x0fff0ff0, "uxth%c\t%12-15R, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06ff0c70, 0x0fff0ff0, "uxth%c\t%12-15R, %0-3R, ror #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06cf0070, 0x0fff0ff0, "uxtb16%c\t%12-15R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06cf0470, 0x0fff0ff0, "uxtb16%c\t%12-15R, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06cf0870, 0x0fff0ff0, "uxtb16%c\t%12-15R, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06cf0c70, 0x0fff0ff0, "uxtb16%c\t%12-15R, %0-3R, ror #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06ef0070, 0x0fff0ff0, "uxtb%c\t%12-15R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06ef0470, 0x0fff0ff0, "uxtb%c\t%12-15R, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06ef0870, 0x0fff0ff0, "uxtb%c\t%12-15R, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06ef0c70, 0x0fff0ff0, "uxtb%c\t%12-15R, %0-3R, ror #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06b00070, 0x0ff00ff0, "sxtah%c\t%12-15R, %16-19r, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06b00470, 0x0ff00ff0, "sxtah%c\t%12-15R, %16-19r, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06b00870, 0x0ff00ff0, "sxtah%c\t%12-15R, %16-19r, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06b00c70, 0x0ff00ff0, "sxtah%c\t%12-15R, %16-19r, %0-3R, ror #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06800070, 0x0ff00ff0, "sxtab16%c\t%12-15R, %16-19r, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06800470, 0x0ff00ff0, "sxtab16%c\t%12-15R, %16-19r, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06800870, 0x0ff00ff0, "sxtab16%c\t%12-15R, %16-19r, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06800c70, 0x0ff00ff0, "sxtab16%c\t%12-15R, %16-19r, %0-3R, ror #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06a00070, 0x0ff00ff0, "sxtab%c\t%12-15R, %16-19r, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06a00470, 0x0ff00ff0, "sxtab%c\t%12-15R, %16-19r, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06a00870, 0x0ff00ff0, "sxtab%c\t%12-15R, %16-19r, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06a00c70, 0x0ff00ff0, "sxtab%c\t%12-15R, %16-19r, %0-3R, ror #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06f00070, 0x0ff00ff0, "uxtah%c\t%12-15R, %16-19r, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06f00470, 0x0ff00ff0, "uxtah%c\t%12-15R, %16-19r, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06f00870, 0x0ff00ff0, "uxtah%c\t%12-15R, %16-19r, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06f00c70, 0x0ff00ff0, "uxtah%c\t%12-15R, %16-19r, %0-3R, ror #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06c00070, 0x0ff00ff0, "uxtab16%c\t%12-15R, %16-19r, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06c00470, 0x0ff00ff0, "uxtab16%c\t%12-15R, %16-19r, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06c00870, 0x0ff00ff0, "uxtab16%c\t%12-15R, %16-19r, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06c00c70, 0x0ff00ff0, "uxtab16%c\t%12-15R, %16-19r, %0-3R, ROR #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06e00070, 0x0ff00ff0, "uxtab%c\t%12-15R, %16-19r, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06e00470, 0x0ff00ff0, "uxtab%c\t%12-15R, %16-19r, %0-3R, ror #8"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06e00870, 0x0ff00ff0, "uxtab%c\t%12-15R, %16-19r, %0-3R, ror #16"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06e00c70, 0x0ff00ff0, "uxtab%c\t%12-15R, %16-19r, %0-3R, ror #24"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06800fb0, 0x0ff00ff0, "sel%c\t%12-15R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xf1010000, 0xfffffc00, "setend\t%9?ble"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x0700f010, 0x0ff0f0d0, "smuad%5'x%c\t%16-19R, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x0700f050, 0x0ff0f0d0, "smusd%5'x%c\t%16-19R, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x07000010, 0x0ff000d0, "smlad%5'x%c\t%16-19R, %0-3R, %8-11R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x07400010, 0x0ff000d0, "smlald%5'x%c\t%12-15Ru, %16-19Ru, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x07000050, 0x0ff000d0, "smlsd%5'x%c\t%16-19R, %0-3R, %8-11R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x07400050, 0x0ff000d0, "smlsld%5'x%c\t%12-15Ru, %16-19Ru, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x0750f010, 0x0ff0f0d0, "smmul%5'r%c\t%16-19R, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x07500010, 0x0ff000d0, "smmla%5'r%c\t%16-19R, %0-3R, %8-11R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x075000d0, 0x0ff000d0, "smmls%5'r%c\t%16-19R, %0-3R, %8-11R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xf84d0500, 0xfe5fffe0, "srs%23?id%24?ba\t%16-19r%21'!, #%0-4d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06a00010, 0x0fe00ff0, "ssat%c\t%12-15R, #%16-20W, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06a00010, 0x0fe00070, "ssat%c\t%12-15R, #%16-20W, %0-3R, lsl #%7-11d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06a00050, 0x0fe00070, "ssat%c\t%12-15R, #%16-20W, %0-3R, asr #%7-11d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06a00f30, 0x0ff00ff0, "ssat16%c\t%12-15r, #%16-19W, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x01800f90, 0x0ff00ff0, "strex%c\t%12-15R, %0-3R, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x00400090, 0x0ff000f0, "umaal%c\t%12-15R, %16-19R, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x0780f010, 0x0ff0f0f0, "usad8%c\t%16-19R, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x07800010, 0x0ff000f0, "usada8%c\t%16-19R, %0-3R, %8-11R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06e00010, 0x0fe00ff0, "usat%c\t%12-15R, #%16-20d, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06e00010, 0x0fe00070, "usat%c\t%12-15R, #%16-20d, %0-3R, lsl #%7-11d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06e00050, 0x0fe00070, "usat%c\t%12-15R, #%16-20d, %0-3R, asr #%7-11d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x06e00f30, 0x0ff00ff0, "usat16%c\t%12-15R, #%16-19d, %0-3R"},

    /* V5J instruction.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5J), 0x012fff20, 0x0ffffff0, "bxj%c\t%0-3R"},

    /* V5 Instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5), 0xe1200070, 0xfff000f0, "bkpt\t0x%16-19X%12-15X%8-11X%0-3X"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5), 0xfa000000, 0xfe000000, "blx\t%B"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5), 0x012fff30, 0x0ffffff0, "blx%c\t%0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5), 0x016f0f10, 0x0fff0ff0, "clz%c\t%12-15R, %0-3R"},

    /* V5E "El Segundo" Instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5E), 0x000000d0, 0x0e1000f0, "ldrd%c\t%12-15r, %s"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5E), 0x000000f0, 0x0e1000f0, "strd%c\t%12-15r, %s"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5E), 0xf450f000, 0xfc70f000, "pld\t%a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x01000080, 0x0ff000f0, "smlabb%c\t%16-19R, %0-3R, %8-11R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x010000a0, 0x0ff000f0, "smlatb%c\t%16-19R, %0-3R, %8-11R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x010000c0, 0x0ff000f0, "smlabt%c\t%16-19R, %0-3R, %8-11R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x010000e0, 0x0ff000f0, "smlatt%c\t%16-19r, %0-3r, %8-11R, %12-15R"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x01200080, 0x0ff000f0, "smlawb%c\t%16-19R, %0-3R, %8-11R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x012000c0, 0x0ff000f0, "smlawt%c\t%16-19R, %0-3r, %8-11R, %12-15R"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x01400080, 0x0ff000f0, "smlalbb%c\t%12-15Ru, %16-19Ru, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x014000a0, 0x0ff000f0, "smlaltb%c\t%12-15Ru, %16-19Ru, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x014000c0, 0x0ff000f0, "smlalbt%c\t%12-15Ru, %16-19Ru, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x014000e0, 0x0ff000f0, "smlaltt%c\t%12-15Ru, %16-19Ru, %0-3R, %8-11R"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x01600080, 0x0ff0f0f0, "smulbb%c\t%16-19R, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x016000a0, 0x0ff0f0f0, "smultb%c\t%16-19R, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x016000c0, 0x0ff0f0f0, "smulbt%c\t%16-19R, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x016000e0, 0x0ff0f0f0, "smultt%c\t%16-19R, %0-3R, %8-11R"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x012000a0, 0x0ff0f0f0, "smulwb%c\t%16-19R, %0-3R, %8-11R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x012000e0, 0x0ff0f0f0, "smulwt%c\t%16-19R, %0-3R, %8-11R"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x01000050, 0x0ff00ff0, "qadd%c\t%12-15R, %0-3R, %16-19R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x01400050, 0x0ff00ff0, "qdadd%c\t%12-15R, %0-3R, %16-19R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x01200050, 0x0ff00ff0, "qsub%c\t%12-15R, %0-3R, %16-19R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5ExP), 0x01600050, 0x0ff00ff0, "qdsub%c\t%12-15R, %0-3R, %16-19R"},

    /* ARM Instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x052d0004, 0x0fff0fff, "push%c\t{%12-15r}\t\t; (str%c %12-15r, %a)"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x04400000, 0x0e500000, "strb%t%c\t%12-15R, %a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x04000000, 0x0e500000, "str%t%c\t%12-15r, %a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x06400000, 0x0e500ff0, "strb%t%c\t%12-15R, %a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x06000000, 0x0e500ff0, "str%t%c\t%12-15r, %a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x04400000, 0x0c500010, "strb%t%c\t%12-15R, %a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x04000000, 0x0c500010, "str%t%c\t%12-15r, %a"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x04400000, 0x0e500000, "strb%c\t%12-15R, %a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x06400000, 0x0e500010, "strb%c\t%12-15R, %a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x004000b0, 0x0e5000f0, "strh%c\t%12-15R, %s"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x000000b0, 0x0e500ff0, "strh%c\t%12-15R, %s"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00500090, 0x0e5000f0, UNDEFINED_INSTRUCTION},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00500090, 0x0e500090, "ldr%6's%5?hb%c\t%12-15R, %s"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00100090, 0x0e500ff0, UNDEFINED_INSTRUCTION},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00100090, 0x0e500f90, "ldr%6's%5?hb%c\t%12-15R, %s"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x02000000, 0x0fe00000, "and%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00000000, 0x0fe00010, "and%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00000010, 0x0fe00090, "and%20's%c\t%12-15R, %16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x02200000, 0x0fe00000, "eor%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00200000, 0x0fe00010, "eor%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00200010, 0x0fe00090, "eor%20's%c\t%12-15R, %16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x02400000, 0x0fe00000, "sub%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00400000, 0x0fe00010, "sub%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00400010, 0x0fe00090, "sub%20's%c\t%12-15R, %16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x02600000, 0x0fe00000, "rsb%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00600000, 0x0fe00010, "rsb%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00600010, 0x0fe00090, "rsb%20's%c\t%12-15R, %16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x02800000, 0x0fe00000, "add%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00800000, 0x0fe00010, "add%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00800010, 0x0fe00090, "add%20's%c\t%12-15R, %16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x02a00000, 0x0fe00000, "adc%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00a00000, 0x0fe00010, "adc%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00a00010, 0x0fe00090, "adc%20's%c\t%12-15R, %16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x02c00000, 0x0fe00000, "sbc%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00c00000, 0x0fe00010, "sbc%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00c00010, 0x0fe00090, "sbc%20's%c\t%12-15R, %16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x02e00000, 0x0fe00000, "rsc%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00e00000, 0x0fe00010, "rsc%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00e00010, 0x0fe00090, "rsc%20's%c\t%12-15R, %16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_VIRT), 0x0120f200, 0x0fb0f200, "msr%c\t%C, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V3), 0x0120f000, 0x0db0f000, "msr%c\t%C, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V3), 0x01000000, 0x0fb00cff, "mrs%c\t%12-15R, %R"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x03000000, 0x0fe00000, "tst%p%c\t%16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01000000, 0x0fe00010, "tst%p%c\t%16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01000010, 0x0fe00090, "tst%p%c\t%16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x03300000, 0x0ff00000, "teq%p%c\t%16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01300000, 0x0ff00010, "teq%p%c\t%16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01300010, 0x0ff00010, "teq%p%c\t%16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x03400000, 0x0fe00000, "cmp%p%c\t%16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01400000, 0x0fe00010, "cmp%p%c\t%16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01400010, 0x0fe00090, "cmp%p%c\t%16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x03600000, 0x0fe00000, "cmn%p%c\t%16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01600000, 0x0fe00010, "cmn%p%c\t%16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01600010, 0x0fe00090, "cmn%p%c\t%16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x03800000, 0x0fe00000, "orr%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01800000, 0x0fe00010, "orr%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01800010, 0x0fe00090, "orr%20's%c\t%12-15R, %16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x03a00000, 0x0fef0000, "mov%20's%c\t%12-15r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01a00000, 0x0def0ff0, "mov%20's%c\t%12-15r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01a00000, 0x0def0060, "lsl%20's%c\t%12-15R, %q"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01a00020, 0x0def0060, "lsr%20's%c\t%12-15R, %q"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01a00040, 0x0def0060, "asr%20's%c\t%12-15R, %q"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01a00060, 0x0def0ff0, "rrx%20's%c\t%12-15r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01a00060, 0x0def0060, "ror%20's%c\t%12-15R, %q"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x03c00000, 0x0fe00000, "bic%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01c00000, 0x0fe00010, "bic%20's%c\t%12-15r, %16-19r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01c00010, 0x0fe00090, "bic%20's%c\t%12-15R, %16-19R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x03e00000, 0x0fe00000, "mvn%20's%c\t%12-15r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01e00000, 0x0fe00010, "mvn%20's%c\t%12-15r, %o"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x01e00010, 0x0fe00090, "mvn%20's%c\t%12-15R, %o"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x06000010, 0x0e000010, UNDEFINED_INSTRUCTION},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x049d0004, 0x0fff0fff, "pop%c\t{%12-15r}\t\t; (ldr%c %12-15r, %a)"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x04500000, 0x0c500000, "ldrb%t%c\t%12-15R, %a"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x04300000, 0x0d700000, "ldrt%c\t%12-15R, %a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x04100000, 0x0c500000, "ldr%c\t%12-15r, %a"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0001, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0002, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0004, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0008, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0010, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0020, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0040, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0080, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0100, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0200, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0400, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0800, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d1000, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d2000, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d4000, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d8000, 0x0fffffff, "stmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x092d0000, 0x0fff0000, "push%c\t%m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08800000, 0x0ff00000, "stm%c\t%16-19R%21'!, %m%22'^"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08000000, 0x0e100000, "stm%23?id%24?ba%c\t%16-19R%21'!, %m%22'^"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0001, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0002, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0004, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0008, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0010, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0020, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0040, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0080, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0100, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0200, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0400, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0800, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd1000, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd2000, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd4000, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd8000, 0x0fffffff, "ldmfd%c\t%16-19R!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08bd0000, 0x0fff0000, "pop%c\t%m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08900000, 0x0f900000, "ldm%c\t%16-19R%21'!, %m%22'^"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x08100000, 0x0e100000, "ldm%23?id%24?ba%c\t%16-19R%21'!, %m%22'^"},

    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x0a000000, 0x0e000000, "b%24'l%c\t%b"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x0f000000, 0x0f000000, "svc%c\t%0-23x"},

    /* The rest.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0x03200000, 0x0fff00ff, "nop%c\t{%0-7d}" UNPREDICTABLE_INSTRUCTION},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00000000, 0x00000000, UNDEFINED_INSTRUCTION},
    {ARM_FEATURE_CORE_LOW(0), 0x00000000, 0x00000000, 0}};

/* print_insn_thumb16 recognizes the following format control codes:

   %S                   print Thumb register (bits 3..5 as high number if bit 6 set)
   %D                   print Thumb register (bits 0..2 as high number if bit 7 set)
   %<bitfield>I         print bitfield as a signed decimal
          (top bit of range being the sign bit)
   %N                   print Thumb register mask (with LR)
   %O                   print Thumb register mask (with PC)
   %M                   print Thumb register mask
   %b			print CZB's 6-bit unsigned branch destination
   %s			print Thumb right-shift immediate (6..10; 0 == 32).
   %c			print the condition code
   %C			print the condition code, or "s" if not conditional
   %x			print warning if conditional an not at end of IT block"
   %X			print "\t; unpredictable <IT:code>" if conditional
   %I			print IT instruction suffix and operands
   %W			print Thumb Writeback indicator for LDMIA
   %<bitfield>r		print bitfield as an ARM register
   %<bitfield>d		print bitfield as a decimal
   %<bitfield>H         print (bitfield * 2) as a decimal
   %<bitfield>W         print (bitfield * 4) as a decimal
   %<bitfield>a         print (bitfield * 4) as a pc-rel offset + decoded symbol
   %<bitfield>B         print Thumb branch destination (signed displacement)
   %<bitfield>c         print bitfield as a condition code
   %<bitnum>'c		print specified char iff bit is one
   %<bitnum>?ab		print a if bit is one else print b.  */

static const struct opcode16 thumb_opcodes[] = {
    /* Thumb instructions.  */

    /* ARMv8-M Security Extensions instructions.  */
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8M), 0x4784, 0xff87, "blxns\t%3-6r"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8M), 0x4704, 0xff87, "bxns\t%3-6r"},

    /* ARM V8 instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xbf50, 0xffff, "sevl%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xba80, 0xffc0, "hlt\t%0-5x"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_PAN), 0xb610, 0xfff7, "setpan\t#%3-3d"},

    /* ARM V6K no-argument instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0xbf00, 0xffff, "nop%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0xbf10, 0xffff, "yield%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0xbf20, 0xffff, "wfe%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0xbf30, 0xffff, "wfi%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0xbf40, 0xffff, "sev%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6K), 0xbf00, 0xff0f, "nop%c\t{%4-7d}"},

    /* ARM V6T2 instructions.  */
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0xb900, 0xfd00, "cbnz\t%0-2r, %b%X"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0xb100, 0xfd00, "cbz\t%0-2r, %b%X"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xbf00, 0xff00, "it%I%X"},

    /* ARM V6.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xb660, 0xfff8, "cpsie\t%2'a%1'i%0'f%X"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xb670, 0xfff8, "cpsid\t%2'a%1'i%0'f%X"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0x4600, 0xffc0, "mov%c\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xba00, 0xffc0, "rev%c\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xba40, 0xffc0, "rev16%c\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xbac0, 0xffc0, "revsh%c\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xb650, 0xfff7, "setend\t%3?ble%X"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xb200, 0xffc0, "sxth%c\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xb240, 0xffc0, "sxtb%c\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xb280, 0xffc0, "uxth%c\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6), 0xb2c0, 0xffc0, "uxtb%c\t%0-2r, %3-5r"},

    /* ARM V5 ISA extends Thumb.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5T), 0xbe00, 0xff00, "bkpt\t%0-7x"}, /* Is always unconditional.  */
    /* This is BLX(2).  BLX(1) is a 32-bit instruction.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V5T), 0x4780, 0xff87, "blx%c\t%3-6r%x"}, /* note: 4 bit register number.  */
    /* ARM V4T ISA (Thumb v1).  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x46C0, 0xFFFF, "nop%c\t\t\t; (mov r8, r8)"},
    /* Format 4.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4000, 0xFFC0, "and%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4040, 0xFFC0, "eor%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4080, 0xFFC0, "lsl%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x40C0, 0xFFC0, "lsr%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4100, 0xFFC0, "asr%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4140, 0xFFC0, "adc%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4180, 0xFFC0, "sbc%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x41C0, 0xFFC0, "ror%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4200, 0xFFC0, "tst%c\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4240, 0xFFC0, "neg%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4280, 0xFFC0, "cmp%c\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x42C0, 0xFFC0, "cmn%c\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4300, 0xFFC0, "orr%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4340, 0xFFC0, "mul%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4380, 0xFFC0, "bic%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x43C0, 0xFFC0, "mvn%C\t%0-2r, %3-5r"},
    /* format 13 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xB000, 0xFF80, "add%c\tsp, #%0-6W"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xB080, 0xFF80, "sub%c\tsp, #%0-6W"},
    /* format 5 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4700, 0xFF80, "bx%c\t%S%x"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4400, 0xFF00, "add%c\t%D, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4500, 0xFF00, "cmp%c\t%D, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4600, 0xFF00, "mov%c\t%D, %S"},
    /* format 14 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xB400, 0xFE00, "push%c\t%N"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xBC00, 0xFE00, "pop%c\t%O"},
    /* format 2 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x1800, 0xFE00, "add%C\t%0-2r, %3-5r, %6-8r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x1A00, 0xFE00, "sub%C\t%0-2r, %3-5r, %6-8r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x1C00, 0xFE00, "add%C\t%0-2r, %3-5r, #%6-8d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x1E00, 0xFE00, "sub%C\t%0-2r, %3-5r, #%6-8d"},
    /* format 8 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x5200, 0xFE00, "strh%c\t%0-2r, [%3-5r, %6-8r]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x5A00, 0xFE00, "ldrh%c\t%0-2r, [%3-5r, %6-8r]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x5600, 0xF600, "ldrs%11?hb%c\t%0-2r, [%3-5r, %6-8r]"},
    /* format 7 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x5000, 0xFA00, "str%10'b%c\t%0-2r, [%3-5r, %6-8r]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x5800, 0xFA00, "ldr%10'b%c\t%0-2r, [%3-5r, %6-8r]"},
    /* format 1 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x0000, 0xFFC0, "mov%C\t%0-2r, %3-5r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x0000, 0xF800, "lsl%C\t%0-2r, %3-5r, #%6-10d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x0800, 0xF800, "lsr%C\t%0-2r, %3-5r, %s"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x1000, 0xF800, "asr%C\t%0-2r, %3-5r, %s"},
    /* format 3 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x2000, 0xF800, "mov%C\t%8-10r, #%0-7d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x2800, 0xF800, "cmp%c\t%8-10r, #%0-7d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x3000, 0xF800, "add%C\t%8-10r, #%0-7d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x3800, 0xF800, "sub%C\t%8-10r, #%0-7d"},
    /* format 6 */
    /* TODO: Disassemble PC relative "LDR rD,=<symbolic>" */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x4800, 0xF800, "ldr%c\t%8-10r, [pc, #%0-7W]\t; (%0-7a)"},
    /* format 9 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x6000, 0xF800, "str%c\t%0-2r, [%3-5r, #%6-10W]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x6800, 0xF800, "ldr%c\t%0-2r, [%3-5r, #%6-10W]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x7000, 0xF800, "strb%c\t%0-2r, [%3-5r, #%6-10d]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x7800, 0xF800, "ldrb%c\t%0-2r, [%3-5r, #%6-10d]"},
    /* format 10 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x8000, 0xF800, "strh%c\t%0-2r, [%3-5r, #%6-10H]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x8800, 0xF800, "ldrh%c\t%0-2r, [%3-5r, #%6-10H]"},
    /* format 11 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x9000, 0xF800, "str%c\t%8-10r, [sp, #%0-7W]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0x9800, 0xF800, "ldr%c\t%8-10r, [sp, #%0-7W]"},
    /* format 12 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xA000, 0xF800, "add%c\t%8-10r, pc, #%0-7W\t; (adr %8-10r, %0-7a)"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xA800, 0xF800, "add%c\t%8-10r, sp, #%0-7W"},
    /* format 15 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xC000, 0xF800, "stmia%c\t%8-10r!, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xC800, 0xF800, "ldmia%c\t%8-10r%W, %M"},
    /* format 17 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xDF00, 0xFF00, "svc%c\t%0-7d"},
    /* format 16 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xDE00, 0xFF00, "udf%c\t#%0-7d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xDE00, 0xFE00, UNDEFINED_INSTRUCTION},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xD000, 0xF000, "b%8-11c.n\t%0-7B%X"},
    /* format 18 */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xE000, 0xF800, "b%c.n\t%0-10B%x"},

    /* The E800 .. FFFF range is unconditionally redirected to the
       32-bit table, because even in pre-V6T2 ISAs, BL and BLX(1) pairs
       are processed via that table.  Thus, we can never encounter a
       bare "second half of BL/BLX(1)" instruction here.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x0000, 0x0000, UNDEFINED_INSTRUCTION},
    {ARM_FEATURE_CORE_LOW(0), 0, 0, 0}};

/* Thumb32 opcodes use the same table structure as the ARM opcodes.
   We adopt the convention that hw1 is the high 16 bits of .value and
   .mask, hw2 the low 16 bits.

   print_insn_thumb32 recognizes the following format control codes:

       %%		%

       %I		print a 12-bit immediate from hw1[10],hw2[14:12,7:0]
       %M		print a modified 12-bit immediate (same location)
       %J		print a 16-bit immediate from hw1[3:0,10],hw2[14:12,7:0]
       %K		print a 16-bit immediate from hw2[3:0],hw1[3:0],hw2[11:4]
       %H		print a 16-bit immediate from hw2[3:0],hw1[11:0]
       %S		print a possibly-shifted Rm

       %L		print address for a ldrd/strd instruction
       %a		print the address of a plain load/store
       %w		print the width and signedness of a core load/store
       %m		print register mask for ldm/stm

       %E		print the lsb and width fields of a bfc/bfi instruction
       %F		print the lsb and width fields of a sbfx/ubfx instruction
       %b		print a conditional branch offset
       %B		print an unconditional branch offset
       %s		print the shift field of an SSAT instruction
       %R		print the rotation field of an SXT instruction
       %U		print barrier type.
       %P		print address for pli instruction.
       %c		print the condition code
       %x		print warning if conditional an not at end of IT block"
       %X		print "\t; unpredictable <IT:code>" if conditional

       %<bitfield>d	print bitfield in decimal
       %<bitfield>D     print bitfield plus one in decimal
       %<bitfield>W	print bitfield*4 in decimal
       %<bitfield>r	print bitfield as an ARM register
       %<bitfield>R	as %<>r but r15 is UNPREDICTABLE
       %<bitfield>c	print bitfield as a condition code

       %<bitfield>'c	print specified char iff bitfield is all ones
       %<bitfield>`c	print specified char iff bitfield is all zeroes
       %<bitfield>?ab... select from array of values in big endian order

   With one exception at the bottom (done because BL and BLX(1) need
   to come dead last), this table was machine-sorted first in
   decreasing order of number of bits set in the mask, then in
   increasing numeric order of mask, then in increasing numeric order
   of opcode.  This order is not the clearest for a human reader, but
   is guaranteed never to catch a special-case bit pattern with a more
   general mask, which is important, because this instruction encoding
   makes heavy use of special-case bit patterns.  */
static const struct opcode32 thumb32_opcodes[] = {
    /* ARMv8-M and ARMv8-M Security Extensions instructions.  */
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8M), 0xe97fe97f, 0xffffffff, "sg"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8M), 0xe840f000, 0xfff0f0ff, "tt\t%8-11r, %16-19r"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8M), 0xe840f040, 0xfff0f0ff, "ttt\t%8-11r, %16-19r"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8M), 0xe840f080, 0xfff0f0ff, "tta\t%8-11r, %16-19r"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V8M), 0xe840f0c0, 0xfff0f0ff, "ttat\t%8-11r, %16-19r"},

    /* ARM V8.2 RAS extension instructions.  */
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_RAS), 0xf3af8010, 0xffffffff, "esb"},

    /* V8 instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xf3af8005, 0xffffffff, "sevl%c.w"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xf78f8000, 0xfffffffc, "dcps%0-1d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8c00f8f, 0xfff00fff, "stlb%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8c00f9f, 0xfff00fff, "stlh%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8c00faf, 0xfff00fff, "stl%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8c00fc0, 0xfff00ff0, "stlexb%c\t%0-3r, %12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8c00fd0, 0xfff00ff0, "stlexh%c\t%0-3r, %12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8c00fe0, 0xfff00ff0, "stlex%c\t%0-3r, %12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8c000f0, 0xfff000f0, "stlexd%c\t%0-3r, %12-15r, %8-11r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8d00f8f, 0xfff00fff, "ldab%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8d00f9f, 0xfff00fff, "ldah%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8d00faf, 0xfff00fff, "lda%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8d00fcf, 0xfff00fff, "ldaexb%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8d00fdf, 0xfff00fff, "ldaexh%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8d00fef, 0xfff00fff, "ldaex%c\t%12-15r, [%16-19R]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xe8d000ff, 0xfff000ff, "ldaexd%c\t%12-15r, %8-11r, [%16-19R]"},

    /* CRC32 instructions.  */
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xfac0f080, 0xfff0f0f0, "crc32b\t%8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xfac0f090, 0xfff0f0f0, "crc32h\t%9-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xfac0f0a0, 0xfff0f0f0, "crc32w\t%8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xfad0f080, 0xfff0f0f0, "crc32cb\t%8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xfad0f090, 0xfff0f0f0, "crc32ch\t%8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_COPROC(CRC_EXT_ARMV8), 0xfad0f0a0, 0xfff0f0f0, "crc32cw\t%8-11R, %16-19R, %0-3R"},

    /* Speculation Barriers.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8014, 0xffffffff, "csdb"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3bf8f40, 0xffffffff, "ssbb"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3bf8f44, 0xffffffff, "pssbb"},

    /* V7 instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0xf910f000, 0xff70f000, "pli%c\t%a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0xf3af80f0, 0xfffffff0, "dbg%c\t#%0-3d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xf3bf8f51, 0xfffffff3, "dmb%c\t%U"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V8), 0xf3bf8f41, 0xfffffff3, "dsb%c\t%U"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0xf3bf8f50, 0xfffffff0, "dmb%c\t%U"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0xf3bf8f40, 0xfffffff0, "dsb%c\t%U"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V7), 0xf3bf8f60, 0xfffffff0, "isb%c\t%U"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_DIV), 0xfb90f0f0, 0xfff0f0f0, "sdiv%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_DIV), 0xfbb0f0f0, 0xfff0f0f0, "udiv%c\t%8-11r, %16-19r, %0-3r"},

    /* Virtualization Extension instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_VIRT), 0xf7e08000, 0xfff0f000, "hvc%c\t%V"},
    /* We skip ERET as that is SUBS pc, lr, #0.  */

    /* MP Extension instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_MP), 0xf830f000, 0xff70f000, "pldw%c\t%a"},

    /* Security extension instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_SEC), 0xf7f08000, 0xfff0f000, "smc%c\t%K"},

    /* Instructions defined in the basic V6T2 set.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8000, 0xffffffff, "nop%c.w"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8001, 0xffffffff, "yield%c.w"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8002, 0xffffffff, "wfe%c.w"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8003, 0xffffffff, "wfi%c.w"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8004, 0xffffffff, "sev%c.w"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8000, 0xffffff00, "nop%c.w\t{%0-7d}"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf7f0a000, 0xfff0f000, "udf%c.w\t%H"},

    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0xf3bf8f2f, 0xffffffff, "clrex%c"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8400, 0xffffff1f, "cpsie.w\t%7'a%6'i%5'f%X"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8600, 0xffffff1f, "cpsid.w\t%7'a%6'i%5'f%X"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3c08f00, 0xfff0ffff, "bxj%c\t%16-19r%x"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe810c000, 0xffd0ffff, "rfedb%c\t%16-19r%21'!"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe990c000, 0xffd0ffff, "rfeia%c\t%16-19r%21'!"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3e08000, 0xffe0f000, "mrs%c\t%8-11r, %D"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8100, 0xffffffe0, "cps\t#%0-4d%X"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe8d0f000, 0xfff0fff0, "tbb%c\t[%16-19r, %0-3r]%x"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe8d0f010, 0xfff0fff0, "tbh%c\t[%16-19r, %0-3r, lsl #1]%x"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8500, 0xffffff00, "cpsie\t%7'a%6'i%5'f, #%0-4d%X"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3af8700, 0xffffff00, "cpsid\t%7'a%6'i%5'f, #%0-4d%X"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3de8f00, 0xffffff00, "subs%c\tpc, lr, #%0-7d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3808000, 0xffe0f000, "msr%c\t%C, %16-19r"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0xe8500f00, 0xfff00fff, "ldrex%c\t%12-15r, [%16-19r]"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0xe8d00f4f, 0xfff00fef, "ldrex%4?hb%c\t%12-15r, [%16-19r]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe800c000, 0xffd0ffe0, "srsdb%c\t%16-19r%21'!, #%0-4d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe980c000, 0xffd0ffe0, "srsia%c\t%16-19r%21'!, #%0-4d"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa0ff080, 0xfffff0c0, "sxth%c.w\t%8-11r, %0-3r%R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa1ff080, 0xfffff0c0, "uxth%c.w\t%8-11r, %0-3r%R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa2ff080, 0xfffff0c0, "sxtb16%c\t%8-11r, %0-3r%R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa3ff080, 0xfffff0c0, "uxtb16%c\t%8-11r, %0-3r%R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa4ff080, 0xfffff0c0, "sxtb%c.w\t%8-11r, %0-3r%R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa5ff080, 0xfffff0c0, "uxtb%c.w\t%8-11r, %0-3r%R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0xe8400000, 0xfff000ff, "strex%c\t%8-11r, %12-15r, [%16-19r]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe8d0007f, 0xfff000ff, "ldrexd%c\t%12-15r, %8-11r, [%16-19r]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa80f000, 0xfff0f0f0, "sadd8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa80f010, 0xfff0f0f0, "qadd8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa80f020, 0xfff0f0f0, "shadd8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa80f040, 0xfff0f0f0, "uadd8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa80f050, 0xfff0f0f0, "uqadd8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa80f060, 0xfff0f0f0, "uhadd8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa80f080, 0xfff0f0f0, "qadd%c\t%8-11r, %0-3r, %16-19r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa80f090, 0xfff0f0f0, "qdadd%c\t%8-11r, %0-3r, %16-19r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa80f0a0, 0xfff0f0f0, "qsub%c\t%8-11r, %0-3r, %16-19r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa80f0b0, 0xfff0f0f0, "qdsub%c\t%8-11r, %0-3r, %16-19r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa90f000, 0xfff0f0f0, "sadd16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa90f010, 0xfff0f0f0, "qadd16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa90f020, 0xfff0f0f0, "shadd16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa90f040, 0xfff0f0f0, "uadd16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa90f050, 0xfff0f0f0, "uqadd16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa90f060, 0xfff0f0f0, "uhadd16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa90f080, 0xfff0f0f0, "rev%c.w\t%8-11r, %16-19r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa90f090, 0xfff0f0f0, "rev16%c.w\t%8-11r, %16-19r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa90f0a0, 0xfff0f0f0, "rbit%c\t%8-11r, %16-19r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa90f0b0, 0xfff0f0f0, "revsh%c.w\t%8-11r, %16-19r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfaa0f000, 0xfff0f0f0, "sasx%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfaa0f010, 0xfff0f0f0, "qasx%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfaa0f020, 0xfff0f0f0, "shasx%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfaa0f040, 0xfff0f0f0, "uasx%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfaa0f050, 0xfff0f0f0, "uqasx%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfaa0f060, 0xfff0f0f0, "uhasx%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfaa0f080, 0xfff0f0f0, "sel%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfab0f080, 0xfff0f0f0, "clz%c\t%8-11r, %16-19r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfac0f000, 0xfff0f0f0, "ssub8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfac0f010, 0xfff0f0f0, "qsub8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfac0f020, 0xfff0f0f0, "shsub8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfac0f040, 0xfff0f0f0, "usub8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfac0f050, 0xfff0f0f0, "uqsub8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfac0f060, 0xfff0f0f0, "uhsub8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfad0f000, 0xfff0f0f0, "ssub16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfad0f010, 0xfff0f0f0, "qsub16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfad0f020, 0xfff0f0f0, "shsub16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfad0f040, 0xfff0f0f0, "usub16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfad0f050, 0xfff0f0f0, "uqsub16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfad0f060, 0xfff0f0f0, "uhsub16%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfae0f000, 0xfff0f0f0, "ssax%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfae0f010, 0xfff0f0f0, "qsax%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfae0f020, 0xfff0f0f0, "shsax%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfae0f040, 0xfff0f0f0, "usax%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfae0f050, 0xfff0f0f0, "uqsax%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfae0f060, 0xfff0f0f0, "uhsax%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb00f000, 0xfff0f0f0, "mul%c.w\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb70f000, 0xfff0f0f0, "usad8%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa00f000, 0xffe0f0f0, "lsl%20's%c.w\t%8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa20f000, 0xffe0f0f0, "lsr%20's%c.w\t%8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa40f000, 0xffe0f0f0, "asr%20's%c.w\t%8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa60f000, 0xffe0f0f0, "ror%20's%c.w\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0xe8c00f40, 0xfff00fe0, "strex%4?hb%c\t%0-3r, %12-15r, [%16-19r]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3200000, 0xfff0f0e0, "ssat16%c\t%8-11r, #%0-4D, %16-19r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3a00000, 0xfff0f0e0, "usat16%c\t%8-11r, #%0-4d, %16-19r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb20f000, 0xfff0f0e0, "smuad%4'x%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb30f000, 0xfff0f0e0, "smulw%4?tb%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb40f000, 0xfff0f0e0, "smusd%4'x%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb50f000, 0xfff0f0e0, "smmul%4'r%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa00f080, 0xfff0f0c0, "sxtah%c\t%8-11r, %16-19r, %0-3r%R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa10f080, 0xfff0f0c0, "uxtah%c\t%8-11r, %16-19r, %0-3r%R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa20f080, 0xfff0f0c0, "sxtab16%c\t%8-11r, %16-19r, %0-3r%R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa30f080, 0xfff0f0c0, "uxtab16%c\t%8-11r, %16-19r, %0-3r%R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa40f080, 0xfff0f0c0, "sxtab%c\t%8-11r, %16-19r, %0-3r%R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfa50f080, 0xfff0f0c0, "uxtab%c\t%8-11r, %16-19r, %0-3r%R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb10f000, 0xfff0f0c0, "smul%5?tb%4?tb%c\t%8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf36f0000, 0xffff8020, "bfc%c\t%8-11r, %E"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xea100f00, 0xfff08f00, "tst%c.w\t%16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xea900f00, 0xfff08f00, "teq%c\t%16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xeb100f00, 0xfff08f00, "cmn%c.w\t%16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xebb00f00, 0xfff08f00, "cmp%c.w\t%16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf0100f00, 0xfbf08f00, "tst%c.w\t%16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf0900f00, 0xfbf08f00, "teq%c\t%16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf1100f00, 0xfbf08f00, "cmn%c.w\t%16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf1b00f00, 0xfbf08f00, "cmp%c.w\t%16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xea4f0000, 0xffef8000, "mov%20's%c.w\t%8-11r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xea6f0000, 0xffef8000, "mvn%20's%c.w\t%8-11r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe8c00070, 0xfff000f0, "strexd%c\t%0-3r, %12-15r, %8-11r, [%16-19r]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb000000, 0xfff000f0, "mla%c\t%8-11r, %16-19r, %0-3r, %12-15r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb000010, 0xfff000f0, "mls%c\t%8-11r, %16-19r, %0-3r, %12-15r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb700000, 0xfff000f0, "usada8%c\t%8-11R, %16-19R, %0-3R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb800000, 0xfff000f0, "smull%c\t%12-15R, %8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfba00000, 0xfff000f0, "umull%c\t%12-15R, %8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfbc00000, 0xfff000f0, "smlal%c\t%12-15R, %8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfbe00000, 0xfff000f0, "umlal%c\t%12-15R, %8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfbe00060, 0xfff000f0, "umaal%c\t%12-15R, %8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0xe8500f00, 0xfff00f00, "ldrex%c\t%12-15r, [%16-19r, #%0-7W]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf04f0000, 0xfbef8000, "mov%20's%c.w\t%8-11r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf06f0000, 0xfbef8000, "mvn%20's%c.w\t%8-11r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf810f000, 0xff70f000, "pld%c\t%a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb200000, 0xfff000e0, "smlad%4'x%c\t%8-11R, %16-19R, %0-3R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb300000, 0xfff000e0, "smlaw%4?tb%c\t%8-11R, %16-19R, %0-3R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb400000, 0xfff000e0, "smlsd%4'x%c\t%8-11R, %16-19R, %0-3R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb500000, 0xfff000e0, "smmla%4'r%c\t%8-11R, %16-19R, %0-3R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb600000, 0xfff000e0, "smmls%4'r%c\t%8-11R, %16-19R, %0-3R, %12-15R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfbc000c0, 0xfff000e0, "smlald%4'x%c\t%12-15R, %8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfbd000c0, 0xfff000e0, "smlsld%4'x%c\t%12-15R, %8-11R, %16-19R, %0-3R"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xeac00000, 0xfff08030, "pkhbt%c\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xeac00020, 0xfff08030, "pkhtb%c\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3400000, 0xfff08020, "sbfx%c\t%8-11r, %16-19r, %F"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3c00000, 0xfff08020, "ubfx%c\t%8-11r, %16-19r, %F"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf8000e00, 0xff900f00, "str%wt%c\t%12-15r, %a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfb100000, 0xfff000c0, "smla%5?tb%4?tb%c\t%8-11r, %16-19r, %0-3r, %12-15r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xfbc00080, 0xfff000c0, "smlal%5?tb%4?tb%c\t%12-15r, %8-11r, %16-19r, %0-3r"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3600000, 0xfff08020, "bfi%c\t%8-11r, %16-19r, %E"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf8100e00, 0xfe900f00, "ldr%wt%c\t%12-15r, %a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3000000, 0xffd08020, "ssat%c\t%8-11r, #%0-4D, %16-19r%s"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3800000, 0xffd08020, "usat%c\t%8-11r, #%0-4d, %16-19r%s"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf2000000, 0xfbf08000, "addw%c\t%8-11r, %16-19r, %I"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0xf2400000, 0xfbf08000, "movw%c\t%8-11r, %J"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf2a00000, 0xfbf08000, "subw%c\t%8-11r, %16-19r, %I"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0xf2c00000, 0xfbf08000, "movt%c\t%8-11r, %J"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xea000000, 0xffe08000, "and%20's%c.w\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xea200000, 0xffe08000, "bic%20's%c.w\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xea400000, 0xffe08000, "orr%20's%c.w\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xea600000, 0xffe08000, "orn%20's%c\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xea800000, 0xffe08000, "eor%20's%c.w\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xeb000000, 0xffe08000, "add%20's%c.w\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xeb400000, 0xffe08000, "adc%20's%c.w\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xeb600000, 0xffe08000, "sbc%20's%c.w\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xeba00000, 0xffe08000, "sub%20's%c.w\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xebc00000, 0xffe08000, "rsb%20's%c\t%8-11r, %16-19r, %S"},
    {ARM_FEATURE_CORE_HIGH(ARM_EXT2_V6T2_V8M), 0xe8400000, 0xfff00000, "strex%c\t%8-11r, %12-15r, [%16-19r, #%0-7W]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf0000000, 0xfbe08000, "and%20's%c.w\t%8-11r, %16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf0200000, 0xfbe08000, "bic%20's%c.w\t%8-11r, %16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf0400000, 0xfbe08000, "orr%20's%c.w\t%8-11r, %16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf0600000, 0xfbe08000, "orn%20's%c\t%8-11r, %16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf0800000, 0xfbe08000, "eor%20's%c.w\t%8-11r, %16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf1000000, 0xfbe08000, "add%20's%c.w\t%8-11r, %16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf1400000, 0xfbe08000, "adc%20's%c.w\t%8-11r, %16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf1600000, 0xfbe08000, "sbc%20's%c.w\t%8-11r, %16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf1a00000, 0xfbe08000, "sub%20's%c.w\t%8-11r, %16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf1c00000, 0xfbe08000, "rsb%20's%c\t%8-11r, %16-19r, %M"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe8800000, 0xffd00000, "stmia%c.w\t%16-19r%21'!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe8900000, 0xffd00000, "ldmia%c.w\t%16-19r%21'!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe9000000, 0xffd00000, "stmdb%c\t%16-19r%21'!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe9100000, 0xffd00000, "ldmdb%c\t%16-19r%21'!, %m"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe9c00000, 0xffd000ff, "strd%c\t%12-15r, %8-11r, [%16-19r]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe9d00000, 0xffd000ff, "ldrd%c\t%12-15r, %8-11r, [%16-19r]"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe9400000, 0xff500000,
     "strd%c\t%12-15r, %8-11r, [%16-19r, #%23`-%0-7W]%21'!%L"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe9500000, 0xff500000,
     "ldrd%c\t%12-15r, %8-11r, [%16-19r, #%23`-%0-7W]%21'!%L"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe8600000, 0xff700000, "strd%c\t%12-15r, %8-11r, [%16-19r], #%23`-%0-7W%L"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xe8700000, 0xff700000, "ldrd%c\t%12-15r, %8-11r, [%16-19r], #%23`-%0-7W%L"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf8000000, 0xff100000, "str%w%c.w\t%12-15r, %a"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf8100000, 0xfe100000, "ldr%w%c.w\t%12-15r, %a"},

    /* Filter out Bcc with cond=E or F, which are used for other instructions.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3c08000, 0xfbc0d000, "undefined (bcc, cond=0xF)"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf3808000, 0xfbc0d000, "undefined (bcc, cond=0xE)"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf0008000, 0xf800d000, "b%22-25c.w\t%b%X"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V6T2), 0xf0009000, 0xf800d000, "b%c.w\t%B%x"},

    /* These have been 32-bit since the invention of Thumb.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xf000c000, 0xf800d001, "blx%c\t%B%x"},
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V4T), 0xf000d000, 0xf800d000, "bl%c\t%B%x"},

    /* Fallback.  */
    {ARM_FEATURE_CORE_LOW(ARM_EXT_V1), 0x00000000, 0x00000000, UNDEFINED_INSTRUCTION},
    {ARM_FEATURE_CORE_LOW(0), 0, 0, 0}};

#endif