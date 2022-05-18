#! /usr/bin/python3

#./flash_certs.py --partitions ../esp-firmware/partitions.csv --port /dev/ttyUSB0 --root-ca AmazonRootCA1.pem --cert e04e8c141d-certificate.pem.crt --key e04e8c141d-private.pem.key


import os
import sys
import subprocess
import argparse

idf_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), '../esp-firmware/ESP8266_RTOS_SDK/')
parttool_dir = os.path.join(idf_path, "components", "partition_table")
sys.path.append(parttool_dir)

import gen_esp32part as gen

ESPTOOL_PY = os.path.join(idf_path, "components", "esptool_py", "esptool", "esptool.py")
PARTITIONS_FILE = '../../firmware/partitions_two_ota.csv'

def write_data_partition(cert_file, partitions_file, partition_name, port):

    with open(partitions_file, 'r') as f:
        partition_table = gen.PartitionTable.from_csv(f.read())
        partition = partition_table.find_by_name(partition_name)

    invoke_args = \
            [sys.executable, ESPTOOL_PY,
            "--chip", "esp8266",
            "--port", port,
            "--baud", "921600",
            "--before", "default_reset",
            "--after", "hard_reset",
            "write_flash",
            "-z",
            "--flash_mode", "dio",
            "--flash_freq", "40m",
            "--flash_size", "detect",
            str(partition.offset), cert_file]

    subprocess.check_call(invoke_args)

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('--partitions')
    parser.add_argument('--port')
    parser.add_argument('--root-ca')
    parser.add_argument('--cert')
    parser.add_argument('--key')
    args = parser.parse_args()

    write_data_partition(args.root_ca, args.partitions,
                        'aws_root_ca_pem', args.port)

    write_data_partition(args.cert, args.partitions,
                        'cert_pem_crt', args.port)

    write_data_partition(args.key, args.partitions,
                        'private_pem_key', args.port)



