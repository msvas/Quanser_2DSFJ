#! /bin/sh

case "$1" in
    start|restart|force-reload)
    #IO0 - GPIO - END1
        # gpio33 = in
    if [ ! -d /sys/class/gpio/gpio33 ] ; then
      echo -n "33" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio30/direction

          # gpio32 = 1 = in
    if [ ! -d /sys/class/gpio/gpio32 ] ; then
        echo -n "32" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio32/direction
    echo -n "1" > /sys/class/gpio/gpio32/value

          # gpio11 = in
    if [ ! -d /sys/class/gpio/gpio11 ] ; then
        echo -n "11" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio11/direction
    echo -n "rising" > /sys/class/gpio/gpio11/edge

    chgrp quanser /sys/class/gpio/gpio11/value
    chmod g+rw /sys/class/gpio/gpio11/value

    #IO1 - GPIO - END2
          # gpio45 = 0
    if [ ! -d /sys/class/gpio/gpio45 ] ; then
        echo -n "45" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio45/direction
    echo -n "0" > /sys/class/gpio/gpio45/value

        # gpio29 = in
    if [ ! -d /sys/class/gpio/gpio29 ] ; then
        echo -n "29" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio29/direction

          # gpio28 = 1 = in
    if [ ! -d /sys/class/gpio/gpio28 ] ; then
        echo -n "28" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio28/direction
    echo -n "1" > /sys/class/gpio/gpio28/value

          # gpio12 = in
    if [ ! -d /sys/class/gpio/gpio12 ] ; then
        echo -n "12" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio12/direction
    echo -n "rising" > /sys/class/gpio/gpio12/edge

    chgrp quanser /sys/class/gpio/gpio12/value
    chmod g+rw /sys/class/gpio/gpio12/value

    #IO2 - GPIO - DIR
          # gpio77 = 0
    if [ ! -d /sys/class/gpio/gpio77 ] ; then
        echo -n "77" > /sys/class/gpio/export
    fi
    echo -n "0" > /sys/class/gpio/gpio77/value

        # gpio35 = in
    if [ ! -d /sys/class/gpio/gpio35 ] ; then
      echo -n "35" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio35/direction

          # gpio34 = 0 = out
    if [ ! -d /sys/class/gpio/gpio34 ] ; then
        echo -n "34" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio34/direction
    echo -n "0" > /sys/class/gpio/gpio34/value

          # gpio13 = out
    if [ ! -d /sys/class/gpio/gpio13 ] ; then
        echo -n "13" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio13/direction

    chgrp quanser /sys/class/gpio/gpio13/value
    chmod g+rw /sys/class/gpio/gpio13/value

    #IO3 - PWM - PWM2
    if [ ! -d /sys/class/pwm/pwmchip0/pwm1 ] ; then
        echo -n "1" > /sys/class/pwm/pwmchip0/export
    fi
    chgrp quanser /sys/class/pwm/pwmchip0/device/pwm_period
    chmod g+w /sys/class/pwm/pwmchip0/device/pwm_period
    chgrp quanser /sys/class/pwm/pwmchip0/pwm1/duty_cycle
    chmod g+w /sys/class/pwm/pwmchip0/pwm1/duty_cycle
    chgrp quanser /sys/class/pwm/pwmchip0/pwm1/enable
    chmod g+w /sys/class/pwm/pwmchip0/pwm1/enable

          # gpio16 = 0 = out
    if [ ! -d /sys/class/gpio/gpio16 ] ; then
        echo -n "16" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio16/direction
    echo -n "0" > /sys/class/gpio/gpio16/value

          # gpio17 = in = no pull-up nor pull-down
    if [ ! -d /sys/class/gpio/gpio17 ] ; then
        echo -n "17" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio17/direction

          # gpio76 = 0
    if [ ! -d /sys/class/gpio/gpio76 ] ; then
        echo -n "76" > /sys/class/gpio/export
    fi
    echo -n "0" > /sys/class/gpio/gpio76/value

          # gpio64 = 1 */
    if [ ! -d /sys/class/gpio/gpio64 ] ; then
        echo -n "64" > /sys/class/gpio/export
    fi
    echo -n "1" > /sys/class/gpio/gpio64/value

    #IO4 - GPIO - RST
        # gpio37 = in = no resistors
    if [ ! -d /sys/class/gpio/gpio37 ] ; then
        echo -n "37" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio37/direction

          # gpio36 = 0 = out
    if [ ! -d /sys/class/gpio/gpio36 ] ; then
        echo -n "36" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio36/direction
    echo -n "0" > /sys/class/gpio/gpio36/value

          # gpio6 = out
    if [ ! -d /sys/class/gpio/gpio6 ] ; then
        echo -n "6" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio6/direction
    echo -n "0" > /sys/class/gpio/gpio6/value

    chgrp quanser /sys/class/gpio/gpio6/value
    chmod g+rw /sys/class/gpio/gpio6/value

    #IO5 - PWM - PWM1
    if [ ! -d /sys/class/pwm/pwmchip0/pwm3 ] ; then
        echo -n "3" > /sys/class/pwm/pwmchip0/export
    fi
    chgrp quanser /sys/class/pwm/pwmchip0/pwm3/duty_cycle
    chmod g+w /sys/class/pwm/pwmchip0/pwm3/duty_cycle
    chgrp quanser /sys/class/pwm/pwmchip0/pwm3/enable
    chmod g+w /sys/class/pwm/pwmchip0/pwm3/enable
          # gpio18 = 0 = out
    if [ ! -d /sys/class/gpio/gpio18 ] ; then
        echo -n "18" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio18/direction
    echo -n "0" > /sys/class/gpio/gpio18/value
          # gpio19 = in = no pull-up nor pull-down
    if [ ! -d /sys/class/gpio/gpio19 ] ; then
        echo -n "19" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio19/direction
          # gpio66 = 1
    if [ ! -d /sys/class/gpio/gpio66 ] ; then
        echo -n "66" > /sys/class/gpio/export
    fi
    echo -n "1" > /sys/class/gpio/gpio66/value

    #IO6 - GPIO - D0
          # gpio68 = 0
    if [ ! -d /sys/class/gpio/gpio68 ] ; then
        echo -n "68" > /sys/class/gpio/export
    fi
    echo -n "0" > /sys/class/gpio/gpio68/value

          # gpio20 = 1 = in
    if [ ! -d /sys/class/gpio/gpio20 ] ; then
        echo -n "20" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio20/direction
    echo -n "1" > /sys/class/gpio/gpio20/value

          # gpio1 = in
    if [ ! -d /sys/class/gpio/gpio1 ] ; then
        echo -n "1" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio1/direction
    chgrp quanser /sys/class/gpio/gpio1/value
    chmod g+rw /sys/class/gpio/gpio1/value

    #IO7 - GPIO - D1
          # gpio38 = in
    if [ ! -d /sys/class/gpio/gpio38 ] ; then
        echo -n "38" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio38/direction
    chgrp quanser /sys/class/gpio/gpio38/value
    chmod g+rw /sys/class/gpio/gpio38/value

    #IO8 - GPIO - D2
          # gpio40 = in
    if [ ! -d /sys/class/gpio/gpio40 ] ; then
        echo -n "40" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio40/direction
    chgrp quanser /sys/class/gpio/gpio40/value
    chmod g+rw /sys/class/gpio/gpio40/value

    #IO9 - GPIO - D3
          # gpio70 = 0
    if [ ! -d /sys/class/gpio/gpio70 ] ; then
        echo -n "70" > /sys/class/gpio/export
    fi
    echo -n "0" > /sys/class/gpio/gpio70/value

          # gpio22 = 1 = in
    if [ ! -d /sys/class/gpio/gpio22 ] ; then
        echo -n "22" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio22/direction
    echo -n "1" > /sys/class/gpio/gpio22/value

          # gpio4 = in
    if [ ! -d /sys/class/gpio/gpio4 ] ; then
        echo -n "4" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio4/direction

    chgrp quanser /sys/class/gpio/gpio4/value
    chmod g+rw /sys/class/gpio/gpio4/value

    #IO10 - GPIO - D4
          # gpio74 = 0
    if [ ! -d /sys/class/gpio/gpio74 ] ; then
        echo -n "74" > /sys/class/gpio/export
    fi
    echo -n "0" > /sys/class/gpio/gpio74/value

          # gpio26 = 1 = in
    if [ ! -d /sys/class/gpio/gpio26 ] ; then
        echo -n "26" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio26/direction
    echo -n "1" > /sys/class/gpio/gpio26/value

          # gpio10 = in
    if [ ! -d /sys/class/gpio/gpio10 ] ; then
        echo -n "10" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio10/direction
    chgrp quanser /sys/class/gpio/gpio10/value
    chmod g+rw /sys/class/gpio/gpio10/value

    #IO11 - GPIO - D5
          # gpio72 = 0
    if [ ! -d /sys/class/gpio/gpio72 ] ; then
        echo -n "72" > /sys/class/gpio/export
    fi
    echo -n "0" > /sys/class/gpio/gpio72/value

          # gpio44 = 0
    if [ ! -d /sys/class/gpio/gpio44 ] ; then
        echo -n "44" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio44/direction
    echo -n "0" > /sys/class/gpio/gpio44/value

          # gpio24 = 1 = in
    if [ ! -d /sys/class/gpio/gpio24 ] ; then
        echo -n "24" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio24/direction
    echo -n "1" > /sys/class/gpio/gpio24/value

          # gpio5 = in
    if [ ! -d /sys/class/gpio/gpio5 ] ; then
        echo -n "5" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio5/direction
    chgrp quanser /sys/class/gpio/gpio5/value
    chmod g+rw /sys/class/gpio/gpio5/value

    #IO12 - GPIO - D6
          # gpio42 = 1 = in
    if [ ! -d /sys/class/gpio/gpio42 ] ; then
        echo -n "42" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio42/direction
    echo -n "1" > /sys/class/gpio/gpio42/value

          # gpio15 = in
    if [ ! -d /sys/class/gpio/gpio15 ] ; then
        echo -n "15" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio15/direction
    chgrp quanser /sys/class/gpio/gpio15/value
    chmod g+rw /sys/class/gpio/gpio15/value

    #IO13 - GPIO - D7
          # gpio46 = 0
    if [ ! -d /sys/class/gpio/gpio46 ] ; then
        echo -n "46" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio46/direction
    echo -n "0" > /sys/class/gpio/gpio46/value

          # gpio30 = 1 = in
    if [ ! -d /sys/class/gpio/gpio30 ] ; then
        echo -n "30" > /sys/class/gpio/export
    fi
    echo -n "out" > /sys/class/gpio/gpio30/direction
    echo -n "1" > /sys/class/gpio/gpio30/value

          # gpio7 = in
    if [ ! -d /sys/class/gpio/gpio7 ] ; then
        echo -n "7" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio7/direction

    chgrp quanser /sys/class/gpio/gpio7/value
    chmod g+rw /sys/class/gpio/gpio7/value

    #IO14 - ADC - DC1
          # gpio49 = in
    if [ ! -d /sys/class/gpio/gpio49 ] ; then
        echo -n "49" > /sys/class/gpio/export
    fi
    echo -n "in" > /sys/class/gpio/gpio49/direction

    chgrp quanser /sys/bus/iio/devices/iio:device0/in_voltage0_raw
    chmod g+r /sys/bus/iio/devices/iio:device0/in_voltage0_raw
    chgrp quanser /sys/bus/iio/devices/iio:device0/in_voltage0_scale
    chmod g+r /sys/bus/iio/devices/iio:device0/in_voltage0_scale
    ;;
esac

exit 0
