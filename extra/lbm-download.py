#!/usr/bin/env python3

import os, sys
import requests
from io import BytesIO
from zipfile import ZipFile

def download_and_extract(url, subfolder, dest):
    response = requests.get(url)
    response.raise_for_status()
    with ZipFile(BytesIO(response.content)) as zip_file:
        # Filter for files within the specific subfolder
        subfolder_files = [f for f in zip_file.namelist() if f.startswith(subfolder)]

        # Extract each file, adjusting the path
        for file_path in subfolder_files:
            # Extract only if it is a file (not an empty directory)
            if not file_path.endswith("/"):
                new_path = file_path[len(subfolder):]  # Remove the subfolder part of the path
                with zip_file.open(file_path) as source:
                    data = source.read()
                target_path = f"{dest}/{new_path}"  # Define new extraction path
                # Create target directory if not exists
                os.makedirs(os.path.dirname(target_path), exist_ok=True)
                with open(target_path, "wb") as target_file:
                    target_file.write(data)

base_url, version = sys.argv[1:]
download_and_extract(f"{base_url}/v{version}.zip", f"SWL2001-{version}/lbm_lib", "./lbm_lib")
