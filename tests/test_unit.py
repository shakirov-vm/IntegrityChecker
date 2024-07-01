import os
import shutil
import subprocess
import signal
import time
import json

test_directory_path = "/tmp/tests/crc32_daemon"
dump_json_path = "/tmp/crc32_daemon/dump.json"

def prepare_target_directory():
    shutil.rmtree(test_directory_path)
    os.makedirs(test_directory_path)

def start_daemon_common(request, timeout):

    binary = request.config.getoption("--bin")
    args = (binary, "--dir", test_directory_path, "--timeout", timeout)

    popen = subprocess.Popen(args)
    return popen

def stop_daemon(popen):

    os.kill(popen.pid, signal.SIGTERM)

def rm_dump_json():

    if os.path.exists(dump_json_path):
        os.remove(dump_json_path)

def get_files_info_from_json():

    with open(dump_json_path) as file:
        dump_json = json.load(file)

    files_info = []

    for item in dump_json:
        if isinstance(item, dict):
            files_info.append(item)

    return files_info

def test_new_file(request):

    prepare_target_directory()
    rm_dump_json()
    popen = start_daemon_common(request, "100")
    time.sleep(1)

    file = open(test_directory_path + "/a.txt", "w")
    file.close()
    os.kill(popen.pid, signal.SIGUSR1)
    
    time.sleep(1)

    files_info = get_files_info_from_json()

    for file in files_info:
        if file["path"] == (test_directory_path + "/a.txt"):
            assert file["status"] == "NEW"

    stop_daemon(popen)

def test_absent_file(request):

    prepare_target_directory()
    rm_dump_json()

    file = open(test_directory_path + "/a.txt", "w")
    file.close()

    popen = start_daemon_common(request, "100")
    time.sleep(1)

    os.remove(test_directory_path + "/a.txt")
    os.kill(popen.pid, signal.SIGUSR1)
    
    time.sleep(1)

    files_info = get_files_info_from_json()

    for file in files_info:
        if file["path"] == (test_directory_path + "/a.txt"):
            assert file["status"] == "ABSENT"

    stop_daemon(popen)

def test_change_file(request):

    prepare_target_directory()
    rm_dump_json()

    file = open(test_directory_path + "/a.txt", "w")
    file.write("1")
    file.close()

    popen = start_daemon_common(request, "100")
    time.sleep(1)

    file = open(test_directory_path + "/a.txt", "w")
    file.write("2")
    file.close()

    os.kill(popen.pid, signal.SIGUSR1)
    
    time.sleep(1)

    files_info = get_files_info_from_json()

    for file in files_info:
        if file["path"] == (test_directory_path + "/a.txt"):
            assert file["status"] == "FAIL"

    stop_daemon(popen)


def test_ok_file(request):

    prepare_target_directory()
    rm_dump_json()

    file = open(test_directory_path + "/a.txt", "w")
    file.write("1")
    file.close()

    popen = start_daemon_common(request, "100")
    time.sleep(1)

    os.kill(popen.pid, signal.SIGUSR1)
    
    time.sleep(1)

    files_info = get_files_info_from_json()
    
    for file in files_info:
        if file["path"] == (test_directory_path + "/a.txt"):
            assert file["status"] == "OK"

    stop_daemon(popen)