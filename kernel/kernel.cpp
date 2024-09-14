void print_string(const char* str) {
    char *video_memory = (char *)0xb8000;
    int i = 0;

    while (str[i] != '\0') {
        video_memory[i * 2] = str[i];
        video_memory[i * 2 + 1] = 0x07;
        i++;
    }
}

extern "C" void kernel_main() {
    print_string("----O1-OS--- Created by OpenAI O1 Reasoning Model----");
    while (1) {
        asm volatile ("hlt");
    }
}
