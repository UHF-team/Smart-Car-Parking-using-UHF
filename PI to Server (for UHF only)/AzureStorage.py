#---------------------- Import Libraries ----------------------#

import os, simplejson
from azure.storage.blob import BlobServiceClient, __version__
from pySerialTransfer import pySerialTransfer as txfer
from azure.core.exceptions import ResourceExistsError
from datetime import datetime

#------------------------- Functions --------------------------#

def connectAzureAccount(AccountName, AccountKey):
    connect_str = "DefaultEndpointsProtocol=http;" + "AccountName=" + AccountName + ";" + "AccountKey=" + AccountKey + ";"
    blob_service_client = BlobServiceClient.from_connection_string(connect_str)
    return blob_service_client

def createContainer(blob_service_client, container_name):
    container_client = blob_service_client.get_container_client(container_name)
    try:
        container_client.create_container()
    except ResourceExistsError:
        print("Already exist " + container_name + "!")
        pass
    return container_client

def createLocalFolder(local_path):
    if not os.path.exists(local_path):
        os.mkdir(local_path)
        print("Created local file folder!")
    else:
        print("Local file folder exist!")

def getSerialData(SERIAL_PORT):
    link = txfer.SerialTransfer(SERIAL_PORT, 115200)
        
    while not link.available():
        if link.status < 0:
            print('ERROR: {}'.format(link.status))
    
    response = ''
    for index in range(link.bytesRead):
        if (index < 7):
            response += chr(link.rxBuff[index])
        else:
            hex_String = str(hex(link.rxBuff[index]))
            temp_arr = hex_String.split("0x")
            if (hex(link.rxBuff[index]) <= 0x10):
                temp_arr[1] = '0' + temp_arr
            response += temp_arr[1]
    
    print('Response received: ' + response)
    link.close()
    return response

def parseLoRaMessage(LoRa_message, local_path):
    print(LoRa_message)
    data_datetime = datetime.now().strftime("%d/%m/%Y, %H:%M:%S")
    data_time = datetime.strptime(data_datetime, '%d/%m/%Y, %H:%M:%S').strftime("%H%M%S")
    data_date = datetime.strptime(data_datetime, '%d/%m/%Y, %H:%M:%S').strftime("%d%m%Y")
    data_deviceID = LoRa_message[0] + LoRa_message[1] + LoRa_message[2] + LoRa_message[3] + LoRa_message[4] + LoRa_message[5] + LoRa_message[6]
    data_ID = ""

    for i in range(7, len(LoRa_message)):
        data_ID += LoRa_message[i]

    package = {
            'time': data_datetime,
            'deviceID': data_deviceID,
            'data': data_ID
        }
    
    data_json = simplejson.dumps(package)

    local_file_name = data_ID + "_" + data_time + ".json"
    upload_file_path = os.path.join(local_path, local_file_name)

    file = open(upload_file_path, 'w')
    file.write(data_json)
    file.close()

    return local_file_name, data_date

def uploadJsonPackage(blob_service_client, local_path, upload_container, upload_jsonFile):
    blob_client = blob_service_client.get_blob_client(container=upload_container, blob=upload_jsonFile)

    upload_file_path = os.path.join(local_path, upload_jsonFile)
    with open(upload_file_path, "rb") as data:
        blob_client.upload_blob(data)

def cleanUp(local_path, upload_jsonFile):
    upload_file_path = os.path.join(local_path, upload_jsonFile)
    os.remove(upload_file_path)

def listBlobs(container_client):
    # List the blobs in the container
    blob_list = container_client.list_blobs()
    
    for blob in blob_list:
        print("\t" + blob.name)
    