from helpers import *
# This function performs a write operation.
def wrcas(col_addrs, bg, ba, slot):
    # Convert col_addrs, bg, and ba to binary and append each bit to column_address, bank_group, and bank_address, respectively.
    col_addr = hex_to_bin(col_addrs)
    row_addr = hex_to_bin(col_addrs) # bad name -- sor
    bg_g   = hex_to_bin(bg)
    ba_a   = hex_to_bin(ba)
    bank_group = []
    bank_address = []
    column_address = []
    row_address = []

    for i in range(17):
        row_address.append(row_addr[slot*2 + i*8]) 
    for i in range(10):
        column_address.append(col_addr[slot*2 + i*8])
    for i in range(2):
        bank_group.append(bg_g[slot*2 + i*8])
        bank_address.append(ba_a[slot*2 + i*8])
    print ("wrcas bg:{} ba:{} row:{} col:{}".format(''.join(bank_group), ''.join(bank_address), ''.join(row_address), ''.join(column_address)))

# This function performs a read operation.
def rdcas(col_addrs, bg, ba, slot):
    # Convert col_addrs, bg, and ba to binary and append each bit to column_address, bank_group, and bank_address, respectively.
    col_addr = hex_to_bin(col_addrs)
    row_addr = hex_to_bin(col_addrs) # bad name -- sor
    bg_g   = hex_to_bin(bg)
    ba_a   = hex_to_bin(ba)
    bank_group = []
    bank_address = []
    column_address = []
    row_address = []

    for i in range(17):
        row_address.append(row_addr[slot*2 + i*8]) 
    for i in range(10):
        column_address.append(col_addr[slot*2 + i*8])
    for i in range(2):
        bank_group.append(bg_g[slot*2 + i*8])
        bank_address.append(ba_a[slot*2 + i*8])
    print ("rdcas bg:{} ba:{} col:{} row:{}".format(''.join(bank_group), ''.join(bank_address), ''.join(column_address), ''.join(row_address),))
    print (''.join(bank_group) + ''.join(bank_address) + ''.join(column_address) + ''.join(row_address) )


# This function decodes a given command.
def command_decoder(given_row):
    # Check the given_row for the appropriate command and call the appropriate function with the appropriate parameters.
    if ( given_row[4] == '1'):  #write command
        if (given_row[7] == '00fc'):
            wrcas(given_row[3], given_row[8], given_row[9], 0)
        elif (given_row[7] == '00cf'):
            wrcas(given_row[3], given_row[8], given_row[9], 2)

    elif ( given_row[5] == '1'): #read command
        if (given_row[7] == '00fc'):
            rdcas(given_row[3], given_row[8], given_row[9], 0)
        elif (given_row[7] == '00cf'):
            rdcas(given_row[3], given_row[8], given_row[9], 2)

def decode_file_addresses(csv_file):
    with open(csv_file, 'r') as file_obj:
        reader_obj = csv.reader(file_obj)
        fields = next(reader_obj)
        for row in reader_obj:
            command_decoder(row)