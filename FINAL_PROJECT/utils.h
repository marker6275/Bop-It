typedef struct current_order_t {
  char* task;
  struct current_order_t *next;
} current_order_t;

char* choose(void);

void game_init(void);

void opening_sequence(void);

void print_new_task(char* task, int level);

void print_do_sequence(void);

void print_done(char* task);

void print_good(void);

void print_lose(int score);