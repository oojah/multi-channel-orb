CFLAGS=-mmcu=attiny48 -O2
TTY=/dev/ttyACM0

orb_cycle_test.hex : orb_cycle_test.elf
	avr-objcopy -j .text -j.data -O ihex $^ $@

orb_cycle_test.elf : cycle.c timer.c
	avr-gcc $(CFLAGS) $^ -o $@

flash : orb_cycle_test.hex
	avrdude -c arduino -p t48 -P ${TTY} -b 19200 -U flash:w:$^

#avr-gcc -mmcu=attiny48 -O2 timer.c cycle.c
#./dump a.out hex
#avrdude -c arduino -p t48 -P /dev/ttyACM0 -b 19200 -U flash:w:hex

#-Wall "$@" pwm.c timer.c twi.c parse.c learn_tiny.c tiny.c led.c && ./dump a.out hex && ./avrdude -eUf:w:hex
