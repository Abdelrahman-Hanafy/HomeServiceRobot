
#!/bin/sh
xterm  -e "roscore " &
sleep 5
xterm  -e "roslaunch my_robot world.launch "  &
sleep 5
xterm  -e "roslaunch my_robot gmapping.launch "  &
sleep 5
xterm  -e "roslaunch my_robot navigation.launch " &
sleep 5
xterm  -e "roslaunch my_robot teleop.launch " 
