from pathlib import Path
from address_decode import *

pathlist = Path('/home/n869p538/AddrRes').glob('**/*.csv')
for path in pathlist:
    path_in_str = str(path)
    print(path_in_str)
    decode_file_addresses(path_in_str)
