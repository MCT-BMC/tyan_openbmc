#include <iostream>
#include <fstream>
#include <mtd/mtd-user.h>
#include <sys/ioctl.h>

constexpr auto FDBAR = 0x0;
constexpr auto FRBA = FDBAR + 0x40;
constexpr auto FLREG11 = FRBA + 0x2C;

constexpr auto I40E_SR_SIZE_IN_WORDS = (0x10000 >> 0x01); //64K
constexpr auto I40E_SR_VPD_MODULE_MAX_SIZE_IN_WORDS = (0x400 >> 0x01); //1K
constexpr auto I40E_SR_PCIE_ALT_MODULE_MAX_SIZE_IN_WORDS = (0x400 >> 0x01); //1K
constexpr auto I40E_SR_PCIE_ALT_MODULE_WORD_OFFSET = I40E_SR_SIZE_IN_WORDS - I40E_SR_PCIE_ALT_MODULE_MAX_SIZE_IN_WORDS;
constexpr auto I40E_SR_SW_CHECKSUM_WORD_OFFSET = 0x3F;
constexpr auto I40E_SR_SW_CHECKSUM_BASE = 0xBABA;

constexpr auto MTD_PATH = "/dev/mtd/pnor";
constexpr auto MAC_EEPROM_PATH = "/sys/bus/i2c/devices/6-0050/eeprom";

constexpr auto MAC_OFFSET = 0x1F00;

struct offsetToBlockOffset {
    uint32_t block;
    uint32_t blockOffset;
};

struct ReservedAddress
{
    uint32_t regionBase10Gbe;
    uint16_t offset;
    uint16_t pfMac;
    uint32_t pfMacAddressOffset;

    uint16_t pfMacAddress[4];
    uint16_t portMacAddress1[2];
    uint16_t portMacAddress2[2];

    struct offsetToBlockOffset region10GbeOffset = {0, 0};
    struct offsetToBlockOffset pfMacOffset = {0, 0};
    struct offsetToBlockOffset portMacOffset[2] = {{0, 0}, {0, 0}};

    uint16_t portOffset;
    uint16_t portOffset1;
    uint16_t portOffset2;
    uint32_t portMacAddressOffset1;
    uint32_t portMacAddressOffset2;
};

struct mtd_info_user mtd;
struct ReservedAddress reservedAddress;
uint8_t hostMac[4][6];

template <typename T>
T readRegister(std::fstream &device, uint32_t offset)
{
    unsigned char buffer[sizeof(T)];
    T data=0;

    device.seekg(offset, std::ios::beg);
    device.read((char*)&data, sizeof(T));

    return data;
}

int getMtdInfo()
{
    int fd = -1;
    /* get some info about the flash device */
    fd = open(MTD_PATH, O_SYNC | O_RDWR);
    if(ioctl(fd, MEMGETINFO, &mtd) < 0)
    {
        std::cerr << "Error in " << __func__ << std::endl;
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}

int getMacInfo()
{
    uint8_t mac[6];

    std::fstream device(MAC_EEPROM_PATH, std::ios::in | std::ios::out | std::ios::binary);

    for(uint8_t i=0; i < sizeof(mac); i++)
    {
        device.seekg(MAC_OFFSET+i, std::ios::beg);
        device.read((char*)&mac[i], sizeof(uint8_t));
#ifdef DEBUG
        printf("MAC address[%d]:0x%x\n",i,mac[i]);
#endif
    }

    
    for(uint8_t i=0; i < sizeof(hostMac)/sizeof(hostMac[0]); i++)
    {
       for(uint8_t j=0; j < sizeof(hostMac[0]); j++)
       {
            if(j != sizeof(hostMac[0]) - 1){
                hostMac[i][j] = mac[j];
            }
            else
            {
                hostMac[i][j] = mac[j] - sizeof(hostMac[0]) + i + 1;
            }
#ifdef DEBUG
            printf("MAC address[%d][%d]:0x%x\n",i,j,hostMac[i][j]);
#endif
       }
    }

    device.close();
    return 0;
}

struct offsetToBlockOffset Get_BlockAndOffset_FromOffset(uint32_t offset)
{
    struct offsetToBlockOffset data;
    data.block = offset/(0x10000);
    data.blockOffset = offset%(0x10000);
    return data;
};

int CalculateBiosCheckSum(uint16_t *region10Gbe)
{
    uint16_t vpdOffset;
    uint16_t checkSum16;
    uint16_t index;

    //VPD area pointer at 0x2F
    vpdOffset = region10Gbe[0x2F];
#ifdef DEBUG
    printf("VPD Offset 0x%x\n", vpdOffset);
#endif

    checkSum16 = 0;

    for( index = 0; index < I40E_SR_SIZE_IN_WORDS; index++ )
    {
        //Skip checksum
        if( index == I40E_SR_SW_CHECKSUM_WORD_OFFSET )
            continue;
        //Skip Vpd area
        if( index == vpdOffset )
        {
            index += (I40E_SR_VPD_MODULE_MAX_SIZE_IN_WORDS - 0x01);
            continue;
        }
        //Skip PCIe ALT
        if( index == I40E_SR_PCIE_ALT_MODULE_WORD_OFFSET )
            break;
        checkSum16 += region10Gbe[index];
    }

    checkSum16 = I40E_SR_SW_CHECKSUM_BASE - checkSum16;
    region10Gbe[I40E_SR_SW_CHECKSUM_WORD_OFFSET] = checkSum16;
#ifdef DEBUG
    printf("Calulate Checksum %x\n", checkSum16);
#endif
    return 0;
}

int storeReservedAddress()
{
    std::fstream mtdDevice(MTD_PATH, std::ios::in | std::ios::out | std::ios::binary);

    reservedAddress.regionBase10Gbe = readRegister<uint16_t>(mtdDevice,FLREG11);

    reservedAddress.regionBase10Gbe = reservedAddress.regionBase10Gbe << 12;

    reservedAddress.offset = readRegister<uint16_t>(mtdDevice,reservedAddress.regionBase10Gbe + 0x48*2);

    reservedAddress.pfMac = readRegister<uint16_t>(mtdDevice,reservedAddress.regionBase10Gbe +
                                     0x18*2 + reservedAddress.offset*2);

    reservedAddress.pfMacAddressOffset= (uint32_t)(reservedAddress.regionBase10Gbe + 0x18*2 +
                                         reservedAddress.offset*2 + reservedAddress.pfMac*2);

    reservedAddress.region10GbeOffset = Get_BlockAndOffset_FromOffset(reservedAddress.regionBase10Gbe);
    reservedAddress.pfMacOffset = Get_BlockAndOffset_FromOffset(reservedAddress.pfMacAddressOffset);

    reservedAddress.portOffset = readRegister<uint16_t>(mtdDevice,reservedAddress.regionBase10Gbe + 0x7*2);

    reservedAddress.portOffset1 = readRegister<uint16_t>(mtdDevice,reservedAddress.regionBase10Gbe +
                                      0xB*2 + reservedAddress.portOffset*2);

    reservedAddress.portOffset2 = readRegister<uint16_t>(mtdDevice,reservedAddress.regionBase10Gbe + 0xC*2 +
                                      reservedAddress.portOffset*2);

    reservedAddress.portMacAddressOffset1 = (uint32_t)(reservedAddress.regionBase10Gbe + reservedAddress.portOffset1*2 +
                                              reservedAddress.portOffset2*2);

    reservedAddress.portMacOffset[0] = Get_BlockAndOffset_FromOffset(reservedAddress.portMacAddressOffset1);

    reservedAddress.portOffset1 = readRegister<uint16_t>(mtdDevice,reservedAddress.regionBase10Gbe + 0xD*2 +
                                      reservedAddress.portOffset*2);

    reservedAddress.portOffset2 = readRegister<uint16_t>(mtdDevice,reservedAddress.regionBase10Gbe + 0xE*2 +
                                      reservedAddress.portOffset*2);

    reservedAddress.portMacAddressOffset2 = reservedAddress.regionBase10Gbe + reservedAddress.portOffset1*2 +
                                             reservedAddress.portOffset2*2;

    reservedAddress.portMacOffset[1] = Get_BlockAndOffset_FromOffset(reservedAddress.portMacAddressOffset2);

#ifdef DEBUG
    printf("[Debug] RegionBase10GBE %x\n", reservedAddress.regionBase10Gbe);
    printf("[Debug] regionBase10Gbe[0x48] 0x%x\n", reservedAddress.offset);
    printf("[Debug] regionBase10Gbe[0x18+offset] 0x%x\n", reservedAddress.pfMac);
    printf("[Debug] pfMacAddress 0x%x\n", reservedAddress.pfMacAddressOffset);

    reservedAddress.pfMacAddress[0] = readRegister<uint16_t>(mtdDevice,reservedAddress.pfMacAddressOffset);
    reservedAddress.pfMacAddress[1] = readRegister<uint16_t>(mtdDevice,reservedAddress.pfMacAddressOffset+2);
    reservedAddress.pfMacAddress[2] = readRegister<uint16_t>(mtdDevice,reservedAddress.pfMacAddressOffset+4);
    reservedAddress.pfMacAddress[3] = readRegister<uint16_t>(mtdDevice,reservedAddress.pfMacAddressOffset+6);
    printf("[Debug] MAC[0] Value 0x%x\n", reservedAddress.pfMacAddress[0]);
    printf("[Debug] MAC[0] Value 0x%x\n", reservedAddress.pfMacAddress[1]);
    printf("[Debug] MAC[0] Value 0x%x\n", reservedAddress.pfMacAddress[2]);
    printf("[Debug] MAC[0] Value 0x%x\n", reservedAddress.pfMacAddress[3]);

    printf("region10Gbe block 0x%x, offset 0x%x\n", reservedAddress.region10GbeOffset.block, reservedAddress.region10GbeOffset.blockOffset);
    printf("PfMac block 0x%x, offset 0x%x\n", reservedAddress.pfMacOffset.block, reservedAddress.pfMacOffset.blockOffset);
    printf("===Part1===\n");
    printf("[Debug] regionBase10Gbe[0x7] 0x%x\n", reservedAddress.portOffset);
    printf("[Debug] regionBase10Gbe[0xB] 0x%x\n", reservedAddress.portOffset1);
    printf("[Debug] regionBase10Gbe[0xC] 0x%x\n", reservedAddress.portOffset2);
    printf("[Debug] portMacAddressOffset1 0x%x\n", reservedAddress.portMacAddressOffset1);

    reservedAddress.portMacAddress1[0] = readRegister<uint16_t>(mtdDevice,reservedAddress.portMacAddressOffset1);
    reservedAddress.portMacAddress1[1] = readRegister<uint16_t>(mtdDevice,reservedAddress.portMacAddressOffset1+2);
    printf("[Debug] MAC[1] Value 0x%x\n", reservedAddress.portMacAddress1[0]);
    printf("[Debug] MAC[1] Value 0x%x\n", reservedAddress.portMacAddress1[1]);


    printf("PortMac block 0x%x, offset 0x%x\n", reservedAddress.portMacOffset[0].block, reservedAddress.portMacOffset[0].blockOffset);
    printf("===Part2===\n");
    printf("[Debug] regionBase10Gbe[0xB] 0x%x\n", reservedAddress.portOffset1);
    printf("[Debug] regionBase10Gbe[0xC] 0x%x\n", reservedAddress.portOffset2);
    printf("[Debug] portMacAddress2 0x%x\n", reservedAddress.portMacAddressOffset2);

    reservedAddress.portMacAddress2[0] = readRegister<uint16_t>(mtdDevice,reservedAddress.portMacAddressOffset2);
    reservedAddress.portMacAddress2[1] = readRegister<uint16_t>(mtdDevice,reservedAddress.portMacAddressOffset2+2);
    printf("[Debug] MAC[2] Value 0x%x\n", reservedAddress.portMacAddress2[0]);
    printf("[Debug] MAC[2] Value 0x%x\n", reservedAddress.portMacAddress2[0]);

    printf("PortMac block 0x%x, offset 0x%x\n", reservedAddress.portMacOffset[1].block, reservedAddress.portMacOffset[1].blockOffset);
#endif

    mtdDevice.close();
    return 0;
}

int recoveryReservedAddress()
{
    // for reserving system MAC address
    unsigned long vFlashOffset;
    unsigned long flashOffset;
    unsigned long startOffset;
    uint16_t copyLength;
    uint8_t *flashBuf = NULL;
    uint8_t *tmpFlashBuf = NULL;

    int fd;

    if(getMtdInfo())
    {
        return -1;
    }

    if(getMacInfo())
    {
        return -1;
    }

    startOffset = 0;

    vFlashOffset = (mtd.erasesize)*511;
#ifdef DEBUG
    printf("Set vFlashOffset to 0x%lu\n", vFlashOffset);
#endif
    //step 1. Read whole block data back
    copyLength =  reservedAddress.pfMacOffset.block -  reservedAddress.region10GbeOffset.block + 1;

    fd = open(MTD_PATH, O_RDWR);
    if (fd == -1) {
        return -1;
    }

    // Allocate a buffer for storing the block of data to be read from flash.  It will be erase block size
    // copy 2 block
    flashBuf = (uint8_t *)malloc((mtd.erasesize)*copyLength);
    // Assign it to a tempory pointer for future use
    tmpFlashBuf = flashBuf;
    flashOffset = (mtd.erasesize)*reservedAddress.region10GbeOffset.block;
#ifdef DEBUG
    printf("lseek offset: %lx\n", startOffset + flashOffset);
#endif
    if (lseek(fd, startOffset + flashOffset, SEEK_SET) == -1)
    {
        std::cerr << "Error in lseek " << __func__ << std::endl;
        return -1;
    }

    if( (read(fd,flashBuf,(mtd.erasesize)*copyLength))!= (long int)(mtd.erasesize*copyLength))
    {
        std::cerr << "Error in read " << __func__ << std::endl;
        return -1;
    }

    //step 2. Modify data
    //PF MAC Address
    for (int port_num = 0; port_num < 4; port_num++)
    {
        for (int cnt = 0; cnt < 6; cnt++)
        {
            //block is not the same
            if (reservedAddress.pfMacOffset.block != reservedAddress.region10GbeOffset.block)
            {
                *(flashBuf + (mtd.erasesize) * (reservedAddress.pfMacOffset.block - reservedAddress.region10GbeOffset.block) +
                reservedAddress.pfMacOffset.blockOffset + 2 * (port_num + 1)+cnt+port_num * 6) = hostMac[port_num][cnt];
            }
        }
    }
    //Port Mac 1
    for (int port_num = 0; port_num < 4; port_num++)
    {
        for (int cnt = 0; cnt < 4; cnt++)
        {
            if (reservedAddress.portMacOffset[0].block != reservedAddress.region10GbeOffset.block)
            {
                *(flashBuf + (mtd.erasesize) * (reservedAddress.portMacOffset[0].block - reservedAddress.region10GbeOffset.block) + 
                reservedAddress.portMacOffset[0].blockOffset+cnt + port_num * 4) = hostMac[port_num][cnt];
            }
        }
    }
    // Port MAC 2
    for (int port_num = 0; port_num < 4; port_num++)
    {
        for (int cnt = 0; cnt < 2; cnt++)
        {
            if (reservedAddress.portMacOffset[1].blockOffset != reservedAddress.region10GbeOffset.block)
            {

                *(flashBuf + (mtd.erasesize) * (reservedAddress.portMacOffset[1].block - reservedAddress.region10GbeOffset.block) +
                reservedAddress.portMacOffset[1].blockOffset+cnt+port_num * 4) = hostMac[port_num][4+cnt];
            }
        }
    }

    // the offset should not be divided by 2
    CalculateBiosCheckSum((uint16_t *)(flashBuf + reservedAddress.region10GbeOffset.blockOffset));

    flashOffset = (mtd.erasesize)*reservedAddress.region10GbeOffset.block;
#ifdef DEBUG
    printf("flashOffset 0x%lu\n", flashOffset);
#endif
    struct erase_info_user erase_info;

    //step 3. Write whole block data back
    for(uint32_t j=0; j<copyLength; j++)
    {
#ifdef DEBUG
        printf("lseek offset: %lx\n", startOffset + flashOffset);
#endif
        if (lseek(fd, startOffset + flashOffset, SEEK_SET) == -1)
        {
            close(fd);
            return -1;
        }

        erase_info.start = startOffset + flashOffset;
        printf("For reserving system MAC ,Upgrading the block =  %ld\n",(flashOffset / (mtd.erasesize)));
        erase_info.length = (mtd.erasesize);
#ifdef DEBUG
        printf("erase start: %x\n", erase_info.start);
#endif
        if((ioctl (fd, MEMERASE, &erase_info)) == -1)
        {
            close(fd);
            return -1;
        }
        // Seek to the appropriate offset
        if (lseek(fd, startOffset + flashOffset, SEEK_SET) == -1)
        {
            close(fd);
            return -1; 
        }

        if (write(fd, flashBuf + j*(mtd.erasesize), mtd.erasesize) != (signed int)mtd.erasesize)
        {
            close(fd);
            return -1; 
        }

        flashOffset += (mtd.erasesize);
    }

    vFlashOffset = (mtd.erasesize)*512;

    free(flashBuf);
    close(fd);

    return 0;
}
