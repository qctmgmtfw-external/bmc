/****************************************************************
 **                                                            **
 **       (C)Copyright 2012, American Megatrends Inc.          **
 **                                                            **
 **                  All Rights Reserved.                      **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 *
 * Filename: license.c
 *  Validating the license key
 *
 *  Author: Aruna Devi D   <arunadevid@amiindia.co.in>
 ****************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "dbgout.h"  
#include "IPMI_AMILicense.h"
#include "nwcfg.h"

#define MEDIA_SERVICE	"MEDIA"
#define LMEDIA_SERVICE	"LMEDIA"
#define RMEDIA_SERVICE	"RMEDIA"
#define MAX_FILE_NAME 256
/*-----------------------------------------------------------------
 *@fn IsValidMACAddr
 *@brief This function will compare the MAC address with BMC MAC address 
 *@param LicMAC - MAC address from status file
 *@param SysMAC- BMC MAC address
 *@return Returns FALSE on failure
 *@return Return TRUE of Comparision  
 */

BOOL  IsValidMACAddr(INT8U *LicMAC, INT8U *SysMAC)
{       
    if (0xFF == LicMAC[3]  && 0xFF == LicMAC[4] && 0xFF == LicMAC[5])
    {
        if( (0xFF != LicMAC[0]  || 0xFF != LicMAC[1] || 0xFF != LicMAC[2]) && ( 0 != memcmp(LicMAC, SysMAC, MAC_OUI_LEN)) )
        {
            TDBG("\n IsValidMACAddr() : Invalid OUI of MAC address\n");
            return FALSE;
        }
    }
    else if ( 0 != memcmp(LicMAC, SysMAC, MAC_ADDR_LEN) )
    {
        TDBG("\n IsValidMACAddr() : Invalid MAC address \n");
        return FALSE;
    }

    return TRUE;
}

/*-----------------------------------------------------------------
 *@fn GetAllLicenseStatus
 *@brief This function will invoke GetLicenseStatus for each app and return 
 *@brief 4-byte value to license app
 *@param Status[out] - 4 byte Status(1-valid license, 0-invalid license)
 *@return Returns -1 on failure 
 */
int GetAllLicenseStatus(INT32U *Status)
{
    FILE* fd_liccfg, *fd_cnt;
    char Str[256];
    char AppCode[APPCODE_LEN] = {0}, ScriptName[16] = {0}, DaemonName[16] = {0}, Type[1] = {0};
    INT32U bit_num = 0;
    INT16U RetVal  = 0;
    BOOL IsValidLic = FALSE;
    BOOL IsTrialLic = FALSE;
    
    if(Status == NULL)
        return -1;

    fd_liccfg = fopen(LICENSE_CONF, "r");
    if(fd_liccfg == NULL)
    {
        TCRIT("\nUnable to open license.conf file \n");
        return -1;
    }
    
    while( NULL != fgets(Str, sizeof(Str), fd_liccfg) )
    {
        sscanf(Str, "%[^:]:%[^:]:%[^:]:%[^:\n]", AppCode, ScriptName, DaemonName, Type);
        if(0 == strcmp(Type, "v"))
        {
            IsValidLic = FALSE;
            RetVal = GetLicenseStatus(AppCode, &IsValidLic);

            if(IsValidLic)
            {
                if ( 0xFF != RetVal )
                {
                    IsTrialLic = TRUE;
                }

                *Status |= (1 << bit_num);
            }
        }
        bit_num++;
    }
    
    fclose(fd_liccfg);

    if (!IsTrialLic)
    {
        fd_cnt = fopen(LICENSE_CNT_DAYS_FILE, "w");
        if(NULL == fd_cnt)
        {
            TCRIT("\n Problem in opening %s file\n", LICENSE_CNT_DAYS_FILE);
            return -1;
        }
        fprintf(fd_cnt, "%d", 0);
        fclose(fd_cnt);
    }

    return 0;
}

/*-----------------------------------------------------------------
 *@fn GetServiceLicenseStatus
 *@brief This function will validate the license key of each app
 *@brief at a time and return boolean value 
 *@param AppCode - app string ex:"KVM"
 *@param Status -( TRUE - valid license False - in-valid license)
 *@return Returns -1 on failure
 */
static INT16 GetServiceLicenseStatus(const char *AppCode, BOOL *Status)
{
    FILE *fd_status, *fd_cntdays, *fd_tmp;
    INT16U InitialDayCount = 0, Validity = 0, CurrDayCount = 0;
    char App_Code[APPCODE_LEN] = {0}, StatusFileBuffer[BUFFER_LENGTH];
    UINT8 Sys_MACAddr[MAC_ADDR_LEN] = {0};
    INT16 BalValidity = 0;
    UINT8 IsValidHwId = FALSE;
    NWCFG_STRUCT  NWConfig;

    if(AppCode == NULL || Status == NULL)
          return -1;

    fd_cntdays = fopen(LICENSE_CNT_DAYS_FILE, "r");
    if(fd_cntdays == NULL)
    {
       TCRIT("\n Problem in opening %s file\n", LICENSE_CNT_DAYS_FILE);
       return -1;
    }
    fscanf(fd_cntdays, "%hd", &CurrDayCount);
    fclose(fd_cntdays);
        
    fd_status = fopen(LICENSE_STATUS_FILE, "r");
    if(fd_status == NULL)
    {
        TCRIT("\n problem in opening %s file\n", LICENSE_STATUS_FILE);
        return 0;
    }

    *Status = FALSE;
    while( NULL != fgets(StatusFileBuffer, BUFFER_LENGTH, fd_status))
    {
        sscanf(StatusFileBuffer, "%[^|]|%hd|%hd|%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", App_Code, &Validity, &InitialDayCount,
                                                                                                      &Sys_MACAddr[0], &Sys_MACAddr[1], &Sys_MACAddr[2], 
                                                                                                      &Sys_MACAddr[3], &Sys_MACAddr[4], &Sys_MACAddr[5]);
        if(strcmp(AppCode, App_Code) == 0)
        {
            *Status = TRUE;
            break;
        }
    }

    if (TRUE == *Status)
    {
        if(0 != nwGetNWInformation(&NWConfig, ETH0_INDEX))
        {
            TCRIT("\nGetLicenseStatus(): Problem in loading MAC address\n");
            fclose(fd_status);
            return -1;
        }
            
        IsValidHwId = IsValidMACAddr (Sys_MACAddr, NWConfig.MAC);

        if(0xFF == Validity)
        {
            BalValidity = Validity;
        }
        else
        {            
            BalValidity = Validity - (CurrDayCount - InitialDayCount);
        }

        if(BalValidity <= 0 || FALSE == IsValidHwId )
        {
            TDBG("\n %s License validity expired or Invalid MAC Address", AppCode);
            *Status=FALSE;

            fd_tmp = fopen(LICENSE_TEMP_FILE, "w");
            if(fd_tmp == NULL)
            {
                fclose(fd_status);
                TCRIT("Problem in opening %s file\n", LICENSE_TEMP_FILE);
                return -1;
            }
                        
            fseek(fd_status, 0, SEEK_SET);
            while( NULL != fgets(StatusFileBuffer, BUFFER_LENGTH, fd_status))
            {
                sscanf(StatusFileBuffer, "%[^|]", App_Code);

                if( 0 != strcmp(App_Code, "") && 0 != strcmp(App_Code, "\n") && 0 != strcmp(AppCode, App_Code))
                {
                    fputs(StatusFileBuffer, fd_tmp);
                }
            }
            fclose(fd_status);
            fclose(fd_tmp);
            
            fd_tmp= fopen(LICENSE_TEMP_FILE, "r");
            if(NULL == fd_tmp)
            {
                TCRIT("Problem in opening %s file\n",LICENSE_TEMP_FILE);
                return -1;
            }
                        
            fd_status = fopen(LICENSE_STATUS_FILE, "w");
            if(NULL == fd_status)
            {
                fclose(fd_tmp);
                TCRIT("Problem in opening %s file\n",LICENSE_STATUS_FILE);
                return -1;
            }
                        
            while( NULL != fgets(StatusFileBuffer, BUFFER_LENGTH, fd_tmp))
            {
                fputs(StatusFileBuffer, fd_status);
            }
            fclose(fd_status);
            fclose(fd_tmp);

            return 0;
        }  
    }

    fclose(fd_status);
    return BalValidity;  
}

/*-----------------------------------------------------------------
 *@fn GetLicenseStatus
 *@brief This function will validate the license key of each app
 *@brief at a time and return boolean value 
 *@param AppCode - app string ex:"KVM"
 *@param Status -( TRUE - valid license False - in-valid license)
 *@return Returns -1 on failure
 */
INT16 GetLicenseStatus(const char *AppCode, BOOL *Status)
{
    INT16 BalValidity = 0;
    BalValidity = GetServiceLicenseStatus( AppCode, Status);
    if (TRUE == *Status) {
        TDBG ("GetLicenseStatus : %s %d %d\n", AppCode, (int)*Status, BalValidity);
        return BalValidity;
    }
    else
    {
        if ( 0 == strcmp (AppCode, MEDIA_SERVICE))
        {
            BalValidity = GetServiceLicenseStatus(LMEDIA_SERVICE, Status);
            if (TRUE == *Status) {
                TDBG ("LMedia is having valid license for MEDIA : %d\n", BalValidity);
                return BalValidity;
            }
            BalValidity = GetServiceLicenseStatus(RMEDIA_SERVICE, Status);
            TDBG ("Checking valid license for RMedia instead of MEDIA : %d %d\n", (int)*Status, BalValidity);
            return BalValidity;
        }
        else
        {
            TDBG ("GetLicenseStatus : %s %d %d\n", AppCode, (int)*Status, BalValidity);
            return BalValidity;
        }
    }
}


/*-----------------------------------------------------------------
 *@fn GetNoLicenseFilePres
 *@brief This function will validate the license key of each app
 *@brief at a time and return boolean value 
 *@param service - app string ex:"KVM"
 *@param Status -( TRUE -  no license file present
 *@param           False - file not present )
 *@return Returns -1 on failure
 */
INT16 GetNoLicenseFilePres(const char *service, BOOL *Status)
{
    FILE* fd_liccfg;
    char Str[256];
    char File_Name[MAX_FILE_NAME];
    char AppCode[8] = {0}, ScriptName[16] = {0}, DaemonName[16] = {0}, Type[1] = {0};
    struct stat sb;
    int ret=-1;
    fd_liccfg = fopen(LICENSE_CONF, "r");
    if( NULL == fd_liccfg )
    {
        TCRIT("\nUnable to open license.conf file \n");
        return -1;
    }

    while( NULL != fgets(Str, sizeof(Str), fd_liccfg) )
    {
		sscanf(Str, "%[^:]:%[^:]:%[^:]:%[^:\n]", AppCode, ScriptName, DaemonName, Type);
		if ( 0 == strcmp(service,AppCode))
		{
			ret=snprintf(File_Name,sizeof(File_Name),"/var/tmp/licstat/%s_nolicense",DaemonName);
			if(ret>=(signed)sizeof(File_Name))
			{
				TCRIT("Buffer overflow\n");
				fclose(fd_liccfg);
				return -1;
			}
			if ( 0 == stat(File_Name,&sb) )
			{
				*Status = TRUE;
			}
			break;
        }
    }

    fclose(fd_liccfg);
    return 0;
}
