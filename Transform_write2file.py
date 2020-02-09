"""
Example script that allows a user to "push" the Crazyflie 2.X around
using your hands while it's hovering.

This examples uses the Flow and Multi-ranger decks to measure distances
in all directions and tries to keep away from anything that comes closer
than 0.2m by setting a velocity in the opposite direction.

The demo is ended by either pressing Ctrl-C or by holding your hand above the
Crazyflie.
"""
import logging
import sys
import time

import cflib.crtp
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.log import LogConfig
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.positioning.motion_commander import MotionCommander
from cflib.utils.multiranger import Multiranger

URI = 'radio://0/50/250K'

if len(sys.argv) > 1:
    URI = sys.argv[1]

# Only output errors from the logging framework
logging.basicConfig(level=logging.ERROR)

# sub function to achieve block avoidance.
def is_close(range):
    MIN_DISTANCE = 0.2  # m

    if range is None:
        return False
    else:
        return range < MIN_DISTANCE
        
# if is_close(multi_ranger.front):
#     velocity_x -= VELOCITY
# if is_close(multi_ranger.back):
#     velocity_x += VELOCITY

# if is_close(multi_ranger.left):
#     velocity_y -= VELOCITY
# if is_close(multi_ranger.right):
#     velocity_y += VELOCITY

# if is_close(multi_ranger.up):
#     keep_flying = False

def _param_change_callback(name, value):
    print(name + ' changed to value ' + value)

def position_callback(timestamp, data, logconf):
    x = data['stabilizer.roll']
    y = data['stabilizer.pitch']
    z = data['stabilizer.yaw']
    print('attitude: ({}, {}, {})'.format(x, y, z))


# sub function to achieve logging, print selected data to shell
# You can give arbitrary name, and period.
def start_data_printing(scf):
    log_conf = LogConfig(name='Yaw', period_in_ms=50)
    log_conf.add_variable('stabilizer.roll', 'float')
    log_conf.add_variable('stabilizer.pitch', 'float')
    log_conf.add_variable('stabilizer.yaw', 'float')

    scf.cf.log.add_config(log_conf)
    log_conf.data_received_cb.add_callback(position_callback)
    log_conf.start()

# sub_function to achieve transform, T_var is the transform variable, 0 is the expand status, 1 is the shrink status
def UAV_transform(T_var):
    param_name = "transform.tfmEnable"
    param_value = T_var
    scf.cf.param.set_value(param_name, param_value)
    scf.cf.param.add_update_callback(group='transform', name=None,
            cb=_param_change_callback)

if __name__ == '__main__':
    # Initialize the low-level drivers (don't list the debug drivers)
    cflib.crtp.init_drivers(enable_debug_driver=False)
    cf = Crazyflie(rw_cache='./cache')

    with SyncCrazyflie(URI, cf=cf) as scf:
        with MotionCommander(scf) as motion_commander:
            with Multiranger(scf) as multi_ranger:
                start_data_printing(scf)
                keep_flying = True
                time.sleep(2)
                UAV_transform(0)
                while keep_flying:
                    time.sleep(2)  #initially take off time
                    VELOCITY = 0.09 
                    velocity_x = 0.0
                    velocity_y = 0.0
                    for i in range(0,5):
                        if i == 3:
                            UAV_transform(1)
                        velocity_y -= VELOCITY
                        motion_commander.start_linear_motion(
                        velocity_x, velocity_y, 0)

                        time.sleep(1.36)

                    #time.sleep(0.0)
                    VELOCITY = 0.09
                    velocity_x = 0.0
                    velocity_y = 0.0
                    for i in range(0,5):
                        if i == 1:
                            UAV_transform(0)
                        if i == 4:
                            UAV_transform(1)
                        velocity_x += VELOCITY
                        motion_commander.start_linear_motion(
                        velocity_x, velocity_y, 0)
                        time.sleep(0.78)

                    #time.sleep(0.0)
                    VELOCITY = 0.09
                    velocity_x = 0.0
                    velocity_y = 0.0
                    for i in range(0,5):
                        velocity_y += VELOCITY
                        motion_commander.start_linear_motion(
                        velocity_x, velocity_y, 0)
                        time.sleep(1.3)

                    #time.sleep(0.0)
                    VELOCITY = 0.09
                    velocity_x = 0.0
                    velocity_y = 0.0
                    for i in range(0,5):
                        velocity_x -= VELOCITY
                        motion_commander.start_linear_motion(
                        velocity_x, velocity_y, 0)
                        time.sleep(0.7)      
                   

                    keep_flying = False

            print('Demo terminated!')
