import tkinter as tk
from tkinter import ttk

class CrunchOSSetup:
    def __init__(self, root):
        self.root = root
        self.root.title("CrunchOS Setup")
        self.root.geometry("900x700")
        self.root.configure(bg='#f3f3f3')
        self.root.resizable(False, False)
        
        # Style
        style = ttk.Style()
        style.theme_use('clam')
        style.configure('TButton', font=('Segoe UI', 12), padding=10, relief='flat', background='#0078d4', foreground='white')
        style.map('TButton', background=[('active', '#106ebe')])
        
        # Header
        header_frame = tk.Frame(self.root, bg='#0078d4', height=100)
        header_frame.pack(fill='x')
        header_label = tk.Label(header_frame, text="CrunchOS", font=('Segoe UI', 24, 'bold'), bg='#0078d4', fg='white')
        header_label.pack(pady=20)
        
        # Content
        self.content_frame = tk.Frame(self.root, bg='#f3f3f3')
        self.content_frame.pack(fill='both', expand=True, padx=50, pady=50)
        
        self.show_welcome()
        
        # Footer
        footer_frame = tk.Frame(self.root, bg='#f3f3f3')
        footer_frame.pack(fill='x', pady=20)
        self.back_button = ttk.Button(footer_frame, text="Back", command=self.back)
        self.back_button.pack(side='left', padx=20)
        self.next_button = ttk.Button(footer_frame, text="Next", command=self.next)
        self.next_button.pack(side='right', padx=20)
        
        self.step = 0
        self.steps = [self.show_welcome, self.show_license, self.show_install]
    
    def show_welcome(self):
        for widget in self.content_frame.winfo_children():
            widget.destroy()
        tk.Label(self.content_frame, text="Welcome to CrunchOS Setup", font=('Segoe UI', 18), bg='#f3f3f3').pack(pady=20)
        tk.Label(self.content_frame, text="This will install CrunchOS on your system.\n\nInspired by modern setups, this process is quick and easy.", font=('Segoe UI', 12), bg='#f3f3f3', justify='left').pack(pady=20)
        self.back_button.config(state='disabled')
    
    def show_license(self):
        for widget in self.content_frame.winfo_children():
            widget.destroy()
        tk.Label(self.content_frame, text="License Agreement", font=('Segoe UI', 18), bg='#f3f3f3').pack(pady=20)
        license_text = tk.Text(self.content_frame, height=10, font=('Segoe UI', 10), bg='white', wrap='word')
        license_text.insert('1.0', "By installing CrunchOS, you agree to the terms...\n\n(This is a placeholder license text.)")
        license_text.config(state='disabled')
        license_text.pack(fill='both', expand=True, pady=20)
        self.back_button.config(state='normal')
    
    def show_install(self):
        for widget in self.content_frame.winfo_children():
            widget.destroy()
        tk.Label(self.content_frame, text="Ready to Install", font=('Segoe UI', 18), bg='#f3f3f3').pack(pady=20)
        tk.Label(self.content_frame, text="Click Install to begin.", font=('Segoe UI', 12), bg='#f3f3f3').pack(pady=20)
        self.next_button.config(text="Install")
    
    def back(self):
        if self.step > 0:
            self.step -= 1
            self.steps[self.step]()
    
    def next(self):
        if self.step < len(self.steps) - 1:
            self.step += 1
            self.steps[self.step]()
        else:
            # Install logic here
            tk.messagebox.showinfo("Installation", "CrunchOS installed successfully!")
            self.root.quit()

if __name__ == "__main__":
    root = tk.Tk()
    app = CrunchOSSetup(root)
    root.mainloop()