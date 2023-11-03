import os
import subprocess
import sys
import shutil
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

VULKAN_SDK = os.environ.get('VULKAN_SDK')
VULKAN_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/1.3.268.0/windows/vulkan_sdk.exe'
HAZEL_VULKAN_VERSION = '1.3.268.0'
VULKAN_SDK_EXE_PATH = 'L3gion/vendor/VulkanSDK/VulkanSDK.exe'

def InstallVulkanSDK():
    print('Downloading {} to {}'.format(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH))
    Utils.DownloadFile(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH)
    print("Done!")
    print("Running Vulkan SDK installer...")
    os.startfile(os.path.abspath(VULKAN_SDK_EXE_PATH))
    print("Re-run this script after installation")

def InstallVulkanPrompt():
    print("Would you like to install the Vulkan SDK?")
    install = Utils.YesOrNo()
    if (install):
        InstallVulkanSDK()
        quit()

def CheckVulkanSDK():
    if (VULKAN_SDK is None):
        print("You don't have the Vulkan SDK installed!")
        InstallVulkanPrompt()
        return False
    elif (HAZEL_VULKAN_VERSION not in VULKAN_SDK):
        print(f"Located Vulkan SDK at {VULKAN_SDK}")
        print(f"You don't have the correct Vulkan SDK version! (L3gion requires {HAZEL_VULKAN_VERSION})")
        InstallVulkanPrompt()
        return False

    print(f"Correct Vulkan SDK located at {VULKAN_SDK}")
    return True

OutputDirectory = "L3gion/vendor/VulkanSDK"

def CopyVulkanFolders(installDir, destDir):
    folders_to_copy = ["Bin", "Bin32", "Lib", "Lib32"]
    for folder in folders_to_copy:
        src_path = os.path.join(installDir, folder)
        dest_path = os.path.join(destDir, folder)
        if os.path.exists(dest_path):
            print(f'The Debug lib folder "{folder}" already exists in "{destDir}"!')
        else:
            if os.path.exists(src_path):
                shutil.copytree(src_path, dest_path)
                print(f'The Debug lib folder "{folder}" has been copied to "{destDir}"!')
            else:
                print(f'The "{src_path}" does not exist!')

def CheckVulkanSDKDebugLibs():
    shadercdLib = Path(f"{OutputDirectory}/Lib/shaderc_sharedd.lib")
    if (not shadercdLib.exists()):
        print(f"No Vulkan SDK debug libs found. (Checked {shadercdLib})")
        print("Moving files to ", f"{OutputDirectory}")
        CopyVulkanFolders(VULKAN_SDK, OutputDirectory)

    print(f"Vulkan SDK debug libs located at {OutputDirectory}")
    return True