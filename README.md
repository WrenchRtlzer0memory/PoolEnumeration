# Windows Pool Tag Analyzer  

🚀 **WRENCH IS HUNGRY FOR YOU BABY!** 🔥  

## Overview  

This project retrieves and displays **Windows kernel pool tag information** using `NtQuerySystemInformation`. Pool tags help in debugging memory leaks and analyzing Windows internals.  

## Features  

- Retrieves **paged** and **non-paged** memory usage for each pool tag.  
- Loads descriptions from `pooltag.txt` to provide additional context.  
- Displays a formatted table of pool tag usage.  
- Uses `NtQuerySystemInformation(SystemPoolTagInformation)` to fetch system pool tag details.  
- **Compatible with Windows 10 and Windows 11.**  

## How It Works  

1. **Loads pool tag descriptions** from `pooltag.txt`.  
2. Calls `NtQuerySystemInformation(SystemPoolTagInformation)` to retrieve pool tag allocations.  
3. Dynamically resizes memory if `STATUS_INFO_LENGTH_MISMATCH` occurs.  
4. Displays pool tag data with allocation counts and memory usage.  

## Output Example  

```
🚀 WRENCH IS HUNGRY FOR YOU BABY Love me all Night ! 🔥

Tag   Count   PagedBytes   NonPagedBytes   Description
ABC1   1024     512000         20480       DriverX - Used for buffer allocation
DEF2    512     102400         40960       UNKNOWN pooltag 'DEF2', please update pooltag.txt
```

## Supported Platforms  

✅ **Windows 10**  
✅ **Windows 11**  
