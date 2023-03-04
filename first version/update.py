import socket
import os
import shutil
import time
import filecmp

# Define the port number to listen on
port = 5000

# Define the path to the project directory
project_dir = '/home/pi/eep/'
backup_dir = os.path.join(project_dir, 'backup/')
temp_dir = os.path.join(project_dir, 'temp/')

# Define a function to perform the update
def perform_update(update_archive):
    # Extract the contents of the update archive to a temporary directory
    shutil.unpack_archive(update_archive, temp_dir)

    # Backup the existing project directory
    if os.path.exists(backup_dir):
        shutil.rmtree(backup_dir)
    shutil.copytree(project_dir, backup_dir)

    # Iterate over the files in the temporary directory and compare them to the existing files
    for dirpath, dirnames, filenames in os.walk(temp_dir):
        for filename in filenames:
            src_path = os.path.join(dirpath, filename)
            rel_path = os.path.relpath(src_path, temp_dir)
            dst_path = os.path.join(project_dir, rel_path)
            if os.path.exists(dst_path) and filecmp.cmp(src_path, dst_path):
                # The file already exists and hasn't changed, so skip it
                continue
            # The file is new or has changed, so copy it to the project directory
            dst_dir = os.path.dirname(dst_path)
            if not os.path.exists(dst_dir):
                os.makedirs(dst_dir)
            shutil.copy2(src_path, dst_path)

    # Cleanup the temporary directory
    shutil.rmtree(temp_dir)

    # Restart the program to load the updated code
    os.execv(__file__, [__file__])

# Create a TCP/IP socket and bind it to the port
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(('0.0.0.0', port))

# Listen for incoming connections
sock.listen(1)
print(f'Listening on port {port}...')

# Start the main loop to listen for update requests
while True:
    # Wait for a connection
    print('Waiting for connection...')
    conn, addr = sock.accept()
    print(f'Connected by {addr}')

    # Receive the size of the update archive
    size = int.from_bytes(conn.recv(4), byteorder='big')
    print(f'Size of update archive: {size}')

    # Receive the update archive
    update_data = b''
    while len(update_data) < size:
        chunk = conn.recv(size - len(update_data))
        if not chunk:
            break
        update_data += chunk
    print(f'Received {len(update_data)} bytes of update archive')

    # Save the update archive to a file
    update_archive = os.path.join(project_dir, 'update.zip')
    with open(update_archive, 'wb') as f:
        f.write(update_data)
    print(f'Saved update archive to {update_archive}')

    # Perform the update
    try:
        perform_update(update_archive)
    except Exception as e:
        # If the update failed, restore the backup and print an error message
        shutil.rmtree(project_dir)
        shutil.copytree(backup_dir, project_dir)
        print(f'Error performing update: {e}')

    # Close the connection
    conn.close()

    # Wait for a moment to allow the system to stabilize before accepting new connections
    time.sleep(1)
