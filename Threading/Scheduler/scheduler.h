void init_scheduler();
struct CPUContext* timer_irq_handler(struct CPUContext* current_context);
struct Thread* get_next_thread();
struct Thread* get_new_thread(struct CPUContext* current_context);