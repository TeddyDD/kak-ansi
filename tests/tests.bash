#!/usr/bin/env bash
source tests/functions.bash

h2 "Fixing Output"
t 'removes ANSI escapes'              -in ' \e[32m 1.\e[39m hello' -out '  1. hello'
#t '\e(0 selects line-drawing'
#t '\e(B resets line-drawing'
#t 'ASCII SO selects line-drawing'
#t 'ASCII SI resets line-drawing'
#t 'removes trailing spaces'

h2 "Computing Ranges"
t 'emits face at EOF' -in '\e[32mxxx' -range '1.1,1.3|green'
#t 'does not emit default face'
#t 'emits face with fg'
#t 'emits face with bg'
#t 'emits face with attrs'
t 'new face for fg change' -in '\e[32mxxx\e[31myyy' -range '1.1,1.3|green' -range '1.4,1.6|red'
t 'new face for bg change' -in '\e[45mxxx\e[41myyy' -range '1.1,1.3|default,magenta' -range '1.4,1.6|default,red'
#t 'new face for attr change'
t 'no new face if no change' -in '\e[31mxxx\e[31myyy' -range '1.1,1.6|red'
#t 'handles change at 2.1'
#t 'handles change at EOL'

h2 "Foreground Color"
t 'adds ranges for fg colors' -in ' \e[32m 1.' -range '1.2,1.4|green'
t '\e[39m resets fg' -in ' \e[32m 1.\e[39mxx' -range '1.2,1.4|green'
t '\e[0m resets fg' -in ' \e[32m 1.\e[0m hello' -range '1.2,1.4|green'
t '\e[m resets fg' -in ' \e[32m 1.\e[m hello' -range '1.2,1.4|green'
#t '\e[38;5;nm sets fg palette'
#t '\e[38;2;r;g;bm sets true fg'
#t 'can set palette colors'

h2 "Background Color"
t 'adds ranges for bg colors' -in ' \e[41m 1.\e[49m hello' -range '1.2,1.4|default,red'
t '\e[0m resets bg'           -in ' \e[42m 1.\e[0m hello'  -range '1.2,1.4|default,green'
t '\e[m resets bg'            -in ' \e[42m 1.\e[m hello'   -range '1.2,1.4|default,green'
#t '\e[49m resets bg'
#t '\e[48;5;0-7m sets bg palette'
#t '\e[48;5;8-15m sets bg palette'
#t '\e[48;5;Nm sets bg color'
#t '\e[48;5;Nm sets bg grey'
#t '\e[48;2;r;g;bm sets true bg'
#t 'can set palette colors'

h2 "Attributes"
#t '\e[1m sets bold'
#t '\e[2m sets dim'
#t '\e[3m sets italic'
#t '\e[4m sets underline'
#t '\e[5m sets blink'
#t '\e[7m sets reverse'
#t '\e[21m resets bold'
#t '\e[22m sets normal intensity'
#t '\e[23m resets italic'
#t '\e[24m resets underline'
#t '\e[25m resets blink'
#t '\e[27m resets inverse'
#t '\e[0m resets all attributes'
