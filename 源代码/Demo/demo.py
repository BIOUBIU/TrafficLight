import serial
import tkinter as tk
from tkinter import ttk
from PIL import ImageTk, Image

# 创建串口对象
ser = serial.Serial('COM14', 9600)  # 将COM1替换为你的串口号和波特率

# 创建主窗口
window = tk.Tk()
window.title("项目演示程序")

style = ttk.Style()
style.configure('TButton', font=('黑体', 15))

# 创建标题
title_label = tk.Label(window, text="公交车项目演示程序", font=("Arial", 24))
title_label.pack(pady=10)

# 加载街区地图图片
image = Image.open("C:\\Users\\20139\\Documents\\GitHub\\TrafficLight\\源代码\\Demo\\map.png")  # 将map.png替换为你的街区地图图片路径
image = image.resize((1023, 666), Image.LANCZOS)
photo = ImageTk.PhotoImage(image)
label = tk.Label(window, image=photo)
label.pack()


# 发送信息函数
def send_message(message):
    ser.write(message.encode())
    print("已发送信息:", message)
    showText(message)

def showText(mes):
    if(mes == 'A'):
        text_box.insert(tk.END, f"已发送: 123.4527253,41.841248,08:18:28,2021-10-21,16:18:28,2021-10-21,1,180,58\r\n")
        text_box.see(tk.END)  # 滚动到文本框的最后一行
    elif(mes == 'B'):
        text_box.insert(tk.END, f"已发送: 123.4527145,41.840816,08:18:28,2021-10-21,16:18:28,2021-10-21,1,180,58\r\n")
        text_box.see(tk.END)  # 滚动到文本框的最后一行
    elif(mes == 'C'):
        text_box.insert(tk.END, f"已发送: 123.4527145,41.840816,08:18:28,2021-10-21,16:18:28,2021-10-21,1,270,58\r\n")
        text_box.see(tk.END)  # 滚动到文本框的最后一行
    elif(mes == 'D'):
        text_box.insert(tk.END, f"已发送: 123.4513144,41.840764,08:18:28,2021-10-21,16:18:28,2021-10-21,1,270,58\r\n")
        text_box.see(tk.END)  # 滚动到文本框的最后一行
    else:
        return

# 创建按钮及其点击事件
button_a = ttk.Button(window, text="常规路段",style='TButton', command=lambda: send_message("A"))
button_a.place(x=1050, y=200)

button_b = ttk.Button(window, text="路口-转向前", style='TButton',command=lambda: send_message("B"))
button_b.place(x=1060, y=430)

button_c = ttk.Button(window, text="路口-转向后", style='TButton',command=lambda: send_message("C"))
button_c.place(x=950, y=530)

button_d = ttk.Button(window, text="公交车站", style='TButton',command=lambda: send_message("D"))
button_d.place(x=350, y=550)

# 创建文本框
text_box = tk.Text(window, width=80, height=5)
text_box.pack(pady=10)
# 创建滚动条
scrollbar = tk.Scrollbar(window)
scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

# 将滚动条与文本框关联
text_box.config(yscrollcommand=scrollbar.set)
scrollbar.config(command=text_box.yview)

window.mainloop()