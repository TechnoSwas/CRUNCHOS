#include <stdint.h>

#define MULTIBOOT_MAGIC    0x1BADB002
#define MULTIBOOT_FLAGS    0x00
#define MULTIBOOT_CHECKSUM -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

__attribute__((section(".multiboot"))) __attribute__((aligned(4)))
unsigned int multiboot_header[] = {
    MULTIBOOT_MAGIC,
    MULTIBOOT_FLAGS,
    MULTIBOOT_CHECKSUM
};

#define VGA ((volatile uint16_t*)0xB8000)
#define W 80
#define H 25

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

int cursor_row = 0, cursor_col = 0;
uint8_t current_color = 0x07;

void set(int x, int y, char c, uint8_t color) {
    if (x < 0 || x >= W || y < 0 || y >= H) return;
    VGA[y * W + x] = (color << 8) | c;
}

void scroll() {
    for (int y = 0; y < H - 1; y++)
        for (int x = 0; x < W; x++)
            VGA[y * W + x] = VGA[(y+1) * W + x];
    for (int x = 0; x < W; x++)
        VGA[(H-1) * W + x] = (current_color << 8) | ' ';
}

void putchar(char c) {
    if (c == '\n') { cursor_col = 0; cursor_row++; }
    else {
        set(cursor_col, cursor_row, c, current_color);
        cursor_col++;
        if (cursor_col >= W) { cursor_col = 0; cursor_row++; }
    }
    if (cursor_row >= H) { scroll(); cursor_row = H - 1; }
}

void print(const char* s) { for (int i = 0; s[i]; i++) putchar(s[i]); }
void println(const char* s) { print(s); putchar('\n'); }

void fill_screen() {
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            VGA[y * W + x] = (current_color << 8) | ' ';
    cursor_row = 0; cursor_col = 0;
}

void fill_row(int y, uint8_t color) {
    for (int x = 0; x < W; x++)
        VGA[y * W + x] = (color << 8) | ' ';
}

void print_at(const char* s, int x, int y, uint8_t color) {
    for (int i = 0; s[i]; i++)
        set(x + i, y, s[i], color);
}

uint8_t read_key() {
    while (!(inb(0x64) & 1));
    return inb(0x60);
}

const char keymap[] = "  1234567890-=  qwertyuiop[]  asdfghjkl;'` \\zxcvbnm,./";

void print_int(int n) {
    if (n < 0) { putchar('-'); n = -n; }
    if (n == 0) { putchar('0'); return; }
    char buf[12]; int i = 0;
    while (n > 0) { buf[i++] = '0' + n % 10; n /= 10; }
    for (int j = i-1; j >= 0; j--) putchar(buf[j]);
}

int str_eq(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i] && a[i] == b[i]) i++;
    return a[i] == b[i];
}

int str_len(const char* s) { int i = 0; while (s[i]) i++; return i; }

void sleep_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 800; i++);
}

// === FAKE FS ===
#define MAX_FILES 16
#define MAX_DIRS 8
char filenames[MAX_FILES][32];
char filedata[MAX_FILES][128];
int file_count = 0;
char dirnames[MAX_DIRS][32];
int dir_count = 0;
char cwd[32] = "/";
char current_user[32] = "user";

// === COMMANDS ===
void cmd_help() {
    println("CrunchOS Shell Commands:");
    println("  help       - show this list");
    println("  clear      - clear screen");
    println("  ls         - list files");
    println("  mkdir      - make directory");
    println("  rm         - remove file");
    println("  echo       - print text");
    println("  cat        - show file contents");
    println("  pwd        - current directory");
    println("  uname      - OS info");
    println("  version    - OS version");
    println("  whoami     - current user");
    println("  date       - show date");
    println("  uptime     - system uptime");
    println("  mem        - memory info");
    println("  cpu        - CPU info");
    println("  disk       - disk info");
    println("  reboot     - restart system");
    println("  shutdown   - power off");
    println("  fortune    - random quote");
    println("  weather    - weather report");
    println("  banner     - big text");
    println("  neofetch   - system info");
    println("  crunch     - run .crunch app");
    println("  about      - about CrunchOS");
}

void cmd_ls() {
    if (dir_count == 0 && file_count == 0) { println("(empty)"); return; }
    for (int i = 0; i < dir_count; i++) { print("[DIR]  "); println(dirnames[i]); }
    for (int i = 0; i < file_count; i++) { print("[FILE] "); println(filenames[i]); }
}

void cmd_mkdir(const char* name) {
    if (str_len(name) == 0) { println("Usage: mkdir <name>"); return; }
    if (dir_count >= MAX_DIRS) { println("Error: too many directories"); return; }
    for (int i = 0; i < dir_count; i++)
        if (str_eq(dirnames[i], name)) { println("Error: already exists"); return; }
    int i = 0;
    while (name[i]) { dirnames[dir_count][i] = name[i]; i++; }
    dirnames[dir_count][i] = 0;
    dir_count++;
    print("Created: "); println(name);
}

void cmd_rm(const char* name) {
    for (int i = 0; i < file_count; i++) {
        if (str_eq(filenames[i], name)) {
            for (int j = i; j < file_count - 1; j++) {
                int k = 0;
                while (filenames[j+1][k]) { filenames[j][k] = filenames[j+1][k]; k++; }
                filenames[j][k] = 0;
            }
            file_count--;
            print("Removed: "); println(name);
            return;
        }
    }
    println("Error: file not found");
}

void cmd_echo(const char* text) { println(text); }

void cmd_cat(const char* name) {
    for (int i = 0; i < file_count; i++)
        if (str_eq(filenames[i], name)) { println(filedata[i]); return; }
    println("Error: file not found");
}

void cmd_pwd()     { println(cwd); }
void cmd_uname()   { println("CrunchOS 0.1 (x86 32-bit) GRUB/QEMU"); }
void cmd_whoami()  { println(current_user); }
void cmd_date()    { println("Thu May 14 2026 (RTC not yet implemented)"); }
void cmd_uptime()  { println("Uptime: since last boot"); }
void cmd_about()   { println("CrunchOS v0.1 - The OS built different."); println("Built by TechnoSwas & Enrico."); }
void cmd_crunch(const char* name) {
    if (str_len(name) == 0) { println("Usage: crunch <app.crunch>"); return; }
    print("Loading: "); println(name);
    println("Error: .crunch format coming soon!");
}

void cmd_version() {
    println("CrunchOS v0.1 Alpha");
    println("Built by TechnoSwas & Enrico");
    println("Kernel: CrunchOS Kernel (x86, 32-bit)");
}

void cmd_mem() {
    println("Memory Information:");
    println("  Total:  512 MB");
    println("  Used:   ~2 MB (kernel)");
    println("  Free:   ~510 MB");
}

void cmd_cpu() {
    println("CPU Information:");
    println("  Arch:   x86 32-bit");
    println("  Mode:   Protected Mode");
    println("  EMU:    QEMU");
}

void cmd_disk() {
    println("Disk Information:");
    println("  /dev/sda  20GB  CrunchOS");
}

void cmd_reboot() {
    println("Rebooting...");
    sleep_ms(500);
    uint8_t good = 0x02;
    while (good & 0x02) good = inb(0x64);
    outb(0x64, 0xFE);
}

void cmd_shutdown() {
    println("Shutting down CrunchOS...");
    sleep_ms(500);
    current_color = 0x00;
    fill_screen();
    outb(0x604, 0x2000);
}

void cmd_fortune() {
    const char* quotes[] = {
        "The best OS is the one you built yourself.",
        "There is no place like 127.0.0.1.",
        "It works on my machine.",
        "Have you tried turning it off and on again?",
        "CrunchOS: The OS built different.",
        "sudo make me a sandwich.",
        "To understand recursion, see recursion.",
        "Real programmers use cat to write code.",
    };
    static int idx = 0;
    println(quotes[idx % 8]);
    idx++;
}

void cmd_weather() {
    println("CrunchOS Weather Service");
    println("Location: CrunchCity, Kernel Space");
    println("Temp: 42C  Condition: Partly cloudy with a chance of segfaults");
    println("Wind: 404 km/h (not found)");
}

void cmd_banner(const char* text) {
    if (str_len(text) == 0) { println("Usage: banner <text>"); return; }
    println("");
    print("  ");
    for (int i = 0; text[i] && i < 15; i++) {
        putchar(text[i]); putchar(text[i]); putchar(text[i]); putchar(' ');
    }
    println(""); println("");
}

void cmd_neofetch() {
    uint8_t s = current_color;
    current_color = 0x0B;
    println("        .o+`         CrunchOS v0.1");
    println("       `ooo/         ---------------");
    current_color = 0x07;
    println("      `+oooo:        OS: CrunchOS 0.1 x86");
    println("     `+oooooo:       Kernel: CrunchOS Kernel");
    println("     -+oooooo+:      Shell: crunchsh");
    println("    `/:-:++oooo+:    CPU: x86 32-bit (QEMU)");
    println("   `/++++/+++++++:   Memory: ~510MB free");
    println("  `/++++++++++++++:  Disk: /dev/sda 20GB");
    print("  Builder: "); println("TechnoSwas & Enrico");
    current_color = s;
}

// === READ STRING ===
int read_string(char* buf, int max, int echo, int hidden) {
    int len = 0;
    while (1) {
        uint8_t key = read_key();
        if (key & 0x80) continue;
        if (key == 0x1C) { buf[len] = 0; if (echo) putchar('\n'); return len; }
        if (key == 0x0E && len > 0) {
            len--;
            if (echo) { cursor_col--; set(cursor_col, cursor_row, ' ', current_color); }
            continue;
        }
        if (key < sizeof(keymap) && keymap[key] != ' ' && len < max-1) {
            buf[len++] = keymap[key];
            if (echo) putchar(hidden ? '*' : keymap[key]);
        }
    }
}

// === DRAW PROGRESS ===
void draw_progress(int percent) {
    int bars = percent * 40 / 100;
    set(20, 14, '[', 0x1F);
    for (int i = 0; i < 40; i++)
        set(21+i, 14, i < bars ? '#' : ' ', i < bars ? 0x1A : 0x1F);
    set(61, 14, ']', 0x1F);
    char buf[4];
    buf[0] = '0' + percent/100;
    buf[1] = '0' + (percent%100)/10;
    buf[2] = '0' + percent%10;
    buf[3] = 0;
    print_at(buf, 63, 14, 0x1F);
    print_at("%", 66, 14, 0x1F);
}

// === SHELL ===
void shell() {
    current_color = 0x07;
    fill_screen();
    println("CrunchOS v0.1 - The OS built different.");
    println("Type 'help' for commands.");
    println("");

    char cmd[80];
    while (1) {
        current_color = 0x0A; print(current_user); print("@crunchos:");
        current_color = 0x0B; print(cwd);
        current_color = 0x07; print("$ ");

        int len = 0;
        while (1) {
            uint8_t key = read_key();
            if (key & 0x80) continue;
            if (key == 0x1C) { cmd[len] = 0; putchar('\n'); break; }
            if (key == 0x0E && len > 0) { len--; cursor_col--; set(cursor_col, cursor_row, ' ', current_color); continue; }
            if ((key < sizeof(keymap) && keymap[key] != ' ' && len < 79) || key == 0x39) {
                char ch = (key == 0x39) ? ' ' : keymap[key]; cmd[len++] = ch; putchar(ch);
            }
        }

        if (len == 0) continue;

        char name[32]; char args[48];
        int i = 0, j = 0;
        while (cmd[i] && cmd[i] != ' ') { name[j++] = cmd[i++]; }
        name[j] = 0;
        while (cmd[i] == ' ') i++;
        j = 0;
        while (cmd[i]) { args[j++] = cmd[i++]; }
        args[j] = 0;

        if      (str_eq(name,"help"))     cmd_help();
        else if (str_eq(name,"clear"))    { current_color=0x07; fill_screen(); }
        else if (str_eq(name,"ls"))       cmd_ls();
        else if (str_eq(name,"mkdir"))    cmd_mkdir(args);
        else if (str_eq(name,"rm"))       cmd_rm(args);
        else if (str_eq(name,"echo"))     cmd_echo(args);
        else if (str_eq(name,"cat"))      cmd_cat(args);
        else if (str_eq(name,"pwd"))      cmd_pwd();
        else if (str_eq(name,"uname"))    cmd_uname();
        else if (str_eq(name,"version"))  cmd_version();
        else if (str_eq(name,"whoami"))   cmd_whoami();
        else if (str_eq(name,"date"))     cmd_date();
        else if (str_eq(name,"uptime"))   cmd_uptime();
        else if (str_eq(name,"mem"))      cmd_mem();
        else if (str_eq(name,"cpu"))      cmd_cpu();
        else if (str_eq(name,"disk"))     cmd_disk();
        else if (str_eq(name,"reboot"))   cmd_reboot();
        else if (str_eq(name,"shutdown")) cmd_shutdown();
        else if (str_eq(name,"fortune"))  cmd_fortune();
        else if (str_eq(name,"weather"))  cmd_weather();
        else if (str_eq(name,"banner"))   cmd_banner(args);
        else if (str_eq(name,"neofetch")) cmd_neofetch();
        else if (str_eq(name,"crunch"))   cmd_crunch(args);
        else if (str_eq(name,"about"))    cmd_about();
        else { print("command not found: "); println(name); }
    }
}

// === SETUP SCREEN ===
void show_setup();

void show_install() {
    // username
    current_color = 0x1F;
    fill_screen();
    fill_row(0, 0x70);
    print_at("  CrunchOS Setup - Create Account                                          v0.1", 0, 0, 0x70);
    fill_row(24, 0x70);
    print_at("  Enter your details                                       CrunchOS 2026", 0, 24, 0x70);
    print_at("Create your CrunchOS account", 26, 4, 0x1E);
    print_at("------------------------------------------------------------------------", 4, 5, 0x1B);
    print_at("Username: [                    ]", 20, 8, 0x1F);
    cursor_row = 8; cursor_col = 31;
    read_string(current_user, 20, 1, 0);

    print_at("Password: [                    ]", 20, 11, 0x1F);
    cursor_row = 11; cursor_col = 31;
    char password[21];
    read_string(password, 20, 1, 1);

    // partition
    current_color = 0x1F;
    fill_screen();
    fill_row(0, 0x70);
    print_at("  CrunchOS Setup - Disk Partition                                          v0.1", 0, 0, 0x70);
    fill_row(24, 0x70);
    print_at("  Select partition layout                                  CrunchOS 2026", 0, 24, 0x70);
    print_at("Choose disk partition layout:", 25, 4, 0x1E);
    print_at("------------------------------------------------------------------------", 4, 5, 0x1B);
    print_at("  [1] Erase disk and install CrunchOS (recommended)", 15, 8, 0x1F);
    print_at("  [2] Manual partitioning", 15, 10, 0x1F);
    print_at("  [3] Install alongside existing OS", 15, 12, 0x1F);
    print_at("------------------------------------------------------------------------", 4, 14, 0x1B);
    print_at("  Disk: /dev/sda  Size: 20GB  Status: Ready", 15, 16, 0x1A);
    read_key();

    // installing
    current_color = 0x1F;
    fill_screen();
    fill_row(0, 0x70);
    print_at("  CrunchOS Setup - Installing                                              v0.1", 0, 0, 0x70);
    fill_row(24, 0x70);
    print_at("  Please wait...                                           CrunchOS 2026", 0, 24, 0x70);
    print_at("Installing CrunchOS for:", 28, 5, 0x1F);
    print_at(current_user, 34, 6, 0x1E);

    const char* steps[] = {
        "Formatting disk...             ",
        "Copying kernel files...        ",
        "Setting up bootloader...       ",
        "Creating user account...       ",
        "Installing base system...      ",
        "Configuring CrunchOS...        ",
        "Finalizing installation...     ",
        "Installation complete!         ",
    };
    int percents[] = {5, 20, 35, 50, 65, 80, 95, 100};

    for (int s = 0; s < 8; s++) {
        print_at("                                                  ", 15, 11, 0x1F);
        print_at(steps[s], 24, 11, 0x1E);
        for (int p = (s==0?0:percents[s-1]); p <= percents[s]; p++) {
            draw_progress(p);
            sleep_ms(2000);
        }
    }

    // done!
    current_color = 0x1F;
    fill_screen();
    fill_row(0, 0x70);
    print_at("  CrunchOS Setup - Complete!                                               v0.1", 0, 0, 0x70);
    fill_row(24, 0x70);
    print_at("  CrunchOS 2026", 0, 24, 0x70);
    print_at("Installation Complete!", 29, 6, 0x1A);
    print_at("------------------------------------------------------------------------", 4, 7, 0x1B);
    print_at("Welcome to CrunchOS,", 30, 9, 0x1F);
    print_at(current_user, 34, 10, 0x1E);
    print_at("Press any key to start CrunchOS...", 23, 13, 0x1F);
    read_key();

    // boot into shell!
    shell();
}

void show_repair() {
    current_color = 0x1F;
    fill_screen();
    fill_row(0, 0x70);
    print_at("  CrunchOS Repair                                                          v0.1", 0, 0, 0x70);
    fill_row(24, 0x70);
    print_at("  Press ESC to return                                     CrunchOS 2026", 0, 24, 0x70);
    print_at("System Information", 31, 3, 0x1E);
    print_at("------------------------------------------------------------------------", 4, 4, 0x1B);
    print_at("  OS:      CrunchOS v0.1", 2, 6, 0x1F);
    print_at("  Kernel:  CrunchOS Kernel (x86, 32-bit)", 2, 7, 0x1F);
    print_at("  Boot:    GRUB 2.12", 2, 8, 0x1F);
    print_at("  Memory:  512MB", 2, 9, 0x1F);
    print_at("------------------------------------------------------------------------", 4, 11, 0x1B);
    print_at("  [R] Reinstall bootloader", 2, 13, 0x1F);
    print_at("  [C] Check filesystem", 2, 14, 0x1F);
    print_at("  [ESC] Return to menu", 2, 15, 0x1F);

    while (1) {
        uint8_t key = read_key();
        if (key == 0x01) return;
        if (key == 0x13) print_at("  Reinstalling bootloader... Done!          ", 2, 18, 0x0A);
        if (key == 0x2E) print_at("  Filesystem OK. No errors found.           ", 2, 18, 0x0A);
    }
}

void show_setup() {
    current_color = 0x1F;
    fill_screen();
    fill_row(0, 0x70);
    print_at("  CrunchOS Setup                                                              v0.1", 0, 0, 0x70);
    print_at("Welcome to CrunchOS!", 30, 5, 0x1F);
    print_at("The OS built different.", 28, 6, 0x1E);
    print_at("--------------------------------------------------------------------------------", 0, 8, 0x1B);
    print_at("  [1] Install CrunchOS", 20, 11, 0x1F);
    print_at("  [2] Boot from Live Disk", 20, 13, 0x1F);
    print_at("  [3] Repair Installation", 20, 15, 0x1F);
    print_at("--------------------------------------------------------------------------------", 0, 17, 0x1B);
    fill_row(24, 0x70);
    print_at("  Use number keys to select an option                     CrunchOS 2026", 0, 24, 0x70);

    while(1) {
        uint8_t key = read_key();
        if (key == 0x02) { show_install(); return; }
        if (key == 0x03) { shell(); show_setup(); }
        if (key == 0x04) { show_repair(); show_setup(); }
    }
}

void kernel_main(void) {
    show_setup();
}
