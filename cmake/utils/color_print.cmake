
# color is given like normal, black, red, green, yellow, blue, magenta, cyan, white
function(color_print color msg)
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --${color} --bold ${msg})
endfunction()

# color_print(cyan "Hi is there anyone?")
