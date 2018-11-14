from selenium import webdriver
import os
import subprocess


driver = webdriver.Chrome(service_log_path=os.path.devnull)
driver.set_window_size(1500, 900)
fname = "file://" + os.getcwd() + "/opcodes.html"
driver.get(fname)
driver.save_screenshot("../images/opcode_map.png")
driver.quit()
subprocess.check_output([
    "convert", "../images/opcode_map.png", 
    "-trim", "../images/opcode_map.png"])
