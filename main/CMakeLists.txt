idf_component_register(SRCS "app_main.c"
                            "app_wifi.c"
                            "app_webserver.c"
                            "rest_server.c"
                            "heater/app_heater.c"
                            "heater/event_dispatcher.c"
                            "heater/mainloop_timer.c"
                            "heater/misc.c"
                            "heater/shell.c"
                            "heater/pwm.c"
                            "heater/gpio.c"
                            "heater/soft_pwm.c"
                            "heater/adc.c"
                            "heater/settings.c"
                            "heater/fan.c"
                            "heater/glow_plug.c"
                            "heater/oil_pump.c"
                            "heater/ntc50.c"
                            "heater/heater.c"
                            "utils/soft_timer.c"
                            "utils/io_driver.c"
                            "utils/io_driver_notifier.c"
                            "utils/sock_util.c"
                            "utils/circ_buffer.c"
                            "utils/stream.c"
                            "utils/tcp_server.c"
                            "utils/tcp_server_ipv4.c"
                            "utils/telnet_reader.c"
                            "utils/cli.c"
                            "utils/cli_telnet.c"
                    INCLUDE_DIRS "."
                                 "heater"
                                 "utils")

set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../web-front")
if(EXISTS ${WEB_SRC_DIR}/dist)
  spiffs_create_partition_image(www ${WEB_SRC_DIR}/dist FLASH_IN_PROJECT)
else()
  message(FATAL_ERROR "${WEB_SRC_DIR}/dist doesn't exit. Please run 'npm run build' in ${WEB_SRC_DIR}")
endif()
