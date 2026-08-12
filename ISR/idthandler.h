#include <stdint.h>

struct idt_entry_t {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr_t {
    uint16_t limit;           
    uint32_t base;            
} __attribute__((packed));

enum IDT_Exceptions
{
    INTERRUPT_DIVIDE_ERR = 0x0,
    INTERRUPT_DEBUG_EXCEPTION = 0x01,
    INTERRUPT_NMI_INTERRUPT = 0x02,
    INTERRUPT_BREAKPOINT = 0x03,
    INTERRUPT_OVERFLOW = 0x04,
    INTERRUPT_BOUND_RANGE_EXCEEDED = 0x05,
    INTERRUPT_INVALID_OPCODE = 0x06,
    INTERRUPT_DEVICE_NOT_AVAILABLE = 0x07,
    INTERRUPT_DOUBLE_FAULT = 0x08,
    INTERRUPT_COPROCESSOR_SEGMENT_OVERRUN = 0x09,
    INTERRUPT_INVALID_TSS = 0x0A,
    INTERRUPT_SEGMENT_NOT_PRESENT = 0x0B,
    INTERRUPT_STACK_SEGMENT_FAULT = 0x0C,
    INTERRUPT_GENERAL_PROTECTION_FAULT = 0x0D,
    INTERRUPT_PAGE_FAULT = 0x0E,
    INTERRUPT_RESERVED_0 = 0x0F,
    INTERRUPT_FLOATING_POINT_EXCEPTION = 0x10,
    INTERRUPT_ALIGNMENT_CHECK = 0x11,
    INTERRUPT_MACHINE_CHECK = 0x12,
    INTERRUPT_SIMD_FLOATING_POINT_EXCEPTION = 0x13,
    INTERRUPT_VIRTUALIZATION_EXCEPTION = 0x14,
    INTERRUPT_CONTROL_PROTECTION_EXCEPTION = 0x15,
    INTERRUPT_RESERVED_1 = 0x16,
    INTERRUPT_RESERVED_2 = 0x17,
    INTERRUPT_RESERVED_3 = 0x18,
    INTERRUPT_RESERVED_4 = 0x19,
    INTERRUPT_RESERVED_5 = 0x1A,
    INTERRUPT_RESERVED_6 = 0x1B,
    INTERRUPT_HYPERVISOR_INJECTION_EXCEPTION = 0x1C,
    INTERRUPT_VMM_COMMUNICATION_EXCEPTION = 0x1D,
    INTERRUPT_SECURITY_EXCEPTION = 0x1E,
    INTERRUPT_RESERVED_7 = 0x1F,
    INTERRUPT_UNKNOWN = 0xFFFF,
};

struct RegisterState
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;   // original ESP before pushad
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t interrupt;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};


void idt_set_gate(uint8_t num, uint32_t handler);
void idt_init();
void idt_load(struct idt_ptr_t* idtptr);
