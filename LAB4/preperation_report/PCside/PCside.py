import serial as ser


def main():

    state4 = False
    menu = "\n1. Blink RGB LED, color by color with delay of X[ms]\n" \
           "2. Count up onto LCD screen with delay of X[ms]\n" \
           "3. Circular tone series via Buzzer with delay of X[ms]\n" \
           "4. Get delay time X[ms]:\n" \
           "5. LDR 3-digit value [v] onto LCD\n" \
           "6. Clear LCD screen\n" \
           "7. Show menu\n" \
           "8. Sleep\n"

    s = ser.Serial('COM3', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1)

    enableTX = True

    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()

    i = 0
    X = "0"
    print(menu)

    while 1:
    #RX
        while s.in_waiting > 0:  # while the input buffer isn't empty
            bytes_r_Char = s.read()  # read one byte.
            r_char = bytes_r_Char.decode('ascii')
            if r_char == '7':
                print(menu)
                print("Option 7 done, state is now state0")
            elif r_char == 'R':
                if o_Char != '4' and o_Char != '6' and o_Char != '8':
                    print("Option " + o_Char + " is running, MSP430 state is now state" + o_Char)
                else:
                    print("Option " + o_Char + " done, MSP430 state is now state0")
                    state4 = False
                    X = "0"
            elif r_char == 'E':
                print("MSP430 received invalid Char, MSP430 state is now state0")
                state4 = False
                X = "0"

            if s.in_waiting == 0:
                enableTX = True

        while enableTX:  # while the output buffer isn't empty
            if not state4:
                o_Char = input("Enter Menu Option: ")
                while len(o_Char) != 1 or o_Char < '0' or o_Char > '9':
                    o_Char = input("Enter Menu Option: ")
                if o_Char == '4':
                    state4 = True

                bytes_o_Char = bytes(o_Char, 'ascii')
                s.write(bytes_o_Char)
            else:
                if X == "0":
                    X = input("Enter X Value between 1 to 1000 (ms): ")
                    X = X + '\n'
                    i = 0
                bytes_Xi = bytes(X[i], 'ascii')
                s.write(bytes_Xi)
                i += 1
            enableTX = False


if __name__ == '__main__':
    main()
