import tkinter as tk
from tkinter import messagebox
import paho.mqtt.client as mqtt

# MQTT Broker Configuration
BROKER = "192.168.137.197"  # Replace with your broker's IP or hostname
PORT = 1884
USERNAME = "admin"
PASSWORD = "DEEUFPE"

# Topics
TOPICS = {
    "casa/solar": "Radiância",
    "casa/led": ["on", "off"],
    "casa/ventilador": ["horario", "antihorario", "desligar"]
}

# MQTT Client
client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        messagebox.showinfo("MQTT", "Connected to MQTT Broker!")
        for topic in TOPICS.keys():
            client.subscribe(topic)
    else:
        messagebox.showerror("MQTT", f"Failed to connect, return code {rc}")

def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload.decode()
    if topic in TOPICS:
        if topic == "casa/solar":
            solar_label.config(text=f"Radiância: {payload}")
        elif topic == "casa/led":
            led_label.config(text=f"LED: {payload}")
        elif topic == "casa/ventilador":
            fan_label.config(text=f"Ventilador: {payload}")

def toggle_led(state):
    client.publish("casa/led", state)

def control_fan(state):
    client.publish("casa/ventilador", state)

# Initialize MQTT Client
client.username_pw_set(USERNAME, PASSWORD)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT, 60)

# Tkinter GUI
root = tk.Tk()
root.title("MQTT Control Panel")

# Solar Radiance
solar_label = tk.Label(root, text="Radiância: --", font=("Arial", 14))
solar_label.pack(pady=10)

# LED Control
led_label = tk.Label(root, text="LED: --", font=("Arial", 14))
led_label.pack(pady=10)

led_frame = tk.Frame(root)
led_frame.pack(pady=5)
tk.Button(led_frame, text="Ligar LED", command=lambda: toggle_led("on")).pack(side=tk.LEFT, padx=5)
tk.Button(led_frame, text="Desligar LED", command=lambda: toggle_led("off")).pack(side=tk.LEFT, padx=5)

# Fan Control
fan_label = tk.Label(root, text="Ventilador: --", font=("Arial", 14))
fan_label.pack(pady=10)

fan_frame = tk.Frame(root)
fan_frame.pack(pady=5)
tk.Button(fan_frame, text="Horário", command=lambda: control_fan("horario")).pack(side=tk.LEFT, padx=5)
tk.Button(fan_frame, text="Anti-Horário", command=lambda: control_fan("antihorario")).pack(side=tk.LEFT, padx=5)
tk.Button(fan_frame, text="Desligar", command=lambda: control_fan("desligar")).pack(side=tk.LEFT, padx=5)

# Start MQTT Loop
client.loop_start()

# Run Tkinter Main Loop
root.mainloop()

# Stop MQTT Loop on Exit
client.loop_stop()
client.disconnect()