import tkinter as tk
import serial

# 创建串口对象
ser = serial.Serial('COM8', 9600)  # 替换为你的串口号和波特率

# 发送信息函数
def send_message(message):
    ser.write(message.encode())
    text_box.insert(tk.END, f"已发送: {message}\n")

# 创建主窗口
window = tk.Tk()
window.title("项目演示程序")

# 创建标题
title_label = tk.Label(window, text="项目演示程序", font=("Arial", 24))
title_label.pack(pady=10)

# 创建街区地图图片
map_image = tk.PhotoImage(file="C:\\Users\\20139\\Documents\\GitHub\\TrafficLight\\源代码\\Demo\\map.png")  # 替换为你的街区地图图片路径
map_label = tk.Label(window, image=map_image)
map_label.pack()

# 创建按钮
button_frame = tk.Frame(window)
button_frame.pack(pady=10)

button_a = tk.Button(button_frame, text="A", width=10, height=2, command=lambda: send_message("A"))
button_a.grid(row=0, column=0, padx=10)

button_b = tk.Button(button_frame, text="B", width=10, height=2, command=lambda: send_message("B"))
button_b.grid(row=0, column=1, padx=10)

button_c = tk.Button(button_frame, text="C", width=10, height=2, command=lambda: send_message("C"))
button_c.grid(row=1, column=0, padx=10)

button_d = tk.Button(button_frame, text="D", width=10, height=2, command=lambda: send_message("D"))
button_d.grid(row=1, column=1, padx=10)

# 创建文本框
text_box = tk.Text(window, width=30, height=5)
text_box.pack(pady=10)

# 运行主循环
window.mainloop()