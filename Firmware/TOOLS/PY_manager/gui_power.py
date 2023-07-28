"""
    一键启动trojan与v2aryN
"""
import os
from tkinter import *

BUTTON_SIZE = 28
MARGIN = 28


class GUI:
    """
        界面封装
    """

    def __init__(self):
        self.window = Tk()
        self.window.title("充电桩调试器")
        self.window.geometry("768x600")
        self.window.resizable(False, False)
        self.var_btn = StringVar(value="启动")
        self.window.protocol('WM_DELETE_WINDOW', self.on_click_exit_app)

        self.canvas = Canvas(self.window, width=768, height=600, bg="white")
        # self.canvas.bind("<Button-1>", self.mouse_click)
        self.canvas.pack()

        self.window.update()  #

        self.light = []
        self.title = [
            "指示灯1",
            "指示灯2",
            "指示灯3",
            "指示灯4",
            "指示灯5",
            "指示灯6",
            "指示灯7",
            "指示灯8",
            "指示灯9",
            "指示灯10",
            "指示灯11",
            "指示灯12",
            "指示灯13",
            "指示灯14",
            "指示灯15",
            "指示灯16",
        ]

        self.status = 0

    def change_led_status(self, led_num, enbale):
        """
            更改LED的状态
        :param led_num:
        :param enbale:
        :return:
        """
        color = "green" if enbale else "grey"
        led_widget = self.light[led_num][0]
        self.canvas.itemconfig(led_widget, fill=color)

    @staticmethod
    def on_click_exit_app():
        os._exit(0)

    def draw_main(self):
        """
            绘制主页面
        :return:
        """
        y = 0
        for row in range(2):
            if y == 0:
                y = MARGIN + MARGIN + ((MARGIN + BUTTON_SIZE) * row)
            else:
                y += 110
            x = 0
            for column in range(8):
                if x == 0:
                    x = MARGIN + MARGIN + ((MARGIN + BUTTON_SIZE) * column)
                else:
                    x += 90

                led_localtion = (x, y, x + BUTTON_SIZE, y + BUTTON_SIZE)
                text_localtion = (x + BUTTON_SIZE / 2, y + BUTTON_SIZE + 20)

                # 绘制 LED 指示灯
                led_widget = self.canvas.create_oval(led_localtion, fill="green", outline=""),
                # 绘制 LED 指示文本
                text_widget = self.canvas.create_text(
                    text_localtion,
                    text=self.title[len(self.light)],
                    fill="red",
                    font="宋体 14"
                )

                # 添加进池中
                self.light.append((led_widget, text_widget))

    def start(self):
        self.draw_main()
        self.change_led_status(1, 0)
        self.window.mainloop()


if __name__ == '__main__':
    GUI().start()
