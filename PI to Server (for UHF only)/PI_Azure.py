#---------------------- Import Libraries ----------------------#
import AzureStorage

#--------------------- Configure Parameter --------------------#
local_path = "./data"
AccountName = "gndevstorage"
AccountKey = "rfsKLF1aCLmPcFsFYsHmWjK1fsOyL5gnkGZ9YRB36ElCtL8wDah02VRUY31FDeCcLzwPWKA3xseIprWHW7GjgA=="
container_name_UHF = "uhf-detection"
container_name_USER = "user-data"
    
#------------------------ Main program ------------------------#
if __name__ == "__main__":

    blob_service_client = AzureStorage.connectAzureAccount(AccountName, AccountKey)
    container_client_UHF = AzureStorage.createContainer(blob_service_client, container_name_UHF)
    container_client_USER = AzureStorage.createContainer(blob_service_client, container_name_USER)

    AzureStorage.createLocalFolder(local_path)

    while(1):
        LoRa_message = AzureStorage.getSerialData("COM3") # "/dev/serial0" on PI
        upload_jsonFile, data_date = AzureStorage.parseLoRaMessage(LoRa_message, local_path)
        upload_container = container_name_UHF + "/" + data_date

        AzureStorage.uploadJsonPackage(blob_service_client, local_path, upload_container, upload_jsonFile)

        print(container_name_UHF + "blobs list: ")
        AzureStorage.listBlobs(container_client_UHF)
        print(container_name_USER + "blobs list: ")
        AzureStorage.listBlobs(container_client_USER)

        AzureStorage.cleanUp(local_path, upload_jsonFile)
    