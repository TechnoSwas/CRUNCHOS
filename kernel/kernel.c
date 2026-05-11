void main() {
    // VGA text buffer lives at 0xB8000
    char* vga = (char*)0xB8000;
    
    const char* msg = "CrunchOS Kernel Loaded!";
    int i = 0;
    while (msg[i]) {
        vga[i * 2]     = msg[i];  // character
        vga[i * 2 + 1] = 0x0A;   // green text
        i++;
    }
    
    while(1); // hang forever
}
