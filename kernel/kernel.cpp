// kernel.cpp

extern "C" void kernel_main();

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define MAX_OBJECTS 10
#define CATCHER_WIDTH 5   // Width of the catcher
#define FALL_RATE 10      // Objects move down every 10 game ticks

struct FallingObject {
    int x;
    int y;
    bool active;
};

static char *video_memory = (char *)0xB8000;
static int player_x = VGA_WIDTH / 2;
static FallingObject objects[MAX_OBJECTS];
static uint32_t rand_seed = 123456789;
static uint32_t score = 0;
static uint32_t game_tick = 0;
static uint32_t objectFallCounter = 0;

// Function prototypes
void clear_screen();
void initialize_game();
void handle_input();
void update_game();
void render();
void delay(uint32_t count);
uint8_t inb(uint16_t port);
void display_score();
uint32_t random();
void spawn_object();

extern "C" void kernel_main() {
    clear_screen();
    initialize_game();

    while (1) {
        handle_input();
        update_game();
        render();
        delay(5000000); // Adjust for game speed
    }
}

// Function implementations

void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = 0x07; // Light grey on black
    }
}

void initialize_game() {
    // Initialize falling objects
    for (int i = 0; i < MAX_OBJECTS; i++) {
        objects[i].active = false;
    }
    // Initialize player position
    player_x = (VGA_WIDTH - CATCHER_WIDTH) / 2;
}

void handle_input() {
    uint8_t scancode;
    if (inb(0x64) & 1) {
        scancode = inb(0x60);

        // Ignore key release events
        if (scancode & 0x80) {
            return;
        }

        switch (scancode) {
            case 0x4B: // Left arrow
                if (player_x > 0) player_x--;
                break;
            case 0x4D: // Right arrow
                if (player_x < VGA_WIDTH - CATCHER_WIDTH) player_x++;
                break;
            default:
                break;
        }
    }
}

void update_game() {
    game_tick++;

    // Spawn new object randomly
    if (random() % 20 == 0) { // Adjust the frequency as needed
        spawn_object();
    }

    // Move objects down at a slower rate
    if (objectFallCounter >= FALL_RATE) {
        objectFallCounter = 0;

        // Update falling objects
        for (int i = 0; i < MAX_OBJECTS; i++) {
            if (objects[i].active) {
                objects[i].y += 1;

                // Check for collision with player
                if (objects[i].y == VGA_HEIGHT - 1) {
                    // Check if object is within the catcher's width
                    if (objects[i].x >= player_x && objects[i].x < player_x + CATCHER_WIDTH) {
                        // Caught the object
                        score++;
                    }
                    // Deactivate object whether caught or missed
                    objects[i].active = false;
                } else if (objects[i].y >= VGA_HEIGHT) {
                    // Object has fallen off the screen
                    objects[i].active = false;
                }
            }
        }
    } else {
        objectFallCounter++;
    }
}

void render() {
    // Clear screen
    clear_screen();

    // Display score
    display_score();

    // Draw player (catcher)
    for (int i = 0; i < CATCHER_WIDTH; i++) {
        int player_idx = ((VGA_HEIGHT - 1) * VGA_WIDTH + player_x + i) * 2;
        video_memory[player_idx] = '_';
        video_memory[player_idx + 1] = 0x0A; // Light green on black
    }

    // Draw falling objects
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (objects[i].active) {
            int x = objects[i].x;
            int y = objects[i].y;
            if (y >= 0 && y < VGA_HEIGHT - 1) { // Exclude bottom row
                int idx = (y * VGA_WIDTH + x) * 2;
                video_memory[idx] = '*';
                video_memory[idx + 1] = 0x0E; // Yellow on black
            }
        }
    }
}

void delay(uint32_t count) {
    volatile uint32_t i;
    for (i = 0; i < count; i++) {
        // Empty loop for delay
    }
}

uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void display_score() {
    char score_str[20] = "Score: ";
    int idx = 7; // Position after "Score: "
    uint32_t temp_score = score;
    if (temp_score == 0) {
        score_str[idx++] = '0';
    } else {
        char digits[10];
        int digit_count = 0;
        while (temp_score > 0 && digit_count < 10) {
            digits[digit_count++] = '0' + (temp_score % 10);
            temp_score /= 10;
        }
        // Reverse the digits
        for (int i = digit_count - 1; i >= 0; i--) {
            score_str[idx++] = digits[i];
        }
    }
    score_str[idx] = '\0';

    // Display the score at the top-left corner
    for (int i = 0; score_str[i] != '\0'; i++) {
        video_memory[i * 2] = score_str[i];
        video_memory[i * 2 + 1] = 0x0F; // White on black
    }
}

uint32_t random() {
    rand_seed = rand_seed * 1103515245 + 12345;
    return rand_seed;
}

void spawn_object() {
    // Find an inactive object slot
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (!objects[i].active) {
            objects[i].active = true;
            objects[i].x = random() % VGA_WIDTH;
            objects[i].y = 0; // Start at the top
            break;
        }
    }
}
