#### Reversing AxDIMM's Physical Address Space Mapping for Subsequent SP Util Analysis
Observations:
1 Row stores 16KB

Matches w/ 1 Alteration:
Address_Length
0x100100000_0x000001000
0x100140000_0x000001000
0x100180000_0x000001000
0x1001C0000_0x000001000

Alteration
ba:00
ba:10
ba:01
ba:11

does this alteration pattern hold for every MB granular offset?
MB_0x0..->ba_0x4..->ba_0x8..->ba_0xC..->ba
0x100200000 holds
0x100400000 holds, with offsets 0x40000->ba:00 0xC0000->ba:01 0x80000->ba:11 && 0x00000 ->ba:10
8 holds w/ 0x0..->ba:01 0x8..->00 0x4..->11  0xC..->10
16_0->00_4->10_8->01_C->11


does the bank address change within a 256KB range?