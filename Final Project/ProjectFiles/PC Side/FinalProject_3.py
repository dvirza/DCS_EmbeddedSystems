import tkinter as tk
import serial
import threading
import time
import os
from tkinter import ttk, filedialog, messagebox
import math

# Initialize serial communication (adjust 'COM4' to your actual port if necessary)
try:
    ser = serial.Serial('COM4', 9600, timeout=1)
except serial.SerialException as e:
    print(f"Serial Error: {e}")
    ser = None

# Canvas dimensions
CANVAS_WIDTH = 600
CANVAS_HEIGHT = 600

# Center of the canvas
CENTER_X = CANVAS_WIDTH // 2
CENTER_Y = CANVAS_HEIGHT // 2

# Offset values to move the points right and down
OFFSET_X = 20  # Adjust this value to move the points more or less to the right
OFFSET_Y = 20  # Adjust this value to move the points more or less down

# Bias variables to control the position bias of all drawing points
BIAS_X = 0  # Adjust this value to control horizontal bias
BIAS_Y = 0  # Adjust this value to control vertical bias

# Range values (assumed joystick range)
JOYSTICK_MAX = 32767
JOYSTICK_MIN = -32768

# Set the zoom factor to 8x
ZOOM_FACTOR = 32.0

# Normalize the joystick input range to canvas dimensions and apply zoom
def normalize(value, min_value, max_value, canvas_size):
    normalized_value = ((value - min_value) / (max_value - min_value)) * canvas_size
    return (normalized_value - (canvas_size / 2)) * ZOOM_FACTOR + (canvas_size / 2)

def manual_control():
    """Function for Manual Control Button."""
    if ser and ser.is_open:
        ser.write(b'1')

def move_pointer(canvas, pointer, current_tool, prev_pos, new_pos):
    """Move the pointer on the canvas and draw based on the current tool."""
    x1, y1 = prev_pos
    x2, y2 = new_pos

    # Apply the bias and offset to the coordinates
    x1 += OFFSET_X + BIAS_X
    y1 += OFFSET_Y + BIAS_Y
    x2 += OFFSET_X + BIAS_X
    y2 += OFFSET_Y + BIAS_Y

    if current_tool.get() == "Pen":
        canvas.create_line(x1, y1, x2, y2, fill="black", width=2, smooth=True)
    elif current_tool.get() == "Erase":
        canvas.create_line(x1, y1, x2, y2, fill="white", width=10, smooth=True)

    canvas.coords(pointer, x2 - 5, y2 - 5, x2 + 5, y2 + 5)

def receive_coordinates(canvas, pointer, current_tool, current_tool_label, stop_event):
    """Receive coordinates from MCU, process them, and update the pointer position."""
    prev_x, prev_y = CENTER_X, CENTER_Y
    x_samples = []
    y_samples = []

    # Track the last time coordinates were received
    last_received_time = time.time()

    while not stop_event.is_set():
        try:
            if ser and ser.is_open:
                # Check if 0.5 seconds have passed since the last coordinates were received
                current_time = time.time()
                if current_time - last_received_time > 0.05:
                    toggle_tool(current_tool, current_tool_label)
                    last_received_time = current_time  # Reset the timer after toggling

                # Read 4 bytes for coordinates (little-endian)
                data = ser.read(4)
                if len(data) < 4:
                    continue

                x_bytes = data[0:2]
                y_bytes = data[2:4]

                # Convert bytes to integers (little-endian)
                x_raw = int.from_bytes(x_bytes, byteorder='little', signed=True)
                y_raw = int.from_bytes(y_bytes, byteorder='little', signed=True)

                # Update the last received time since coordinates have been received
                last_received_time = time.time()

                # Normalize the joystick values to the canvas dimensions and invert axes
                x_normalized = normalize(-x_raw, JOYSTICK_MIN, JOYSTICK_MAX, CANVAS_WIDTH)
                y_normalized = normalize(-y_raw, JOYSTICK_MIN, JOYSTICK_MAX, CANVAS_HEIGHT)

                x_samples.append(x_normalized)
                y_samples.append(y_normalized)

                # When enough samples are collected, compute the average and update pointer
                if len(x_samples) == 4:
                    avg_x = sum(x_samples) / 4
                    avg_y = sum(y_samples) / 4

                    # Ensure the pointer stays within canvas bounds
                    avg_x = max(0, min(CANVAS_WIDTH, avg_x))
                    avg_y = max(0, min(CANVAS_HEIGHT, avg_y))

                    move_pointer(canvas, pointer, current_tool, (prev_x, prev_y), (avg_x, avg_y))
                    prev_x, prev_y = avg_x, avg_y

                    # Clear samples for next batch
                    x_samples.clear()
                    y_samples.clear()

        except Exception as e:
            print(f"Error in receiving coordinates: {e}")
            break


def toggle_tool(current_tool, current_tool_label):
    """Toggle the current tool cyclically."""
    if current_tool.get() == "Pen":
        current_tool.set("Erase")
    elif current_tool.get() == "Erase":
        current_tool.set("Neutral")
    else:
        current_tool.set("Pen")
    current_tool_label.config(text=f"Current Tool: {current_tool.get()}")

def open_paint_mode():
    """Function to open Paint Mode window."""
    if ser and ser.is_open:
        ser.write(b'5')


    root.withdraw()  # Add this line

    # Create a new window for Paint Mode
    paint_window = tk.Toplevel(root)
    paint_window.title("Paint Mode")
    paint_window.geometry(f"{CANVAS_WIDTH + 20}x{CANVAS_HEIGHT + 100}")  # Extra space for buttons and labels

    # Create a canvas (whiteboard)
    canvas = tk.Canvas(paint_window, width=CANVAS_WIDTH, height=CANVAS_HEIGHT, bg="white")
    canvas.pack(pady=10)

    # Initial pointer position at center with offset and bias applied
    pointer = canvas.create_oval(CENTER_X - 5 + OFFSET_X + BIAS_X, CENTER_Y - 5 + OFFSET_Y + BIAS_Y,
                                 CENTER_X + 5 + OFFSET_X + BIAS_X, CENTER_Y + 5 + OFFSET_Y + BIAS_Y, fill="red")

    # Variable to track the current tool
    current_tool = tk.StringVar(value="Pen")

    # Label to display the current tool
    current_tool_label = tk.Label(paint_window, text=f"Current Tool: {current_tool.get()}", font=("Arial", 14))
    current_tool_label.pack(pady=5)

    # Frame for buttons
    button_frame = tk.Frame(paint_window)
    button_frame.pack(pady=5)

    # Buttons for tool selection
    pen_button = tk.Button(button_frame, text="Pen", width=10, command=lambda: update_tool("Pen", current_tool, current_tool_label))
    erase_button = tk.Button(button_frame, text="Erase", width=10, command=lambda: update_tool("Erase", current_tool, current_tool_label))
    neutral_button = tk.Button(button_frame, text="Neutral", width=10, command=lambda: update_tool("Neutral", current_tool, current_tool_label))
    clear_button = tk.Button(button_frame, text="Clear Canvas", width=10, command=lambda: clear_canvas(canvas))
    back_button = tk.Button(button_frame, text="Back", width=10, command=lambda: close_paint_mode(paint_window, stop_event))

    pen_button.grid(row=0, column=0, padx=5, pady=5)
    erase_button.grid(row=0, column=1, padx=5, pady=5)
    neutral_button.grid(row=0, column=2, padx=5, pady=5)
    clear_button.grid(row=0, column=3, padx=5, pady=5)
    back_button.grid(row=0, column=4, padx=5, pady=5)

    # Event to stop the threading when window is closed
    stop_event = threading.Event()
    paint_window.protocol("WM_DELETE_WINDOW", lambda: close_paint_mode(paint_window, stop_event))

    # Start a thread to receive coordinates and handle the pointer movement and tool toggle
    threading.Thread(target=receive_coordinates, args=(canvas, pointer, current_tool, current_tool_label, stop_event), daemon=True).start()

def update_tool(tool_name, current_tool, current_tool_label):
    """Update the current tool."""
    current_tool.set(tool_name)
    current_tool_label.config(text=f"Current Tool: {current_tool.get()}")

def clear_canvas(canvas):
    """Clear all drawings from the canvas."""
    canvas.delete("all")
    # Redraw the pointer at center with offset and bias applied
    canvas.create_oval(CENTER_X - 5 + OFFSET_X + BIAS_X, CENTER_Y - 5 + OFFSET_Y + BIAS_Y,
                       CENTER_X + 5 + OFFSET_X + BIAS_X, CENTER_Y + 5 + OFFSET_Y + BIAS_Y, fill="red")

def close_paint_mode(window, stop_event):
    """Close the Paint Mode window and send exit command to MCU."""
    root.deiconify()  # Add this line

    if ser and ser.is_open:
        ser.write(b'0')
    stop_event.set()
    window.destroy()

def open_calibration_mode():
    """Function to open Calibration Mode window."""
    if ser and ser.is_open:
        ser.write(b'6')

    root.withdraw()  # Hide the main window

    # Create a new window for Calibration Mode
    calibration_window = tk.Toplevel(root)
    calibration_window.title("Calibration Mode")
    calibration_window.geometry("300x200")

    # Default angle value
    angle = tk.DoubleVar(value=0.088)

    # Label to display the angle with more decimal points
    angle_label = tk.Label(calibration_window, textvariable=angle, font=("Arial", 14))
    angle_label.pack(pady=20)

    # Event to stop the threading when window is closed
    stop_event = threading.Event()

    # Back button
    back_button = tk.Button(calibration_window, text="Back", width=10,
                            command=lambda: close_calibration_mode(calibration_window, stop_event))
    back_button.pack(pady=20)

    # Start a thread to receive the calibration value and update the angle
    calibration_window.protocol("WM_DELETE_WINDOW", lambda: close_calibration_mode(calibration_window, stop_event))
    threading.Thread(target=receive_calibration_value, args=(angle, stop_event), daemon=True).start()


def close_calibration_mode(window, stop_event):
    """Close the Calibration Mode window and send exit command to MCU."""
    root.deiconify()  # Show the main window

    if ser and ser.is_open:
        ser.write(b'0')
    stop_event.set()
    window.destroy()


def receive_calibration_value(angle_var, stop_event):
    """Receive the calibration value from the MCU and update the angle."""
    while not stop_event.is_set():
        try:
            if ser and ser.is_open:
                # Read 1 byte for the LSB and MSB parts each
                lsb_data = ser.read(1)
                msb_data = ser.read(1)

                if len(lsb_data) < 1 or len(msb_data) < 1:
                    continue

                # Convert bytes to integer (assuming little-endian)
                lsb = int.from_bytes(lsb_data, byteorder='little', signed=False)
                msb = int.from_bytes(msb_data, byteorder='little', signed=False)

                # Combine LSB and MSB to form the full 16-bit unsigned integer
                val = (msb << 8) | lsb

                if val != 0:  # Ensure we don't divide by zero
                    # Update the angle and show more decimal places
                    new_angle = 360 / val
                    angle_var.set(f"PHI = {new_angle:.6f}")  # Shows six decimal points

        except Exception as e:
            print(f"Error in receiving calibration value: {e}")
            break

# Assuming the serial communication setup and other modes remain the same as your original code
# Function to decode raw data based on opcode table
def decode_data(raw_data):
    decoded_commands = []
    i = 0
    while i < len(raw_data):
        try:
            if raw_data[i:i+2] == '01':
                command = 'inc_lcd ' + str(int(raw_data[i+2:i+4], 16))
                decoded_commands.append(command)
                i += 4
            elif raw_data[i:i+2] == '02':
                command = 'dec_lcd ' + str(int(raw_data[i+2:i+4], 16))
                decoded_commands.append(command)
                i += 4
            elif raw_data[i:i+2] == '03':
                command = 'rra_lcd ' + str(int(raw_data[i+2:i+4], 16))
                decoded_commands.append(command)
                i += 4
            elif raw_data[i:i+2] == '04':
                command = 'set_delay ' + str(int(raw_data[i+2:i+4], 16))
                decoded_commands.append(command)
                i += 4
            elif raw_data[i:i+2] == '05':
                command = 'clear_all_leds'
                decoded_commands.append(command)
                i += 2
            elif raw_data[i:i+2] == '06':
                command = 'stepper_deg ' + str(int(raw_data[i+2:i+4], 16))
                decoded_commands.append(command)
                i += 4
            elif raw_data[i:i+2] == '07':
                left = str(int(raw_data[i+2:i+4], 16))
                right = str(int(raw_data[i+4:i+6], 16))
                command = f'stepper_scan {left}, {right}'
                decoded_commands.append(command)
                i += 6
            elif raw_data[i:i+2] == '08':
                command = 'sleep'
                decoded_commands.append(command)
                i += 2
            else:
                decoded_commands.append('Unknown Command')
                i += 2
        except:
            decoded_commands.append('Unknown Command')
            i += 2

    return '\n'.join(decoded_commands)

# Modified Script Mode GUI with decoding feature and adjusted layout
# Modified Script Mode with robust decoding
def open_script_mode():
    if ser and ser.is_open:
        ser.write(b'7')
    root.withdraw()  # Hide the main window

    file_contents = ["", "", ""]
    file_index = 0
    current_file = []

    # Receive and decode the files from the MCU with error handling for non-UTF-8 bytes
    while ser and ser.is_open:
        data = ser.read(1)
        try:
            char = data.decode('utf-8')  # Attempt to decode as UTF-8
        except UnicodeDecodeError:
            char = ''  # If decoding fails, skip the character

        if char == '\n':
            if current_file:
                file_contents[file_index] = "".join(current_file)
                current_file = []
                file_index += 1
            if file_index == 3:
                break
        else:
            current_file.append(char)


    # Create the Script Mode window
    script_window = tk.Toplevel(root)
    script_window.title("Script Mode")
    script_window.geometry("750x750")

    # Browse button to select a file
    selected_file_path = tk.StringVar()
    tk.Button(script_window, text="Browse", command=lambda: browse_file(selected_file_path)).pack(pady=5)

    # ComboBox to select the file number
    file_number = tk.StringVar(value='0')
    file_number_combobox = ttk.Combobox(script_window, textvariable=file_number, values=['0', '1', '2'], state='readonly')
    file_number_combobox.pack(pady=5)
    file_number_combobox.set('0')

    # Frame to hold the raw data and decoded commands side by side
    content_frame = tk.Frame(script_window)
    content_frame.pack(pady=10)

    raw_textboxes = []
    decoded_textboxes = []

    for i in range(3):
        # Raw data text box
        raw_frame = tk.Frame(content_frame)
        raw_frame.grid(row=i, column=0, padx=10, pady=5)
        tk.Label(raw_frame, text=f"Raw Data {i}:").pack()  # Labels updated to 0, 1, 2
        raw_box = tk.Text(raw_frame, width=40, height=10)
        raw_box.pack()
        raw_box.insert('1.0', file_contents[i])
        raw_textboxes.append(raw_box)

        # Decoded commands text box
        decoded_frame = tk.Frame(content_frame)
        decoded_frame.grid(row=i, column=1, padx=10, pady=5)
        tk.Label(decoded_frame, text=f"Decoded Commands {i}:").pack()  # Labels updated to 0, 1, 2
        decoded_box = tk.Text(decoded_frame, width=40, height=10)
        decoded_box.pack()
        decoded_textboxes.append(decoded_box)

        # Decode and display the initial data from the MCU
        decoded_box.insert('1.0', decode_data(file_contents[i]))

    button_frame = tk.Frame(script_window)
    button_frame.pack(pady=10)

    tk.Button(button_frame, text="Load", command=lambda: load_file(selected_file_path, file_number.get(), raw_textboxes[int(file_number.get())], decoded_textboxes[int(file_number.get())])).grid(row=0, column=0, padx=5)
    tk.Button(button_frame, text="Execute", command=lambda: execute_script(file_number.get())).grid(row=0, column=1, padx=5)
    tk.Button(button_frame, text="Back", command=lambda: close_script_mode(script_window)).grid(row=0, column=2, padx=5)

# Global flags and variables
animation_active = False
bb_received = False  # Flag to track first '0xBB' reception
bb_timeout = 1.0  # Timeout window in seconds to detect consecutive '0xBB'
angle_window_open = False  # Flag to check if angle window is open
angle_window = None  # Reference to the angle display window

def browse_file(selected_file_path):
    # Open a file dialog to select a text file
    file_path = filedialog.askopenfilename(filetypes=[("Text Files", "*.txt")])
    if file_path:
        selected_file_path.set(file_path)

# Function to receive data from MCU and control the opening and closing of the angle window
def receive_data(stop_event):
    global animation_active, bb_received, angle_window_open, angle_window
    last_bb_time = 0  # Timestamp of the last '0xBB' received

    while not stop_event.is_set():
        try:
            if ser and ser.is_open:
                # Read 1 byte from MCU
                data = ser.read(1)
                if data == b'\xBB':
                    current_time = time.time()
                    if bb_received and (current_time - last_bb_time) <= bb_timeout:
                        # If second '0xBB' is within the timeout window, toggle the animation state
                        animation_active = not animation_active
                        bb_received = False  # Reset flag after toggling

                        if animation_active and not angle_window_open:
                            # Open the angle window to start showing angles
                            open_angle_animation()
                        elif angle_window_open:
                            # Close the angle window if it is already open
                            close_angle_animation()
                    else:
                        # Mark the first '0xBB' reception
                        bb_received = True
                        last_bb_time = current_time

                elif animation_active and angle_window_open:
                    # If receiving angles and window is open, interpret the received bytes as an angle value
                    data += ser.read(1)  # Read the second byte for the full angle value
                    angle = int.from_bytes(data, byteorder='little', signed=False)
                    if 0 <= angle <= 360:
                        # Update the canvas and label with the new angle
                        update_circle(angle_window.canvas, angle_window.marker, angle)
                        angle_window.angle_label.config(text=f"Current Angle: {angle}°")

        except Exception as e:
            print(f"Error in receiving data: {e}")
            break

# Function to update the circle with the received angle
def update_circle(canvas, marker, angle):
    # Clear previous marker
    canvas.delete("marker")

    # Calculate the position on the circle based on the angle
    radius = 100
    center_x, center_y = 150, 150
    angle_rad = math.radians(angle)
    x = center_x + radius * math.cos(angle_rad)
    y = center_y - radius * math.sin(angle_rad)

    # Draw the marker on the circle
    canvas.create_line(center_x, center_y, x, y, fill="red", width=2, tags="marker")
    canvas.create_oval(x - 5, y - 5, x + 5, y + 5, fill="red", tags="marker")

# Function to open the angle animation window
def open_angle_animation():
    global angle_window_open, angle_window
    angle_window_open = True
    angle_window = AngleDisplayWindow()

# Function to close the angle animation window
def close_angle_animation():
    global angle_window_open, angle_window
    if angle_window_open and angle_window is not None:
        angle_window.close()
        angle_window_open = False
        angle_window = None

# Class to handle the angle display window
class AngleDisplayWindow:
    def __init__(self):
        self.window = tk.Toplevel(root)
        self.window.title("Angle Display")
        self.window.geometry("300x300")

        # Label to show the current angle
        self.angle_label = tk.Label(self.window, text="Waiting for angles...", font=("Arial", 14))
        self.angle_label.pack(pady=10)

        # Create a canvas to draw the circle and markers
        self.canvas = tk.Canvas(self.window, width=300, height=300, bg="white")
        self.canvas.pack()

        # Draw the base circle
        self.canvas.create_oval(50, 50, 250, 250, outline="black", width=2)

        # Create an empty marker for updates
        self.marker = None

        # Event to close the window
        self.window.protocol("WM_DELETE_WINDOW", self.close)

    def close(self):
        global animation_active
        animation_active = False
        self.window.destroy()

# Function to display a success popup when acknowledgment is received
def show_success_popup():
    messagebox.showinfo("Success", "File loaded successfully!")

# Function to wait for acknowledgment from MCU with a 1-second timeout
def wait_for_acknowledgment():
    if ser and ser.is_open:
        try:
            start_time = time.time()
            # Continuously check for acknowledgment with a timeout
            while time.time() - start_time < 1.0:  # 1-second timeout
                ack = ser.read(1)
                if ack == b'A':
                    show_success_popup()  # Show the popup on acknowledgment
                    return
        except Exception as e:
            print(f"Error waiting for acknowledgment: {e}")

# Updated Load function to send the raw file data and decode after sending
def load_file(selected_file_path, file_number, raw_box, decoded_box):
    # Use selected_file_path.get() to obtain the actual file path
    file_path = selected_file_path.get()
    if ser and ser.is_open and file_path:
        with open(file_path, 'r') as file:
            file_content = file.read()

            # Send load command 'L' followed by the selected file number
            ser.write(b'L')
            ser.write(file_number.encode('utf-8'))

            # Send the file data exactly as is, skipping internal '\n' and adding '\n' at the end
            sent_content = ""
            for char in file_content:
                if char != '\n':
                    ser.write(char.encode('utf-8'))
                    sent_content += char
            ser.write(b'\n')  # Send end of file
            sent_content += '\n'  # Reflect the end of file in the preview

            # Display the raw data that was sent in the corresponding text box
            raw_box.delete('1.0', tk.END)
            raw_box.insert('1.0', sent_content)

            # Decode and display the data in the relevant decoded commands box after sending
            decoded_box.delete('1.0', tk.END)
            decoded_box.insert('1.0', decode_data(sent_content.strip()))

            # Start a thread to wait for acknowledgment from the MCU with a timeout
            threading.Thread(target=wait_for_acknowledgment, daemon=True).start()

# Updated execute_script function to start receiving data but not automatically show the angle window
def execute_script(file_number):
    if ser and ser.is_open:
        ser.write(b'E')
        ser.write(file_number.encode('utf-8'))

    # Start a thread to continuously receive data and control angle window visibility
    stop_event = threading.Event()
    threading.Thread(target=receive_data, args=(stop_event,), daemon=True).start()

# Function to close the script mode window
def close_script_mode(window):

    root.deiconify()  # Add this line

    if ser and ser.is_open:
        ser.write(b'0')
    window.destroy()



# Main root window setup (assuming root is defined somewhere in your main application)
root = tk.Tk()
root.title("MCU Control Interface")
root.geometry("300x500")


def sleep_mode():
    if ser and ser.is_open:
        ser.write(b'0')

# Create buttons for the main menu
manual_button = tk.Button(root, text="Manual Control", width=20, height=2, command=manual_control)
paint_button = tk.Button(root, text="Paint Mode", width=20, height=2, command=open_paint_mode)
calibration_button = tk.Button(root, text="Calibration Mode", width=20, height=2, command=open_calibration_mode)
script_button = tk.Button(root, text="Script Mode", width=20, height=2, command=open_script_mode)

sleep_button = tk.Button(root, text="Sleep", width=20, height=2, command=sleep_mode)



manual_button.pack(pady=20)
paint_button.pack(pady=20)
calibration_button.pack(pady=20)
script_button.pack(pady=20)

sleep_button.pack(pady=20)


# Run the GUI main loop
root.mainloop()
