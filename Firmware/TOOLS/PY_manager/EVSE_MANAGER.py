import threading
import time
import struct
import winsound


import serial
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

        self.canvas = Canvas(self.window, width=768, height=600, bg="white")
        # self.canvas.bind("<Button-1>", self.mouse_click)
        self.canvas.pack()

        self.window.update()  #

        self.light = []
        self.title = [
            "emgstate",
            "outputen",
            "lock",
            "output",
            "sw_error",
            "overtemp",
            "overvol",
            "undervol",
            "overcur",
            "emgstop",
            "chargegun",
            "charging",
            "S1_Switch",
            "rfidlock",
            "blelock",
            "outvol",
        ]

        self.status = 0

        self.text_stopreason   = None
        self.text_aimcurrent   = None
        self.text_elec_voltage = None
        self.text_elec_current = None
        self.text_elec_ap      = None
        self.text_elec_energy  = None

        self.serial_instance = serial.Serial(port="COM7", baudrate=115200)

    def change_led_status(self, led_num, enbale):
        """
            更改LED的状态
        :param led_num:
        :param enbale:
        :return:
        """
        color = "black" if enbale else "white"
        led_widget = self.light[led_num][0]
        self.canvas.itemconfig(led_widget, fill=color)

    def change_text(self, text_widget, text):
        """
            更改文本控件的文本
        :param text_widget:
        :param text:
        :return:
        """
        self.canvas.itemconfig(text_widget, text=text)

    def start_run(self):
        """
            启动轮询
        :return:
        """
        while True:
            line = b''
            line = self.serial_instance.read_until(b'&\r\n')

            print(line)
            # 收到了有效数据，状态包
            if line.startswith(b'^'):
                statemech = int.from_bytes(line[1:5], byteorder='little', signed=False)

                emgstate        = (statemech >> 0 ) & 0x0001
                outputen        = (statemech >> 1 ) & 0x0001
                lock            = (statemech >> 2 ) & 0x0001
                output          = (statemech >> 3 ) & 0x0001
                sw_error        = (statemech >> 4 ) & 0x0001
                overtemp        = (statemech >> 5 ) & 0x0001
                overvoltage     = (statemech >> 6 ) & 0x0001
                undervoltag     = (statemech >> 7 ) & 0x0001
                overcurrent     = (statemech >> 8 ) & 0x0001
                emgstop         = (statemech >> 9 ) & 0x0001
                chargegun       = (statemech >> 10) & 0x0001
                chargestart     = (statemech >> 11) & 0x0001
                S1_Switch_State = (statemech >> 12) & 0x0001
                rfidlock        = (statemech >> 13) & 0x0001
                blelock         = (statemech >> 14) & 0x0001
                outvol          = (statemech >> 15) & 0x0001

                self.change_led_status(0 , emgstate       )
                self.change_led_status(1 , outputen       )
                self.change_led_status(2 , lock           )
                self.change_led_status(3 , output         )
                self.change_led_status(4 , sw_error       )
                self.change_led_status(5 , overtemp       )
                self.change_led_status(6 , overvoltage    )
                self.change_led_status(7 , undervoltag    )
                self.change_led_status(8 , overcurrent    )
                self.change_led_status(9 , emgstop        )
                self.change_led_status(10, chargegun      )
                self.change_led_status(11, chargestart    )
                self.change_led_status(12, S1_Switch_State)
                self.change_led_status(13, rfidlock       )
                self.change_led_status(14, blelock        )
                self.change_led_status(15, outvol         )

                # print("lock: ", lock)
                stopreason = int.from_bytes(line[5:6], byteorder='little', signed=False)
                self.change_text(self.text_stopreason, f"停止原因: {stopreason}")

                aimcurrent = int.from_bytes(line[6:8], byteorder='little', signed=False)
                self.change_text(self.text_aimcurrent, f"aimcurrent: {aimcurrent/10}")

                #print("stopreason: ", stopreason)
                #print("aimcurrent: ", aimcurrent)
            # 收到了有效数据，参数包
            if line.startswith(b'$'):
                #elec_voltage = struct.unpack(">ffff", line[1:17])[0]
                elec_voltage = int.from_bytes(line[1:5], byteorder='little', signed=False)
                self.change_text(self.text_elec_voltage, f"电压值: {elec_voltage}")

                # elec_current = struct.unpack(">ffff", line[1:17])[1]
                elec_current = int.from_bytes(line[5:9], byteorder='little', signed=False)
                self.change_text(self.text_elec_current, f"电流值: {elec_current}")

                # elec_ap = struct.unpack(">ffff", line[1:17])[2]
                elec_ap = int.from_bytes(line[9:13], byteorder='little', signed=False)
                self.change_text(self.text_elec_ap, f"AP值: {elec_ap}")

                # elec_energy = struct.unpack(">ffff", line[1:17])[3]
                elec_energy = int.from_bytes(line[13:17], byteorder='little', signed=False)
                self.change_text(self.text_elec_energy, f"电能值: {elec_energy}")

                # print(elec_voltage, elec_current, elec_ap, elec_energy)
                pass
            if line.startswith(b'#METER'):
                winsound.Beep(300, 200);
                winsound.Beep(500, 200);
                winsound.Beep(700, 200);
        return

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

                led_location = (x, y, x + BUTTON_SIZE, y + BUTTON_SIZE)
                text_location = (x + BUTTON_SIZE / 2, y + BUTTON_SIZE + 20)

                # 绘制 LED 指示灯
                led_widget = self.canvas.create_oval(led_location, fill="green", outline=""),
                # 绘制 LED 指示文本
                text_widget = self.canvas.create_text(
                    text_location,
                    text=self.title[len(self.light)],
                    fill="red",
                    font="宋体 14"
                )

                # 添加进池中
                self.light.append((led_widget, text_widget))

        text_base_x = 40
        text_base_y = 350
        text_base_margin = 40
        self.text_stopreason   = self.canvas.create_text((text_base_x, text_base_y), text="test", font="宋体 14", anchor="sw")
        text_base_y += text_base_margin
        self.text_aimcurrent   = self.canvas.create_text((text_base_x, text_base_y), text="test", font="宋体 14", anchor="sw")
        text_base_y += text_base_margin
        self.text_elec_voltage = self.canvas.create_text((text_base_x, text_base_y), text="test", font="宋体 14", anchor="sw")
        text_base_y += text_base_margin
        self.text_elec_current = self.canvas.create_text((text_base_x, text_base_y), text="test", font="宋体 14", anchor="sw")
        text_base_y += text_base_margin
        self.text_elec_ap      = self.canvas.create_text((text_base_x, text_base_y), text="test", font="宋体 14", anchor="sw")
        text_base_y += text_base_margin
        self.text_elec_energy  = self.canvas.create_text((text_base_x, text_base_y), text="test", font="宋体 14", anchor="sw")

    def start(self):
        self.draw_main()
        threading.Thread(target=self.start_run).start()
        self.window.mainloop()


if __name__ == '__main__':
    a = b'$C\\\xcc\xcd<\xa3\xd7\x0c;\x03\x12p\x00\x00\x00\x00$\r\n^\n\\\x03\x00\xb4&\r\n'
    print (struct.unpack(">ffff", a[1:17])[0])
    GUI().start()
