// kernel.cpp
extern "C" void kernel_main() {
    const char *str = "Hello, World! This is an OS Kernel, created with help from OpenAI O1-Preview";
    char *video_memory = (char *)0xb8000;
    int i = 0;

    // Each character cell consists of the character and its attribute byte
    while (str[i] != '\0') {
        video_memory[i * 2] = str[i];       // Character
        video_memory[i * 2 + 1] = 0x07;     // Attribute byte (white on black)
        i++;
    }

    // Infinite loop to prevent the kernel from exiting
    while (1) {
        asm volatile ("hlt");
    }
}

